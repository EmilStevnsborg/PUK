import numpy as np
import os
import cv2

def draw_square(image, size, color):
    rows, cols, channels = image.shape
    i = np.random.randint(0, rows - size)
    j = np.random.randint(0, cols - size)
    image[i:(i+size), j:(j+size), :] = color

def draw_circle(image, radius, color):
    rows, cols, channels = image.shape
    ci = np.random.randint(radius, rows - radius)
    cj = np.random.randint(radius, cols - radius)

    for i in range(radius):
        discriminant = radius**2 - i**2
        sqrt_discriminant = int(np.sqrt(discriminant))
        cj1 = cj - sqrt_discriminant
        cj2 = cj + sqrt_discriminant
        image[ci+i:(ci+i+1), cj1:(cj2+1), :] = color
        image[ci-i:(ci-i+1), cj1:(cj2+1), :] = color


def draw_triangle(image, size, color):
    rows, cols, channels = image.shape
    si = np.random.randint(size, rows - size)
    sj = np.random.randint(size, cols - size)

    for i in range(size):
        image[si+i:(si+i+1), (sj-i):(sj+i), :] = color

def gen_image(channels, rows, cols, path):
    image = np.zeros((rows, cols, channels), dtype=np.uint8)
    num_shapes = np.random.randint(30, 80)

    for _ in range(num_shapes):
        shape_type = np.random.choice(["square", "circle", "triangle"])
        shape_color = np.random.randint(50, 200, size=channels).reshape(1, 1, channels)

        if shape_type == "square":
            size = np.random.randint(5, min(rows, cols) // 4)
            draw_square(image, size, shape_color)
        elif shape_type == "circle":
            radius = np.random.randint(5, min(rows, cols) // 8)
            draw_circle(image, radius, shape_color)
        elif shape_type == "triangle":
            size = np.random.randint(5, min(rows, cols) // 4)
            draw_triangle(image, size, shape_color)

    with open(path + ".bin", "wb") as file:
        file.write(image.tobytes())
    
    if channels == 3:
        cv2.imwrite(path + ".png", image.astype(np.uint8))

gen_image(3, 500, 300, "quantitative/cases/1")