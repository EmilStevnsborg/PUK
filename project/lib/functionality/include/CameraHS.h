#ifndef CAMERAHS_H
#define CAMERAHS_H

#include "Camera.h"
#include <stdint.h>
#include <string>
#include <cstdio>
#include <cstdlib> 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

typedef uint8_t byte;

struct buffer {
    void   *start;
    size_t  length;
};

class CameraHS : public Camera {
    private:
        int channels;
        int rows;
        int cols;

        const char* devicePath;

        // camera interaction
        int fd;

        struct buffer* buffers;
        unsigned int n_buffers;
        
        // helpers
        void errno_exit(const char *s);
        int xioctl(int fh, int request, void *arg);
        
        // setup
        void open_device();
        void init_device();
        void init_mmap();
        void start_capturing();
        
        // image processing
        int read_frame(Buffer* outputBuffer);
        void process_image(const void *p, int size, Buffer* outputBuffer);

        // closing
        void stop_capturing();
        void close_device();
        void uninit_device();

    public:
        CameraHS(std::string devicePath, bool snapshot, 
                 int channels, int rows, int cols);
        
        // line scan
        void Stream(Buffer* outputBuffer, int line);
        // snapshot
        void Stream(Buffer* outputBuffer);

        ~CameraHS() {
            stop_capturing();
            uninit_device();
            close_device();
        };

};

#endif //CAMERAHS_H