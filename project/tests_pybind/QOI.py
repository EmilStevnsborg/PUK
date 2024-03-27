import numpy as np

class QOI:
    def __init__(self, flat_image, channels, rows, cols):
        
        # flat array of uint8
        self.buffer = flat_image
        self.last_pixel = self.buffer.shape[0]-channels

        self.channels = channels
        self.rows = rows
        self.cols = cols

        self.buffer_size = rows*cols*channels

        # prev pixel
        self.prev_pixel = np.zeros(channels, dtype=np.uint8)
        self.seen_pixels = np.array([np.zeros(channels, dtype=np.uint8) for _ in range(64)])

        self.header_size = 14
        self.end_marker_size = 8

        self.header = self.create_header()
        self.end_marker = np.array([1], dtype=np.uint8)

        # max size of output buffer
        self.max_size = rows * cols * (channels + 1) + self.header_size + self.end_marker_size

        # OUTPUT BYTE ARRAY

        # options 2-bit
        self.QOI_OP_RUN = int("1100000", 2)
        self.QOI_OP_INDEX = int("0000000", 2)
        self.QOI_OP_DIFF = int("0100000", 2)
        # options 8-bit
        self.QOI_OP_CHANNELS = int("1111110", 2)

        # output is pure byte array
        self.bytes = np.zeros(self.max_size, dtype=np.uint8)
        self.bytes[:self.header_size] = self.header
        self.index = self.header_size

        # number of times pixel is equal to the previous pixel
        self.run = 0
    
    # HELPER FUNCTIONS
        
    def intToBytes(self, int32):
        return np.array([int32], dtype=np.uint32).view(np.uint8)

    def create_header(self):
        qoif = np.array([ord(c) for c in "qoif"], dtype=np.uint8)
        width = self.intToBytes(self.cols)
        height = self.intToBytes(self.rows)
        channels = np.array([self.channels], dtype=np.uint8)
        sRGB = np.array([1], dtype=np.uint8)

        # Concatenate the arrays
        header = np.concatenate((qoif, width, height, channels, sRGB))

        return header
    
    def hash_function(self, pixel):
        return sum(pixel) % 64
    
    def write_to_file(self):
        with open("img.bin", "wb") as f:
            f.write(self.bytes[:self.index+1].tobytes())

    def __call__(self):
        for i in range(0, self.last_pixel, self.channels):
            pixel_i = self.buffer[i:i+self.channels]
            if np.array_equal(pixel_i, self.prev_pixel):
                self.run += 1
                if (self.run == 62 or i == self.last_pixel):
                    run_length = self.run-1
                    self.bytes[self.index] = np.array([self.QOI_OP_RUN + run_length], dtype=np.uint8)
                    self.run = 0
                    self.index += 1
            else:
                if (self.run > 0):
                    run_length = self.run-1
                    self.bytes[self.index] = np.array([self.QOI_OP_RUN + run_length], dtype=np.uint8)
                    self.run = 0
                    self.index += 1
                
                hash = self.hash_function(pixel_i)
                if np.array_equal(pixel_i, self.seen_pixels[hash]):
                    self.bytes[self.index] = np.array([self.QOI_OP_INDEX + hash], dtype=np.uint8)
                    self.index += 1
                else:
                    self.seen_pixels[hash] = pixel_i

                    diff = np.absolute(pixel_i - self.prev_pixel)
                    if not np.any(diff > 1):
                        # Use 2 bits for each channel difference
                        byte = self.QOI_OP_DIFF
                        bit_index = 2  # Start index for encoding channel diff bits
                        
                        for i in range(self.self.channels):
                            if bit_index > 7:
                                self.bytes[self.index] = byte
                                self.index += 1
                                byte = 0 
                                bit_index = 0

                            # Encode the difference using 2 bits
                            add = diff[i]
                            byte |= (add & 0b11) << bit_index
                            bit_index += 2  # Move to the next 2 bits
                    
                        if bit_index > 0:
                            self.bytes[self.index] = byte
                            self.index += 1

                    else:
                        self.bytes[self.index] = np.array([self.QOI_OP_CHANNELS], dtype=np.uint8)
                        for i in range(1, self.channels+1):
                            self.bytes[self.index + i] = pixel_i[i-1]

                        self.index += pixel_i.shape[0] + 1
            
            self.prev_pixel = pixel_i

        self.bytes[self.index] = self.end_marker
        
        self.write_to_file()

        print(f"{self.index} and size original {self.buffer.shape[0]}")
    