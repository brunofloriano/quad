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
    clock_t tsim = 5; //tempo de simulacao em segundos
    clock_t tam = 1000; //tempo de amostragem em microsegundos
    clock_t tInicio, tFim, tDecorrido;
    float angulos[2];
    char X;
    int contador2 = 0;

    int USB = inicializacao();
    cout << "\n Pressione qualquer tecla para iniciar \n" << endl;
    cin >> X;

    tInicio = clock();
    tFim = clock();
    tDecorrido = ((tFim - tInicio) / (CLOCKS_PER_SEC / 1000));
    //Loop
    while(tDecorrido<tsim*1000){
	if(tDecorrido>contador2){
    inicializacao();
    medicao(angulos, tsim1,tam1,USB);
    cout << angulos[0] << " " << angulos[1] << endl;
    contador2 = contador2 + tam;
    }
	tFim = clock();
	tDecorrido = ((tFim - tInicio) / (CLOCKS_PER_SEC / 1000));
}

return 0;
}
