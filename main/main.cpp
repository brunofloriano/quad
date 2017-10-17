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
#include <sys/time.h>
#include <sys/select.h>

#include "time.h"
#include "gqueue.h"
#include "gmatlabdatafile.h"
#include "gdatalogger.h"

#define DEVICENAME                      "/dev/ttyUSB0"
#define BROADCASTID			            254
#define TASK_PERIOD_US                  200000
#define PI                              3.14159265
#define ROLL_DIREITA                    1
#define ROLL_ESQUERDA                   0
#define PITCH_FRENTE                    1
#define PITCH_TRAS                      0

GDATALOGGER gDataLogger;

int timer_nr;
timer_t timer;
void timer_start (void);
void timer_stop (void);
void controle (union sigval sigval);

    command cmd;
    char *dev_name = (char*)DEVICENAME;
    dynamixel::PacketHandler *packetHandler = dynamixel::PacketHandler::getPacketHandler(1);
    dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler(dev_name);
    dynamixel::GroupSyncWrite groupSyncWrite(portHandler, packetHandler, 30, 2);


    double out;
    double v_medicao[12], v_desejada, v_aplicada;
    double roll_medido, pitch_medido;
    double velocidade_roll, velocidade_pitch;
    double T = 0;
    double tempo = 0.0;
    static double roll = 0, pitch = 0;
    static double v_1_roll = 0, v_1_pitch = 0;

    float angulos[2];
    float fc = 1;
    float K_roll_R = 1.501, K_roll_L = 1.5;
    float K_pitch_F = 1.41, K_pitch_R = 1.41;
    float K_UP = 0, K_DOWN = 0;//-0.4;
    float K[12];
    float threshold = 0;//0.0024;
    float tam = TASK_PERIOD_US/1000; //tempo de amostragem em milisegundos

    int i = 1;
    int v_medicao_int;
    int queda_roll, queda_pitch;
    volatile int USB;

    struct termios tty;
    struct termios tty_old;
    
    static timestruct_t timestruct;

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

int inicializacao(){

    USB = open( "/dev/ttyACM0", O_RDWR| O_NOCTTY | O_NONBLOCK);
    //close(USB);
    //USB = open( "/dev/ttyACM0", O_RDWR| O_NOCTTY | O_NONBLOCK);

    //USB Handling//
    memset (&tty, 0, sizeof tty);

    /* Error Handling */
    if ( tcgetattr ( USB, &tty ) != 0 )
    {
        //std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
        printf("Erro %d from tcgetattr",(int)errno);
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
        //std::cout << "Error " << errno << " from tcsetattr" << std::endl;
        printf("Erro from tcsetattr \n");
    }

return USB;
}


