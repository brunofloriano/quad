#include <iostream>
#include <time.h>
#include "medicao_acel.h"
//#include "filtro.h"
#include <fstream>


using namespace std;

int main(){
    clock_t tsim = 10; //tempo de simulacao em segundos
    clock_t tam = 500; //tempo de amostragem em milisegundos
    int contador2 = 0;
    float angulos[2];
    angulos[0] = 0;
    angulos[1] = 0;
    float roll, pitch;
    //float fc; //frequencia de corte do filtro passa baixas
    char C;
    clock_t tInicio, tFim, tDecorrido;
    ofstream arq4("valores_acelerometro.txt");

    cout << "\n Selecione um tempo de amostragem: \n" << endl;
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

    //roll = filtro(tam, 1, roll);
    //pitch = filtro(tam, 1, pitch);

    cout << roll << " " << pitch <<endl;
    arq4 << roll <<" "<<pitch <<endl;
    contador2 = contador2 + tam;
            }

	tFim = clock();
	tDecorrido = ((tFim - tInicio) / (CLOCKS_PER_SEC / 1000));
}

return 0;
}
