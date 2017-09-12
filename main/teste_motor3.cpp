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

int inicializacao(){

    int USB = open( "/dev/ttyACM0", O_RDWR| O_NOCTTY );
    close(USB);
    USB = open( "/dev/ttyACM0", O_RDWR| O_NOCTTY );

    //USB Handling//
    struct termios tty;
    struct termios tty_old;
    memset (&tty, 0, sizeof tty);

    /* Error Handling */
    if ( tcgetattr ( USB, &tty ) != 0 )
    {
        std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
    }

    /* Save old tty parameters */
    tty_old = tty;
    /* Set Baud Rate */
    cfsetospeed (&tty, (speed_t)B1000000);
    cfsetispeed (&tty, (speed_t)B1000000);
    /* Setting other Port Stuff */
    tty.c_cflag     &=  ~PARENB;            // Make 8n1
    tty.c_cflag     &=  ~CSTOPB;
    tty.c_cflag     &=  ~CSIZE;
    tty.c_cflag     |=  CS8;
    tty.c_cflag     &=  ~CRTSCTS;           // no flow control
    tty.c_cc[VMIN]   =  1;                  // read doesn't block
    tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
    tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines
    /* Make raw */
    cfmakeraw(&tty);
    /* Flush Port, then applies attributes */
    //tcflush( USB, TCIFLUSH );
    if ( tcsetattr ( USB, TCSANOW, &tty ) != 0)
    {
        std::cout << "Error " << errno << " from tcsetattr" << std::endl;
    }

return USB;
}

int medicao(float *angulos, int USB){

    command cmd;
    char buf = '\0';
    char response[1024];

    int n = 0, n_endl, spot = 0, n_written = 0;
    float temp_val[7];
    float xAcel, yAcel, zAcel;

    string temp,temp2;
    size_t inic, fim;


            memset(temp_val, 0, sizeof temp_val);
            n=0;
            spot=0;
            buf='\0';
            n_endl=0;
            memset(response, '\0', sizeof response);

            n_written = write( USB, "1", 1 );

            do
            {
                n = read( USB, &buf, 1 );
            }
            while( buf != '<' && n > 0);
            do
            {
                n = read( USB, &buf, 1 );
                sprintf(&response[spot],"%c",buf);
                spot += n;
            }
            while( buf != '>' && n > 0);
            temp=response;
            inic=temp.find('\n');
            fim=inic;
            while(fim!=string::npos)
            {
                fim=temp.find('\n',inic+1);
                temp2=temp.substr(inic+1,fim-inic-1);
                inic=fim;
                if(n_endl<7)
                {
                    temp_val[n_endl]+=atof(temp2.c_str());
                }
                n_endl++;
            }
            tcflush( USB, TCIFLUSH );


    xAcel = temp_val[1];
    yAcel = -temp_val[0];
    zAcel = temp_val[2];

    angulos[0] = atan(-xAcel/zAcel)*180/PI;   //roll
    angulos[1] = 3+atan(yAcel/(sqrt(xAcel*xAcel+zAcel*zAcel)))*180/PI; //pitch



close(USB);
return 0;
}

void controle(){
    command cmd;
    char *dev_name = (char*)DEVICENAME;
    dynamixel::PacketHandler *packetHandler = dynamixel::PacketHandler::getPacketHandler(1);
    dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler(dev_name);
    dynamixel::GroupSyncWrite groupSyncWrite(portHandler, packetHandler, 30, 2);
    
    
    float tam = 100; //tempo de amostragem em milisegundos
    float out;
    float angulos[2];
    float v_medicao, v_desejada, v_aplicada;
    float roll_medido, pitch_medido, roll = 0, pitch = 0;
    float velocidade_roll, velocidade_pitch;
    float fc = 1;
    float K_roll_R = 1.001, K_roll_L = 1;
    float K_pitch_F = 1.01, K_pitch_R = 1.01;
    float K_UP = 1, K_DOWN = -1;
    float K[13];
    float threshold = 0.02;
    
    int i = 1;
    int v_medicao_int;
    int USB = inicializacao();
    
    

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

    filtro(tam, fc, roll_medido, roll, &out);
    velocidade_roll = (out - roll)*(PI/180)/(tam/1000);  //em rad/s
    roll = out;
    filtro(tam, fc, pitch_medido, pitch, &out);
    velocidade_pitch = (out - pitch)*(PI/180)/(tam/1000);  //em rad/s
    pitch = out;

   if(abs(velocidade_roll)<threshold){velocidade_roll = 0;}
   if(abs(velocidade_pitch)<threshold){velocidade_pitch = 0;}

    printf("%f %f \n", velocidade_roll, velocidade_pitch);
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

    cmd.write_mov_speed(portHandler, packetHandler, i, velocidade(2.2*v_aplicada));

	i++;
    }
	i = 1;
    
    
    
    }

int main(){

    command cmd;
    char *dev_name = (char*)DEVICENAME;
    dynamixel::PacketHandler *packetHandler = dynamixel::PacketHandler::getPacketHandler(1);
    dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler(dev_name);
    dynamixel::GroupSyncWrite groupSyncWrite(portHandler, packetHandler, 30, 2);


    clock_t tInicio, tFim;
    float tDecorrido;
    float tsim = 10; //tempo de simulacao em segundos
    float tam = 100; //tempo de amostragem em milisegundos
    float contador2 = 0;
    int i = 1;


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

    printf("Inicializacao finalizada, pressione qualquer tecla para iniciar \n");
    cmd.getch();



    tInicio = clock();
    tFim = clock();
    tDecorrido = ((float)(tFim - tInicio) / (CLOCKS_PER_SEC/1000));


    //----------------------Loop principal------------------------------------//
    while(tDecorrido < tsim*1000){
	if(tDecorrido>contador2){
    controle();
    contador2 = contador2 + tam;
    }

	tFim = clock();
	tDecorrido = ((float)(tFim - tInicio)*24/10 / (CLOCKS_PER_SEC/1000));
}


    //-----------Fim da simulacao, parar os motores -------------//
    i=1;
    while(i<13){

    cmd.write_mov_speed(portHandler, packetHandler, i, velocidade(0));
	i++;
    }

    //-------------------------Finalize------------------------//
    printf("Sessao finalizada, pressione qualquer tecla para desbloquear \n");
    cmd.getch();
    cmd.write_torque(portHandler, packetHandler, BROADCASTID, 0);

return 0;
}
