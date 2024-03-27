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
    
    result = min_max_normalize(result).astype(np.uint8)

    return result


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

# gray scale
def grayscale(image):
    intensity_weights = np.array([0.299, 0.587, 0.114])

    image_float = image.astype(np.float32)

    gray_image = np.expand_dims(np.sum(image_float * intensity_weights, axis=2), axis=2)

    return gray_image

# sobel
def sobel(image):
    image_float = image.astype(np.float32)

    Kx = np.array([[-1, 0, 1], [-2, 0, 2], [-1, 0, 1]], np.float32)
    Ky = np.array([[1, 2, 1], [0, 0, 0], [-1, -2, -1]], np.float32)
    
    G = np.zeros_like(image_float, dtype=np.float32)
    theta = np.zeros_like(image_float, dtype=np.float32)
    
    for channel in range(image_float.shape[2]):
        Ix = convolve(image_float[:,:,channel], Kx, mode="reflect")
        Iy = convolve(image_float[:,:,channel], Ky, mode="reflect")
        
        G[:,:,channel] = np.hypot(Ix, Iy)
        
        theta[:,:,channel] = np.arctan2(Iy, Ix)

    return G, theta

def non_max_suppression(image, D):
    M, N, C = image.shape
    Z = np.zeros((M, N, C), dtype=np.int32)
    angle = D * 180. / np.pi
    angle[angle < 0] += 180

    for channel in range(C):
        for i in range(1, M - 1):
            for j in range(1, N - 1):
                try:
                    q = 255
                    r = 255

                    # angle 0
                    if (0 <= angle[i, j] < 22.5) or (157.5 <= angle[i, j] <= 180):
                        q = image[i, j + 1, channel]
                        r = image[i, j - 1, channel]
                    # angle 45
                    elif (22.5 <= angle[i, j] < 67.5):
                        q = image[i + 1, j - 1, channel]
                        r = image[i - 1, j + 1, channel]
                    # angle 90
                    elif (67.5 <= angle[i, j] < 112.5):
                        q = image[i + 1, j, channel]
                        r = image[i - 1, j, channel]
                    # angle 135
                    elif (112.5 <= angle[i, j] < 157.5):
                        q = image[i - 1, j - 1, channel]
                        r = image[i + 1, j + 1, channel]

                    if (image[i, j, channel] >= q) and (image[i, j, channel] >= r):
                        Z[i, j, channel] = image[i, j, channel]
                    else:
                        Z[i, j, channel] = 0

                except IndexError as e:
                    pass

    return Z

def threshold(image, lowThreshold, highThreshold):
    
    M, N, C = image.shape
    res = np.zeros((M, N, C), dtype=np.uint8)
    
    weak = np.uint8(25)
    strong = np.uint8(255)
    
    strong_i, strong_j, strong_k = np.where(image >= highThreshold)
    zeros_i, zeros_j, zeros_k = np.where(image < lowThreshold)
    
    weak_i, weak_j, weak_k = np.where((image <= highThreshold) & (image >= lowThreshold))
    
    res[strong_i, strong_j, strong_k] = strong
    res[weak_i, weak_j, weak_k] = weak
    res[zeros_i, zeros_j, zeros_k] = 0
    
    return (res, weak, strong)

def hysteresis(image, weak, strong):
    M, N, C = image.shape  
    for channel in range(C):
        for i in range(1, M-1):
            for j in range(1, N-1):
                if (image[i,j, channel] == weak):
                    try:
                        if (((image[i+1, j-1, channel] == strong) or 
                             (image[i+1, j, channel] == strong) or 
                             (image[i+1, j+1, channel] == strong) or 
                             (image[i, j-1, channel] == strong) or 
                             (image[i, j+1, channel] == strong) or 
                             (image[i-1, j-1, channel] == strong) or 
                             (image[i-1, j, channel] == strong) or 
                             (image[i-1, j+1, channel] == strong))):
                            image[i, j, channel] = strong
                        else:
                            image[i, j, channel] = 0
                    except IndexError as e:
                        pass
    return image

def min_max_normalize(image):
    max_ = image.max()
    min_ = image.min()
    return (image - min_) / (max_ - min_) * 255

def sobel_min_max(image):
    grads, dir = sobel(image)
    result = min_max_normalize(grads)
    return result.astype(np.uint8)

def canny_edge(image, lowThreshold, highThreshold):
    gray_image = grayscale(image)
    blurred_image = gaussian_blur(gray_image, 5, 5, 0, 0)
    grads, dirs = sobel(blurred_image)

    suppressed_grads = non_max_suppression(grads, dirs)
    res, weak, strong = threshold(suppressed_grads, lowThreshold, highThreshold)
    result = hysteresis(res, weak, strong)

    return result.astype(np.uint8)
    
