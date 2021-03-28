# ASSIGNMENT 8: Range Image Segmentation
## Submitted by: Huzefa Shabbir Hussain Kagalwala

### Problem Statement:
In this project I had to segment a range image based upon surface normals, using the range image of a chair. Some C-code was also provided to convert the pixels into 3D coordinates. The segmentation process used the image grid for grouping pixels, but used the 3D coordinates for calculating surface normals for region predicates.

Please read `lab8.pdf` for more details.

### Implementation:
- The `odetics-to-coords.c` file was used to convert the range data at each pixel into 3D coordinates. This script used the model of the laser scanner used to perform this conversion.
- The range image was thresholded to remove the background.
- Surface normals were calculated between points lying within 3 units.
- Region growing was performed based on predicates, which can be read about further in the report `Huzefa_Kagalwala_Lab8.pdf`


|Range Image|Reflectance Image|
|--------------|-------------------|
|![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/8-Range%20Image%20Segmentation/Data/chair-range.png)|![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/8-Range%20Image%20Segmentation/Data/chair-reflectance.png)|

**Surface Normals**

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/8-Range%20Image%20Segmentation/Data/surface-normals.png)

**Region Growing Output**

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/8-Range%20Image%20Segmentation/Data/seg_annot.png)

### Instructions:
“Huzefa_Kagalwala_Lab8.c” is the code which implements the range image segmentation algorithm. Run it using the following commands:

   `gcc -o <executable-name> Huzefa_Kagalwala_Lab8.c -lm`.

   Then run it, using the following command: `./executable-name chair-range.ppm chair-reflectance.ppm`.

The following outputs will be obtained:
1. `Output.csv`. This file contains the 3D coordinates of the range data and the components of the surface normals calculated.
2. `threshold.ppm`. The thresholded image containing only the pixels to be considered for region growing.
3. `seg.ppm`. The image file which shows the segments grown.

**Note:** Keep the image files in the same directory as the code file, for the file to run.