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
    clock_t tsim = 10; //tempo de simulacao em segundos
    clock_t tam = 1000; //tempo de amostragem em microsegundos
    clock_t tInicio, tFim, tDecorrido;
    int contador=0, contador2 = 0, contador3 = 0;
    char C;
    float angulos[2];

    ofstream arq4("valores_acelerometro.txt");

    cout << "\n Pressione qualquer tecla para iniciar \n" << endl;
    cin >> C;


    medicao(angulos);

    cout << angulos[0] << " " << angulos[1] << endl;




return 0;
}
