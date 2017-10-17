#ifndef COMMAND_H
#define COMMAND_H
#include "dynamixel_sdk.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <termios.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>


class command
{
public:
    void DelayMicrosecondsNoSleep (int delay_us);
    int read_pos(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, uint8_t id);
    void write_pos(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, uint8_t id, uint32_t value);
    int read_mov_speed(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, uint8_t id);
    void write_mov_speed(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, uint8_t id, uint32_t value);
    void write_max_torque(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, uint8_t id, uint32_t value);
    void write_torque_limit(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, uint8_t id, uint32_t value);
    void write_torque(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, uint8_t id, uint8_t value);
    void write_cw_angle_limit(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, uint8_t id, uint32_t value);
    void write_ccw_angle_limit(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, uint8_t id, uint32_t value);
    void write_torque_enable(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, uint8_t id, uint32_t value);
    void calibra(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler, int *calibra);
    void config_ram(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler);
    int getch();
    int kbhit(void);
};

#endif
