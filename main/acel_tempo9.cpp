#include <iostream>
#include <time.h>
#include "medicao_acel.h"
#include "filtro.h"
#include <fstream>


using namespace std;

int main(){
    clock_t tsim = 10; //tempo de simulacao em segundos
    clock_t tam = 100; //tempo de amostragem em milisegundos
    int contador2 = 0;
    float angulos[2];
    angulos[0] = 0;
    angulos[1] = 0;
    float roll = 0, pitch = 0, roll_medido, pitch_medido;
    float fc = 1000; //frequencia de corte do filtro passa baixas
    char C;
    clock_t tInicio, tFim, tDecorrido;
    ofstream arq4("valores_acelerometro.txt");

    //cout << "\n Selecione um tempo de amostragem: \n" << endl;
    //cin >> tam;
    cout << "\n Pressione qualquer tecla para continuar: \n" << endl;
    cin >> C;

    tInicio = clock();
    tFim = clock();
    tDecorrido = ((tFim - tInicio) / (CLOCKS_PER_SEC / 1000));
    //Loop
    while(tDecorrido<tsim*1000){
	if(tDecorrido>contador2){

    medicao_angulos(angulos);
    roll_medido = angulos[0];
    pitch_medido = angulos[1];

    roll = filtro(tam, fc, roll_medido, roll);
    //pitch = filtro(tam, fc, pitch_medido, pitch);

    //cout << roll << " " << pitch <<endl;
    arq4 << roll <<" "<<pitch <<endl;
    contador2 = contador2 + tam;
            }

	tFim = clock();
	tDecorrido = ((tFim - tInicio) / (CLOCKS_PER_SEC / 1000));
}

cout << "\n Fim do Programa! \n" << endl;

return 0;
}
