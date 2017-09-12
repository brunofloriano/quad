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
#include <iostream>

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
    float tsim = 5; //tempo de simulacao em segundos
    pthread_t id;
    int i = 1;
    long tid = 1;
    
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
    printf("Ok1 \n");
    pthread_create(&id, NULL, controle, (void *)tid);
    printf("Ok2 \n");

    tInicio = clock();
    tFim = clock();
    tDecorrido = ((float)(tFim - tInicio)*24/10 / (CLOCKS_PER_SEC/1000));
    printf("Ok3 \n");
    //----------------------Loop para condição de parada------------------------------------//
    while(tDecorrido < tsim*1000){
	tFim = clock();
	tDecorrido = ((float)(tFim - tInicio)*24/10 / (CLOCKS_PER_SEC/1000));
}
    printf("Ok4 \n");
    i = 1;
  //-----------Fim da simulacao, parar os motores -------------//
    while(i<13){

    cmd.write_mov_speed(portHandler, packetHandler, i, velocidade(0));
	i++;
    }
    printf("Ok5 \n");
    //-------------------------Finalize------------------------//
    printf("Sessao finalizada, pressione qualquer tecla para desbloquear \n");
    //cmd.getch();
	printf("Ok6 \n");
    cmd.write_torque(portHandler, packetHandler, BROADCASTID, 0);

return 0;
}
