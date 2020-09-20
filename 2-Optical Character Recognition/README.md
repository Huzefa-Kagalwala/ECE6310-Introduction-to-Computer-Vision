# ASSIGNMENT 2: Optical Character Recognition
## Submitted by: Huzefa Shabbir Hussain Kagalwala

### Problem Statement:
In this project I had to implement a matched filter (normalized cross-correlation) to recognize letters in an image of text.
Please read `lab2.pdf` for more details.

### Implementation

**Original Image**:

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/2-Optical%20Character%20Recognition/Data/parenthood.png)

**Normalized Matched Spatial Filter Image**:

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/2-Optical%20Character%20Recognition/Data/normalized_msf.png)

**Terminal output to check file similarity**

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/1-Convolution%2CSeperable%20Filters%20and%20Sliding%20Windows/Images/No_diff.png)

**Conclusion**: It is clear from the above, that all the three different implementations provide the same result. The time it takes to run each implementation is shown in the next section.

### Instructions:
“Huzefa_Kagalwala_Lab2.c” is the code which implements the matched spatial filter and outputs the detections:

   `gcc -o <executable-name> Huzefa_Kagalwala_Lab2.c`.

   Then run it, using the following command: `./executable-name parenthood_e_template.ppm parenthood.ppm parenthood_gt.txt`.


**Note:** Keep the groundtruth data, original image and the template image in the same directory as the file, for the file to run.