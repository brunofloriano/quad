#include "medicao6.1.cpp"

#ifndef PI
#define PI    3.14159265
#endif // PI

using namespace std;

int main(){

    float angulos[2];
    float roll, pitch;

    medicao_angulos(angulos);
    roll = angulos[0];
    pitch = angulos[1];

    cout << roll << " " << pitch << " \n" <<endl;


return 0;
}
