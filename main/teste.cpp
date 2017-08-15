#include <iostream>
#include "medicao6.1.h"

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
