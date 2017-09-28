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

using namespace std;

int main(){
    command cmd;
    char *dev_name = (char*)DEVICENAME;
    dynamixel::PacketHandler *packetHandler = dynamixel::PacketHandler::getPacketHandler(1);
    dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler(dev_name);
    dynamixel::GroupSyncWrite groupSyncWrite(portHandler, packetHandler, 30, 2);

    clock_t tInicio, tFim;
    float tDecorrido;
    float tsim = 10; //tempo de simulacao em segundos
    pthread_t id;
    int i = 1;
    char comando[256];
    
    //-------------Timer Variables--------------//
    clockid_t clockid = CLOCK_REALTIME;
    timer_t timerid;
    struct sigevent se;
    struct itimerspec ts;
    long nanosecs = 100*(1000*1000); //tempo de amostragem em ns
    
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = &timerid;
    se.sigev_notify_function = controle;
    se.sigev_notify_attributes = NULL;
    
    ts.it_value.tv_sec = nanosecs / 1000000000;
    ts.it_value.tv_nsec = nanosecs;
    ts.it_interval.tv_sec = nanosecs / 1000000000;
    ts.it_interval.tv_nsec = nanosecs;
    
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

    printf("Pressione qualquer tecla para iniciar \n");
    cmd.getch();
    
    timer_create(clockid, &se, &timerid);
    timer_settime(timerid, 0, &ts, 0);


    tInicio = clock();
    tFim = clock();
    tDecorrido = ((float)(tFim - tInicio) / (CLOCKS_PER_SEC/1000));
    //----------------------Loop para condição de parada------------------------------------//
    while(tDecorrido < tsim*1000){
	tFim = clock();
	tDecorrido = ((float)(tFim - tInicio) / (CLOCKS_PER_SEC/1000));
}
    
  //-----------Fim da simulacao, parar os motores -------------//
    end();
    pthread_cancel(id);
    i = 1;
    while(i<13){

    cmd.write_mov_speed(portHandler, packetHandler, i, velocidade(0));
	i++;
    }
    //-------------------------Finalize------------------------//
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
