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

int main(){

    command cmd;
    char *dev_name = (char*)DEVICENAME;
    dynamixel::PacketHandler *packetHandler = dynamixel::PacketHandler::getPacketHandler(1);
    dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler(dev_name);
    dynamixel::GroupSyncWrite groupSyncWrite(portHandler, packetHandler, 30, 2);

    clock_t tsim = 10; //tempo de simulacao em segundos
    clock_t tam = 100; //tempo de amostragem em microsegundos
    clock_t tInicio, tFim, tDecorrido;
    float v = -2.5;
    float out;
    float angulos[2];
    float roll_medido, pitch_medido, roll = 0, pitch = 0;
    float velocidade_roll, velocidade_pitch;
    float fc = 1;
    float K1 = 1000,K4 = 1, K7 = 1, K10 = 1;    //roll gains
    int contador2 = 0;
    int USB = inicializacao();




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
    cmd.getch();

    for(int i=0; i<12;i++)
    {
        cmd.write_pos(portHandler, packetHandler, i+1, cmd.read_pos(portHandler, packetHandler, i+1));

    }


    tInicio = clock();
    tFim = clock();
    tDecorrido = ((tFim - tInicio) / (CLOCKS_PER_SEC / 1000));
    //Loop
    while(tDecorrido<tsim*1000){
	if(tDecorrido>contador2){


    inicializacao();
    medicao(angulos, USB);
    roll_medido = angulos[0];
    pitch_medido = angulos [1];

    filtro(tam, fc, roll_medido, roll, &out);
    velocidade_roll = (out - roll)*(PI/180)/(tam*1000);  //em rad/s
    roll = out;
    filtro(tam, fc, pitch_medido, pitch, &out);
    velocidade_pitch = (out - pitch)*(PI/180)/(tam*1000);  //em rad/s
    pitch = out;


    printf("%f %f \n", velocidade_roll, velocidade_pitch);
    cmd.write_mov_speed(portHandler, packetHandler, 1, velocidade(-K1*velocidade_roll));
    cmd.write_mov_speed(portHandler, packetHandler, 4, velocidade(-K4*velocidade_roll));
    cmd.write_mov_speed(portHandler, packetHandler, 7, velocidade(-K7*velocidade_roll));
    cmd.write_mov_speed(portHandler, packetHandler, 10, velocidade(-K10*velocidade_roll));

    contador2 = contador2 + tam;
    }
	tFim = clock();
	tDecorrido = ((tFim - tInicio) / (CLOCKS_PER_SEC / 1000));
}





    cmd.getch();
    cmd.write_torque(portHandler, packetHandler, BROADCASTID, 0);

return 0;
}