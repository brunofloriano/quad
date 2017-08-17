#include <time.h>
#include <stdio.h>
#include "medicao_acelc.h"
/*#include "filtro.h"*/
/*#include <fstream>*/




int main(){
    clock_t tsim = 10; /*tempo de simulacao em segundos*/
    clock_t tam = 500; /*tempo de amostragem em milisegundos*/
    clock_t tInicio;
    clock_t tFim;
    clock_t tDecorrido;
    int contador2 = 0;
    float roll;
    float pitch;
    float angulos[2];
    angulos[0] = 0;
    angulos[1] = 0;
    /*float fc = 1; frequencia de corte do filtro passa baixas*/
    char C;

    /*ofstream arq4("valores_acelerometro.txt");*/

    printf("\n Aperte qualquer teclar para continuar: \n");
    scanf("%c",&C);

    tInicio = clock();
    tFim = clock();
    tDecorrido = ((tFim - tInicio) / (CLOCKS_PER_SEC / 1000));
    /*Loop*/
    while(tDecorrido<tsim*1000){
	if(tDecorrido>contador2){

    medicao_angulos(angulos);
    roll = angulos[0];
    pitch = angulos[1];

    /*roll = filtro(tam, fc, roll);*/
    /*pitch = filtro(tam, fc, pitch);*/

    printf("%f %f \n",roll, pitch);
    /*arq4 << roll <<" "<<pitch <<endl;*/
    contador2 = contador2 + tam;
            }

	tFim = clock();
	tDecorrido = ((tFim - tInicio) / (CLOCKS_PER_SEC / 1000));
}

return 0;
}
