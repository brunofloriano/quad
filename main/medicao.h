#ifndef MEDICAO_H
#define MEDICAO_H


#include "include/command.h"
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <time.h>

int medicao(clock_t tsim, clock_t tam);

#endif
