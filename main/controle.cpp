#ifndef CONTROLE_CPP
#define CONTROLE_CPP

#include "include/command.h"
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include "medicao.h"
#include "filtro.h"
#include <stdio.h>

#include "gqueue.h"
#include "gmatlabdatafile.h"
#include "gdatalogger.h"

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

#ifndef gDataLogger
GDATALOGGER gDataLogger;
#endif

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

void *controle(void *id){
    command cmd;
    char *dev_name = (char*)DEVICENAME;
    dynamixel::PacketHandler *packetHandler = dynamixel::PacketHandler::getPacketHandler(1);
    dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler(dev_name);
    dynamixel::GroupSyncWrite groupSyncWrite(portHandler, packetHandler, 30, 2);

    float out;
    float angulos[2];
    float v_medicao, v_desejada, v_aplicada;
    float roll_medido, pitch_medido, roll = 0, pitch = 0;
    float velocidade_roll, velocidade_pitch, v_1_roll = 0, v_1_pitch = 0;
    float fc = 1;
    float K_roll_R = 1.001, K_roll_L = 1;
    float K_pitch_F = 1.01, K_pitch_R = 1.01;
    float K_UP = 1, K_DOWN = -1;
    float K[13];
    float threshold = 0.02;

    int tam = 100; //tempo de amostragem em milisegundos
    int i = 1;
    int v_medicao_int;
    int success;
    int USB = inicializacao();
    
    double dados;

    angulos[0] = 0;
    angulos[1] = 0;
    K[0] = 0;
    //----------roll gains-------//
    K[1] = K_roll_L;
    K[4] = K_roll_R;
    K[7] = K_roll_R*1.1;
    K[10] = K_roll_L;
    //----------pitch gains-------------//
    K[2] = K_pitch_R*K_UP;
    K[3] = K_pitch_R*K_DOWN;
    K[5] = -K_pitch_R*K_UP;
    K[6] = -K_pitch_R*K_DOWN;
    K[8] = -K_pitch_F*K_UP;
    K[9] = -K_pitch_F*K_DOWN;
    K[11] = K_pitch_F*K_UP;
    K[12] = K_pitch_F*K_DOWN;
    
    //--------Data logger-------//
	if(!gDataLogger_Init(&gDataLogger,(char*) "gdatalogger/matlabdatafiles/data.mat",NULL)){
		printf("\nErro em gDataLogger_Init\n\n");
		pthread_exit(NULL);
	}
    
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "roll_angle",(char*) "deg",1,1,1000);
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "pitch_angle",(char*) "deg",1,1,1000);
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "roll_speed",(char*) "rad/s",1,1,1000);
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "pitch_speed",(char*) "rad/s",1,1,1000);

    portHandler->openPort();
    portHandler->getBaudRate();
    cmd.config_ram(portHandler, packetHandler);
    inicializacao();
    
while(1){


    inicializacao();
    medicao(angulos, USB);
    roll_medido = angulos[0];
    pitch_medido = angulos [1];

    velocidade_roll = (roll_medido - roll)*(PI/180)/((float)tam/1000);
    velocidade_pitch = (pitch_medido - pitch)*(PI/180)/((float)tam/1000);

    filtro((float)tam, fc, velocidade_roll, v_1_roll, &out);
    velocidade_roll = out;
    v_1_roll = out;

    filtro((float)tam, fc, velocidade_pitch, v_1_pitch, &out);
    velocidade_pitch = out;
    v_1_pitch = out;

    roll = roll_medido;
    pitch = pitch_medido;


   if(abs(velocidade_roll)<threshold){velocidade_roll = 0;}
   if(abs(velocidade_pitch)<threshold){velocidade_pitch = 0;}
   
    dados = (double)roll_medido;
    gDataLogger_InsertVariable(&gDataLogger,(char*) "roll_angle",&dados);
    dados = (double)pitch_medido;
    gDataLogger_InsertVariable(&gDataLogger,(char*) "pitch_angle",&dados);
    dados = (double)velocidade_roll;
    gDataLogger_InsertVariable(&gDataLogger,(char*) "roll_speed",&dados);
    dados = (double)velocidade_pitch;
    gDataLogger_InsertVariable(&gDataLogger,(char*) "pitch_speed",&dados);

    i = 1;
    while(i<13){
    if(i == 1 || i == 4 || i == 7 || i == 10){
        v_desejada = -K[i]*velocidade_roll;
    }
    else{
            v_desejada = -K[i]*velocidade_pitch;
        }

    v_medicao_int = cmd.read_mov_speed(portHandler, packetHandler, i);
    v_medicao = ler_velocidade(v_medicao_int);
    v_aplicada = v_desejada - v_medicao;

    cmd.write_mov_speed(portHandler, packetHandler, i, velocidade(0*v_aplicada));

	i++;
    }
    
    gDataLogger_IPCUpdate(&gDataLogger); // gerencia IPC
    usleep(tam*1000); //sleep for microseconds
}
    return 0;
}
#endif
