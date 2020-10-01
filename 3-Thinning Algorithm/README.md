# ASSIGNMENT 3: Optical Character Recognition
## Submitted by: Huzefa Shabbir Hussain Kagalwala

### Problem Statement:
In this project, I had to implement a Zhang-Suen thinning algorithm which would help in weeding out false positives in detecting the letter "e" in the image provided. This is project builds up on the previous one to make the optical character recognition more robust.
Please read `lab3.pdf` for more details.

### Implementation

This code takes an image: **parenthood.ppm** and detects the letters **"e"** in it using the given template file, **parenthood_e_template.ppm**. The detections are based off a binary image created by applying a certain threshold over the normalized matched spatial filter image formed by convolving the original image and the template.  

After this, the binary image is compared with a groundtruth file and the algorithms' Receiver Operating Characteristic (ROC) curve is plotted. The point which is closest to the ideal value of 0 False Positives and 100% True Positives, gives us the value of the best applicable threshold value to create the binary image. 

**Note:** MATLAB r2020a was used to plot the ROC curve and find the ideal point.

**Original Image**:

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/3-Thinning%20Algorithm/Data/parenthood.png)

**Normalized Matched Spatial Filter Image**:

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/3-Thinning%20Algorithm/Data/normalized_msf.png)

**Binary image of the letter "e"**

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/3-Thinning%20Algorithm/Data/binary_e.png)

**Thinned image of the letetr "e"**

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/3-Thinning%20Algorithm/Data/thinned_e.png)

**Branchpoints and Endpoints detected in the thinned image**

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/3-Thinning%20Algorithm/Data/marked_e.png)

**ROC Curve obtained** 

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/3-Thinning%20Algorithm/Data/roc.png)

The values for the optimum threshold and their corresponding TP and FP are:
|Threshold|True Positives|False Positives|True Positive Rate|False Positive Rate|True Negatives|False Negatives|
|----|----|-----|-----|-----|-----|-----|
|200|144|25|0.9536|0.0225|1086|7|

We can see that in this ROC curve the False Positive Rate axis has been drastically reduced. This indicates that we have been successful in removing false positives from our OCR algorithm. 

The only drawback in this approach is that some letters may have the same attributes. So, what we consider to be an "e", could be another letter too, thereby not eliminating false positives altogether.

### Instructions:
“Huzefa_Kagalwala_Lab3.c” is the code which implements the matched spatial filter and outputs the detections:

   `gcc -o <executable-name> Huzefa_Kagalwala_Lab3.c`.

   Then run it, using the following command: `./executable-name parenthood.ppm normalized_msf.ppm parenthood_gt.txt`.


**Note:** Keep the groundtruth data, original image and the MSFimage in the same directory as the file, for the file to run.