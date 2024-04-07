import numpy as np

def intToBytes(int32):
    return np.array([int32], dtype=np.uint32).view(np.uint8)

def create_header(rows, cols, channels):
    qoif = np.array([ord(c) for c in "qoif"], dtype=np.uint8)
    width = intToBytes(cols)
    height = intToBytes(rows)
    colors = np.array([channels], dtype=np.uint8)
    sRGB = np.array([1], dtype=np.uint8)

    # Concatenate the arrays
    header = np.concatenate((qoif, width, height, colors, sRGB))

    return header

def hash_function(pixel):
    return sum(pixel) % 64

def write_to_file(bytes, index):
    with open("img.bin", "wb") as f:
        f.write(bytes[:index+1].tobytes())

def QOI_encode(flat_image, channels, rows, cols):
    BUFFER = flat_image
    buffer_size = rows*cols*channels

    last_pixel = flat_image.shape[0]-channels
    
    prev_pixel = np.zeros(channels, dtype=np.uint8)
    seen_pixels = np.array([np.zeros(channels, dtype=np.uint8) for _ in range(64)])
    
    header_size = 14
    end_marker_size = 8
    header = create_header(rows, cols, channels)
    end_marker = np.array([1], dtype=np.uint8)
    
    max_size = rows * cols * (channels + 1) + header_size + end_marker_size

    # options 2-bit
    QOI_OP_RUN = int("1100000", 2)
    QOI_OP_INDEX = int("0000000", 2)
    QOI_OP_DIFF = int("0100000", 2)
    # options 8-bit
    QOI_OP_CHANNELS = int("1111110", 2)

    # output is pure byte array
    bytes = np.zeros(max_size, dtype=np.uint8)
    bytes[:header_size] = header
    index = header_size

    # number of times pixel is equal to the previous pixel
    run = 0
    for i in range(0, last_pixel, channels):
        pixel_i = BUFFER[i:i+channels]
        if np.array_equal(pixel_i, prev_pixel):
            run += 1
            if (run == 62 or i == last_pixel):
                run_length = run-1
                bytes[index] = QOI_OP_RUN + run_length
                run = 0
                index += 1
        else:
            if (run > 0):
                run_length = run-1
                bytes[index] = QOI_OP_RUN + run_length
                run = 0
                index += 1
            
            hash = hash_function(pixel_i)
            if np.array_equal(pixel_i, seen_pixels[hash]):
                bytes[index] = QOI_OP_INDEX + hash
                index += 1
            else:
                seen_pixels[hash] = pixel_i

                # ALTER TO STORE DIFF AS NON ABSOLUTE
                diff = np.absolute(pixel_i - prev_pixel)
                if not np.any(diff > 1):
                    # Use 2 bits for each channel difference
                    byte = QOI_OP_DIFF
                    bit_index = 2  # Start index for encoding channel diff bits
                    
                    for i in range(channels):
                        if bit_index > 7:
                            bytes[index] = byte
                            index += 1
                            byte = 0 
                            bit_index = 0

                        # Encode the difference using 2 bits
                        add = diff[i]
                        byte |= (add & 0b11) << bit_index
                        bit_index += 2  # Move to the next 2 bits
                
                    if bit_index > 0:
                        bytes[index] = byte
                        index += 1

                else:
                    bytes[index] = QOI_OP_CHANNELS
                    for i in range(1, channels+1):
                        bytes[index + i] = pixel_i[i-1]

                    index += pixel_i.shape[0] + 1
        
        prev_pixel = pixel_i

    bytes[index] = end_marker
    
    write_to_file(bytes, index)

    print(f"{index} and size original {BUFFER.shape[0]}")

    