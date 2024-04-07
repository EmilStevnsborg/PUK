import numpy as np

def hash_function(pixel):
    return sum(pixel) % 64

def QOI_decode(path):
    
    # options 2-bit
    QOI_OP_RUN = int("1100000", 2)
    QOI_OP_INDEX = int("0000000", 2)
    QOI_OP_DIFF = int("0100000", 2)
    # options 8-bit
    QOI_OP_CHANNELS = int("1111110", 2)

    with open(path, "rb") as f:
        binary_data = f.read()
    
    # Extracting header information
    qoif = binary_data[:4]
    width = int.from_bytes(binary_data[4:8], byteorder="little")
    height = int.from_bytes(binary_data[8:12], byteorder="little")
    channels = int.from_bytes(binary_data[12:13], byteorder="little")
    sRGB = int.from_bytes(binary_data[13:14], byteorder="little")

    img_size = width*height*channels
    output_idx = 0
    img = np.zeros(img_size, dtype=np.uint8)

    input_idx = 14

    prev_pixel = np.zeros(channels, dtype=np.uint8)
    seen_pixels = np.array([np.zeros(channels, dtype=np.uint8) for _ in range(64)])

    while (input_idx < len(binary_data) - 8):
        byte = binary_data[input_idx]
        
        if (byte == QOI_OP_CHANNELS):
            for i in range(1,channels+1):
                prev_pixel[i] = binary_data[input_idx+i]
            
            hash = hash_function(prev_pixel)
            seen_pixels[hash] = prev_pixel

            img[output_idx:output_idx+channels+1] = prev_pixel

            output_idx += channels
            input_idx += channels
            break
        
        # check for run by verifying first two bits
        if (byte & 0b11000000 == QOI_OP_RUN):
            # run length encoded in the last 6 bits + bias
            run_length = (byte & 0b00111111) + 1
            for i in range(run_length):
                img[output_idx:output_idx+channels+1] = prev_pixel
                output_idx += channels
            
            input_idx += 1
        
        if (byte & 0b11000000 == QOI_OP_INDEX):
            # index encoded in the last six pixels
            index = byte & 0b00111111
            img[output_idx:output_idx+channels+1] = seen_pixels[index]
            output_idx += channels
            input_idx += 1
        
        if (byte & 0b11000000 == QOI_OP_DIFF):
            pixel = np.zeros(channels, np.uint8)
            bit_idx = 2
            for i in range(channels):
                if bit_idx == 2:
                    mask = 0b00110000
                if bit_idx == 4:
                    mask = 0b00001100
                if bit_idx == 6:
                    mask = 0b00000011
                if bit_idx == 8:
                    mask = 0b11000000
                    bit_idx = 2
                    input_idx += 1
                
        

    print(img)
