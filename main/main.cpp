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
    long tid = 1;
    char comando[256];
    
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
    pthread_create(&id, NULL, controle, (void *)tid);

    tInicio = clock();
    tFim = clock();
    tDecorrido = ((float)(tFim - tInicio) / (CLOCKS_PER_SEC/1000));
    //----------------------Loop para condição de parada------------------------------------//
    while(tDecorrido < tsim*1000){
	tFim = clock();
	tDecorrido = ((float)(tFim - tInicio)*10/12 / (CLOCKS_PER_SEC/1000));
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
    //sprintf(comando, "git add -A");
    //system(comando);
    //sprintf(comando, "git commit -m 'Teste Git'");
    //system(comando);
    //sprintf(comando, "git push");
    //system(comando);

return 0;
}
