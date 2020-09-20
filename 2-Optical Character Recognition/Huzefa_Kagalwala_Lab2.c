/* This program is written for the ECE6310-Intro to Computer Vision course, Lab 2.
  ** Submitted by Huzefa Kagalwala (C48290423) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[])
{
  FILE *fpt_temp, *fpt_image, *fpt_gt, *fpt;
  unsigned char *image, *image_temp, *msf, *binary;
  char header_image[80], header_temp[80], template, temp_letter;
  int ROWS_image, COLS_image, BYTES_image, ROWS_temp, COLS_temp, BYTES_temp;
  int row, col, i, m, r, c, r2, c2, sum = 0, tp, tn, fp, fn, det, not_det, tp_array[256], fp_array[256], fn_array[256], tn_array[256], t_array[256];
  double avg = 0.0, *mz_temp, *prelim_msf, max, min, dummy, tpr, fpr, fpr_array[256], tpr_array[256];

  if (argc != 4)
  {
    printf("Usage: HW2 [template] [image] [groundtruth]\n");
    exit(0);
  }

  // Opening the image and reading it into a variable
  fpt_image = fopen(argv[2],"rb");
  if (fpt_image == NULL)
  {
    printf("Unable to open parenthood.ppm for opening\n");
    exit(0);
  }
  i=fscanf(fpt_image,"%s %d %d %d ",header_image,&COLS_image,&ROWS_image,&BYTES_image);
  if (i != 4  ||  strcmp(header_image,"P5") != 0  ||  BYTES_image != 255)
  {
    printf("%s is not an 8-bit PPM greyscale (P5) image\n",argv[2]);
    fclose(fpt_image);
    exit(0);
  }
  image = (unsigned char *)calloc(ROWS_image*COLS_image, sizeof(unsigned char));
  if (image == NULL)
  {
  printf("Unable to allocate %d x %d memory\n",COLS_image,ROWS_image);
  exit(0);
  }
  fread(image, sizeof(unsigned char), ROWS_image*COLS_image, fpt_image);
  fclose(fpt_image);

  // Opening the template and reading it into a file
  fpt_temp = fopen(argv[1],"rb");
  if (fpt_temp == NULL)
  {
    printf("Unable to open parenthood_e_template.ppm for opening\n");
    exit(0);
  }
  i=fscanf(fpt_temp,"%s %d %d %d ",header_temp,&COLS_temp,&ROWS_temp,&BYTES_temp);
  if (i != 4  ||  strcmp(header_temp,"P5") != 0  ||  BYTES_temp != 255)
  {
    printf("%s is not an 8-bit PPM greyscale (P5) image\n",argv[1]);
    fclose(fpt_image);
    exit(0);
  }
  image_temp = (unsigned char *)calloc(ROWS_temp*COLS_temp, sizeof(unsigned char));
  if (image_temp == NULL)
  {
  printf("Unable to allocate %d x %d memory\n",COLS_temp,ROWS_temp);
  exit(0);
  }
  fread(image_temp, sizeof(unsigned char), ROWS_temp*COLS_temp, fpt_temp);
  fclose(fpt_temp);

  // Calculating the normalized MSF Image:
  // STEP 1: Calculating the zero mean centred template
  
  for (r = 0; r < ROWS_temp; r++)
  {
    for (c = 0; c < COLS_temp; c++)
    {
      sum = sum + image_temp[(r*COLS_temp)+c];
    }
  }
  double total = ROWS_temp*COLS_temp;
  avg = sum/total;
  mz_temp = (double *)calloc(ROWS_temp*COLS_temp, sizeof(double));
  for (r = 0; r < ROWS_temp; r++)
  {
    for (c = 0; c < COLS_temp; c++)
    {
      mz_temp[(r*COLS_temp)+c] = image_temp[(r*COLS_temp)+c] - avg;
    }
  }

  //STEP 2: Convolving the image with the zero mean centred template
  prelim_msf = (double *)calloc(ROWS_image*COLS_image, sizeof(double));
  int row_lim = ROWS_temp/2; int col_lim = COLS_temp/2;
  for (r = 0; r < ROWS_image; r++)
  {
    for ( c = 0; c < COLS_image; c++)
    {
      dummy = 0.0;
      // Handling edge cases by setting those pixels to zero
      if ( r<row_lim || c<col_lim || r>=(ROWS_image-row_lim) || c>=(COLS_image-col_lim) ){
        prelim_msf[r*COLS_image+c] = 0.0;
      }
      else{
        for (r2 = -row_lim; r2 <= row_lim; r2++)
        {
          for (c2 = -col_lim; c2 <= col_lim; c2++)
          {
            // Performing the convolution
            dummy = dummy + (image[(r+r2)*COLS_image+(c+c2)]*mz_temp[(r2+row_lim)*COLS_temp+(c2+col_lim)]);
          }
        }
        prelim_msf[r*COLS_image+c] = dummy;
      }
    }
  }

  // STEP 3: Normalizing the image
  max = 0; min = 99999999;
  for (int j = 0; j < ROWS_image*COLS_image; j++)
  {
    if (prelim_msf[j] > max)
    {
      max = prelim_msf[j];
    }
    if (prelim_msf[j] < min)
    {
      min = prelim_msf[j];
    }
  }
  float factor = max - min; float feed = 0;
  msf = (unsigned char *)calloc(ROWS_image*COLS_image, sizeof(unsigned char));
  for (r = 0; r < ROWS_image; r++)
  {
    for (c = 0; c < COLS_image; c++)
    {
      //This condition just ensures that the pixels omitted in the edge case handling aren't normalized.
      if (r<row_lim || c<col_lim || r>=(ROWS_image-row_lim) || c>=(COLS_image-col_lim)){
        msf[r*COLS_image+c] = 0;
      }
      else{
        // Normalizing the image
        feed = (255*(prelim_msf[r*COLS_image+c]-min))/(factor);
        int f = round(feed); // Rounding at the last ensures precision isn't lost in the arithmetic operations
        msf[r*COLS_image+c] = f;
      }
    }
  }
  // Writing the normalized MSF image into an 8-bit grayscale image
  fpt=fopen("normalized_msf.ppm","wb");
  fprintf(fpt,"P5 %d %d 255\n",COLS_image,ROWS_image);
  fwrite(msf,COLS_image*ROWS_image,sizeof(unsigned char),fpt);
  fclose(fpt);

  // STEP 4: Calculating the ROC Curves
  template = (char)('e'); m = 0;
  for (int T = 0; T <= 255; T++)
  {
    // Creating binary image
    binary = (unsigned char *)calloc(ROWS_image*COLS_image, sizeof(unsigned char));
    for (r = 0; r < ROWS_image; r++)
    {
      for (c = 0; c < COLS_image; c++)
      {
        if (msf[r*COLS_image+c] > T) {binary[r*COLS_image+c] = 255;}
        else if (msf[r*COLS_image+c] <= T) {binary[r*COLS_image+c] = 0;}
      }
    }
    tp = tn = fp = fn = 0;
    // Opening the groundtruth file for comparison
    fpt_gt = fopen(argv[3], "rb");
    if ((fpt_gt=fopen(argv[3],"rb")) == NULL)
        {
        printf("Unable to open parenthood_gt.txt for reading\n");
        exit(0);
        }
    while(1)
    {
      det = not_det = 0;
      i = fscanf(fpt_gt, "%s %d %d", &temp_letter, &col, &row);
      if ( i != 3) {break;}
      else
      {
        for (r = row-row_lim; r <= row+row_lim; r++)
      {
        for (c = col-col_lim; c <= col+col_lim; c++)
        {
          if (msf[r*COLS_image+c] > T)
          {
            det = 1;
          }
          else if (msf[r*COLS_image+c] <= T) {not_det = 1;}
        }
      }
      if (det == 1 && temp_letter==template) {tp++;}
      else if (det == 1 && temp_letter!=template) {fp++;}
      else if (not_det == 1 && temp_letter==template) {fn++;}
      else if (not_det == 1 && temp_letter!=template) {tn++;}
      }
    }
    fclose(fpt_gt);
    // Storing the values in an array to export
    t_array[m] = T;
    tp_array[m] = tp;
    fp_array[m] = fp;
    tn_array[m] = tn;
    fn_array[m] = fn;
    tpr_array[m] = tp/(double)(tp + fn);
    fpr_array[m] = fp/(double)(fp + tn);
    m++;
  }

  // Exporting the arrays for plotting
  fpt = fopen("Data.txt","wb");
  fprintf(fpt, "Threshold\tTP\tFP\tTN\tFN\tTPR\tFPR\n");
  for (int j = 0; j < 256; j++)
  {
    fprintf(fpt, "%d\t%d\t%d\t%d\t%d\t%0.7f\t%0.7f\n", t_array[j], tp_array[j], fp_array[j], tn_array[j], fn_array[j], tpr_array[j], fpr_array[j]);
  }
  fclose(fpt);
}