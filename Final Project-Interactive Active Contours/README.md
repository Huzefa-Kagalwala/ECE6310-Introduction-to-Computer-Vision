# FINAL PROJECT: Interactive Active Contours
## Submitted by: Huzefa Shabbir Hussain Kagalwala

### Problem Statement:
In this project I had to segment a range image based upon surface normals, using the range image of a chair. Some C-code was also provided to convert the pixels into 3D coordinates. The segmentation process used the image grid for grouping pixels, but used the 3D coordinates for calculating surface normals for region predicates.

Please read `project.pdf` for more details.

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
The folder `Interactive Active Contours` contains the project files for the application. Open MS Visual Studio and load the file `Project.sln` into it to open the project. 

Now build the solution and run it.
