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
  if(argc!=2){printf(" wrong arguments");exit(1); }/*if argc, the length is not two, seems something is wrong*/
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
  float tempFirst,tempMax,tempMin,tempCurrent,tempCurrent2;//declare all the variable that is available.
  int i = 0; 
  char maxc[7],minc[7],curc[7]; 
  float cumulative=0;/*this is the value that represent the cumulative change*/
  float change;/*difference between this time and last time*/

  while (1) {
    int fd = open(devPath, O_RDONLY);
    if (fd == -1)
    {perror ("Couldn't open the w1 device.");return 1;}
    while ((numRead = read(fd, buf, 256)) > 0)
    {
      strncpy(tmp, strstr(buf, "t=") + 2, 5);
      tempCurrent2=strtof(tmp, NULL);
      change=tempCurrent2-tempCurrent;/*the current is still last time's "Current"*/
      cumulative+=change;
      if (tempCurrent2 > tempMax) {
          tempMax = tempCurrent2;
        }
      if (tempCurrent2 < tempMin) {
          tempMin = tempCurrent2;
        }
      if (i == 0) {
        tempMax = tempCurrent2;
        tempMin = tempCurrent2;
        tempFirst = tempCurrent2;
        gcvt(tempMax/1000, 6, maxc); gcvt(tempMin/1000, 6, minc); gcvt(tempFirst/1000 , 6, curc);
        ifttt("https://maker.ifttt.com/trigger/temp/with/key/b1QwPwFliGUWnU6LYgRbb1",maxc,minc,curc);
        i++;
      }
      else if(cumulative>=1000 ||cumulative<=-1000 )
      {
       gcvt(tempMax/1000, 6, maxc); gcvt(tempMin/1000, 6, minc); gcvt(tempCurrent2/1000, 6, curc);
       ifttt("https://maker.ifttt.com/trigger/temp/with/key/b1QwPwFliGUWnU6LYgRbb1",maxc,minc,curc);
       cumulative=0;
      }
      tempCurrent = tempCurrent2;/*assign the value to current here*/ 
      
      
      printf("Current Temp: %.3f C\nMax Temp: %.3f C\nMin Temp: %.3f C\n\n\n", tempCurrent / 1000, tempMax / 1000, tempMin / 1000);
      /*print out what is going on.*/
      time_t second = time(NULL);
      while (difftime(time(NULL), second) <= 1) {
        
      }
    }
    close(fd);
  }
  /* return 0; --never called due to loop */
}

