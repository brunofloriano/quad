#include <stdio.h>
#include <math.h>

int filtro(float tam, float fc, double Ak, double Afko, double *Afk){
    double pi = 3.1415926;
    double T = tam/1000; //tempo de amostragem em segundos
    double a1 = 1/(1+T*2*pi*fc), a2 = T*2*pi*fc/(1+T*2*pi*fc);
    
    if ( fpclassify(Afko) == FP_NAN){
        Afko = 0;
        }

    if ( fpclassify(*Afk) == FP_NAN){
        *Afk = 0;
        }

    *Afk = a1*Afko + a2*Ak;
    //printf("%f %f %f %f %f %f %f \n",a1, a2, T,fc,Ak,Afko,*Afk);

return 0;

}
