#ifndef MEDICAO_H
#define MEDICAO_H


#include "include/command.h"
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <time.h>

  extern  vector<float> xAccel;
  extern  vector<float> yAccel;
  extern  vector<float> zAccel;
  extern  vector<float> S1;
  extern  vector<float> S2;
  extern  vector<float> S3;
  extern  vector<float> S4;
  extern  vector<float> roll;
  extern  vector<float> pitch;
  extern  int  contador3;

int medicao();

#endif
