# ASSIGNMENT 2: Optical Character Recognition
## Submitted by: Huzefa Shabbir Hussain Kagalwala

### Problem Statement:
In this project I had to implement a matched filter (normalized cross-correlation) to recognize letters in an image of text.
Please read `lab2.pdf` for more details.

### Implementation

This code takes an image **parenthood.ppm** and detects the letters **e** in it using the given template file, **parenthood_e_template.ppm**. The detections are based off a binary image created by applying a certain threshold over the normalized matched spatial filter image formed by convolving the original image and the template.  

After this, the binary image is compared with a groundtruth file and the algorithms' Receiver Operating Curve (ROC) is plotted. The point which is closest to the ideal value of 0 False Positives and 100% True Positives, gives us the value of the best applicable threshold value to create the binary image. 

**Note:** MATLAB r20202a was used to plot the ROC curve and find the ideal point.

**Original Image**:

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/2-Optical%20Character%20Recognition/Data/parenthood.png)

**Normalized Matched Spatial Filter Image**:

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/2-Optical%20Character%20Recognition/Data/normalized_msf.png)

**Comaprison of several binary images at different threshold values**

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/2-Optical%20Character%20Recognition/Data/Collage.png)

**ROC Curve obtained** 

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/2-Optical%20Character%20Recognition/Data/roc.png)


### Instructions:
“Huzefa_Kagalwala_Lab2.c” is the code which implements the matched spatial filter and outputs the detections:

   `gcc -o <executable-name> Huzefa_Kagalwala_Lab2.c`.

   Then run it, using the following command: `./executable-name parenthood_e_template.ppm parenthood.ppm parenthood_gt.txt`.


**Note:** Keep the groundtruth data, original image and the template image in the same directory as the file, for the file to run.