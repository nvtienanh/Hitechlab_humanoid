#ifndef SAM_PACKET_H
#define SAM_PACKET_H

#include <iostream>

#include <ros/ros.h>

#include <uxa_serial_msgs/receive.h>
#include <uxa_serial_msgs/transmit.h>

#include <uxa_sam_msgs/position_move.h>
#include <uxa_sam_msgs/std_position_move.h>
#include <uxa_sam_msgs/sam_response.h>

using namespace std;

#define _MSG_BUFF_SIZE  20

void SERIAL_SUB_FUNC(const uxa_serial_msgs::receive::ConstPtr &msg);
void SAM_POS_MOVE_FUNC(const uxa_sam_msgs::position_move::ConstPtr &msg);
void SAM_STD_POS_MOVE_FUNC(const uxa_sam_msgs::std_position_move::ConstPtr &msg);
void Init_Message();
void Message_sender(unsigned char *Send_data, int Size);
void SAM_send_position(unsigned char id, unsigned char torq, unsigned char pos);
void SAM_get_state(unsigned char id);
void SAM_send_std_position(unsigned char id, unsigned int pos);
extern ros::ServiceClient sam_driver_client;
#endif // SAM_PACKET_H
