
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
char		header[320];
int		ROWS,COLS,BYTES;
int		r,c,r2,c2;
struct timespec	tp1,tp2;
double sum;

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
fread(image,sizeof(unsigned char),COLS*ROWS,fpt);
fclose(fpt);

	/* allocate memory for smoothed version of image */
smoothed=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));

	/* query timer */
clock_gettime(CLOCK_REALTIME,&tp1);

	/* smooth image, skipping the border points */
for (r=0; r<ROWS; r++)
  for (c=0; c<COLS; c++)
    {
      sum = 0.0; //Initialize value to zero
      if ( r<3 || c<3 || r>=(ROWS-3) || c>=(COLS-3) ){ // If the kernel goes outside the image, then make the pixel value zero.
        sum = 0.0;
      } else { // Otherwise perform the convolution operation
        for (r2=-3; r2<=3; r2++)
          for (c2=-3; c2<=3; c2++) {
            sum+=image[(r+r2)*COLS+(c+c2)];
          }
          int feed = round(sum/49.0); //Round off the average value of the convolution to preserve precision
          smoothed[r*COLS+c]=feed;    // Feed the value to the new image
      }
    }

	/* query timer */
clock_gettime(CLOCK_REALTIME,&tp2);
//printf("%ld %ld\n",(long int)tp2.tv_sec,tp2.tv_nsec);

	/* report how long it took to smooth */
  printf("%ld\n",tp2.tv_nsec-tp1.tv_nsec);

	/* write out smoothed image to see result */
fpt=fopen("smoothed.ppm","w");
fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
fwrite(smoothed,COLS*ROWS,sizeof(unsigned char),fpt);
fclose(fpt);
}