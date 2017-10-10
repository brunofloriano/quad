#ifndef MEDICAO_CPP
#define MEDICAO_CPP

#include "include/command.h"
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <sys/select.h>

#define PI    3.14159265

using namespace std;

int medicao(float *angulos, int USB){

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

    if ( fpclassify(angulos[0]) == FP_NAN){
        angulos[0] = 0;
        }

    if ( fpclassify(angulos[1]) == FP_NAN){
        angulos[1] = 0;
        }


//close(USB);
return 0;
}

#endif // MEDICAO_CPP
