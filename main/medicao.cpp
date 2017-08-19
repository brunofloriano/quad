#include "include/command.h"
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <time.h>
#include "medicao.h"

#define PI    3.14159265

using namespace std;

int medicao(clock_t tsim, clock_t tam){

    command cmd;
    char buf = '\0';
    char response[1024];

    int USB = open( "/dev/ttyACM0", O_RDWR| O_NOCTTY );
    close(USB);
    USB = open( "/dev/ttyACM0", O_RDWR| O_NOCTTY );

    int n = 0, n_endl, spot = 0, n_written = 0;
    int contador=0, contador2 = 0, contador3 = 0;
    int X = 1;
    char C;
    float temp_val[7];
    float xAcel, yAcel, zAcel;
    float angulos;

    clock_t tInicio, tFim, tDecorrido, t;

    string temp,temp2;
    size_t inic, fim;

    vector<float> xAccel;
    vector<float> yAccel;
    vector<float> zAccel;
    vector<float> S1;
    vector<float> S2;
    vector<float> S3;
    vector<float> S4;
    vector<float> roll;
    vector<float> pitch;

    ofstream arq4("valores_acelerometro.txt");

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


    cout << "\n Pressione qualquer tecla para iniciar \n" << endl;
    cin >> C;

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


            xAccel.push_back(temp_val[1]);
            yAccel.push_back(-temp_val[0]);
            zAccel.push_back(temp_val[2]);
            S1.push_back(temp_val[3]);
            S2.push_back(temp_val[4]);
            S3.push_back(temp_val[5]);
            S4.push_back(temp_val[6]);
            roll.push_back(atan(-xAccel[contador3%10]/zAccel[contador3%10])*180/PI);
            pitch.push_back(3+atan(yAccel[contador3%10]/(sqrt(xAccel[contador3%10]*xAccel[contador3%10]+zAccel[contador3%10]*zAccel[contador3%10])))*180/PI);

        //cout<<roll[contador%10]<<" "<<lido[0]*0.29<<" "<<abs(roll[contador%10]-lido[0]*0.29)<<endl;



           cout << roll[contador3%10] << " " << pitch[contador3%10] << endl;
          // cout<<xAccel[contador%10]<<" "<<yAccel[contador%10]<<" "<<zAccel[contador%10]<<" "<<S1[contador%10]<<" "<<S2[contador%10]<<" "<<S3[contador%10]<<" "<<S4[contador%10]<<" "<<roll[contador%10]<<" "<<pitch[contador%10]<<endl;
            arq4<< roll[contador3%10]<<" "<<pitch[contador3%10]<<endl;
            contador2 = contador2 + tam;
	    contador3++;
            }
	contador++;
	tFim = clock();
	tDecorrido = ((tFim - tInicio) / (CLOCKS_PER_SEC / 1000));
}
close(USB);
return 0;
}
