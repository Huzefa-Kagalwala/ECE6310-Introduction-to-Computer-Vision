/* This program is written for the ECE6310-Intro to Computer Vision course, Lab 3.
  ** Submitted by Huzefa Kagalwala (C48290423) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[])
{
  FILE *fpt_msf, *fpt_gt, *fpt;
  unsigned char *image, *msf, *binary, *slice, *padded_slice, *marked;
  char header_image[80], header_msf[80], template, temp_letter;
  int ROWS_image, COLS_image, BYTES_image, ROWS_msf, COLS_msf, BYTES_msf, insert, count, neighbors, val, endpoints, branchpoints;
  int row, col, i, j, m, r, c, r2, c2, tp, tn, fp, fn, det, not_det, erasures, w, v;
  int tp_array[256], fp_array[256], fn_array[256], tn_array[256], t_array[256];
  double fpr, tpr, fpr_array[256], tpr_array[256];

  // STEP 1: Opening the input and MSF image
  if (argc != 4)
  {
    printf("Usage: HW2 [input] [msf] [groundtruth]\n");
    exit(0);
  }

  // Opening the image and reading it into a variable
  fpt = fopen(argv[1],"rb");
  if (fpt == NULL)
  {
    printf("Unable to %s for opening\n", argv[1]);
    exit(0);
  }
  i=fscanf(fpt,"%s %d %d %d ",header_image,&COLS_image,&ROWS_image,&BYTES_image);
  if (i != 4  ||  strcmp(header_image,"P5") != 0  ||  BYTES_image != 255)
  {
    printf("%s is not an 8-bit PPM greyscale (P5) image\n",argv[1]);
    fclose(fpt);
    exit(0);
  }
  image = (unsigned char *)calloc(ROWS_image*COLS_image, sizeof(unsigned char));
  if (image == NULL)
  {
  printf("Unable to allocate %d x %d memory\n",COLS_image,ROWS_image);
  exit(0);
  }
  fread(image, sizeof(unsigned char), ROWS_image*COLS_image, fpt);
  fclose(fpt);

  // Opening the MSF image and reading it into a variable
  fpt = fopen(argv[2],"rb");
  if (fpt == NULL)
  {
    printf("Unable to open %s for opening\n", argv[2]);
    exit(0);
  }
  i=fscanf(fpt,"%s %d %d %d ",header_msf,&COLS_msf,&ROWS_msf,&BYTES_msf);
  if (i != 4  ||  strcmp(header_msf,"P5") != 0  ||  BYTES_msf != 255)
  {
    printf("%s is not an 8-bit PPM greyscale (P5) image\n",argv[2]);
    fclose(fpt);
    exit(0);
  }
  msf = (unsigned char *)calloc(ROWS_msf*COLS_msf, sizeof(unsigned char));
  if (msf == NULL)
  {
  printf("Unable to allocate %d x %d memory\n",COLS_msf,ROWS_msf);
  exit(0);
  }
  fread(msf, sizeof(unsigned char), ROWS_msf*COLS_msf, fpt);
  fclose(fpt);

  // STEP 2: Looping through a set of steps for various T values
  template = (char)('e'); m = 0;

  for (int T = 0; T <= 255; T++)
  {
  tp = tn = fp = fn = 0;
  
  // Opening groundtruth file for comparison
  int row_lim = 15/2; int col_lim = 9/2;
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
    if (i != 3){break;}
    else
    {
      // Computing the initial system response from the MSF image
      for (r = row-row_lim; r <= row+row_lim; r++)
      {
        for(c = col-col_lim; c <= col+col_lim; c++)
        {
          if (msf[r*COLS_image+c] > T) {det = 1;}
          else if (msf[r*COLS_image+c] <= T) {not_det = 1;}
        }
      }
      // Refining the detections if detection is true
      if (det == 1)
      {
        det = 0; not_det = 0;

        // create a slice out of the original image
        slice = (unsigned char *)calloc(15*9, sizeof(unsigned char));
        int n = 0;
        for (r = -row_lim; r <= row_lim; r++)
        {
          for(c = -col_lim; c <= col_lim; c++)
          {
            insert = image[(row+r)*COLS_image+(col+c)];
            // Binarize the image here itself
            if (insert > 128) {slice[n] = 0;}
            else if (insert <= 128) {slice[n] = 255;}
            /* The binarization is flipped here because I will be padding this image with zeros in the
            next step and it is convenient if I let the background be black and the edge be white*/
            n++;
          }
        }
        // Pad this image to spoof the blank pixels at edge cases
        int COLS = 9; int ROWS = 15;
        padded_slice = (unsigned char *)calloc((15+2)*(9+2), sizeof(unsigned char));
        for (r = 1; r <= ROWS; r++)
        {
          for (c = 1; c <= COLS; c++)
          {
            padded_slice[r*(COLS+2)+c] = slice[(r-1)*COLS+(c-1)];
          }
        }
        while(1)
        {
          marked = (unsigned char *)calloc((ROWS+2)*(COLS+2), sizeof(unsigned char));
          for (r = 1; r <= ROWS; r++)
          {
            for (c = 1; c <= COLS; c++)
            {
              // Execute algorithm only for the edge pixels
              if (padded_slice[r*(COLS+2)+c] == 255)
              {
                // Calculating the edge to non-edge transitions in clockwise fashion
                count = (padded_slice[(r-1)*(COLS+2)+(c-1)] - padded_slice[(r-1)*(COLS+2)+c] == 255) +
                        (padded_slice[(r-1)*(COLS+2)+c] - padded_slice[(r-1)*(COLS+2)+(c+1)] == 255) +
                        (padded_slice[(r-1)*(COLS+2)+(c+1)] - padded_slice[r*(COLS+2)+(c+1)] == 255) +
                        (padded_slice[r*(COLS+2)+(c+1)] - padded_slice[(r+1)*(COLS+2)+(c+1)] == 255) +
                        (padded_slice[(r+1)*(COLS+2)+(c+1)] - padded_slice[(r+1)*(COLS+2)+c] == 255) +
                        (padded_slice[(r+1)*(COLS+2)+c] - padded_slice[(r+1)*(COLS+2)+(c-1)] == 255) +
                        (padded_slice[(r+1)*(COLS+2)+(c-1)] - padded_slice[r*(COLS+2)+(c-1)] == 255) +
                        (padded_slice[r*(COLS+2)+(c-1)] - padded_slice[(r-1)*(COLS+2)+(c-1)] == 255);
                // Calclating the edge neighbors
                neighbors = 0;
                for (w = -1; w <= 1; w++)
                {
                  for (v = -1; v <=1; v++)
                  {
                    if (w != 0 || v != 0)
                    {
                      if (padded_slice[(r+w)*(COLS+2)+(c+v)] == 255) {neighbors++;}
                    }
                  }
                }
                // Evaluating the third condition of N or W or (W and S) not being edges
                val = 0;
                if (padded_slice[(r-1)*(COLS+2)+c] == 0 || padded_slice[r*(COLS+2)+(c+1)] == 0 || (padded_slice[r*(COLS+2)+(c-1)] == 0 && padded_slice[(r+1)*(COLS+2)+c] == 0))
                {
                  val = 1;
                }
                // Marking pixels for erasure
                if (count == 1 && neighbors >=2 && neighbors <= 6 && val == 1) {marked[r*(COLS+2)+c] = 255;}
              }
              else if (padded_slice[r*(COLS+2)+c] == 0) {continue;}
            }
          }
          erasures = 0;
          // Erasing the marked pixels
          for (r = 1; r<=ROWS; r++)
          {
            for (c=1; c<=COLS; c++)
            {
              if (marked[r*(COLS+2)+c]==255){padded_slice[r*(COLS+2)+c]=0; erasures++;}
            }
          }
          if (erasures == 0) {break;}
        }
        //Count the endpoints and branchpoints
        endpoints = branchpoints = 0;
        for (r = 1; r <= ROWS; r++)
        {
          for (c = 1; c <=COLS; c++)
          {
            if (padded_slice[r*(COLS+2)+c] == 255)
            {
              count = (padded_slice[(r-1)*(COLS+2)+(c-1)] - padded_slice[(r-1)*(COLS+2)+c] == 255) +
                      (padded_slice[(r-1)*(COLS+2)+c] - padded_slice[(r-1)*(COLS+2)+(c+1)] == 255) +
                      (padded_slice[(r-1)*(COLS+2)+(c+1)] - padded_slice[r*(COLS+2)+(c+1)] == 255) +
                      (padded_slice[r*(COLS+2)+(c+1)] - padded_slice[(r+1)*(COLS+2)+(c+1)] == 255) +
                      (padded_slice[(r+1)*(COLS+2)+(c+1)] - padded_slice[(r+1)*(COLS+2)+c] == 255) +
                      (padded_slice[(r+1)*(COLS+2)+c] - padded_slice[(r+1)*(COLS+2)+(c-1)] == 255) +
                      (padded_slice[(r+1)*(COLS+2)+(c-1)] - padded_slice[r*(COLS+2)+(c-1)] == 255) +
                      (padded_slice[r*(COLS+2)+(c-1)] - padded_slice[(r-1)*(COLS+2)+(c-1)] == 255);
              if (count == 1) {endpoints++;}
              else if (count > 2) {branchpoints++;}
            }
          }
        }
        // Re-evaluating the detections
        if (endpoints == 1 && branchpoints == 1) {det = 1;}
        else if (endpoints != 1 || branchpoints != 1) {not_det = 1;}
      }
      if (det == 1 && temp_letter==template) {tp++;}
      else if (det == 1 && temp_letter!=template) {fp++;}
      else if (not_det == 1 && temp_letter==template) {fn++;}
      else if (not_det == 1 && temp_letter!=template) {tn++;}
    }
  }
  fclose(fpt_gt);
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
  //fprintf(fpt, "Threshold\tTP\tFP\tTN\tFN\tTPR\tFPR\n");
  for (int j = 0; j < 256; j++)
  {
    fprintf(fpt, "%d\t%d\t%d\t%d\t%d\t%0.7f\t%0.7f\n", t_array[j], tp_array[j], fp_array[j], tn_array[j], fn_array[j], tpr_array[j], fpr_array[j]);
  }
  fclose(fpt);
}