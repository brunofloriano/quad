#include "include/command.h"
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include "medicao.h"
#include "filtro.h"
#include <stdio.h>

#define Kp_roll 1
#define Kd_roll 0
#define Kp_pitch 1
#define Kd_pitch 0
#define BAUDRATE                        1000000				// Padrao p/este trabalho
#define DEVICENAME                      "/dev/ttyUSB0"      // Usando o conversor USB
#define dt			                    0.01
#define TASK_PERIOD                     dt*1e9
#define BROADCASTID			            254
#define MAX_SPEED                       1023
#define MAX_TORQUE                      1024
#define TORQUE_LIMIT                    1024
#define V_MAX                           13.3
#define PI                              3.14159265
#define MAX_SPEED_RAD_S                 79.4*V_MAX/16*2*PI/60

using namespace std;

int velocidade(float v){
    int x = MAX_SPEED*abs(v)/(MAX_SPEED_RAD_S);
    int CCW;

    if (v > 0){
        CCW = 0;
    }
    else{CCW = 1024;}


return x + CCW;

}

float ler_velocidade(int x){
    int CCW;
    float v;

    if (x < 1024){
        v = x*(MAX_SPEED_RAD_S)/(MAX_SPEED);
        CCW = 1;
    }
    else{
        v = (x-1024)*(MAX_SPEED_RAD_S)/(MAX_SPEED);
        CCW = -1;}


return v*CCW;

}

int main(){

    command cmd;
    char *dev_name = (char*)DEVICENAME;
    dynamixel::PacketHandler *packetHandler = dynamixel::PacketHandler::getPacketHandler(1);
    dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler(dev_name);
    dynamixel::GroupSyncWrite groupSyncWrite(portHandler, packetHandler, 30, 2);

    ofstream arq3("funcaotransferencia.txt");

    clock_t tInicio, tFim;
    float tDecorrido;
    float tsim = 10; //tempo de simulacao em segundos
    float tam = 100; //tempo de amostragem em milisegundos
    float out;
    float angulos[2];
    float v_medicao, v_desejada, v_aplicada;
    float roll_medido, pitch_medido, roll = 0, pitch = 0;
    float velocidade_roll, velocidade_pitch;
    float fc = 1;
    float K_roll_R = 2, K_roll_L = 2;
    float K_pitch_F = 2, K_pitch_R = 2;
    float K_UP = 1.5, K_DOWN = -1.5;
    float K[13];
    float contador2 = 0;



    int i = 1;
    int v_medicao_int;
    int USB = inicializacao();



    K[0] = 0;
    //roll gains
    K[1] = K_roll_L;
    K[4] = K_roll_R;
    K[7] = K_roll_R*1.1;
    K[10] = K_roll_L;
    //pitch gains
    K[2] = K_pitch_R*K_UP;
    K[3] = K_pitch_R*K_DOWN;
    K[5] = -K_pitch_R*K_UP;
    K[6] = -K_pitch_R*K_DOWN;
    K[8] = -K_pitch_F*K_UP;
    K[9] = -K_pitch_F*K_DOWN;
    K[11] = K_pitch_F*K_UP;
    K[12] = K_pitch_F*K_DOWN;



    if (portHandler->openPort())
    {
        printf("Succeeded to open the port!\n\n");
        printf(" - Device Name : %s\n", dev_name);
        printf(" - Baudrate    : %d\n\n", portHandler->getBaudRate());
    }
    else
    {
        printf("Failed to open the port! [%s]\n", dev_name);
        printf("Press any key to terminate...\n");
        cmd.getch();
        return 0;
    }

    cmd.config_ram(portHandler, packetHandler);

    for(int i=0; i<12;i++)
    {

         cmd.write_mov_speed(portHandler, packetHandler, i+1, 0);

    }


    cmd.getch();



    tInicio = clock();
    tFim = clock();
    tDecorrido = ((float)(tFim - tInicio) / (CLOCKS_PER_SEC/1000));
    //Loop
    while(tDecorrido < tsim*10/24*1000){
	if(tDecorrido>contador2*10/24){


    inicializacao();
    medicao(angulos, USB);
    roll_medido = angulos[0];
    pitch_medido = angulos [1];

    filtro(tam, fc, roll_medido, roll, &out);
    velocidade_roll = (out - roll)*(PI/180)/(tam/1000);  //em rad/s
    roll = out;
    filtro(tam, fc, pitch_medido, pitch, &out);
    velocidade_pitch = (out - pitch)*(PI/180)/(tam/1000);  //em rad/s
    pitch = out;


    printf("%f %f \n", velocidade_roll, velocidade_pitch);


    if (tDecorrido > 1*1000){
    cmd.write_mov_speed(portHandler, packetHandler, 12, velocidade(1));
	}
    v_medicao_int = cmd.read_mov_speed(portHandler, packetHandler, 12);
    v_medicao = ler_velocidade(v_medicao_int);

	arq3 << v_medicao << endl;


    contador2 = contador2 + tam;
    }

	tFim = clock();
	tDecorrido = ((float)(tFim - tInicio) / (CLOCKS_PER_SEC/1000));
    //cout << tInicio << " " << tFim << " " << tDecorrido << " " << CLOCKS_PER_SEC << endl;
}





    cmd.getch();
    cmd.write_torque(portHandler, packetHandler, BROADCASTID, 0);

return 0;
}
