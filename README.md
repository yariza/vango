# Vango

Converting pictures to painterlings. Woo!

## Setup

This project is split into two main tasks:

1. Image Processing / Brush stroke creation.
    - Load up an image, and run OpenCV operations on it.
    - Output a YAML brushstroke file.
2. Image Rendering
    - Parse the YAML brushstroke file
    - Output a rendered image!!

## File Format

The YAML format that represents the brush layers and colors is used to transmit data between the two tasks. It contains information on the following:

- Canvas
    + size {width, height}
    + style
    + palette
    + layers
        * avg width
        * texture(s)
        * avg length
        * avg opacity
        * brush strokes
            - anchor
            - length
            - width
            - opacity
            - color
            - texture (actual)

TBD.


