# ASSIGNMENT 5: Active Contours
## Submitted by: Huzefa Shabbir Hussain Kagalwala

### Problem Statement:
In this project, I had to implement a program for active contours, which is a form of semi-automated image segmentation. An initial set of contour points is provided, which is then moved towards the goal location by a set of driving forces or "energies". The rubber band model of active contours have been implemented here. In this model, a set of points is drawn outside the object of interest, which will then move towards the object and envelope it.
Please read `lab5.pdf` for more details.

### Implementation
The rubber band model of active contours is based on 3 energies or driving forces. Two of these are **internal energies**, which are dependent on the location of the points relative to one another and have no relation with the image and the other **external energy** term is dependent on the gradients of the image, so that points can lock in on an edge.

The contour points are moved in a 7x7 window for 30 iterations. For every contour point, the 3 energies are calculated, and the pixel with the least overall energy in the window is chosen as the new location for the contour point.

**Original Image with initial contour points**:

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/5-Active%20Contours/Data/marked_hawk_initial.png)

**Output of the Sobel Filter**:

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/5-Active%20Contours/Data/sobel_hawk.png)

**Output of the Active Contours Program**

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/5-Active%20Contours/Data/marked_hawk_final.png)

**Animation of the working of the Active COntours Program**

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/5-Active%20Contours/Data/animation.gif)

### Instructions:
“Huzefa_Kagalwala_Lab5.c” is the code which implements the active contours algorithm. Run it using the following commands:

   `gcc -o <executable-name> Huzefa_Kagalwala_Lab5.c -lm`.

   Then run it, using the following command: `./executable-name hawk.ppm normalized_msf.ppm hawk_init.txt`.

The following outputs will be obtained:
1. `sobel_hawk.ppm`. This is the sobel filtered image of the input image.
2. `marked_hawk_initial`. This is the image with the initial contour points marked on it.
3. `marked_hawk_final`. This is the image with the final contour points.
4. `hawk_final.txt`. This text file has the locations of the final contour points.


**Note:** Keep the intial contour points and original image in the same directory as the file, for the file to run.