#include "include/command.h"
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <time.h>
#include "medicao.h"

#define PI    3.14159265

using namespace std;

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

int medicao(float *angulos, clock_t tsim, clock_t tam, int USB){

    command cmd;
    char buf = '\0';
    char response[1024];

    int n = 0, n_endl, spot = 0, n_written = 0;
    int contador2 = 0;
    float temp_val[7];
    float xAcel, yAcel, zAcel;

    clock_t tInicio, tFim, tDecorrido;

    string temp,temp2;
    size_t inic, fim;

    tInicio = clock();
    tFim = clock();
    tDecorrido = ((tFim - tInicio) / (CLOCKS_PER_SEC / 1000));
    //Loop
    while(tDecorrido<tsim*1000){
	if(tDecorrido>contador2){
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


           cout << angulos[0] << " " << angulos[1] << endl;
            contador2 = contador2 + tam;
            }
	tFim = clock();
	tDecorrido = ((tFim - tInicio) / (CLOCKS_PER_SEC / 1000));
}
close(USB);
return 0;
}
