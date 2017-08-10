#include "include/command.h"
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <time.h>
#include "medicao6.cpp"

#define PI    3.14159265


using namespace std;

    vector<float> xAccel;
    vector<float> yAccel;
    vector<float> zAccel;
    vector<float> S1;
    vector<float> S2;
    vector<float> S3;
    vector<float> S4;
    vector<float> roll;
    vector<float> pitch;

int main(){
    clock_t tsim1 = 10; //tempo de simulacao em segundos
    clock_t tam1 = 1000; //tempo de amostragem em microsegundos
    int contador2 = 0, contador3 = 0;
    char C;
    clock_t tInicio, tFim, tDecorrido;

    cout << "\n Pressione qualquer tecla para iniciar \n" << endl;
    cin >> C;

    tInicio = clock();
    tFim = clock();
    tDecorrido = ((tFim - tInicio) / (CLOCKS_PER_SEC / 1000));
    //Loop
    while(tDecorrido<tsim*1000){
	if(tDecorrido>contador2){

    medicao();

    cout << roll[contador3%10] << " " << pitch[contador3%10] << endl;
    // cout<<xAccel[contador%10]<<" "<<yAccel[contador%10]<<" "<<zAccel[contador%10]<<" "<<S1[contador%10]<<" "<<S2[contador%10]<<" "<<S3[contador%10]<<" "<<S4[contador%10]<<" "<<roll[contador%10]<<" "<<pitch[contador%10]<<endl;
    arq4<< roll[contador3%10]<<" "<<pitch[contador3%10]<<endl;
    contador2 = contador2 + tam;
	contador3++;
            }

	tFim = clock();
	tDecorrido = ((tFim - tInicio) / (CLOCKS_PER_SEC / 1000));
}

return 0;
}
