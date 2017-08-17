#include <stdio.h>
float filtro(float tam, float fc, float Ak, float Afk){
    float pi = 3.1415926;
    float T = tam/1000; //tempo de amostragem em segundos
    float a1= 1/(1+T*2*pi*fc), a2 = T*2*pi*fc/(1+T*2*pi*fc);

    printf("\n %f %f %f %f \n", T,fc,Ak,Afk);

    Afk = -a1*Afk + a2*Ak;

return Afk;
}
