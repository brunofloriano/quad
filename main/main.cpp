#include "include/command.h"
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions

#define Kp_roll 1
#define Kd_roll 0
#define Kp_pitch 1
#define Kd_pitch 0
#define BAUDRATE                        1000000				// Padrao p/este trabalho
#define DEVICENAME                      "/dev/ttyUSB0"      // Usando o conversor USB
#define dt			                    0.01
#define TASK_PERIOD                     dt*1e9
#define BROADCASTID			            254
#define MOVING_SPEED                    80
#define MAX_TORQUE                      1024
#define TORQUE_LIMIT                    1024
#define PI                              3.14159265

using namespace std;

int main(int argc, char *argv[])
{
    command cmd;
    string marcha="marcha";
    marcha+=argv[1];
    marcha+=".txt";
    ifstream arq(marcha.c_str());
    fstream arq2("calibra.txt");
    ofstream arq3("valores_sensores.txt");

    char *dev_name = (char*)DEVICENAME;
    char buf = '\0';
    char response[1024];
    
    
    dynamixel::PacketHandler *packetHandler = dynamixel::PacketHandler::getPacketHandler(1);
    dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler(dev_name);
    dynamixel::GroupSyncWrite groupSyncWrite(portHandler, packetHandler, 30, 2);

    uint8_t param_goal_position[2];
    uint8_t j=1;

    bool dxl_addparam_result = false;                // addParam result

    int dxl_comm_result = COMM_TX_FAIL;             // Communication result
    int vetor_centro[12];
    int lido[12] = {0,0,0,0,0,0,0,0,0,0,0,0};           //buffer leitura
    int atual[12];
    int anguloscor[12];
    int cor_fat[12]={0,0,0,0,0,0,0,0,0,0,0,0};
    int X;
    int contador=0;
    int n_written = 0;
    int n = 0, n_endl, spot = 0, emer[2]={0,0};
    int USB = open( "/dev/ttyACM0", O_RDWR| O_NOCTTY );
    close(USB);
    USB = open( "/dev/ttyACM0", O_RDWR| O_NOCTTY );
    
    float temp_val[7];
    float h;

    size_t inic, fim;
    
    string temp,temp2;

    vector<float> xAccel;
    vector<float> yAccel;
    vector<float> zAccel;
    vector<float> S1;
    vector<float> S2;
    vector<float> S3;
    vector<float> S4;
    vector<float> roll;
    vector<float> pitch;

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

    cmd.write_torque(portHandler, packetHandler, BROADCASTID, 0);
    cmd.config_ram(portHandler, packetHandler);
    //Calibraçao
    printf("======================Calibracao================================\n==========Mantenha todas a patas na posicao central ==========\n");
    printf("deseja iniciar a calibracao?? 1 para sim \n");
    cin>>X;

    if(X  == 1)
    {
        printf("X=1   Aperte qualquer tecla para iniciar a calibracao\n");
        cmd.getch();
        cmd.calibra(portHandler, packetHandler, vetor_centro);

        printf("\nCalibração concluida\n");
        printf("aperte qualquer tecla para continuar");
        cmd.write_torque(portHandler, packetHandler, BROADCASTID, 1);

        arq2<<vetor_centro[0]<<" "<<vetor_centro[1]<<" "<<vetor_centro[2]<<" ";
        arq2<<vetor_centro[3]<<" "<<vetor_centro[4]<<" "<<vetor_centro[5]<<" ";
        arq2<<vetor_centro[6]<<" "<<vetor_centro[7]<<" "<<vetor_centro[8]<<" ";
        arq2<<vetor_centro[9]<<" "<<vetor_centro[10]<<" "<<vetor_centro[11]<<endl;
        arq2.close();

    }
    else
    {
        arq2>>vetor_centro[0]>>vetor_centro[1]>>vetor_centro[2];
        arq2>>vetor_centro[3]>>vetor_centro[4]>>vetor_centro[5];
        arq2>>vetor_centro[6]>>vetor_centro[7]>>vetor_centro[8];
        arq2>>vetor_centro[9]>>vetor_centro[10]>>vetor_centro[11];
        arq2.close();
    }

    cmd.getch();
    arq>>lido[0]>>lido[1]>>lido[2];
    arq>>lido[3]>>lido[4]>>lido[5];
    arq>>lido[6]>>lido[7]>>lido[8];
    arq>>lido[9]>>lido[10]>>lido[11];
    cmd.write_mov_speed(portHandler, packetHandler, BROADCASTID, MOVING_SPEED);
    cmd.write_max_torque(portHandler, packetHandler, BROADCASTID, MAX_TORQUE);
    cmd.write_torque_limit(portHandler, packetHandler, BROADCASTID, TORQUE_LIMIT);
    cmd.write_torque(portHandler,packetHandler,BROADCASTID,1);
    for(j=1; j<13; j++)
    {
        anguloscor[j-1]=lido[j-1]+vetor_centro[j-1]+cor_fat[j-1];
        if(lido[j-1]+cor_fat[j-1]>280)anguloscor[j-1]=vetor_centro[j-1]+280;
        else if(lido[j-1]+cor_fat[j-1]<-280)anguloscor[j-1]=vetor_centro[j-1]-280;
        param_goal_position[0] = DXL_LOBYTE(anguloscor[j-1]);
        param_goal_position[1] = DXL_HIBYTE(anguloscor[j-1]);
        dxl_addparam_result = groupSyncWrite.addParam(j, param_goal_position);
        if (dxl_addparam_result != true)
        {
            fprintf(stderr, "[ID:%03d] groupSyncWrite addparam failed", j);
            return 0;
        }
    }

    do
    {
        dxl_comm_result = groupSyncWrite.txPacket();
    }while(dxl_comm_result != COMM_SUCCESS);

    // Clear syncwrite parameter storage
    groupSyncWrite.clearParam();
    do
    {
        for(j=1; j<13; j++)
        {
            atual[j-1]=cmd.read_pos(portHandler,packetHandler,j);
        }
    }while(abs(atual[0]-anguloscor[0])>10||abs(atual[1]-anguloscor[1])>10||abs(atual[2]-anguloscor[2])>10||
        abs(atual[3]-anguloscor[3])>10||abs(atual[4]-anguloscor[4])>10||abs(atual[5]-anguloscor[5])>10||
        abs(atual[6]-anguloscor[6])>10||abs(atual[7]-anguloscor[7])>10||abs(atual[8]-anguloscor[8])>10||
        abs(atual[9]-anguloscor[9])>10||abs(atual[10]-anguloscor[10])>10||abs(atual[11]-anguloscor[11])>10);

    cout<<"\nPosiçao inicial\n";
    cout<<"aperte qualquer tecla para continuar\n";
    cmd.getch();

    //laço principal
    while(!arq.eof())
    {
        arq>>lido[0]>>lido[1]>>lido[2];
        arq>>lido[3]>>lido[4]>>lido[5];
        arq>>lido[6]>>lido[7]>>lido[8];
        arq>>lido[9]>>lido[10]>>lido[11];
        for(j=1; j<13; j++)
        {
            anguloscor[j-1]=lido[j-1]+vetor_centro[j-1]+cor_fat[j-1];
            if(lido[j-1]+cor_fat[j-1]>280)anguloscor[j-1]=vetor_centro[j-1]+280;
            else if(lido[j-1]+cor_fat[j-1]<-280)anguloscor[j-1]=vetor_centro[j-1]-280;
            cmd.write_torque(portHandler,packetHandler,j,1);
            param_goal_position[0] = DXL_LOBYTE(anguloscor[j-1]);
            param_goal_position[1] = DXL_HIBYTE(anguloscor[j-1]);
            dxl_addparam_result = groupSyncWrite.addParam(j, param_goal_position);
            if (dxl_addparam_result != true)
            {
                fprintf(stderr, "[ID:%03d] groupSyncWrite addparam failed", j);
                return 0;
            }
        }

        do
        {
            dxl_comm_result = groupSyncWrite.txPacket();
            //if (dxl_comm_result != COMM_SUCCESS) packetHandler->printTxRxResult(dxl_comm_result);
        }while(dxl_comm_result != COMM_SUCCESS);

        //tcflush( USB, TCIFLUSH );
        // Clear syncwrite parameter storage
        groupSyncWrite.clearParam();
        do
        {
            for(j=1; j<13; j++)
            {
                atual[j-1]=cmd.read_pos(portHandler,packetHandler,j);
            }
        }while(abs(atual[0]-anguloscor[0])>10||abs(atual[1]-anguloscor[1])>10||abs(atual[2]-anguloscor[2])>10||
            abs(atual[3]-anguloscor[3])>10||abs(atual[4]-anguloscor[4])>10||abs(atual[5]-anguloscor[5])>10||
            abs(atual[6]-anguloscor[6])>10||abs(atual[7]-anguloscor[7])>10||abs(atual[8]-anguloscor[8])>10||
            abs(atual[9]-anguloscor[9])>10||abs(atual[10]-anguloscor[10])>10||abs(atual[11]-anguloscor[11])>10);

        
        if(contador%10==0){
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
            roll.push_back(atan(-xAccel[contador%10]/zAccel[contador%10])*180/PI);
            pitch.push_back(3+atan(yAccel[contador%10]/(sqrt(xAccel[contador%10]*xAccel[contador%10]+zAccel[contador%10]*zAccel[contador%10])))*180/PI);
        // cout<<xAccel[contador%10]<<" "<<yAccel[contador%10]<<" "<<zAccel[contador%10]<<" "<<S1[contador%10]<<" "<<S2[contador%10]<<" "<<S3[contador%10]<<" "<<S4[contador%10]<<" "<<roll[contador%10]<<" "<<pitch[contador%10]<<endl;
            arq3<<xAccel[contador%10]<<" "<<yAccel[contador%10]<<" "<<zAccel[contador%10]<<" "<<S1[contador%10]<<" "<<S2[contador%10]<<" "<<S3[contador%10]<<" "<<S4[contador%10]<<" "<<roll[contador%10]<<" "<<pitch[contador%10]<<endl;

        //cout<<roll[contador%10]<<" "<<lido[0]*0.29<<" "<<abs(roll[contador%10]-lido[0]*0.29)<<endl;
            if(atoi(argv[1])==4){
                if (abs(roll[contador%10])>2)
                {
                    h=11*(Kp_roll*tan((roll[contador%10]*PI/180))+Kd_roll*tan(((roll[contador%10]-roll[contador%10-1])*PI/180)));
                    if ((abs(cos(lido[1]*0.29*PI/180)+h/10))>1||
                        (abs(cos(lido[4]*0.29*PI/180)-h/10))>1||
                        (abs(cos(lido[7]*0.29*PI/180)-h/10))>1||
                        (abs(cos(lido[10]*0.29*PI/180)+h/10))>1)
                    {
                        memset(cor_fat, 0, sizeof cor_fat);
                //cout<<"EMERGENCIA roll\n";
                        if ((abs(cos(lido[1]*0.29*PI/180)+h/10))>1||
                            (abs(cos(lido[10]*0.29*PI/180)+h/10))>1)
                        {
                            emer[0]=1;
                        }
                        else{
                            emer[0]=-1;
                        }
                    }
                    else{
                        cor_fat[1]=cor_fat[1]+lido[1]/abs(lido[1])*(acos(cos(lido[1]*0.29*PI/180)+h/10)*180/(PI*0.29)-abs(lido[1]));
                        cor_fat[2]=-cor_fat[1];
                        cor_fat[4]=cor_fat[4]+lido[4]/abs(lido[4])*(acos(cos(lido[4]*0.29*PI/180)-h/10)*180/(PI*0.29)-abs(lido[4]));
                        cor_fat[5]=-cor_fat[4];
                        cor_fat[7]=cor_fat[7]+lido[7]/abs(lido[7])*(acos(cos(lido[7]*0.29*PI/180)-h/10)*180/(PI*0.29)-abs(lido[7]));
                        cor_fat[8]=-cor_fat[7];
                        cor_fat[10]=cor_fat[10]+lido[10]/abs(lido[10])*(acos(cos(lido[10]*0.29*PI/180)+h/10)*180/(PI*0.29)-abs(lido[10]));
                        cor_fat[11]=-cor_fat[10];
                    }
                }
                if (abs(pitch[contador%10])>2)
                {
                    h=13.5*(Kp_pitch*tan((roll[contador%10]*PI/180))+Kd_pitch*tan(((roll[contador%10]-roll[contador%10-1])*PI/180)));
                    if ((abs(cos(lido[1]*0.29*PI/180)+h/10))>1||
                        (abs(cos(lido[4]*0.29*PI/180)+h/10))>1||
                        (abs(cos(lido[7]*0.29*PI/180)-h/10))>1||
                        (abs(cos(lido[10]*0.29*PI/180)-h/10))>1)
                    {
                        memset(cor_fat, 0, sizeof cor_fat);
                //cout<<"EMERGENCIA pitch\n";
                        if ((abs(cos(lido[1]*0.29*PI/180)+h/10))>1||
                            (abs(cos(lido[4]*0.29*PI/180)+h/10))>1)
                        {
                            emer[1]=1;
                        }
                        else{
                            emer[1]=-1;
                        }
                    }
                    else{
                        cor_fat[1]=cor_fat[1]+lido[1]/abs(lido[1])*(acos(cos(lido[1]*0.29*PI/180)+h/10)*180/(PI*0.29)-abs(lido[1]));
                        cor_fat[2]=-cor_fat[1];
                        cor_fat[4]=cor_fat[4]+lido[4]/abs(lido[4])*(acos(cos(lido[4]*0.29*PI/180)+h/10)*180/(PI*0.29)-abs(lido[4]));
                        cor_fat[5]=-cor_fat[4];
                        cor_fat[7]=cor_fat[7]+lido[7]/abs(lido[7])*(acos(cos(lido[7]*0.29*PI/180)-h/10)*180/(PI*0.29)-abs(lido[7]));
                        cor_fat[8]=-cor_fat[7];
                        cor_fat[10]=cor_fat[10]+lido[10]/abs(lido[10])*(acos(cos(lido[10]*0.29*PI/180)-h/10)*180/(PI*0.29)-abs(lido[10]));
                        cor_fat[11]=-cor_fat[10];
                    }
                }
                if(emer[0]==0 && emer[1]==1){
                    cout<<"Queda pra frente\n";
                    emer[0]=0;
                    emer[1]=0;
                }
                else if(emer[0]==0 && emer[1]==-1){
                    cout<<"Queda pra tras\n";
                    emer[0]=0;
                    emer[1]=0;
                }
                else if(emer[0]==1 && emer[1]==0){
                    cout<<"Queda pra direita\n";
                    emer[0]=0;
                    emer[1]=0;
                }
                else if(emer[0]==1 && emer[1]==1){
                    cout<<"Queda na pata 1\n";
                    emer[0]=0;
                    emer[1]=0;
                }
                else if(emer[0]==1 && emer[1]==-1){
                    cout<<"Queda na pata 4\n";
                    emer[0]=0;
                    emer[1]=0;
                }
                else if(emer[0]==-1 && emer[1]==0){
                    cout<<"Queda pra esquerda\n";
                    emer[0]=0;
                    emer[1]=0;
                }
                else if(emer[0]==-1 && emer[1]==1){
                    cout<<"Queda na pata 2\n";
                    emer[0]=0;
                    emer[1]=0;
                }
                else if(emer[0]==-1 && emer[1]==-1){
                    cout<<"Queda na pata 3\n";
                    emer[0]=0;
                    emer[1]=0;
                }
                cout<<"[ "<<cor_fat[0]<<" "<<cor_fat[1]<<" "<<cor_fat[2]<<" "<<cor_fat[3]<<" "<<cor_fat[4]<<" "<<cor_fat[5];
                cout<<" "<<cor_fat[6]<<" "<<cor_fat[7]<<" "<<cor_fat[8]<<" "<<cor_fat[9]<<" "<<cor_fat[10]<<" "<<cor_fat[11]<<" ]\n";
            }
        }
        contador++;
    }
    arq.close();
    arq3.close();
    getchar();
    cmd.write_torque(portHandler, packetHandler, BROADCASTID, (uint8_t) 0);
    // Close port
    portHandler->closePort();
    close(USB);
    return 0;
}