void controle(union sigval arg){
    
    T = time_gettime(&timestruct);
	time_reset(&timestruct);
	tempo += T;
    
    
    medicao(angulos, USB);
    roll_medido = (double)angulos[0];
    pitch_medido = (double)angulos[1];

    velocidade_roll = (roll_medido - roll)*(PI/180)/(tam/1000);
    velocidade_pitch = (pitch_medido - pitch)*(PI/180)/(tam/1000);

    gDataLogger_InsertVariable(&gDataLogger,(char*) "roll_speed_sf",&velocidade_roll);
    gDataLogger_InsertVariable(&gDataLogger,(char*) "pitch_speed_sf",&velocidade_pitch);

    filtro(tam, fc, velocidade_roll, v_1_roll, &out);
    velocidade_roll = out;
    v_1_roll = out;

    filtro(tam, fc, velocidade_pitch, v_1_pitch, &out);
    velocidade_pitch = out;
    v_1_pitch = out;

    roll = roll_medido;
    pitch = pitch_medido;

   if(abs(velocidade_roll)<threshold){velocidade_roll = 0;}
   if(abs(velocidade_pitch)<threshold){velocidade_pitch = 0;}


    gDataLogger_InsertVariable(&gDataLogger,(char*) "roll_angle",&roll_medido);
    gDataLogger_InsertVariable(&gDataLogger,(char*) "pitch_angle",&pitch_medido);
    gDataLogger_InsertVariable(&gDataLogger,(char*) "roll_speed",&velocidade_roll);
    gDataLogger_InsertVariable(&gDataLogger,(char*) "pitch_speed",&velocidade_pitch);

    if(velocidade_roll>0){
        queda_roll = ROLL_ESQUERDA;
        K_roll_R = 1, K_roll_L = 1.5;
        
        K[1-1] = K_roll_L;
        K[4-1] = K_roll_R;
        K[7-1] = K_roll_R*1.1;
        K[10-1] = K_roll_L;
        }
        else{
        queda_roll = ROLL_DIREITA;
        K_roll_R = 1.8, K_roll_L = 1;
        
        K[1-1] = K_roll_L;
        K[4-1] = K_roll_R;
        K[7-1] = K_roll_R*1.1;
        K[10-1] = K_roll_L;
            }
            
    if(velocidade_pitch>0){
        queda_pitch = PITCH_TRAS;
        K_pitch_F = 1, K_pitch_R = 1.41;

        K[2-1] = K_pitch_R*K_UP;
        K[3-1] = K_pitch_R*K_DOWN;
        K[5-1] = -K_pitch_R*K_UP;
        K[6-1] = -K_pitch_R*K_DOWN;
        K[8-1] = -K_pitch_F*K_UP;
        K[9-1] = -K_pitch_F*K_DOWN;
        K[11-1] = K_pitch_F*K_UP;
        K[12-1] = K_pitch_F*K_DOWN;
        
        }
        else{
        queda_pitch = PITCH_FRENTE;
        K_pitch_F = 1.41, K_pitch_R = 1;

        K[2-1] = K_pitch_R*K_UP;
        K[3-1] = K_pitch_R*K_DOWN;
        K[5-1] = -K_pitch_R*K_UP;
        K[6-1] = -K_pitch_R*K_DOWN;
        K[8-1] = -K_pitch_F*K_UP;
        K[9-1] = -K_pitch_F*K_DOWN;
        K[11-1] = K_pitch_F*K_UP;
        K[12-1] = K_pitch_F*K_DOWN;
            }
            
            
    i = 1;
    while(i<13){
    if(i == 1 || i == 4 || i == 7 || i == 10){
        v_desejada = -K[i-1]*velocidade_roll;
    }
    else{
            v_desejada = -K[i-1]*velocidade_pitch;
        }

    v_medicao_int = cmd.read_mov_speed(portHandler, packetHandler, i);
    v_medicao[i-1] = ler_velocidade(v_medicao_int);
    
    if(i == 3 || i == 6 || i == 9 || i == 12){          //motores pitch down
        v_aplicada = 2.2*(v_desejada- v_medicao[i-1]);
        }
    else{
        if(i == 2 || i == 5 || i == 8 || i == 11){      //motores pitch up
            v_aplicada = 2.2*(v_desejada- v_medicao[i-1]);
            }
    
        else{
        v_aplicada = v_desejada;
     //   if(abs(v_desejada)<0.001){v_aplicada = -2.2*v_medicao[i-1];}
    }

    }
    cmd.write_mov_speed(portHandler, packetHandler, i, velocidade(2.3*v_aplicada));

	i++;
    }

    gDataLogger_InsertVariable(&gDataLogger,(char*) "v_motor1",&v_medicao[0]);
    gDataLogger_InsertVariable(&gDataLogger,(char*) "v_motor2",&v_medicao[1]);
    gDataLogger_InsertVariable(&gDataLogger,(char*) "v_motor3",&v_medicao[2]);
    gDataLogger_InsertVariable(&gDataLogger,(char*) "v_motor4",&v_medicao[3]);
    gDataLogger_InsertVariable(&gDataLogger,(char*) "v_motor5",&v_medicao[4]);
    gDataLogger_InsertVariable(&gDataLogger,(char*) "v_motor6",&v_medicao[5]);
    gDataLogger_InsertVariable(&gDataLogger,(char*) "v_motor7",&v_medicao[6]);
    gDataLogger_InsertVariable(&gDataLogger,(char*) "v_motor8",&v_medicao[7]);
    gDataLogger_InsertVariable(&gDataLogger,(char*) "v_motor9",&v_medicao[8]);
    gDataLogger_InsertVariable(&gDataLogger,(char*) "v_motor10",&v_medicao[9]);
    gDataLogger_InsertVariable(&gDataLogger,(char*) "v_motor11",&v_medicao[10]);
    gDataLogger_InsertVariable(&gDataLogger,(char*) "v_motor12",&v_medicao[11]);

    T = time_gettime(&timestruct);
    tempo += T;
    gDataLogger_InsertVariable(&gDataLogger,(char*) "T",&T);
    gDataLogger_InsertVariable(&gDataLogger,(char*) "tempo",&tempo);
    time_reset(&timestruct);

}

int main(){

    int i;
    char comando[256];

    //----------roll gains-------//
    K[1-1] = K_roll_L;
    K[4-1] = K_roll_R;
    K[7-1] = K_roll_R*1.1;
    K[10-1] = K_roll_L;
    //----------pitch gains-------------//
    K[2-1] = K_pitch_R*K_UP;
    K[3-1] = K_pitch_R*K_DOWN;
    K[5-1] = -K_pitch_R*K_UP;
    K[6-1] = -K_pitch_R*K_DOWN;
    K[8-1] = -K_pitch_F*K_UP;
    K[9-1] = -K_pitch_F*K_DOWN;
    K[11-1] = K_pitch_F*K_UP;
    K[12-1] = K_pitch_F*K_DOWN;

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
    
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "T",(char*) "s",1,1,1000);
    gDataLogger_DeclareVariable(&gDataLogger,(char*) "tempo",(char*) "s",1,1,1000);


    printf("Pressione qualquer tecla para iniciar \n");
    cmd.getch();
    USB = inicializacao();
    timer_start ();
    time_reset(&timestruct);
    printf("Programa em andamento, pressione qualquer tecla para finalizar \n");

    //----------------------Loop para condição de parada------------------------------------//
	while(!kbhit()){
		usleep(20000);
		gDataLogger_IPCUpdate(&gDataLogger); // gerencia IPC
        //gDataLogger_MatfileUpdate(&gDataLogger); // esvazia os buffers no arquivo de log
	}


  //--------------------Fim da simulacao, parar os motores --------------------------------//
    timer_stop ();
	gDataLogger_Close(&gDataLogger);
    close(USB);
    
    i = 1;
    while(i<13){
    cmd.write_mov_speed(portHandler, packetHandler, i, velocidade(0));
	i++;
    }
    //----------------------------------Finalize---------------------------------------//
    cmd.write_torque(portHandler, packetHandler, BROADCASTID, 0);
    printf("Sessao finalizada, exportando dados \n");
    sprintf(comando, "git add -A");
    system(comando);
    sprintf(comando, "git commit -m 'Aquisicao de Dados'");
    system(comando);
    sprintf(comando, "git push");
    system(comando);
return 0;
}
