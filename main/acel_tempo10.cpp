#include <iostream>
#include <time.h>
#include "medicao.h"
#include "filtro.h"
#include <fstream>

#define PI    3.14159265

using namespace std;

int main(){
    clock_t tsim = 10; //tempo de simulacao em segundos
    clock_t tam = 100; //tempo de amostragem em microsegundos
    clock_t tInicio, tFim, tDecorrido;
    float angulos[2];
    float out;
    float roll_medido, pitch_medido, roll = 0, pitch = 0;
    float fc = 1;
    char X;
    int contador2 = 0;

    int USB = inicializacao();
    ofstream arq4("valores_acelerometro.txt");
    cout << "\n Pressione qualquer tecla para iniciar \n" << endl;
    cin >> X;

    tInicio = clock();
    tFim = clock();
    tDecorrido = ((tFim - tInicio) / (CLOCKS_PER_SEC / 1000));
    //Loop
    while(tDecorrido<tsim*1000){
	if(tDecorrido>contador2){
    inicializacao();
    medicao(angulos, USB);
    roll_medido = angulos[0];
    pitch_medido = angulos[1];

    filtro(tam, fc, roll_medido, roll, &out);
    roll = out;
    pitch = filtro(tam, fc, pitch_medido, pitch, &out);
    pitch = out;

    cout << roll << " " << pitch << endl;
    arq4 << roll <<" "<< pitch <<endl;
    contador2 = contador2 + tam;
    }
	tFim = clock();
	tDecorrido = ((tFim - tInicio) / (CLOCKS_PER_SEC / 1000));
}

return 0;
}
