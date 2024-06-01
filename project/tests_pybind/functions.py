import numpy as np
import cv2
from scipy.ndimage import convolve

# median blur
def median_blur(image, kernel_height, kernel_width):
    image_float = image.astype(np.float32)
    result = np.zeros_like(image, dtype=np.float32)
    kernel = np.ones((kernel_height, kernel_width), dtype=np.float32)
    for channel in range(image.shape[2]):
        result[:, :, channel] = convolve(image_float[:, :, channel], 
                                         kernel, mode="reflect")

    return result.astype(np.uint16)


# gaussian blur
def gaussian_blur(image, kernel_height, kernel_width, sigma_x, sigma_y):
    epsilon = 1e-5

    if sigma_y < epsilon and sigma_x < epsilon:
        sigma_x = 0.3 * ((kernel_width - 1) * 0.5 - 1) + 0.8
        sigma_y = 0.3 * ((kernel_height - 1) * 0.5 - 1) + 0.8
    elif sigma_y < epsilon:
        sigma_y = sigma_x

    center_x = kernel_width // 2
    center_y = kernel_height // 2

    kernel = np.zeros((kernel_height, kernel_width), dtype=np.float32)

    sum_ = 0

    for i in range(kernel_height):
        for j in range(kernel_width):
            x = j - center_x
            y = i - center_y
            value = np.exp((-(x * x / (2 * sigma_x * sigma_x) + 
                              y * y / (2 * sigma_y * sigma_y))))
            sum_ += value
            kernel[i][j] = value

    kernel /= sum_

    # Apply convolution to each channel separately
    result = np.zeros_like(image, dtype=np.float32)
    for channel in range(image.shape[2]):
        result[:, :, channel] = convolve(image[:, :, channel], 
                                         kernel, mode="reflect")

    return result.astype(np.uint8)


def sobel(image):
    x = cv2.Sobel(image.astype(np.uint16),
                    cv2.CV_64F, 1, 0, 3)
    y = cv2.Sobel(image.astype(np.uint16),
                    cv2.CV_64F, 0, 1, 3)
    
    return (np.sqrt(x**2 + y**2)).astype(np.uint16)

def canny_edge(image, low_threshold, high_threshold):
    gray = np.expand_dims(cv2.cvtColor(image[:,:,:3], cv2.COLOR_BGR2GRAY), axis=-1)
    
    blurred = gaussian_blur(gray, 5, 5, 0, 0)
    
    x = cv2.Sobel(blurred.astype(np.uint16),
                    cv2.CV_32F, 1, 0, 3)
    y = cv2.Sobel(blurred.astype(np.uint16),
                    cv2.CV_32F, 0, 1, 3)
    grads = (np.sqrt(x**2 + y**2))
    degrees = (np.arctan2(y, x) * (180 / np.pi))
    degrees[degrees < 0] += 180
    
    # non max supression
    R, C = grads.shape
    nmx = np.zeros((R,C), dtype=np.uint16)
    for i in range(R):
        for j in range(C):
            q = 0
            r = 0

            #degrees 0
            if (0 <= degrees[i,j] < 22.5):
                q = grads[i, reflect_index(j+1, C)]
                r = grads[i, reflect_index(j-1, C)]
            #degrees 45
            elif (22.5 <= degrees[i,j] < 67.5):
                q = grads[reflect_index(i-1, R), reflect_index(j-1, C)]
                r = grads[reflect_index(i+1, R), reflect_index(j+1, C)]
            #degrees 90
            elif (67.5 <= degrees[i,j] < 112.5):
                q = grads[reflect_index(i+1, R), j]
                r = grads[reflect_index(i-1, R), j]
            #degrees 135
            elif (112.5 <= degrees[i,j] < 157.5):
                q = grads[reflect_index(i+1, R), reflect_index(j-1, C)]
                r = grads[reflect_index(i-1, R), reflect_index(j+1, C)]
            else:
                q = grads[i, reflect_index(j+1, C)]
                r = grads[i, reflect_index(j-1, C)]

            if (q > grads[i,j]) or (r > grads[i,j]):
                nmx[i,j] = 0
            else: 
                nmx[i,j] = np.floor(grads[i,j]).astype(np.uint16)

    # # hysterisis
    # for i in range(R):
    #     for j in range(C):
    #         if nmx[i,j] > high_threshold:
    #             nmx[i,j] = 255
    #         elif nmx[i,j] < low_threshold:
    #             nmx[i,j] = 0
    #         else:
    #             start_i = max(i-1, 0)
    #             start_j = max(j-1, 0)
    #             if np.any(nmx[start_i:i+2, start_j:j+2] > high_threshold):
    #                 nmx[i, j] = 255
    #             else:
    #                 nmx[i, j] = 0
    

    return nmx.astype(np.uint8)

def min_max_normalize(image):
    max_ = image.max()
    min_ = image.min()
    return ((image - min_) / (max_ - min_) * 255).astype(np.uint8)

def reflect_index(i, max_idx):
    i_new = i
    if i < 0:
        i_new = -i - 1
    elif i >= max_idx:
        i_new = 2 * max_idx - 1 - i
    return i_new
