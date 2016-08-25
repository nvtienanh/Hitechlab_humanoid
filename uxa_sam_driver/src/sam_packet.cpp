#include "../include/sam_packet.h"

uxa_serial_msgs::transmit serial_pub_msg;

ros::Publisher uxa_serial_pub, sam_state_pub;
ros::Subscriber uxa_serial_sub, sam_driver_position_move_sub, sam_driver_std_position_move_sub;

unsigned char Send_buf[_MSG_BUFF_SIZE];

using namespace std;

void SERIAL_SUB_FUNC(const uxa_serial_msgs::receive::ConstPtr &msg)
{
    ROS_INFO("SAM recieve msg : 0x%x",msg->rx_data);
//    sam_state_pub.publish(msg->rx_data);
}

void SAM_POS_MOVE_FUNC(const uxa_sam_msgs::position_move::ConstPtr &msg)
{
    SAM_send_position(msg->id, msg->torqlevel, msg->pos);
}

void SAM_STD_POS_MOVE_FUNC(const uxa_sam_msgs::std_position_move::ConstPtr &msg)
{
    SAM_send_std_position(msg->id, msg->pos14);
}

void Init_Message()
{
    ros::NodeHandle n;

    uxa_serial_pub = n.advertise<uxa_serial_msgs::transmit>
            ("uxa_serial_subscriber", _MSG_BUFF_SIZE);

    sam_state_pub = n.advertise<uxa_serial_msgs::transmit>
            ("sam_state_subscriber", 2);

    uxa_serial_sub = n.subscribe<uxa_serial_msgs::receive>
            ("uxa_serial_publisher", _MSG_BUFF_SIZE, SERIAL_SUB_FUNC);

    sam_driver_position_move_sub = n.subscribe<uxa_sam_msgs::position_move>
            ("sam_driver_position_move", _MSG_BUFF_SIZE, SAM_POS_MOVE_FUNC);

    sam_driver_std_position_move_sub = n.subscribe<uxa_sam_msgs::std_position_move>
            ("sam_driver_std_position_move", _MSG_BUFF_SIZE, SAM_STD_POS_MOVE_FUNC);

}

void Message_sender(unsigned char *Send_data, int Size)
{
    for(char cnt = 0; cnt < Size; cnt++)
    {
        serial_pub_msg.tx_data = Send_data[cnt];
        uxa_serial_pub.publish(serial_pub_msg);
    }
}

void SAM_send_position(unsigned char id, unsigned char torq, unsigned char pos)
{
    unsigned char cnt = 0;

    Send_buf[cnt++] = 0xFF;
    Send_buf[cnt++] = id | (torq << 5);
    Send_buf[cnt++] = pos;
    Send_buf[cnt++] = (Send_buf[1]^Send_buf[2]) & 0x7F;

    Message_sender(Send_buf, cnt);
}

void SAM_send_std_position(unsigned char id, unsigned int pos14)
{
    unsigned char cnt = 0;

    Send_buf[cnt++] = 0xFF;
    Send_buf[cnt++] = (unsigned char)(7 << 5);
    Send_buf[cnt++] = 200;
    Send_buf[cnt++] = id;
    Send_buf[cnt++] = pos14 >> 7;
    Send_buf[cnt++] = (unsigned char)(pos14 & 0x7F);
    Send_buf[cnt++] = (Send_buf[1]^Send_buf[2]^Send_buf[3]^Send_buf[4]^Send_buf[5]) & 0x7F;

    Message_sender(Send_buf, cnt);
}

void SAM_get_state(unsigned char id)
{
    unsigned char cnt = 0;

    Send_buf[cnt++] = 0xFF;
    Send_buf[cnt++] = id | (5 << 5);
    Send_buf[cnt++] = 0x00;
    Send_buf[cnt++] = (Send_buf[1]^Send_buf[2]) & 0x7F;

    Message_sender(Send_buf, cnt);

}
