import numpy as np
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


def min_max_normalize(image):
    max_ = image.max()
    min_ = image.min()
    return ((image - min_) / (max_ - min_) * 255).astype(np.uint8)