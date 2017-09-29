#include "include/command.h"
#include "medicao.h"
#include "filtro.h"
#include "controle.h"

#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <iostream>

#include "gqueue.h"
#include "gmatlabdatafile.h"
#include "gdatalogger.h"

#define DEVICENAME                      "/dev/ttyUSB0"
#define BROADCASTID			            254
#define TASK_PERIOD_US  100000

GDATALOGGER gDataLogger;

int timer_nr;
timer_t timer;
double Tglobal;
long int counter = 0;

volatile double tempo = 0.0;


using namespace std;

void timer_start (void)
{
    struct itimerspec itimer = { { 1, 0 }, { 1, 0 } };
    struct sigevent sigev;

    itimer.it_interval.tv_sec=0;
    itimer.it_interval.tv_nsec=TASK_PERIOD_US * 1000; 
    itimer.it_value=itimer.it_interval;

    memset (&sigev, 0, sizeof (struct sigevent));
    sigev.sigev_value.sival_int = timer_nr;
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_notify_attributes = NULL;
    sigev.sigev_notify_function = controle;

//    if (timer_create (CLOCK_MONOTONIC, &sigev, &timer) < 0)
    if (timer_create (CLOCK_REALTIME, &sigev, &timer) < 0)
    {
        fprintf (stderr, "[%d]: %s\n", __LINE__, strerror (errno));
        exit (errno);
    }

    if (timer_settime (timer, 0, &itimer, NULL) < 0)
    {
        fprintf (stderr, "[%d]: %s\n", __LINE__, strerror (errno));
        exit (errno);
    }
}

void timer_stop (void)
{
    if (timer_delete (timer) < 0)
    {
        fprintf (stderr, "[%d]: %s\n", __LINE__, strerror (errno));
        exit (errno);
    }
}   

void controle(union sigval arg){
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
    float threshold = 0;//0.0024;

    int tam = 100; //tempo de amostragem em milisegundos
    int i = 1;
    int v_medicao_int;
    int USB = inicializacao();
    
    double dados;
    
    char motor[64];

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
    
    portHandler->openPort();
    portHandler->getBaudRate();
    cmd.config_ram(portHandler, packetHandler);
    
    inicializacao();
    medicao(angulos, USB);
    roll_medido = angulos[0];
    pitch_medido = angulos [1];
    
    printf("%f %f \n", roll_medido, pitch_medido);

    velocidade_roll = (roll_medido - roll)*(PI/180)/((float)tam/1000);
    velocidade_pitch = (pitch_medido - pitch)*(PI/180)/((float)tam/1000);
    
    dados = (double)velocidade_roll;
    gDataLogger_InsertVariable(&gDataLogger,(char*) "roll_speed_sf",&dados);
    dados = (double)velocidade_pitch;
    gDataLogger_InsertVariable(&gDataLogger,(char*) "pitch_speed_sf",&dados);

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
    
    dados = (double)v_medicao;
    sprintf(motor,"v_motor%d",i);
    gDataLogger_InsertVariable(&gDataLogger,(char*) motor,&dados);
    
    v_aplicada = v_desejada - v_medicao;

    cmd.write_mov_speed(portHandler, packetHandler, i, velocidade(2.3*0));

	i++;
    }
    
    gDataLogger_IPCUpdate(&gDataLogger); // gerencia IPC

}

int main(){
    command cmd;
    char *dev_name = (char*)DEVICENAME;
    dynamixel::PacketHandler *packetHandler = dynamixel::PacketHandler::getPacketHandler(1);
    dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler(dev_name);
    dynamixel::GroupSyncWrite groupSyncWrite(portHandler, packetHandler, 30, 2);

    float tsim = 10; //tempo de simulacao em segundos
    pthread_t id;
    int i = 1;
    char comando[256];
    int n;
    
        //--------------------------Inicializacao------------------------------//
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

    for(i=0; i<12;i++)
    {
         cmd.write_mov_speed(portHandler, packetHandler, i+1, 0);
    }
    
        //----------------------------Data logger-----------------------------------//
	if(!gDataLogger_Init(&gDataLogger,(char*) "gdatalogger/matlabdatafiles/data.mat",NULL)){
		printf("\nErro em gDataLogger_Init\n\n");
		pthread_exit(NULL);
	}
    
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "roll_angle",(char*) "deg",1,1,1000);
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "pitch_angle",(char*) "deg",1,1,1000);
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "roll_speed",(char*) "rad/s",1,1,1000);
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "pitch_speed",(char*) "rad/s",1,1,1000);
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "roll_speed_sf",(char*) "rad/s",1,1,1000);
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "pitch_speed_sf",(char*) "rad/s",1,1,1000);
    
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "v_motor1",(char*) "rad/s",1,1,1000);
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "v_motor2",(char*) "rad/s",1,1,1000);
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "v_motor3",(char*) "rad/s",1,1,1000);
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "v_motor4",(char*) "rad/s",1,1,1000);
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "v_motor5",(char*) "rad/s",1,1,1000);
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "v_motor6",(char*) "rad/s",1,1,1000);
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "v_motor7",(char*) "rad/s",1,1,1000);
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "v_motor8",(char*) "rad/s",1,1,1000);
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "v_motor9",(char*) "rad/s",1,1,1000);
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "v_motor10",(char*) "rad/s",1,1,1000);
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "v_motor11",(char*) "rad/s",1,1,1000);
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "v_motor12",(char*) "rad/s",1,1,1000);


    printf("Pressione qualquer tecla para iniciar \n");
    cmd.getch();
    
    timer_start ();

    //----------------------Loop para condição de parada------------------------------------//
    n = 0;
	while(!kbhit()){
		usleep(20000);
		gDataLogger_IPCUpdate(&gDataLogger); // gerencia IPC
		if(++n>10){
			gDataLogger_MatfileUpdate(&gDataLogger); // esvazia os buffers no arquivo de log
			printf("\n Tempo decorrido: %f s. Tglobal = %f ms. counter = %li",tempo,Tglobal*1000,counter);
			n = 0;
		}
	}

    
  //--------------------Fim da simulacao, parar os motores --------------------------------//
    timer_stop ();
	gDataLogger_Close(&gDataLogger);  
    i = 1;
    while(i<13){

    cmd.write_mov_speed(portHandler, packetHandler, i, velocidade(0));
	i++;
    }
    //----------------------------------Finalize---------------------------------------//
    printf("Sessao finalizada, pressione qualquer tecla para desbloquear \n");
    cmd.getch();
    cmd.write_torque(portHandler, packetHandler, BROADCASTID, 0);
    sprintf(comando, "git add -A");
    system(comando);
    sprintf(comando, "git commit -m 'Teste Git'");
    system(comando);
    sprintf(comando, "git push");
    system(comando);

return 0;
}
