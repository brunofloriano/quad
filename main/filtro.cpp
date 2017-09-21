#include <stdio.h>
float filtro(float tam, float fc, float Ak, float Afko, float *Afk){
    float pi = 3.1415926;
    float T = tam/1000; //tempo de amostragem em segundos
    float a1 = 1/(1+T*2*pi*fc), a2 = T*2*pi*fc/(1+T*2*pi*fc);

    *Afk = a1*Afko + a2*Ak;
    printf("%f \n",a1, a2, T,fc,Ak,Afko,*Afk);

return 0;

}
