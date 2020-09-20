
	/*
	** This program reads bridge.ppm, a 512 x 512 PPM image.
	** It smooths it using a standard 3x3 mean filter.
	** The program also demonstrates how to time a piece of code.
	**
	** To compile, must link using -lrt  (man clock_gettime() function).
	*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

int main ()

{
FILE		*fpt;
unsigned char	*image;
unsigned char	*smoothed;
double *inter, sum1, sum;
char		header[320];
int		ROWS,COLS,BYTES;
int		r,c,r2,c2;
struct timespec	tp1,tp2;

	/* read image */
if ((fpt=fopen("bridge.ppm","r")) == NULL)
  {
  printf("Unable to open bridge.ppm for reading\n");
  exit(0);
  }
fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
if (strcmp(header,"P5") != 0  ||  BYTES != 255)
  {
  printf("Not a greyscale 8-bit PPM image\n");
  exit(0);
  }
image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
header[0]=fgetc(fpt);	/* read white-space character that separates header */
fread(image,1,COLS*ROWS,fpt);
fclose(fpt);

	/* allocate memory for smoothed version of image */
smoothed=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
inter=(double *)calloc(ROWS*COLS,sizeof(double)); // Keeping this value as float to preserve precision of the arithmetic operations
	/* query timer */
clock_gettime(CLOCK_REALTIME,&tp1);
//printf("%ld %ld\n",(long int)tp1.tv_sec,tp1.tv_nsec);

	/* smooth image, skipping the border points */
  // Performing convolution on columns with sliding window implementation
for (r=0; r<ROWS; r++)
{
  sum1 = 0.0;
  for(c=0; c<COLS; c++)
  {
    if (c<3 || c>=(COLS-3))
    { // If the kernel goes outside the image, then make the pixel value zero.
      sum1=0.0;
    } else{ // Otherwise perform the convolution operation
      if (sum1==0.0)
      { //For the initial step do the full convolution operation
        for(c2=-3; c2<=3; c2++)
        {
          sum1+=image[r*COLS+(c+c2)];
        }
      } else { //Subsequently, implement the sliding window
        sum1 = sum1-image[r*COLS+(c-4)]+image[r*COLS+(c+3)];
      }
      inter[r*COLS+c]=sum1/7.0;
    }
  }
}

// Performing convolution on rows with sliding window implementation
for (r=0; r<ROWS; r++)
{
  for(c=0; c<COLS; c++)
  {
    sum = 0;
    if (r<3 || r>=(ROWS-3))
    { // If the kernel goes outside the image, then make the pixel value zero.
      sum1=0.0;
    } else{ // Otherwise perform the convolution operation
      if (sum==0.0)
      { //For the initial step do the full convolution operation
        for(r2=-3; r2<=3; r2++)
        {
          sum+=inter[(r+r2)*COLS+c];
        }
      } else { //Subsequently, implement the sliding window
        sum = sum-inter[(r-4)*COLS+c]+inter[(r+3)*COLS+c];
      }
      int feed = round(sum/7.0); //Rounding off the value to nearest integer so that prescision isn't lost.
      smoothed[r*COLS+c]=feed;
    }
  }
}

	/* query timer */
clock_gettime(CLOCK_REALTIME,&tp2);
//printf("%ld %ld\n",(long int)tp2.tv_sec,tp2.tv_nsec);

	/* report how long it took to smooth */
  printf("%ld\n",tp2.tv_nsec-tp1.tv_nsec);

	/* write out smoothed image to see result */
fpt=fopen("smoothed_sliding.ppm","w");
fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
fwrite(smoothed,COLS*ROWS,1,fpt);
fclose(fpt);
}