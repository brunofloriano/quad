#include "include/command.h"
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <time.h>
#include "medicao.h"

#define PI    3.14159265

using namespace std;

int main(){
    clock_t tsim1 = 10; //tempo de simulacao em segundos
    clock_t tam1 = 100; //tempo de amostragem em microsegundos

    medicao(tsim1,tam1);

return 0;
}
