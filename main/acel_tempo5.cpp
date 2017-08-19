#include "include/command.h"
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <time.h>
#include "medicao.cpp"

#define PI    3.14159265

using namespace std;

int main(){
    clock_t tsim1 = 10; //tempo de simulacao em segundos
    clock_t tam1 = 500; //tempo de amostragem em microsegundos
    float angulos[2];

    cout << "teste" << endl;

    int USB = inicializacao();
    medicao(angulos, tsim1,tam1,USB);

return 0;
}
