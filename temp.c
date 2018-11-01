#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "ifttt.h"
//28-021313c7caaa
int main (int argc,char *argv[]) {
  if(argc!=2){printf(" wrong arguments");exit(); }
  
  DIR *dir;
  struct dirent *dirent;
  char dev[16];      // Dev ID
  char devPath[128]; // Path to device
  char buf[256];     // Data from device
  char tmp[6];   // Temp C * 1000 reported by device
  char path[] = "/sys/bus/w1/devices";//the path of my device
  ssize_t numRead;

  dir = opendir (path);
  if (dir != NULL)
  {
    while ((dirent = readdir (dir)))
      // 1-wire devices are links beginning with 28-
      if (dirent->d_type == DT_LNK &&
          strstr(dirent->d_name, "28-") != NULL) {
        strcpy(dev, dirent->d_name);
        printf("\nDevice: %s\n", dev);
      }
    (void) closedir (dir);
  }
  else
  {
    perror ("Couldn't open the w1 devices directory");
    return 1;
  }

  // Assemble path to OneWire device
  sprintf(devPath, "%s/%s/w1_slave", path, dev);


  // Read temp continuously
  // Opening the device's file triggers new reading
  float tempFirst,tempMax,tempMin,tempMax2,tempMin2;//declare all the variable that is available.
  int i = 0; 
  char maxc[7],min[7],curc[7]; 
  
    

  while (1) {
    int fd = open(devPath, O_RDONLY);
    if (fd == -1)
    {
      perror ("Couldn't open the w1 device.");
      return 1;
    }

    while ((numRead = read(fd, buf, 256)) > 0)
    {
      strncpy(tmp, strstr(buf, "t=") + 2, 5);
      float tempCurrent = strtof(tmp, NULL);

      if (i == 0) {
        tempMax = tempCurrent;
        tempMin = tempCurrent;
        tempFirst = tempCurrent;
        tempMax2 = tempCurrent;
        tempMin2 = tempCurrent;
        gcvt(tempMax, 6, maxc); gcvt(tempMin, 6, minc); gcvt(tempFirst, 6, curc);
        ifttt("https://maker.ifttt.com/trigger/temp/with/key/b1QwPwFliGUWnU6LYgRbb1",maxc,minc,curc);
        i++;
      }

      /*printf("Device: %s  - ", dev);*/
      printf("Current Temp: %.3f C\nMax Temp: %.3f C\nMin Temp: %.3f C\n\n\n", tempCurrent / 1000, tempMax / 1000, tempMin / 1000);
      /*printf("%.3f F\n\n", (tempC / 1000) * 9 / 5 + 32);*/
      time_t second = time(NULL);
      while (difftime(time(NULL), second) <= 1) {
        while (tempCurrent > tempMax) {
          tempMax = tempCurrent;
        }
        while (tempCurrent < tempMin) {
          tempMin = tempCurrent;
        }
        if (tempCurrent - tempMax2 >= 1000) {
          tempMax2 = tempCurrent;
          gcvt(tempMax2, 6, maxc); gcvt(tempMin, 6, minc); gcvt(tempFirst, 6, curc);
          ifttt("https://maker.ifttt.com/trigger/temp/with/key/b1QwPwFliGUWnU6LYgRbb1",maxc,minc,curc);
        }

        if (tempMin2 - tempCurrent >= 1000) {
          tempMin2 = tempCurrent;
          gcvt(tempMax, 6, maxc); gcvt(tempMin, 6, minc); gcvt(tempFirst, 6, curc);
          ifttt("https://maker.ifttt.com/trigger/temp/with/key/b1QwPwFliGUWnU6LYgRbb1",maxc,minc,curc);

        }
      }
    }
    close(fd);
  }
  /* return 0; --never called due to loop */
}

