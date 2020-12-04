/* This program is written for the ECE6310-Intro to Computer Vision course, Lab 5.
   Submitted by Huzefa Kagalwala (C48290423) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[])
{
  //Change this definition to change the window size. Odd numbers only.
  # define win  7

  FILE *fpt;
  char header_image[80];
  unsigned char *image, *sobel_out, *image_cp, *image_color;
  int COLS, ROWS, BYTES, max, min, next_x, next_y;
  int i, r, c, r2, c2, total_points, px[100], py[100];
  double *Gx_sobel, *Gy_sobel, *sobel, val, avg;
  double ie1[win*win], ie2[win*win], ee[win*win], ie1_norm[win*win], ie2_norm[win*win], ee_norm[win*win], energy_fin[win*win];

  //STEP 1: Opening the input image and the initial contour points.

  if (argc != 3)
  {
    printf("Usage: HW5 [input.ppm] [contour.txt]\n");
    exit(0);
  }
  fpt = fopen(argv[1],"rb");
  if (fpt == NULL)
  {
    printf("Unable to find %s for opening\n", argv[1]);
    exit(0);
  }
  i=fscanf(fpt,"%s %d %d %d ", header_image, &COLS, &ROWS, &BYTES);
  if (i != 4  ||  strcmp(header_image,"P5") != 0  ||  BYTES != 255)
  {
    printf("%s is not an 8-bit PPM greyscale (P5) image\n", argv[1]);
    fclose(fpt);
    exit(0);
  }
  image = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
  if (image == NULL)
  {
  printf("Unable to allocate %d x %d memory\n",COLS,ROWS);
  exit(0);
  }
  fread(image, sizeof(unsigned char), ROWS*COLS, fpt);
  fclose(fpt);

  if ((fpt=fopen(argv[2],"rb")) == NULL)
  {
    printf("Unable to open %s for reading\n", argv[2]);
    exit(0);
  }
  total_points=0;
  while (1)
  {
  i=fscanf(fpt, "%d %d", &px[total_points], &py[total_points]);
  if (i != 2)
    break;
  total_points++;
  if (total_points > 100)
    break;
  }
  fclose(fpt);

  //STEP 2: Plotting the contour points on the image and outputting it. 

  image_cp = (unsigned char *)calloc(ROWS*COLS*3, sizeof(unsigned char));
  //Copying the image in RGB but keeping the grayscale colors intact
  for (i = 0; i < ROWS*COLS; i++) 
  {
    image_cp[i*3+0] = image[i]%255;
    image_cp[i*3+1] = image[i]%255;
    image_cp[i*3+2] = image[i]%255;
  }
  // Plotting the '+' marks in red.
  for(i = 0; i < total_points; i++)
  {
    for (r = py[i] - 3; r <= py[i] + 3; r++)
    {
      for (c = px[i] - 3; c <= px[i] + 3; c++)
      {
        if (r == py[i] || c == px[i])
        {
          image_cp[(r*COLS+c)*3+0] = 255;
          image_cp[(r*COLS+c)*3+1] = 0;
          image_cp[(r*COLS+c)*3+2] = 0;
        }
      }
    }
  }

  fpt=fopen("marked_hawk_initial.ppm","wb");
  if (fpt == NULL)
    {
      printf("Unable to open marked_hawk_initial.ppm for writing\n");
      exit(0);
    }
  fprintf(fpt, "P6 %d %d 255\n", COLS, ROWS);
  fwrite(image_cp, sizeof(unsigned char), ROWS*COLS*3, fpt);
  fclose(fpt);

  // STEP 3: Calculating the gradient image using a Sobel Operator

  int Gx[9] = {1, 0, -1, 2, 0, -2, 1, 0, -1};
  int Gy[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};

  Gx_sobel = (double *)calloc(ROWS*COLS, sizeof(double));
  for (r = 0; r < ROWS; r++)
  {
    for (c = 0; c < COLS; c++)
    {
      val = 0.0;
      if ( r < 1 || c < 1 || r >= (ROWS-1) || c >= (COLS-1))
      {
        //Used for edge case handling
        Gx_sobel[r*COLS+c] = 0.0;
      }
      else
      {
        for (r2 = -1; r2 <= 1; r2++)
        {
          for (c2 = -1; c2 <= 1; c2++)
          {
            val = val + (image[(r+r2)*COLS+(c+c2)]*Gx[(r2+1)*3+(c2+1)]);
          }
        }
        Gx_sobel[r*COLS+c] = val;
      }
    }
  }

  Gy_sobel = (double *)calloc(ROWS*COLS, sizeof(double));
  for (r = 0; r < ROWS; r++)
  {
    for (c = 0; c < COLS; c++)
    {
      val = 0.0;
      if ( r < 1 || c < 1 || r >= (ROWS-1) || c >= (COLS-1))
      {
        Gy_sobel[r*COLS+c] = 0.0;
      }
      else
      {
        for (r2 = -1; r2 <= 1; r2++)
        {
          for (c2 = -1; c2 <= 1; c2++)
          {
            val = val + (image[(r+r2)*COLS+(c+c2)]*Gy[(r2+1)*3+(c2+1)]);
          }
        }
        Gy_sobel[r*COLS+c] = val;
      }
    }
  }

  sobel = (double *)calloc(ROWS*COLS, sizeof(double));
  for (r = 0; r < ROWS; r++)
  {
    for (c = 0; c < COLS; c++)
    {
      sobel[r*COLS+c] = sqrt(pow(Gx_sobel[r*COLS+c], 2) + pow(Gy_sobel[r*COLS+c], 2));
      //sobel[r*COLS+c] = Gx_sobel[r*COLS+c] + Gy_sobel[r*COLS+c];
    }
  }

  //Normalizing this for output
  max = 0; min = 99999999;
  for (int j = 0; j < ROWS*COLS; j++)
  {
    if (sobel[j] > max) {max = sobel[j];}
    if (sobel[j] < min) {min = sobel[j];}
  }
  float factor = max - min; float feed = 0;
  sobel_out = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
  for (r = 0; r < ROWS; r++)
  {
    for (c = 0; c < COLS; c++)
    {
      //This condition just ensures that the pixels omitted in the edge case handling aren't normalized.
      if (r<1 || c<1 || r>=(ROWS-1) || c>=(COLS-1)){
        sobel_out[r*COLS+c] = 0;
      }
      else{
        // Normalizing the image pixels between 0 and 255
        feed = (255*(sobel[r*COLS+c]-min))/(factor);
        int f = round(feed); // Rounding at the last ensures precision isn't lost in the arithmetic operations
        sobel_out[r*COLS+c] = f;
      }
    }
  }

  fpt=fopen("sobel_hawk.ppm","wb");
  if (fpt == NULL)
    {
      printf("Unable to open inverted.ppm for writing\n");
      exit(0);
    }
  fprintf(fpt, "P5 %d %d 255\n", COLS, ROWS);
  fwrite(sobel_out, sizeof(unsigned char), ROWS*COLS, fpt);
  fclose(fpt);

  //STEP 4: Performing the active contour operation
  int n = 0; 
  while(n < 30)
  {
    for (i = 0; i < total_points; i++)
    {
      //STEP 4.1: Calculate Internal Energy 1
      int m = 0;
      for (r = -win/2; r <= win/2; r++)
      {
        for (c = -win/2; c <= win/2; c++)
        {
          /* If the current point is the last value of the array, make the next contour point, the first value of the array.
             Since, we know that the points form a closed loop. */
          if (i + 1 >= total_points) {ie1[m] = pow(((r+py[i]) - py[0]) , 2) + pow(((c+px[i]) - px[0]), 2);}
          else {ie1[m] = pow(((r+py[i]) - py[i+1]) , 2) + pow(((c+px[i]) - px[i+1]), 2);}
          m++;
        }
      }
      //STEP 4.2: Calculate Internal Energy 2
      int o = 0;
      double sum = 0.0;
      // Calculate the average distance between the contour points
      for (int z = 0; z < total_points; z++)
      {
        /* If the current point is the last value of the array, make the next contour point, the first value of the array.
           Since, we know that the points form a closed loop.*/
        if (z + 1 >= total_points) {next_x = px[0]; next_y = py[0];}
        else {next_x = px[z+1]; next_y = py[z+1];}
        sum += sqrt(pow((px[z] - next_x), 2) + pow((py[z] - next_y), 2));
      }
      avg = sum/total_points;
      // Calculate the actual IE2 value
      for (r = -win/2; r <= win/2; r++)
      {
        for (c = -win/2; c <= win/2; c++)
        {
          if (i + 1 >= total_points) {ie2[o] = pow(avg - sqrt(pow(((r+py[i]) - py[0]) , 2) + pow(((c+px[i]) - px[0]), 2)), 2);}
          else {ie2[o] = pow(avg - sqrt(pow(((r+py[i]) - py[i+1]) , 2) + pow(((c+px[i]) - px[i+1]), 2)), 2);}
          o++;
        }
      }
      // STEP 4.3: Calculate the external energy
      int p = 0;
      for (r = -win/2; r <= win/2; r++)
      {
        for (c = -win/2; c <= win/2; c++)
        {
          ee[p] = pow(sobel_out[(r+py[i])*COLS+(c+px[i])], 2);
          p++;
        }
      }
      // STEP 4.4: Normalize the energy terms
      //Normalizing IE1
      int max_ie1 = 0; int min_ie1 = 99999999;
      for (int j = 0; j < win*win; j++)
      {
        if (ie1[j] > max_ie1) {max_ie1 = ie1[j];}
        if (ie1[j] < min_ie1) {min_ie1 = ie1[j];}
      }
      float factor_ie1 = max_ie1 - min_ie1;
      for (r = 0; r < win; r++)
      {
        for (c = 0; c < win; c++)
        {
          ie1_norm[r*win+c] = (ie1[r*win+c]-min_ie1)/(factor_ie1);
        }
      }
      //Normalizing IE2
      float max_ie2 = 0.0; float min_ie2 = 9999999.999999;
      for (int j = 0; j < win*win; j++)
      {
        if (ie2[j] > max_ie2) {max_ie2 = ie2[j];}
        if (ie2[j] < min_ie2) {min_ie2 = ie2[j];}
      } 
      for (r = 0; r < win; r++)
      {
        for (c = 0; c < win; c++)
        {
          ie2_norm[r*win+c] = (ie2[r*win+c] - min_ie2) / (max_ie2 - min_ie2);
        }
      }
      // Normalizing EE
      float max_ee = 0.0; float min_ee = 999999999.9999;
      for(int j = 0; j < win*win; j++)
      {
        if (ee[j] > max_ee) {max_ee = ee[j];}
        if (ee[j] < min_ee) {min_ee = ee[j];}
      }
      for(r = 0; r < win; r++)
      {
        for (c = 0; c < win; c++)
        {
          ee_norm[r*win+c] = (ee[r*win+c] - min_ee) / (max_ee - min_ee);
        }
      }
      // STEP 4.5: Add the energies
      for (r = 0; r < win; r++)
      {
        for (c = 0; c < win; c++)
        {
          energy_fin[r*win+c] = ie1_norm[r*win+c] + ie2_norm[r*win+c] - (0.8*ee_norm[r*win+c]);
        }
      }
      // STEP 4.6: Find the index of the least energy point in the window
      float min_val = 9999999.99999; int min_ind = 0;
      for (int j = 0; j < win*win; j++)
      {
        if (energy_fin[j] < min_val) {min_val = energy_fin[j]; min_ind = j;}
      }
      //STEP 4.7: Update the location of the contour point
      px[i] = px[i] + (min_ind % win) - (win / 2);
      py[i] = py[i] + (min_ind / win) - (win / 2);
    }
    n++;
  }

  // Copying the image in RGB but keeping the grayscale colors intact
  image_color = (unsigned char *)calloc(ROWS*COLS*3, sizeof(unsigned char));
  for (i = 0; i < ROWS*COLS; i++)
  {
    image_color[i*3+0] = image[i]%255;
    image_color[i*3+1] = image[i]%255;
    image_color[i*3+2] = image[i]%255;
  }
  // Plotting the '+' marks in red.
  for (i = 0; i < total_points; i++)
  {
    for (r = py[i] - 3; r <= py[i] + 3; r++)
    {
      for (c = px[i] - 3; c <= px[i] + 3; c++)
      {
        if (r == py[i] || c == px[i])
        {
          image_color[(r*COLS+c)*3 + 0] = 255;
          image_color[(r*COLS+c)*3 + 1] = 0;
          image_color[(r*COLS+c)*3 + 2] = 0;
        }
      }
    }
  }
  fpt = fopen("marked_hawk_final.ppm", "wb");
  if (fpt == NULL)
  {
    printf("Unable to open marked_hawk_final.ppm for writing\n");
    exit(0);
  }
  fprintf(fpt, "P6 %d %d 255\n", COLS, ROWS);
  fwrite(image_color, sizeof(unsigned char), ROWS*COLS*3, fpt);
  fclose(fpt);

  // Exporting the final contour points to a text file

  fpt = fopen("hawk_final.txt","wb");
  fprintf(fpt, "Column\t Row\n");
  for (int j = 0; j < total_points; j++)
  {
    fprintf(fpt, "%d\t %d\n", px[j], py[j]);
  }
  fclose(fpt);
}