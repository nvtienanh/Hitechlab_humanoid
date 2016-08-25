#ifndef UIC_PACKET_H
#define UIC_PACKET_H

#include <iostream>
#include <ros/ros.h>

#include <uxa_serial_msgs/receive.h>
#include <uxa_serial_msgs/transmit.h>

#include <uxa_uic_msgs/remocon.h>
#include <uxa_uic_msgs/motion.h>

using namespace std;

#define _MSG_BUFF_SIZE  20

#define BTN_A       0x01
#define BTN_B       0x02
#define BTN_LR      0x03
#define BTN_U       0x04
#define BTN_RR      0x05
#define BTN_L       0x06
#define BTN_C       0x07
#define BTN_R       0x08
#define BTN_LA      0x09
#define BTN_D       0x0A
#define BTN_RA      0x0B

#define BTN_1       0x0C
#define BTN_2       0x0D
#define BTN_3       0x0E
#define BTN_4       0x0F
#define BTN_5       0x10
#define BTN_6       0x11
#define BTN_7       0x12
#define BTN_8       0x13
#define BTN_9       0x14
#define BTN_0       0x15

#define BTN_STAR_A  0x16
#define BTN_STAR_B  0x17
#define BTN_STAR_LR 0x18
#define BTN_STAR_U  0x19
#define BTN_STAR_RR 0x1A
#define BTN_STAR_L  0x1B
#define BTN_STAR_C  0x1C
#define BTN_STAR_R  0x1D
#define BTN_STAR_LA 0x1E
#define BTN_STAR_D  0x1F
#define BTN_STAR_RA 0x20

#define BTN_STAR_1  0x21
#define BTN_STAR_2  0x22
#define BTN_STAR_3  0x23
#define BTN_STAR_4  0x24
#define BTN_STAR_5  0x25
#define BTN_STAR_6  0x26
#define BTN_STAR_7  0x27
#define BTN_STAR_8  0x28
#define BTN_STAR_9  0x29
#define BTN_STAR_0  0x2A


void SERIAL_SUB_FUNC(const uxa_serial_msgs::receive::ConstPtr &msg);
void UIC_REMOCON_FUNC(const uxa_uic_msgs::remocon::ConstPtr &msg);
void UIC_MOTION_FUNC(const uxa_uic_msgs::motion::ConstPtr &msg);
void Init_Message();
void Message_sender(unsigned char *Send_data, int Size);
void UIC_send_remote(unsigned char remote);
void UIC_send_pc_control_remote(unsigned char remote);
void PC_control();

#endif // UIC_PACKET_H
