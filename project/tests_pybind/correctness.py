import EmbeddedCV
import cv2
import numpy as np
from functions import gaussian_blur, median_blur, canny_edge, sobel, min_max_normalize
from quantitative.generator import gen_image
import pandas as pd

def check_correctness(y_pred, y):
    diff = y_pred.astype(np.int32) - y.astype(np.int32)
    abs_diff = np.abs(diff)
    max_diff = np.max(abs_diff)
    avg_diff = np.mean(abs_diff)
    pixel_diff = np.sum(abs_diff > 1)
    indices = np.where(abs_diff > 1)

    return [max_diff, avg_diff, pixel_diff/abs_diff.size]


def correctness_test(n, channels, rows, cols):
    host = EmbeddedCV.Host()
    cam = EmbeddedCV.CameraSim(True, channels, rows, cols)
    host.Configure(cam)

    results = []
    
    for i in range(n):
        image = gen_image(channels, rows, cols)
        flat_image = image.flatten()
        cam.StoreData(flat_image)

        # QOI

        QOI_host_output = np.zeros(shape=((channels+1)*rows*cols+22), dtype=np.uint8)
        host.QOIencode(QOI_host_output)
        QOI_decoded = np.zeros(shape=(channels*rows*cols), dtype=np.uint8)
        EmbeddedCV.Host.QOIdecode(QOI_host_output, QOI_decoded)
        QOI_decoded_image = QOI_decoded.reshape(rows, cols, channels)
        QOI_stats = check_correctness(y_pred=QOI_decoded_image, 
                                      y=image)
        
        results.append({
            "case" : i,
            "Algorithm": "QOI",
            "Max Diff": QOI_stats[0],
            "Avg Diff": QOI_stats[1],
            "Pixel Diff": QOI_stats[2]
        })
        
        # median blur

        med_host_output = np.zeros(shape=(channels*rows*cols), dtype=np.uint16)
        host.MedianBlur(med_host_output, 3, 3)
        med_host_output = med_host_output.reshape((rows, cols, channels))
        med_naive_output = median_blur(image, 3, 3)
        med_stats = check_correctness(y_pred=min_max_normalize(med_host_output), 
                                      y=min_max_normalize(med_naive_output))
        
        results.append({
            "case" : i,
            "Algorithm": "median_blur",
            "Max Diff": med_stats[0],
            "Avg Diff": med_stats[1],
            "Pixel Diff": med_stats[2]
        })

        # Gaussian blur

        kernel_height, kernel_width = 5, 5
        sigma_x, sigma_y = 1, 1
        gauss_host_output = np.zeros(shape=(channels*rows*cols), dtype=np.uint8)
        host.GaussianBlur(gauss_host_output, 
                          kernel_height, kernel_width, 
                          sigma_x, sigma_y)
        gauss_host_output = gauss_host_output.reshape((rows, cols, channels))
        gauss_naive_output = gaussian_blur(image, kernel_height, kernel_width, sigma_x, sigma_y)
        gauss_stats = check_correctness(y_pred=gauss_host_output, y=gauss_naive_output)

        results.append({
            "case" : i,
            "Algorithm": "gaussian_blur",
            "Max Diff": gauss_stats[0],
            "Avg Diff": gauss_stats[1],
            "Pixel Diff": gauss_stats[2]
        })

        # Sobel

        host_output = np.zeros(shape=(channels*rows*cols), dtype=np.uint16)
        host.Sobel(host_output)
        host_output = host_output.reshape((rows, cols, channels))
        naive_output = sobel(image)
        sobel_stats = check_correctness(y_pred=min_max_normalize(host_output), 
                          y=min_max_normalize(naive_output))
        
        results.append({
            "case" : i,
            "Algorithm": "sobel",
            "Max Diff": sobel_stats[0],
            "Avg Diff": sobel_stats[1],
            "Pixel Diff": sobel_stats[2]
        })

        # Canny

        low_threshold = 40
        high_threshold = 60
        canny_host_output = np.zeros(shape=(1*rows*cols), dtype=np.uint8)
        host.CannyEdge(canny_host_output, low_threshold, high_threshold)
        canny_host_output = canny_host_output.reshape((rows, cols, 1))
        
        canny_naive_output = canny_edge(image, 
                                        low_threshold, 
                                        high_threshold).reshape((rows, cols, 1))
        
        canny_stats = check_correctness(y_pred=canny_host_output, y=canny_naive_output)
        
        results.append({
            "case" : i,
            "Algorithm": "canny_edge",
            "Max Diff": canny_stats[0],
            "Avg Diff": canny_stats[1],
            "Pixel Diff": canny_stats[2]
        })
    
    df = pd.DataFrame(results)
    df.to_csv("correctness_test.csv", index=False)

