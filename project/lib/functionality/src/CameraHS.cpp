#include "CameraHS.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

CameraHS::CameraHS(std::string devicePath, bool snapshot, 
                   int channels, int rows, int cols) :
    Camera(channels, rows, cols, snapshot)
{
    this->devicePath = devicePath.c_str();

    open_device();
    init_device();
    start_capturing();

    // device uninits and closes in deconstructor
}

void CameraHS::Stream(Buffer* outputBuffer, int line) {
    // Capture frame
}

void CameraHS::Stream(Buffer* outputBuffer) {
    unsigned int count = 1;
    while (count -- > 0) {
        for (;;) {
            fd_set fds;
            struct timeval tv;
            int r;

            FD_ZERO(&fds);
            FD_SET(this->fd, &fds);

            /* Timeout. */
            tv.tv_sec = 2;
            tv.tv_usec = 0;

            r = select(this->fd + 1, &fds, NULL, NULL, &tv);
            
            if (-1 == r) {
                if (EINTR == errno) {
                    continue;
                }
                errno_exit("select");
            }

            if (0 == r) {
                fprintf(stderr, "select timeout\n");
                exit(EXIT_FAILURE);
            }

            if (read_frame(outputBuffer)) {
                break;
            }
        }
    }
}

// INITIALIZATION
void CameraHS::open_device() {
    this->fd = open(this->devicePath, O_RDWR | O_NONBLOCK, 0);

    if (-1 == fd) {
        fprintf(stderr, "Cannot open '%s': %d, %s\n", this->devicePath, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}
void CameraHS::init_device() {
    struct v4l2_capability cap;

    // Query device capabilities
    if (-1 == xioctl(this->fd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s is no V4L2 device\n", this->devicePath);
            exit(EXIT_FAILURE);
        } else {
            errno_exit("VIDIOC_QUERYCAP");
        }
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "%s is no video capture device\n", this->devicePath);
        exit(EXIT_FAILURE);
    }

    // Check if the device supports read I/O
    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        fprintf(stderr, "%s does not support streaming\n", this->devicePath);
        exit(EXIT_FAILURE);
    }

    // configure format
    struct v4l2_format fmt;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = this->Cols();
    fmt.fmt.pix.height      = this->Rows();
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
    if (-1 == xioctl(this->fd, VIDIOC_S_FMT, &fmt))
        errno_exit("VIDIOC_S_FMT");

    // memory allocate buffer
    init_mmap();
};
void CameraHS::init_mmap() {
    struct v4l2_requestbuffers req;
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) errno_exit("VIDIOC_REQBUFS");
    if (req.count < 2) exit(EXIT_FAILURE);
    this->buffers = (buffer*) calloc(req.count, sizeof(*buffers));
    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf;
        buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = this->n_buffers;
        if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf)) errno_exit("VIDIOC_QUERYBUF");
        this->buffers[this->n_buffers].length = buf.length;
        this->buffers[this->n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (MAP_FAILED == buffers[this->n_buffers].start) errno_exit("mmap");
    }
}

void CameraHS::start_capturing() {
    enum v4l2_buf_type type;
    for (unsigned int i = 0; i < this->n_buffers; ++i) {
        struct v4l2_buffer buf;

        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (-1 == xioctl(this->fd, VIDIOC_QBUF, &buf))
            errno_exit("VIDIOC_QBUF");
    }
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(this->fd, VIDIOC_STREAMON, &type))
        errno_exit("VIDIOC_STREAMON");
}

// STREAMING
int CameraHS::read_frame(Buffer* outputBuffer) {
    struct v4l2_buffer buf;

    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(this->fd, VIDIOC_DQBUF, &buf)) {
        switch (errno) {
            case EAGAIN:
                return 0;
            default:
                errno_exit("VIDIOC_DQBUF");
        }
    }

    process_image(this->buffers[buf.index].start, buf.bytesused, outputBuffer);

    if (-1 == xioctl(this->fd, VIDIOC_QBUF, &buf)) {
        errno_exit("VIDIOC_QBUF");
    }
    return 1;
}

void CameraHS::process_image(const void *p, int size, Buffer* outputBuffer) {
    const byte* yuyv_data = reinterpret_cast<const byte*>(p);
    
    for (int i = 0, j = 0; i < size; i += 4, j += 6) {
        int y0 = yuyv_data[i];
        int u = yuyv_data[i + 1] - 128;
        int y1 = yuyv_data[i + 2];
        int v = yuyv_data[i + 3] - 128;

        int r0 = (int)(y0 + 1.402 * v);
        int g0 = (int)(y0 - 0.344136 * u - 0.714136 * v);
        int b0 = (int)(y0 + 1.772 * u);

        int r1 = (int)(y1 + 1.402 * v);
        int g1 = (int)(y1 - 0.344136 * u - 0.714136 * v);
        int b1 = (int)(y1 + 1.772 * u);

        // Clamp values to [0, 255]
        outputBuffer->Memory<byte>()[j]     = (byte)std::min(std::max(r0, 0), 255);
        outputBuffer->Memory<byte>()[j + 1] = (byte)std::min(std::max(g0, 0), 255);
        outputBuffer->Memory<byte>()[j + 2] = (byte)std::min(std::max(b0, 0), 255);

        outputBuffer->Memory<byte>()[j + 3] = (byte)std::min(std::max(r1, 0), 255);
        outputBuffer->Memory<byte>()[j + 4] = (byte)std::min(std::max(g1, 0), 255);
        outputBuffer->Memory<byte>()[j + 5] = (byte)std::min(std::max(b1, 0), 255);
    }
}


// UNINITIALIZATION
void CameraHS::stop_capturing() {
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(this->fd, VIDIOC_STREAMOFF, &type)) {
        errno_exit("VIDIOC_STREAMOFF");
    }
}

void CameraHS::close_device() {
        if (-1 == close(this->fd)) {
            errno_exit("close");
        }
        this->fd = -1;
}
void CameraHS::uninit_device() {
    for (unsigned int i = 0; i < this->n_buffers; ++i)
        if (-1 == munmap(this->buffers[i].start, this->buffers[i].length))
            errno_exit("munmap");

}

// HELPERS
void CameraHS::errno_exit(const char *s) {
    fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
    exit(EXIT_FAILURE);
}

int CameraHS::xioctl(int fh, int request, void *arg) {
    int r;
    do {
        r = ioctl(fh, request, arg);
    } while (-1 == r && EINTR == errno);
    return r;
}