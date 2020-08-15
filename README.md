# Seam-Carving

Seam carving using dynamic programming.  
  
In broad terms the seam carving algorithm uses horizontal or vertical seams to iteratively reduce the size of the image. 
A seam is a pixel thick cut through the image that stretches either from left to right or from up to down (depending on the orientation of the seam). Removing the seam reduces the image by one unit. 
For example, a horizontal seam will reduce the size of the image by one row, and a vertical seam by one column.    

The main idea is to find the seams that cut through the non-salient content of the image. In the castle example, we want to generate seams that do not remove details form the castle and the person in the image.    

The program has 3 arguments: an input image, width of the new target, height of the new target, output image. This program will retarget the input image to a new image of given sizes and write it into a file denoted by the last argument. The minimum size of the target image is 2x2 pixels.    

Eaxmple:    

![example](https://github.com/dulekang1993/Seam-Carving/blob/master/example.png)  

Reference : Project Description of p2 for COMP 6651 – Winter 2019 by Dr. Tiberiu Popa, Concordia University.
