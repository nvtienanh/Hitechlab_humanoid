#ifndef UXA_SERIAL_H
#define UXA_SERIAL_H

#include <iostream>

using namespace std;


#include <ros/ros.h>

#include <fcntl.h>
#include <termio.h>
#include <sys/stat.h>

#include <uxa_serial_msgs/receive.h>
#include <uxa_serial_msgs/transmit.h>
#include <uxa_sam_msgs/sam_response.h>

#define _SERIAL_PORT "/dev/ttyUSB0"
#define _SERIAL_BUFF_SIZE    20

//#define _BAUDRATE   B1200
//#define _BAUDRATE   B2400
//#define _BAUDRATE    B4800
//#define _BAUDRATE    B9600
//#define _BAUDRATE    B57600
#define _BAUDRATE    B115200
//#define _BAUDRATE    B230400
//#define _BAUDRATE    B500000
//#define _BAUDRATE    B921600

#define _MSG_BUFF_SIZE  20


int Serial = -1;
unsigned char *msg_buf=new unsigned char [_SERIAL_BUFF_SIZE];

int Init_Serial(const char *Serial_Port);
int Serial_Test(int Serial, unsigned int Test_size);
void Send_Serial_String(int Serial, unsigned char *Trans_chr, int Size);
void Send_Serial_Char(int Serial, unsigned char *Trans_chr);
int Read_Serial_Char(int Serial, unsigned char *Recei_chr);
void rev_func(const uxa_serial_msgs::transmit::ConstPtr &msg);
void dashboard_msg_func(const uxa_serial_msgs::transmit::ConstPtr &msg);

bool response_func(uxa_sam_msgs::sam_response::Request  &req,
         uxa_sam_msgs::sam_response::Response &res);

#endif // UXA_SERIAL_H
