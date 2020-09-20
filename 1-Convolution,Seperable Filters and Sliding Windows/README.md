# ASSIGNMENT 1: Convolutions, Seperable Filters and Sliding Windows
## Submitted by: Huzefa Shabbir Hussain Kagalwala

### Problem Statement:
In this project I had to implement three versions of a 7x7 mean filter. The first version used basic 2D convolution. The second version used separable filters (1x7 and 7x1). The third version used separable filters and a sliding window.
Please read `lab1.pdf` for more details.

### Similarity betwen images

**Original Image**:
![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/1-Convolution%2CSeperable%20Filters%20and%20Sliding%20Windows/Images/smoothed.png)

**2D convolution smoothing**:
![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/1-Convolution%2CSeperable%20Filters%20and%20Sliding%20Windows/Images/smoothed.png)

**Seperable filter smoothing**:
**Seperable filter and Sliding window smoothing**:
**Terminal output to check file similarity**
**Conclusion**: It is clear from the above, that all the three different implementations provide the same result. The time it takes to run each implementation is shown in the next section.

### Timing Data

| Filter      | 2D Convolution | Seperable Filter | Sliding window + Separable Filter |
| ----------- | -----------    |   -----------    | -----------                       |
| **Average over 10 runs** | 23.45087 ms   |   11.92792 ms    | 5.36834 ms                       |

As can be seen, the sliding window combined with separable filter takes just 5.37 milliseconds to smoothen out the image. This is due to the lesser time complexity of the fewer nests of the for loop as well as lesser amount of arithmetic calculations of the sliding window.

### Instructions:
1. “C48290423_smoothen.c” is the code which implements the basic 2D convolution kernel and outputs the image “smoothed.ppm”. To run this first compile the c-file by using the following command:

`gcc -o <executable name> C48290423_smoothen.c`.

Then run it, using the following command: `./executable name`.
2. “C48290423_smoothen_seperable.c” implements the separable filter and outputs the image “smoothed_sep.ppm”. To run this first compile the c-file by using the following command:

`gcc -o <executable name> C48290423_smoothen_seperable.c`.

Then run it, using the following command: `./executable name`.

3. “C48290423_smoothen_sliding.c” implements the sliding window and separable filter, and outputs the image “smoothed_sliding.ppm”. To run this first compile the c-file by using the following command:

`gcc -o <executable name> C48290423_smoothen_sliding.c`.

Then run it, using the following command: `./executable name`.

**Note:** Keep the `bridge.ppm` in the same directory as the file, for the file to run.