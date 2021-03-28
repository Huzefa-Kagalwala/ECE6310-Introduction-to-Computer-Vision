/* This program is written for the ECE6310-Intro to Computer Vision course, Lab 8.
   Submitted by Huzefa Kagalwala (C48290423) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[])
{
  FILE *fpt;
  char header_range[80], header_ref[80];
  unsigned char *range, *reflectance, *mask, *labels, *seg_image;
  int region_coord[2], *indices, *RegionPixels, RegionSize, TotalRegions;
  int i, r, c, COLS_range, ROWS_range, BYTES_range, COLS_ref, ROWS_ref, BYTES_ref;
  double cp[7], P[3][128*128], xangle, yangle, dist, ScanDirectionFlag, SlantCorrection, *normalx, *normaly, *normalz;
  double	x0, y0, z0, x1, y1, z1, x2, y2, z2, ax, ay, az, bx, by, bz, cx, cy, cz;
  double	avg, avg_n[3][30];
  void		RegionGrow();

  // STEP 1: Read in the images.

  // Read the range image
  if (argc != 3)
  {
    printf("Usage: HW8 [input_range.ppm] [input_reflectance.ppm]\n");
    exit(0);
  }
  if ((fpt = fopen(argv[1],"rb")) == NULL)
  {
    printf("Unable to find %s for opening\n", argv[1]);
    exit(0);
  }

  i=fscanf(fpt,"%s %d %d %d ", header_range, &COLS_range, &ROWS_range, &BYTES_range);
  if (i != 4  ||  strcmp(header_range,"P5") != 0  ||  BYTES_range != 255)
  {
    printf("%s is not an 8-bit PPM greyscale (P5) image\n", argv[1]);
    fclose(fpt);
    exit(0);
  }
  range = (unsigned char *)calloc(ROWS_range*COLS_range, sizeof(unsigned char));
  if (range == NULL)
  {
  printf("Unable to allocate %d x %d memory\n",COLS_range,ROWS_range);
  exit(0);
  }
  fread(range, sizeof(unsigned char), ROWS_range*COLS_range, fpt);
  fclose(fpt);

  // Read the reflectance image
  if ((fpt = fopen(argv[2],"rb")) == NULL)
  {
    printf("Unable to find %s for opening\n", argv[2]);
    exit(0);
  }

  i=fscanf(fpt,"%s %d %d %d ", header_ref, &COLS_ref, &ROWS_ref, &BYTES_ref);
  if (i != 4  ||  strcmp(header_ref,"P5") != 0  ||  BYTES_ref != 255)
  {
    printf("%s is not an 8-bit PPM greyscale (P5) image\n", argv[2]);
    fclose(fpt);
    exit(0);
  }
  reflectance = (unsigned char *)calloc(ROWS_ref*COLS_ref, sizeof(unsigned char));
  if (reflectance == NULL)
  {
  printf("Unable to allocate %d x %d memory\n",COLS_ref,ROWS_ref);
  exit(0);
  }
  fread(reflectance, sizeof(unsigned char), ROWS_ref*COLS_ref, fpt);
  fclose(fpt);

  // STEP 2: Threshold the range image

  mask = (unsigned char *)calloc(ROWS_range*COLS_range, sizeof(unsigned char));
  for (r = 0; r < ROWS_range; r++)
  {
    for (c = 0; c < COLS_range; c++)
    {
      if (range[r*COLS_range+c] <= 125) {mask[r*COLS_range+c] = 0;}
      else if (range[r*COLS_range+c] > 125) {mask[r*COLS_range+c] = 255;}
    }
  }

  // STEP 3: Calculate the 3D Coordinates of each pixel.

  cp[0]=1220.7;		/* horizontal mirror angular velocity in rpm */
  cp[1]=32.0;		/* scan time per single pixel in microseconds */
  cp[2]=(COLS_range/2)-0.5;		/* middle value of columns */
  cp[3]=1220.7/192.0;	/* vertical mirror angular velocity in rpm */
  cp[4]=6.14;		/* scan time (with retrace) per line in milliseconds */
  cp[5]=(ROWS_range/2)-0.5;		/* middle value of rows */
  cp[6]=10.0;		/* standoff distance in range units (3.66cm per r.u.) */

  cp[0]=cp[0]*3.1415927/30.0;	/* convert rpm to rad/sec */
  cp[3]=cp[3]*3.1415927/30.0;	/* convert rpm to rad/sec */
  cp[0]=2.0*cp[0];		/* beam ang. vel. is twice mirror ang. vel. */
  cp[3]=2.0*cp[3];		/* beam ang. vel. is twice mirror ang. vel. */
  cp[1]/=1000000.0;		/* units are microseconds : 10^-6 */
  cp[4]/=1000.0;			/* units are milliseconds : 10^-3 */

  ScanDirectionFlag = 1.0; // For downward direction

  for (r = 0; r < ROWS_range; r++)
  {
    for (c = 0; c < COLS_range; c++)
    {
      SlantCorrection = cp[3] * cp[1] * ((double)c - cp[2]);
      xangle = cp[0] * cp[1] * ((double)c - cp[2]);
      yangle = (cp[3] * cp[4] * (cp[5] - (double)r))+	/* Standard Transform Part */
	            SlantCorrection*ScanDirectionFlag;	/*  + slant correction */
      dist = (double)range[r*COLS_range+c] + cp[6];
      P[2][r*COLS_range+c] = sqrt((dist*dist) / (1.0 + (tan(xangle)*tan(xangle))
	                    + (tan(yangle)*tan(yangle))));
      P[0][r*COLS_range+c] = tan(xangle) * P[2][r*COLS_range+c];
      P[1][r*COLS_range+c] = tan(yangle) * P[2][r*COLS_range+c];
    }
  }

  // STEP 4: Calculate the surface normals for the part of the image we want only when there's a valid pixel

  normalx = (double *)calloc(ROWS_range*COLS_range, sizeof(double));
  normaly = (double *)calloc(ROWS_range*COLS_range, sizeof(double));
  normalz = (double *)calloc(ROWS_range*COLS_range, sizeof(double));
  int p_dist = 3;
  for (r = 0; r < ROWS_range; r++)
  {
    for (c = 0; c < COLS_range; c++) 
    {

      if (mask[r*COLS_range+c] == 0 && (r < (ROWS_range - p_dist) || c < (COLS_range - p_dist) || r >= p_dist || c >= p_dist)) 
      {
        x0 = P[0][r*COLS_range+c]; y0 = P[1][r*COLS_range+c]; z0 = P[2][r*COLS_range+c];
        x1 = P[0][(r-p_dist)*COLS_range+(c)]; y1 = P[1][(r-p_dist)*COLS_range+(c)]; z1 = P[2][(r-p_dist)*COLS_range+(c)];
        x2 = P[0][(r)*COLS_range+(c-p_dist)]; y2 = P[1][(r)*COLS_range+(c-p_dist)]; z2 = P[2][(r)*COLS_range+(c-p_dist)];
        ax = x1-x0; ay = y1-y0; az = z1-z0;
        bx = x2-x0; by = y2-y0; bz = z2-z0;
        normalx[r*COLS_range+c] = (ay*bz) - (az*by);
        normaly[r*COLS_range+c] = (az*bx) - (ax*bz);
        normalz[r*COLS_range+c] = (ax*by) - (ay*bx);
      }
    }
    }

  fpt = fopen("Output.csv","wb");
  //fprintf(fpt, "X\t Y\t Z\t U\t V\t W\n");
  for (i = 0; i < ROWS_range*COLS_range; i++)
  {
    fprintf(fpt, "%lf,%lf,%lf,%lf,%lf,%lf\n", P[0][i], P[1][i], P[2][i], normalx[i], normaly[i], normalz[i]);
  }
  fclose(fpt);

  // STEP 5 : Start Region Growing

  labels = (unsigned char *)calloc(ROWS_ref*COLS_ref, sizeof(unsigned char));
  indices = (int *)calloc(ROWS_ref*COLS_ref*3, sizeof(int));
  TotalRegions = 0;
  for (r = 2; r < (ROWS_ref-2); r++)
  {
    for (c = 2; c < (COLS_ref-2); c++)
    {
      if (mask[r*COLS_ref+c] == 0 && (normalx[r*COLS_ref+c] != 0 && normaly[r*COLS_ref+c] != 0 && normalz[r*COLS_ref+c] != 0))
      {
        if (labels[r*COLS_ref+c] != 0) {continue;}
        // Seed condition is no pixel in 5x5 region should be labeled
        int counter = 0;
        for (int r1 = -2; r1 <= 2; r1++)
        {
          for (int c1 = -2; c1 <= 2; c1++)
          {
            if (labels[(r+r1)*COLS_ref+(c+c1)] != 0) {counter++;}
          }
        }
        // No pixels are labeled
        if (counter == 0)
        {
          TotalRegions++;
          if (TotalRegions == 255) {printf("Segmentation incomplete.  Ran out of labels.\n"); break;}
          RegionGrow(mask,labels,ROWS_ref,COLS_ref,r,c,0,TotalRegions,indices,&RegionSize,normalx,normaly,normalz,avg_n,region_coord);
          if (RegionSize < 50)
          {
            for (i = 0; i < RegionSize; i++) {labels[indices[i]] = 0;}
            TotalRegions--;
          }
          else
          {
            printf("Region labeled %d is %d in size.\n",TotalRegions,RegionSize);
            printf("Location: %d %d. Average Normal: %lf %lf %lf \n", region_coord[0], region_coord[1], 
                                                                      avg_n[0][TotalRegions], avg_n[1][TotalRegions], avg_n[2][TotalRegions]);
          }
        }
      } 
      else {continue;}
    }
    if (c < COLS_ref-2) {break;}
  }
  printf("%d total regions were found\n", TotalRegions);

  seg_image = (unsigned char *)calloc(ROWS_ref*COLS_ref*3, sizeof(unsigned char));

  for (i = 0; i < ROWS_ref*COLS_ref; i++)
  {
    seg_image[i*3+0] = (labels[i]*53) % 255;
    seg_image[i*3+1] = (labels[i]*97) % 255;
    seg_image[i*3+2] = (labels[i]*223) % 255;    
  }

  if ((fpt = fopen("seg.ppm", "wb")) == NULL)
  {
    printf("Unable to open file for writing\n");
    exit(0);
  }
  fprintf(fpt, "P6 %d %d 255\n", COLS_ref, ROWS_ref);
  fwrite(seg_image, sizeof(unsigned char), ROWS_ref*COLS_ref*3, fpt);
  fclose(fpt);

}

