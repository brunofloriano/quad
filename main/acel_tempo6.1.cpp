//#include "include/command.h"
//#include <unistd.h>     // UNIX standard function definitions
//#include <fcntl.h>      // File control definitions
//#include <errno.h>      // Error number definitions
//#include <termios.h>    // POSIX terminal control definitions
#include <iostream>
#include <time.h>
#include "medicao6.1.h"
#include <fstream>


using namespace std;

int main(){
    clock_t tsim = 10; //tempo de simulacao em segundos
    clock_t tam = 1000; //tempo de amostragem em milisegundos
    int contador2 = 0;
    float angulos[2];
    float roll, pitch;
    char C;
    clock_t tInicio, tFim, tDecorrido;
    ofstream arq4("valores_acelerometro.txt");

    cout << "\n Pressione qualquer tecla para iniciar \n" << endl;
    cin >> C;

    tInicio = clock();
    tFim = clock();
    tDecorrido = ((tFim - tInicio) / (CLOCKS_PER_SEC / 1000));
    //Loop
    while(tDecorrido<tsim*1000){
	if(tDecorrido>contador2){

    medicao_angulos(angulos);
    roll = angulos[0];
    pitch = angulos[1];

    cout << roll << " " << pitch << " \n" <<endl;
    arq4 << roll <<" "<<pitch << "\n" <<endl;
    contador2 = contador2 + tam;
            }

	tFim = clock();
	tDecorrido = ((tFim - tInicio) / (CLOCKS_PER_SEC / 1000));
}

return 0;
}
