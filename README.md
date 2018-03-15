# Description

This program implements the basic functionalities of a simple image
editing software (such as Microsoft Paint), for a PPM-style image
format, including crop, resize, color region, blur, rotate and fill.

# Image format

The image is represented through a matrix of pixels, where a pixel
is represented by three bytes (ranging from 0 to 255) according to
the RGB (Red, Green, Blue) model.

The equivalent PPM format is as follows:
```
P3
width height
255
byte matrix of size 3 * width x height
```
For example:
```
P3
2 3
255
0 0 255 0 255 0
255 0 0 128 0 128
255 255 255 0 0 0
```
# Usage

Commands are given as their respective code (opCode) followed by the
right parameters, as follows:
```
opCode [p1 p2 p3...]
```

## Initialize image (opCode: 1)
### Format
```
1 width height
byte matrix of size 3 * width x height
```
### Example
```
2 3
0 0 255 0 255 0
255 0 0 128 0 128
255 255 255 0 0 0
```
### Parameter properties
- 1 ≤ height ≤ 1024
- 1 ≤ width ≤ 1024
- 0 ≤ r, g, b ≤ 255 

## Crop (opCode: 2)
The current image is overwritten by the image delimited by the rectangle
between the given coordinates.
### Format
```
2 start_col start_line end_col end_line
```
### Example
###### Original image
```
(0, 0, 255) (0, 255, 0) (255, 0, 0)
(0, 128, 0) (0, 255, 0) (0, 128, 0)
```
###### Command
```
2 0 0 1 0
```
##### Obtained image
```
(0, 0, 255) (0, 255, 0)
```
### Parameter properties
- 0 ≤ start_line ≤ end_line < current image height
- 0 ≤ start_col ≤ end_col < current image width 

## Resize (opCode: 3)
Modifies the dimensions of the image. If one dimension is larger than the
original image, the new lines/columns will be filled by white pixels. If
the dimensions are smaller, the operation is equivalent with a crop
instruction.
### Format
```
3 width height
```
### Example
###### Original image
```
(0, 0, 255) (0, 255, 0) (255, 0, 0)
(0, 128, 0) (0, 255, 0) (0, 128, 0)
```
###### Command
```
3 1 3
```
##### Obtained image
```
(0, 0, 255) 
(0, 128, 0)
(255, 255, 255)
```
### Parameter properties
- 1 ≤ height ≤ 1024
- 1 ≤ width ≤ 1024 

## Color region (opCode: 4)
The rectangle delimited by the given coordinates will be colored with
(r g b) pixels.
### Format
```
4 start_col start_line end_col end_line r g b
```
### Example
###### Original image
```
(0, 0, 255) (0, 255, 0) (255, 0, 0)
(0, 128, 0) (0, 255, 0) (0, 128, 0)
```
###### Command
```
4 1 0 2 1 100 150 200
```
##### Obtained image
```
(0, 0, 255) (100, 150, 200) (100, 150, 200)
(0, 128, 0) (100, 150, 200) (100, 150, 200)
```
### Parameter properties
- 0 ≤ start_line ≤ end_line < Înaltimea curentă a imaginii
- 0 ≤ start_col ≤ end_col < Lățimea curentă a imaginii
- 0 ≤ r, g, b ≤ 255
- 
## Blur (opCode: 5)
The blur effect implies replacing each pixel with the average of its
neighbors (up, down, left, right). This operation will be repeated
num_iter times.
### Format
```
5 num_iter
```
### Example
###### Original image
```
(0, 0, 255) (0, 255, 0) (255, 0, 0)
(0, 128, 0) (0, 255, 0) (0, 128, 0)
```
###### Command
```
5 1
```
##### Obtained image
```
(0, 191, 0) (85, 85, 85) (0, 191, 0)
(0, 127, 127) (0, 170, 0) (127, 127, 0) 
```
### Parameter properties
-  0 ≤ num_iter ≤ 2000

## Rotate (opCode: 6)
The image is rotated num_rot times by 90 degrees.
### Format
```
6 num_rot
```
### Example
###### Original image
```
(0, 0, 255) (0, 255, 0) (255, 0, 0)
(0, 128, 0) (0, 255, 0) (0, 128, 0)
```
###### Command
```
6 1
```
##### Obtained image
```
(0, 128, 0) (0, 0, 255) 
(0, 255, 0) (0, 255, 0)
(0, 128, 0) (255, 0, 0)
```
### Parameter properties
-  0 ≤ num_iter ≤ 3

## Fill (opCode: 7)
The pixel at (start_line, start_col) will be overwritten by (r, g, b)
pixels. The operation is repeated recursively for all its neighbors that
have the same color as the original pixel at (start_line, start_col).
### Format
```
7 start_col start_line r g b
```
### Example
###### Original image
```
(0, 0, 255) (10, 10, 10) (255, 0, 0)    (10, 10, 10)
(10, 10, 10) (0, 255, 0) (10, 10, 10)  (10, 10, 10)
(10, 10, 10) (10, 10, 10) (10, 10, 10) (10, 15, 10)
```
###### Command
```
7 2 1 42 42 42
```
##### Obtained image
```
(0, 0, 255)  (10, 10, 10)  (255, 0, 0)  (42, 42, 42)
(42, 42, 42) (0, 255, 0)  (42, 42, 42) (42, 42, 42)
(42, 42, 42) (42, 42, 42) (42, 42, 42) (10, 15, 10)
```
### Parameter properties
- 0 ≤ start_line < current image height
- 0 ≤ start_col < current image width
- 0 ≤ r, g, b ≤ 255 


## Print (opCode: 8)
The image is printed with the known format.
### Format
```
8
```
### Example
###### Original image
```
(0, 0, 255) (0, 255, 0) (255, 0, 0)
(0, 128, 0) (0, 255, 0) (0, 128, 0)
```
###### Printed message
```
3 2
0 0 255 0 255 0 255 0 0
0 128 0 0 255 0 0 128 0
```

## Exit (opCode: 0)
The program exits successfully.
### Format
```
0
```