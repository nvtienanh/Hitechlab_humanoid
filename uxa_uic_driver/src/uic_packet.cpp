#include "../include/uic_packet.h"


uxa_serial_msgs::transmit serial_pub_msg;

ros::Publisher uxa_serial_pub;
ros::Subscriber uic_driver_remocon_sub, uic_driver_motion_sub;

unsigned char Send_buf[_MSG_BUFF_SIZE] = {0xFF,0xFF,0xAA,0x55,0xAA,0x55,0x37,0xBA,};
unsigned char Send_pc_buf[5] = {0xFF, 0xE0, 0xE1, 0x00,};
using namespace std;

void UIC_REMOCON_FUNC(const uxa_uic_msgs::remocon::ConstPtr &msg)
{
    ROS_INFO("Remote Control Function : %d",msg->btn_code);
    UIC_send_remote(msg->btn_code);
}

void UIC_MOTION_FUNC(const uxa_uic_msgs::motion::ConstPtr &msg)
{

    if(msg->motion_name.compare("basic_motion")==0)
        UIC_send_pc_control_remote(BTN_A);

    else if(msg->motion_name.compare("kick_right")==0)
        UIC_send_pc_control_remote(BTN_B);

    else if(msg->motion_name.compare("turn_left")==0)
        UIC_send_pc_control_remote(BTN_LR);

    else if(msg->motion_name.compare("walk_forward_short")==0)
        UIC_send_pc_control_remote(BTN_U);

    else if(msg->motion_name.compare("turn_right")==0)
        UIC_send_pc_control_remote(BTN_RR);

    else if(msg->motion_name.compare("walk_left")==0)
        UIC_send_pc_control_remote(BTN_L);

    else if(msg->motion_name.compare("walk_right")==0)
        UIC_send_pc_control_remote(BTN_R);

    else if(msg->motion_name.compare("walk_foward_4step")==0)
        UIC_send_pc_control_remote(BTN_LA);

    else if(msg->motion_name.compare("basic_motion")==0)
        UIC_send_pc_control_remote(BTN_D);

    else if(msg->motion_name.compare("walk_foward_6step")==0)
        UIC_send_pc_control_remote(BTN_RA);

    else if(msg->motion_name.compare("demo_introduction")==0)
        UIC_send_pc_control_remote(BTN_1);

    else if(msg->motion_name.compare("dance_gangnamstyle")==0)
        UIC_send_pc_control_remote(BTN_2);

    else if(msg->motion_name.compare("stop")==0)
        UIC_send_pc_control_remote(BTN_C);

    else if(msg->motion_name.compare("pc_control")==0)
        PC_control();

}




void Init_Message()
{
    ros::NodeHandle n;
    uxa_serial_pub = n.advertise<uxa_serial_msgs::transmit>("uxa_serial_subscriber", _MSG_BUFF_SIZE);
    uic_driver_remocon_sub = n.subscribe<uxa_uic_msgs::remocon>("uic_driver_remocon", _MSG_BUFF_SIZE, UIC_REMOCON_FUNC);
    uic_driver_motion_sub = n.subscribe<uxa_uic_msgs::motion>("uic_driver_motion", _MSG_BUFF_SIZE, UIC_MOTION_FUNC);

}

void Message_sender(unsigned char *Send_data, int Size)
{
    for(char cnt = 0; cnt < Size; cnt++)
    {
        serial_pub_msg.tx_data = Send_data[cnt];
        uxa_serial_pub.publish(serial_pub_msg);
    }
}

void UIC_send_remote(unsigned char remote)
{
    Send_buf[8] = 0x14;
    for(char cnt = 9; cnt < 13; cnt++)
        Send_buf[cnt] = 0x00;
    Send_buf[13] = 0x01;
    Send_buf[14] = remote;
    Send_buf[15] = remote;

    Message_sender(Send_buf, 16);
}

void UIC_send_pc_control_remote(unsigned char remote)
{
/*
    unsigned char cnt = 0;
    unsigned char buf[5] = {0xFF, 0xE0, 0xE1, 0x00, remote,
                            (0xFF^0xE0^0xE1^remote) & 0x7E};
    buf[cnt++] = 0xFF;
    buf[cnt++] = (unsigned char)(7 << 5);
    buf[cnt++] = 225;
    buf[cnt++] = 0;
    buf[cnt++] = remote;
    buf[cnt++] = (buf[1]^buf[2]^buf[3]^buf[4]) & 0x7F;
*/
    Send_pc_buf[4] = remote;
//    Send_pc_buf[5] = (0xFF^0xE0^0xE1^remote) & 0x7F;
    Send_pc_buf[5] = (Send_pc_buf[1]^Send_pc_buf[2]^Send_pc_buf[3]^remote) & 0x7F;

    Message_sender(Send_pc_buf, 6);
}

void PC_control()
{
    unsigned char cnt = 0;
    unsigned char buf[17];

    buf[cnt++] = 0xFF;
    buf[cnt++] = 0xFF;
    buf[cnt++] = 0xAA;
    buf[cnt++] = 0x55;
    buf[cnt++] = 0xAA;
    buf[cnt++] = 0x55;
    buf[cnt++] = 0x37;
    buf[cnt++] = 0xBA;
    buf[cnt++] = 0x10;
    buf[cnt++] = 0x00;
    buf[cnt++] = 0x00;
    buf[cnt++] = 0x00;
    buf[cnt++] = 0x00;
    buf[cnt++] = 0x01;
    buf[cnt++] = 0x01;
    buf[cnt++] = 0x01;

    Message_sender(buf, cnt);
}