def correctness_QOI_test(n, channels, rows, cols):
    host = EmbeddedCV.Host()
    cam = EmbeddedCV.CameraSim(True, channels, rows, cols)
    host.Configure(cam)

    results = []
    
    for i in range(n):
        image = gen_image(channels, rows, cols)
        flat_image = image.flatten()
        cam.StoreData(flat_image)

        # Gaussian blur

        kernel_height, kernel_width = 5, 5
        sigma_x, sigma_y = 1, 1
        gauss_host_output = np.zeros(shape=(channels*rows*cols), dtype=np.uint8)
        host.GaussianBlur(gauss_host_output, 
                          kernel_height, kernel_width, 
                          sigma_x, sigma_y)
        gauss_host_output = gauss_host_output.reshape((rows, cols, channels))

        gauss_QOI_output = np.zeros(shape=((channels+1)*rows*cols+22), dtype=np.uint8)
        host.GaussianBlur(gauss_QOI_output, 
                          kernel_height, kernel_width, 
                          sigma_x, sigma_y, "QOI")
        QOI_decoded = np.zeros(shape=(channels*rows*cols), dtype=np.uint8)
        EmbeddedCV.Host.QOIdecode(gauss_QOI_output, QOI_decoded)
        QOI_decoded_image = QOI_decoded.reshape(rows, cols, channels)

        gauss_stats = check_correctness(y_pred=QOI_decoded_image, y=gauss_host_output)

        results.append({
            "case" : i,
            "Algorithm": "gaussian_blur",
            "Max Diff": gauss_stats[0],
            "Avg Diff": gauss_stats[1],
            "Pixel Diff": gauss_stats[2]
        })

        # Median blur

        med_host_output = np.zeros(shape=(channels*rows*cols), dtype=np.uint16)
        host.MedianBlur(med_host_output, 3, 3)
        med_host_output = med_host_output.reshape((rows, cols, channels))
        
        med_QOI_output = np.zeros(shape=((channels+1)*rows*cols+22), dtype=np.uint8)
        host.MedianBlur(med_QOI_output, 3, 3, "QOI")
        QOI_decoded = np.zeros(shape=(channels*rows*cols), dtype=np.uint8)
        EmbeddedCV.Host.QOIdecode(med_QOI_output, QOI_decoded)
        QOI_decoded_image = QOI_decoded.reshape(rows, cols, channels)

        med_stats = check_correctness(y_pred=QOI_decoded_image, 
                                        y=min_max_normalize(med_host_output))
        
        results.append({
            "case" : i,
            "Algorithm": "median_blur",
            "Max Diff": med_stats[0],
            "Avg Diff": med_stats[1],
            "Pixel Diff": med_stats[2]
        })

        # sobel

        sobel_host_output = np.zeros(shape=(channels*rows*cols), dtype=np.uint16)
        host.Sobel(sobel_host_output)
        sobel_host_output = sobel_host_output.reshape((rows, cols, channels))
        
        sobel_QOI_output = np.zeros(shape=((channels+1)*rows*cols+22), dtype=np.uint8)
        host.Sobel(sobel_QOI_output, "QOI")
        QOI_decoded = np.zeros(shape=(channels*rows*cols), dtype=np.uint8)
        EmbeddedCV.Host.QOIdecode(sobel_QOI_output, QOI_decoded)
        QOI_decoded_image = QOI_decoded.reshape(rows, cols, channels)

        sobel_stats = check_correctness(y_pred=QOI_decoded_image, 
                                        y=min_max_normalize(sobel_host_output))
        
        results.append({
            "case" : i,
            "Algorithm": "sobel",
            "Max Diff": sobel_stats[0],
            "Avg Diff": sobel_stats[1],
            "Pixel Diff": sobel_stats[2]
        })

        # Canny

        low_threshold = 40
        high_threshold = 60
        canny_host_output = np.zeros(shape=(1*rows*cols), dtype=np.uint8)
        host.CannyEdge(canny_host_output, low_threshold, high_threshold)
        canny_host_output = canny_host_output.reshape((rows, cols, 1))
        
        canny_QOI_output = np.zeros(shape=((1+1)*rows*cols+22), dtype=np.uint8)
        host.CannyEdge(canny_QOI_output, low_threshold, high_threshold, "QOI")
        QOI_decoded = np.zeros(shape=(1*rows*cols), dtype=np.uint8)
        EmbeddedCV.Host.QOIdecode(canny_QOI_output, QOI_decoded)
        QOI_decoded_image = QOI_decoded.reshape(rows, cols, 1)

        canny_stats = check_correctness(y_pred=QOI_decoded_image, y=canny_host_output)
        
        results.append({
            "case" : i,
            "Algorithm": "canny_edge",
            "Max Diff": canny_stats[0],
            "Avg Diff": canny_stats[1],
            "Pixel Diff": canny_stats[2]
        })
    
    df = pd.DataFrame(results)
    df.to_csv("correctness_QOI_test.csv", index=False)