/*
	** Given an image, a starting point, and a label, this routine
	** paint-fills (8-connected) the area with the given new label
	** according to the given criteria.
	*/

#define MAX_QUEUE 10000	/* max perimeter size (pixels) of border wavefront */

void RegionGrow (int *image,	/* image data */
		unsigned char *labels,	/* segmentation labels */
		int ROWS,int COLS,	/* size of image */
		int r,int c,		/* pixel to paint from */
		int paint_over_label,	/* image label to paint over */
		int new_label,		/* image label for painting */
		int *indices,		/* output:  indices of pixels painted */
		int *count,     
    double *nx,
    double *ny, 
    double *nz,
    double avg_n[3][30],
    int region_coord[2])		
{
int	r2,c2;
int	queue[MAX_QUEUE],qh,qt;
double avgx, avgy, avgz, angle, dot, mag_a, mag_b;
*count=0;
if (labels[r*COLS+c] != paint_over_label)
  return;
labels[r*COLS+c]=new_label;
if (indices != NULL)
  indices[0]=r*COLS+c;
queue[0]=r*COLS+c;
qh=1;	/* queue head */
qt=0;	/* queue tail */

avgx = nx[(queue[qt]/COLS)*COLS+queue[qt]%COLS];
avgy = ny[(queue[qt]/COLS)*COLS+queue[qt]%COLS]; 
avgz = nz[(queue[qt]/COLS)*COLS+queue[qt]%COLS];

while (qt != qh)
  {
  for (r2=-1; r2<=1; r2++)
  {
    for (c2=-1; c2<=1; c2++)
      { 
       if (r2 == 0  &&  c2 == 0)
         continue;
       if ((queue[qt]/COLS+r2) < 0  ||  (queue[qt]/COLS+r2) >= ROWS  ||
	  (queue[qt]%COLS+c2) < 0  ||  (queue[qt]%COLS+c2) >= COLS)
         continue;
      if (labels[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]!=paint_over_label)
        continue;
      if (nx[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2] == 0 && 
          ny[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2] == 0 && nz[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2] == 0) {continue;}
      avgx = (avgx + nx[(r+r2)*COLS+(c+c2)]) / 2.0;
      avgy = (avgy + ny[(r+r2)*COLS+(c+c2)]) / 2.0;
      avgz = (avgz + nz[(r+r2)*COLS+(c+c2)]) / 2.0;
      dot = (avgx*nx[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]) + (avgy*ny[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]) + 
              (avgz*nz[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]); 
      mag_a = sqrt(pow(avgx, 2) + pow(avgy, 2) + pow(avgz, 2));
      mag_b = sqrt(pow(nx[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2], 2) + pow(ny[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2], 2) + 
                  pow(nz[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2], 2));
      angle = acos(dot/(mag_a*mag_b));
		/* test criteria to join region */
      if (angle > 0.174*6.4)
        continue;
      labels[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]=new_label;
      if (indices != NULL)
        indices[*count]=(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2;

      (*count)++;
      queue[qh]=(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2;
      qh=(qh+1)%MAX_QUEUE;
      if (qh == qt)
        {
        printf("Max queue size exceeded\n");
        exit(0);
        }
      }
  }
  qt=(qt+1)%MAX_QUEUE;
  }
  region_coord[0] = r;
  region_coord[1] = c;
  avg_n[0][new_label] = avgx;
  avg_n[1][new_label] = avgy;
  avg_n[2][new_label] = avgz; 
}