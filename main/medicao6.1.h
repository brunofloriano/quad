#ifndef MEDICAO_H
#define MEDICAO_H

#include "include/command.h"
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <time.h>

#ifndef PI
#define PI    3.14159265
#endif // PI

#ifndef g_used_port_num
#define g_used_port_num
#endif // PI

#ifndef g_is_using
#define g_is_using
#endif // PI

int medicao_angulos(float *angulos);
#endif // MEDICAO
