# Seam-Carving
Seam carving using dynamic programming.  
  
In broad terms the seam carving algorithm uses horizontal or vertical seams (Figure 2(red)) to iteratively reduce the size of the image. 
A seam is a pixel thick cut through the image that stretches either from left to right or from up to down (depending on the orientation of the seam). Removing the seam reduces the image by one unit. 
For example, a horizontal seam will reduce the size of the image by one row, and a vertical seam by one column.
