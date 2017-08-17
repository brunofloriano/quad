#ifndef COMMAND_cpp
#define COMMAND_cpp
#include "include/command.h"

void command::DelayMicrosecondsNoSleep (int delay_us)
{
    long int start_time;
    long int time_difference;
    struct timespec gettime_now;

    clock_gettime(CLOCK_REALTIME, &gettime_now);
    start_time = gettime_now.tv_nsec;		//Get nS value
    while (1)
    {
        clock_gettime(CLOCK_REALTIME, &gettime_now);
        time_difference = gettime_now.tv_nsec - start_time;
        if (time_difference < 0)
            time_difference += 1000000000;				//(Rolls over every 1 second)
        if (time_difference > (delay_us * 1000))		//Delay for # nS
            break;
    }
}

int command::read_pos(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, uint8_t id)
{

    uint8_t dxl_error = 0;
    int     dxl_comm_result = COMM_TX_FAIL;
    int16_t value16   = 0;
    do{
        dxl_comm_result=packetHandler->read2ByteTxRx(portHandler, id, 36,(uint16_t*)&value16,&dxl_error);  // Endereço p/posiçao atual 36,37
        if (dxl_comm_result == COMM_SUCCESS)
        {
            if (dxl_error != 0) packetHandler->printRxPacketError(dxl_error);
            return(value16);
        }
    }while(dxl_comm_result != COMM_SUCCESS);
    //packetHandler->printTxRxResult(dxl_comm_result);
    return(-1);
}

void command::write_pos(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, uint8_t id, uint32_t value)
{
    uint8_t dxl_error = 0;
    int dxl_comm_result = COMM_TX_FAIL;
    do{
        dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, id, 30, (uint16_t)value, &dxl_error);
        if (dxl_comm_result == COMM_SUCCESS)
        {
            if (dxl_error != 0) packetHandler->printRxPacketError(dxl_error);
            //fprintf(stderr, "\n Success to write\n\n");
        }
    }while(dxl_comm_result != COMM_SUCCESS);
    //else
    //{
    //    packetHandler->printTxRxResult(dxl_comm_result);
    //fprintf(stderr, "\n Fail to write! \n\n");
    //}
}

void command::write_mov_speed(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, uint8_t id, uint32_t value)
{
    uint8_t dxl_error = 0;
    int dxl_comm_result = COMM_TX_FAIL;

    do{
        dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, id, 32, (uint16_t)value, &dxl_error);
        if (dxl_comm_result == COMM_SUCCESS)
        {
            if (dxl_error != 0) packetHandler->printRxPacketError(dxl_error);
            //fprintf(stderr, "\n Success to write\n\n");
        }
    }while(dxl_comm_result != COMM_SUCCESS);
    //else
    //{
    //    packetHandler->printTxRxResult(dxl_comm_result);
    //fprintf(stderr, "\n Fail to write! \n\n");
    //}
}

void command::write_max_torque(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, uint8_t id, uint32_t value)
{
    uint8_t dxl_error = 0;
    int dxl_comm_result = COMM_TX_FAIL;

    do{
        dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, id, 14, (uint16_t)value, &dxl_error);
        if (dxl_comm_result == COMM_SUCCESS)
        {
            if (dxl_error != 0) packetHandler->printRxPacketError(dxl_error);
            //fprintf(stderr, "\n Success to write\n\n");
        }
    }while(dxl_comm_result != COMM_SUCCESS);
    //else
    //{
    //    packetHandler->printTxRxResult(dxl_comm_result);
    //fprintf(stderr, "\n Fail to write! \n\n");
    //}
}

void command::write_torque_limit(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, uint8_t id, uint32_t value)
{
    uint8_t dxl_error = 0;
    int dxl_comm_result = COMM_TX_FAIL;

    do{
        dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, id, 34, (uint16_t)value, &dxl_error);
        if (dxl_comm_result == COMM_SUCCESS)
        {
            if (dxl_error != 0) packetHandler->printRxPacketError(dxl_error);
            //fprintf(stderr, "\n Success to write\n\n");
        }
    }while(dxl_comm_result != COMM_SUCCESS);
    //else
    //{
    //    packetHandler->printTxRxResult(dxl_comm_result);
    //fprintf(stderr, "\n Fail to write! \n\n");
    //}
}

void command::write_torque(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, uint8_t id, uint8_t value)
{
    int dxl_comm_result = COMM_TX_FAIL;
    uint8_t dxl_error = 0;

    do{
        dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, id, 24, value, &dxl_error);
    }while(dxl_comm_result != COMM_SUCCESS);
    //if (dxl_comm_result != COMM_SUCCESS)
    //{
    //    packetHandler->printTxRxResult(dxl_comm_result);
    //}
    //else if (dxl_error != 0)
    //{
    //    packetHandler->printRxPacketError(dxl_error);
    //}
}

void command::calibra(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, int *calibra)
{
    for(int i=0; i<12;i++)
    {
        calibra[i]=read_pos(portHandler, packetHandler, i+1);
        printf("O servo de endereco %d tem centro em %d\n",i+1,calibra[i]);
    }
}

void command::config_ram(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler)
{
	int dxl_comm_result = COMM_TX_FAIL;
    uint8_t dxl_error = 0;

    dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, 254, 26, (uint8_t)0, &dxl_error);
    dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, 254, 27, (uint8_t)0, &dxl_error);
    dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, 254, 28, (uint8_t)8, &dxl_error);
    dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, 254, 29, (uint8_t)8, &dxl_error);
    dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, 254, 48, (uint16_t)0, &dxl_error);
}

int command::getch()
{
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;

}

int command::kbhit(void)
{

    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;

}

#endif
