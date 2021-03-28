
/* This program is written for the ECE6310-Intro to Computer Vision course, Lab 7.
  ** Submitted by Huzefa Kagalwala (C48290423) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[])
{
  FILE *fpt;
  int i, j, mean_win, mean_lim, total_data, start_idx, end_idx;
  double sumx, sum1x, avgx, sumy, sum1y, avgy, sumz, sum1z, avgz, sum_X, sum_Y, sum_Z, sum_roll, sum_pitch, sum_yaw;
  double sumpitch, sum1pitch, avgpitch, sumroll, sum1roll, avgroll, sumyaw, sum1yaw, avgyaw, start_time, end_time;
  double time[1500], accX[1500], accY[1500], accZ[1500], roll[1500], pitch[1500], yaw[1500];
  double var_X[1500], motion_X[1500], var_Y[1500], motion_Y[1500], var_Z[1500], motion_Z[1500];
  double var_pitch[1500], motion_pitch[1500], var_roll[1500], motion_roll[1500], var_yaw[1500], motion_yaw[1500];
  double smooth_X[1500], smooth_Y[1500], smooth_Z[1500], smooth_pitch[1500], smooth_roll[1500], smooth_yaw[1500];
  char t[100], X[100], Y[100], Z[100], r[100], p[100], y[100];

  // STEP 1: Opening the file and storing all the values in appropriate arrays 
  fpt = fopen("acc_gyro.txt", "rb");
  total_data = 0;

  fscanf(fpt, "%s %s %s %s %s %s %s", t, X, Y, Z, r, p, y);
  while(1)
  {
    i = fscanf(fpt, "%lf %lf %lf %lf %lf %lf %lf", &time[total_data], &accX[total_data], &accY[total_data], &accZ[total_data],
                                                  &pitch[total_data], &roll[total_data], &yaw[total_data]);
    if (i != 7) {break;}
    total_data++;
  }
  fclose(fpt);

  // STEP 2: Smooth the given data using a mean filter
  mean_win = 13; mean_lim = mean_win/2;
  for (i = 0; i < total_data; i++)
  {
    if (i < mean_lim || i > (total_data - mean_lim))
    {
      smooth_X[i] = accX[i];
      smooth_Y[i] = accY[i];
      smooth_Z[i] = accZ[i];
      smooth_roll[i] = roll[i];
      smooth_pitch[i] = pitch[i];
      smooth_yaw[i] = yaw[i];
    }
    else 
    {
      sum_X = 0.0; sum_Y = 0.0; sum_Z = 0.0; sum_pitch = 0.0; sum_roll = 0.0; sum_yaw = 0.0; 
      for (j = -mean_lim; j <= mean_lim; j++)
      {
        sum_X += accX[i+j];
        sum_Y += accY[i+j];
        sum_Z += accZ[i+j];
        sum_pitch += pitch[i+j];
        sum_roll += roll[i+j];
        sum_yaw += yaw[i+j];
      }
      smooth_X[i] = sum_X/mean_win;
      smooth_Y[i] = sum_Y/mean_win;
      smooth_Z[i] = sum_Z/mean_win;
      smooth_pitch[i] = sum_pitch/mean_win;
      smooth_roll[i] = sum_roll/mean_win;
      smooth_yaw[i] = sum_yaw/mean_win;
    }
  }

  // STEP 3: Calculate variances for the data

  int win = 21;
  int lim = win/2;
  for ( i = 0; i < total_data; i++)
  {
    if (i < lim || i > (total_data - lim))
    {
      var_X[i] = 0.0;
      var_Y[i] = 0.0;
      var_Z[i] = 0.0;
      var_pitch[i] = 0.0;
      var_roll[i] = 0.0;
      var_yaw[i] = 0.0;
    }
    else
    {
      // Calculate the average of the values in the window
      sumx  = 0.0; sumy = 0.0; sumz = 0.0; sumpitch = 0.0; sumroll = 0.0; sumyaw = 0.0;
      for (j = -lim; j <= lim; j++)
      {
        sumx += smooth_X[i+j];
        sumy += smooth_Y[i+j];
        sumz += smooth_Z[i+j];
        sumpitch += smooth_pitch[i+j];
        sumroll += smooth_roll[i+j];
        sumyaw += smooth_yaw[i+j];
      }
      avgx = sumx / win; avgy = sumy / win; avgz = sumz / win;
      avgpitch = sumpitch / win; avgroll = sumroll / win; avgyaw = sumyaw / win;
      sum1x = 0.0; sum1y = 0.0; sum1z = 0.0;
      sum1pitch = 0.0; sum1roll = 0.0; sum1yaw = 0.0;
      for (j = -lim; j <= lim; j++)
      {
        sum1x += pow((smooth_X[i+j] - avgx), 2);
        sum1y += pow((smooth_Y[i+j] - avgy), 2);
        sum1z += pow((smooth_Z[i+j] - avgz), 2);
        sum1pitch += pow((smooth_pitch[i+j] - avgpitch), 2);
        sum1roll += pow((smooth_roll[i+j] - avgroll), 2);
        sum1yaw += pow((smooth_yaw[i+j] - avgyaw), 2);
      }
      var_X[i] = sum1x / win; var_Y[i] = sum1y / win; var_Z[i] = sum1z / win;
      var_pitch[i] = sum1pitch / win; var_roll[i] = sum1roll / win; var_yaw[i] = sum1yaw / win;

    }
  } 

  for (i = 0; i < total_data; i++)
  {
    if (var_X[i] <= 0.000041) {motion_X[i] = 0.0;}
    else if (var_X[i] > 0.000041) {motion_X[i] = 1.0;}
    if (var_Y[i] <= 0.00045) {motion_Y[i] = 0.0;}
    else if (var_Y[i] > 0.00045) {motion_Y[i] = 1.0;}
    if (var_Z[i] <= 0.00019 && var_Z[i] >= 0.0132) {motion_Z[i] = 0.0;}
    else if (var_Z[i] > 0.00019 && var_Z[i] < 0.0132) {motion_Z[i] = 1.0;}
    if (var_pitch[i] <= 0.002) {motion_pitch[i] = 0.0;}
    else if (var_pitch[i] > 0.002) {motion_pitch[i] = 1.0;}
    if (var_roll[i] <= 0.000595) {motion_roll[i] = 0.0;}
    else if (var_roll[i] > 0.000595) {motion_roll[i] = 1.0;}
    if (var_yaw[i] <= 0.02) {motion_yaw[i] = 0.0;}
    else if (var_yaw[i] > 0.02) {motion_yaw[i] = 1.0;}
  }

  // Disturbance rejection for roll
  double diff_roll[1500];
  for (i = 0; i < total_data; i++)
  {
    if (i+1  > total_data) {diff_roll[i] = 0.0;}
    else {diff_roll[i] = motion_roll[i+1] - motion_roll[i];}
  }
  int posroll[50], negroll[50];
  int m = 0; int n = 0;
  for (i = 0; i < total_data; i++)
  {
    if (diff_roll[i] == 1.0) {posroll[m] = i; m++;}
    else if (diff_roll[i] == -1.0) {negroll[n] = i; n++;}
    else if (diff_roll[i] == 0.0) {continue;}
  }
  for (i = 0; i < m; i++)
  {
    int diff = negroll[i] - posroll[i];
    if (diff <= 30)
    {
      for (j = posroll[i]; j <= negroll[i]; j++)
      {
        motion_roll[j] = 0.0;
      }
    }
    else {continue;}
  }

  // This condition is actually wrong. Very application specific.
  for (i = 0; i < total_data; i++)
  {
    if (motion_roll[i] == 1.0) {motion_X[i] = 0.0;}
    if (motion_pitch[i] == 1.0) {motion_Y[i] = 0.0;}
    if (motion_yaw[i] == 1.0) {motion_Z[i] = 0.0;}
  }

  // Find the differentials of the motion segments
  double diff_Z[1500], diff_X[1500], diff_Y[1500];
  for (i = 0; i < total_data; i++)
  {
    if (i+1 > total_data) {diff_Z[i] = 0.0; diff_X[i] = 0.0; diff_Y[i] = 0.0;}
    else
    {
      diff_X[i] = motion_X[i+1] - motion_X[i];
      diff_Y[i] = motion_Y[i+1] - motion_Y[i];
      diff_Z[i] = motion_Z[i+1] - motion_Z[i];
    }
  }
  //Disturbance rejection for accZ
  int pos[50], neg[50];
  m = n = 0;
  for (i = 0; i < total_data; i++)
  {
    if (diff_Z[i] == 1) {pos[m] = i; m++;}
    else if (diff_Z[i] == -1) {neg[n] = i; n++;}
    else if (diff_Z[i] == 0) {continue;}
  }
  for (i = 0; i < m; i++)
  {
    int diff = neg[i] - pos[i];
    if (diff <= 10)
    {
      for (j = pos[i]; j <= neg[i]; j++)
      {
        motion_Z[j] = 0.0;
      }
    }
    else {continue;}
  }

  //Disturbance rejection for accX
  int posx[50], negx[50];
  m = n = 0;
  for (i = 0; i < total_data; i++)
  {
    if (diff_X[i] == 1) {posx[m] = i; m++;}
    else if (diff_X[i] == -1) {negx[n] = i; n++;}
    else if (diff_X[i] == 0) {continue;}
  }
  for (i = 0; i < m; i++)
  {
    int diff = negx[i] - posx[i];
    if (diff <= 22)
    {
      for (j = posx[i]; j <= negx[i]; j++)
      {
        motion_X[j] = 0.0;
      }
    }
    else {continue;}
  }

  // STEP 4: Find periods of motion and rest for the phone and find the cumulative distances/rotations per period of motion

  // Find the periods of rest
 
  int rest[1500]; int diff_rest[1500];
  for (i = 0; i < total_data; i++)
  {
    if (motion_X[i] == 0.0 && motion_Y[i] == 0.0 && motion_Z[i] == 0.0 
         && motion_roll[i] == 0.0 && motion_pitch[i] == 0.0 && motion_yaw[i] == 0.0)
    {
      rest[i] = 1;
    }
    else if (motion_X[i] == 1.0 || motion_Y[i] == 1.0 || motion_Z[i] == 1.0 
         || motion_roll[i] == 1.0 || motion_pitch[i] == 1.0 || motion_yaw[i] == 1.0) {rest[i] = 0;}
  }

  for (i = 0; i < total_data; i++)
  {
    if (i+1 > total_data) {diff_rest[i] = 0;}
    else {diff_rest[i] = rest[i+1] - rest[i];}
  }
  diff_rest[0] = 1;
  
  int posrest[200], negrest[200];
  m = n = 0;
  for (i = 0; i < total_data; i++)
  {
    if (diff_rest[i] == 1) {posrest[m] = i; m++;}
    else if (diff_rest[i] == -1) {negrest[n] = i; n++;}
    else if (diff_rest[i] == 0) {continue;}
  }
  for (i = 1; i < m; i++) {posrest[i] = posrest[i]+1;}
  fpt = fopen("Rest.txt","wb");
  for (i = 0; i < m; i++)
  {
    fprintf(fpt, "%f %f %d %d\n", time[posrest[i]], time[negrest[i]], posrest[i], negrest[i]);
  }
  fclose(fpt);

  // Detect periods of motion and integrate the respective motions

  int motion[1500]; int diff_motion[1500];
  for (i = 0; i < total_data; i++)
  {
    if (motion_X[i] == 0.0 && motion_Y[i] == 0.0 && motion_Z[i] == 0.0 
         && motion_roll[i] == 0.0 && motion_pitch[i] == 0.0 && motion_yaw[i] == 0.0)
    {
      motion[i] = 0;
    }
    else if (motion_X[i] == 1.0 || motion_Y[i] == 1.0 || motion_Z[i] == 1.0 
         || motion_roll[i] == 1.0 || motion_pitch[i] == 1.0 || motion_yaw[i] == 1.0) {motion[i] = 1;}
  }
  for (i = 0; i < total_data; i++)
  {
    if (i+1 > total_data) {diff_motion[i] = 0;}
    else {diff_motion[i] = motion[i+1] - motion[i];}
  }
  
  int posmotion[200], negmotion[200];
  m = n = 0;
  for (i = 0; i < total_data; i++)
  {
    if (diff_motion[i] == 1) {posmotion[m] = i; m++;}
    else if (diff_motion[i] == -1) {negmotion[n] = i; n++;}
    else if (diff_motion[i] == 0) {continue;}
  }
  for (i = 0; i < m; i++) {posmotion[i] = posmotion[i]+1;}

  fpt = fopen("Motion.txt","wb");
  int start, end;
  for (i = 0; i < m; i++)
  {
    start = posmotion[i]; end = negmotion[i];
    double init_X = 0.0, init_Y = 0.0, init_Z = 0.0, init_rotX = 0.0, init_rotY = 0.0, init_rotZ = 0.0;
    double distX = 0.0, distY = 0.0, distZ = 0.0;
    double rotX = 0.0, rotY = 0.0, rotZ = 0.0;
    for (int g = start; g <= end; g++)
    {
      double avgX = 0.0, velX = 0.0, avgY = 0.0, velY = 0.0,  avgZ = 0.0, velZ = 0.0; 
      // Distance in X
      velX = init_X + (smooth_X[g]*0.05*9.81);
      avgX = (velX + init_X) / 2.0;
      distX = distX + avgX*0.05;
      init_X = velX;
      // Distance in Y
      velY = init_Y + (smooth_Y[g]*0.05*9.81);
      avgY = (velY + init_Y) / 2.0;
      distY = distY + avgY*0.05;
      init_Y = velY;
      // Distance in Z
      if (smooth_Z[g] < 0) {velZ = init_Z + ((smooth_Z[g] + 1)*0.05*9.81);}
      else if (smooth_Z[g] >= 0) {velZ = init_Z + (smooth_Z[g]*0.05*9.81);}
      avgZ = (velZ + init_Z) / 2.0;
      distZ = distZ + avgZ*0.05;
      init_Z = velZ;
      // Rotation around X
      rotX = (smooth_roll[g]*0.05) + init_rotX;
      init_rotX = rotX;
      // Rotation around Y
      rotY = (smooth_pitch[g]*0.05) + init_rotY;
      init_rotY = rotY;
      // Rotation around Z
      rotZ = (smooth_yaw[g]*0.05) + init_rotZ;
      init_rotZ = rotZ;
    }
    fprintf(fpt, "%lf %lf %d %d %lf %lf %lf %lf %lf %lf\n",  time[posmotion[i]], time[negmotion[i]], posmotion[i], negmotion[i], distX, distY, distZ, rotX, rotY, rotZ);
  }
  fclose(fpt);

} 