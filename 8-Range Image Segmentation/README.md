# ASSIGNMENT 7: Motion Tracking
## Submitted by: Huzefa Shabbir Hussain Kagalwala

### Problem Statement:
In this project, I had to implement a program to calculate the motion of an iPhone along and about the principal axes using accelerometers and gyroscopes. The iPhone was moved individually along or about each axis independently for a period of 2-3 seconds. Between each motion, the iPhone was held at rest for 2-3 seconds. The goal of the lab was to automatically segment the data into periods of motion and periods of rest, and calculate the motion along and about each axis during the periods of motion.

Please read `lab7.pdf` for more details.

### Implementation
- Accelerometer data was double integrated to find the linear motion and Gyroscope data was integrated to find the angular motion.
- The data was first smoothened out using a mean filter.
- The variance between data points was used as a metric to differentiate between motion and rest.
- A simple low pass filter also has been implemented to reject any erroneous motions (as the phone was moved by hand, some jitter is bound to have been introduced in the motion data, which also needed to be filtered out.)

For further details, please read the report `Huzefa_Kagalwala_Lab7.pdf`

|Original Image|Sobel Filter Output|
|--------------|-------------------|
|![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/5-Active%20Contours/Data/hawk.png)|![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/5-Active%20Contours/Data/sobel_hawk.png)|

**Graph showing the filtered motion signals**

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/7-Motion%20Tracking/Data/motion_graph.jpg)

**Table of motion**

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/7-Motion%20Tracking/Data/motion_table.jpg)

### Instructions:
“Huzefa_Kagalwala_Lab7.c” is the code which implements the active contours algorithm. Run it using the following commands:

   `gcc -o <executable-name> Huzefa_Kagalwala_Lab7.c -lm`.

   Then run it, using the following command: `./executable-name`.

The following outputs will be obtained:
1. `Rest.txt`. This text file contains the time stamps and indices of the time stamps the phone was at rest.
2. `Motion.txt`. This text file contains the time stamps and indices of the time stamps the phone was in motion, also the motion traversed.

**Note:** Keep the data file `acc_gyro.txt` in the same directory as the code file, for the file to run.