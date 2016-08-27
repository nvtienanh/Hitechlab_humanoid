#include "../include/uxa_serial.h"

int main(int argc, char **argv)
{
    sleep(10);

    ros::init(argc, argv, "uxa_serial");
    ros::NodeHandle n;

    ros::Publisher uxa_serial_pub = n.advertise<uxa_serial_msgs::receive>("uxa_serial_publisher", _MSG_BUFF_SIZE);
    ros::Publisher dashboard_to_serial_pub = n.advertise<uxa_serial_msgs::receive>("dashboard_to_serial_pub", _MSG_BUFF_SIZE);
    ros::Subscriber uxa_serial_sub = n.subscribe<uxa_serial_msgs::transmit>("uxa_serial_subscriber", _MSG_BUFF_SIZE, rev_func);
    ros::Subscriber dashboard_to_serial_sub = n.subscribe<uxa_serial_msgs::transmit>("dashboard_to_serial_sub", _MSG_BUFF_SIZE, dashboard_msg_func);
    ros::ServiceServer sam_service = n.advertiseService("sam_cmd", response_func);

    ros::Rate loop_rate(1000);
    uxa_serial_msgs::receive msg;
    if((Serial = Init_Serial(_SERIAL_PORT)) != -1)
    {
        unsigned char Trans_chr[_SERIAL_BUFF_SIZE];
        unsigned char Recev_chr[_SERIAL_BUFF_SIZE];
        unsigned char cnt = 0;

        Trans_chr[cnt++] = 0xFF;
        Trans_chr[cnt++] = 0xFF;
        Trans_chr[cnt++] = 0xAA;
        Trans_chr[cnt++] = 0x55;
        Trans_chr[cnt++] = 0xAA;
        Trans_chr[cnt++] = 0x55;
        Trans_chr[cnt++] = 0x37;
        Trans_chr[cnt++] = 0xBA;
        Trans_chr[cnt++] = 0x10;
        Trans_chr[cnt++] = 0x00;
        Trans_chr[cnt++] = 0x00;
        Trans_chr[cnt++] = 0x00;
        Trans_chr[cnt++] = 0x00;
        Trans_chr[cnt++] = 0x01;
        Trans_chr[cnt++] = 0x01;
        Trans_chr[cnt++] = 0x01;

        Send_Serial_String(Serial, Trans_chr, cnt);
        sleep(1);

//        cnt = 0;

//        Trans_chr[cnt++] = 0xFF;
//        Trans_chr[cnt++] = (unsigned char)(7 << 5);
//        Trans_chr[cnt++] = 225;
//        Trans_chr[cnt++] = 0;
//        Trans_chr[cnt++] = 0X07;
//        Trans_chr[cnt++] = (Trans_chr[1]^Trans_chr[2]^Trans_chr[3]^Trans_chr[4]) & 0x7F;

//        Send_Serial_String(Serial, Trans_chr, cnt);
//        sleep(1);

        memset(Trans_chr, '\0', sizeof(Trans_chr));
        memset(Recev_chr, '\0', sizeof(Recev_chr));

        cout << "SERIAL : " <<  "Serial communication stand by." << endl << endl;

        while(ros::ok())
        {
            loop_rate.sleep();
//            if(Read_Serial_Char(Serial, Recev_chr) == 1)
//            {
//                msg.rx_data = Recev_chr[0];
//                ROS_INFO("UXA_serial receive msg : 0x%x",msg.rx_data);
////                uxa_serial_pub.publish(msg);
////                if (Recev_chr[0]=='\r')  ROS_INFO("END DATA======");
//            }
            // Flush the port
//            tcflush(Serial, TCIFLUSH);
            ros::spinOnce();
        }

    }

    cout << endl;
    cout << "SERIAL : " << Serial << " Device close." << endl;
    close(Serial);
    cout << "SERIAL : " << "uxa_serial node terminate." << endl;
    return 0;
}


int Init_Serial(const char *Serial_Port)
{

    termios Serial_Setting;

    if((Serial = open(Serial_Port, O_RDWR | O_NONBLOCK | O_NOCTTY)) == -1)
    {
        cout << "SERIAL : " << Serial_Port << " Device open error" << endl;
        cout << "SERIAL : " << Serial_Port << " Device permission change progress...." << endl;
        for(int temp = 0; temp < 5; temp++)
        {
            if(chmod(Serial_Port, __S_IREAD | __S_IWRITE) == 0){

                cout << "SERIAL : " << Serial_Port << " Device permission change complete" << endl;
                Serial = open(Serial_Port, O_RDWR | O_NONBLOCK | O_NOCTTY);

                if(Serial == -1)
                {
                    cout << "SERIAL : " << Serial_Port << " Device Not Found" << endl;
                    return -1;
                }
                else
                    cout << "SERIAL : " << Serial_Port <<" Device open" << endl;
            }
            else
            {
                cout << "SERIAL : " << Serial_Port << " Device permission change error" << endl;
                return -1;
            }
        }

    }

    else
        cout << "SERIAL : " << Serial_Port << " Device open" << endl;


    memset(&Serial_Setting, 0, sizeof(Serial_Setting));
    Serial_Setting.c_iflag = 0;
    Serial_Setting.c_oflag = 0;
    Serial_Setting.c_cflag = _BAUDRATE | CS8 | CREAD | CLOCAL;
    Serial_Setting.c_lflag = 0;
    Serial_Setting.c_cc[VMIN] = 1;//1
    Serial_Setting.c_cc[VTIME] = 0;

    cfsetispeed(&Serial_Setting, _BAUDRATE);
    cfsetospeed(&Serial_Setting, _BAUDRATE);

    tcflush(Serial, TCIFLUSH);
    tcsetattr(Serial, TCSANOW, &Serial_Setting);

    return Serial;

}

int Serial_Test(int Serial, unsigned int Test_size)
{

    char Trans_chr[Test_size];
    char Recei_chr[Test_size];

    memset(Trans_chr, 0b10101010, sizeof(Trans_chr));
    memset(Recei_chr, '\0', sizeof(Recei_chr));

    write(Serial, Trans_chr, sizeof(Trans_chr));

    if(read(Serial, Recei_chr, sizeof(Recei_chr)) > 0)
    {
        cout << "Receive" << endl;
        if(strcmp(Trans_chr, Recei_chr) != 0)
        {
            close(Serial);
            cout << endl;
            cout << "ERR" << endl;
            cout << "ERR Receive CHR = " << Recei_chr << endl;
            return -1;
        }
        else
        {
            cout << endl;
            cout << "OK" <<endl;
            memset(Recei_chr, '\0', sizeof(Recei_chr));
        }

    }
}

void Send_Serial_String(int Serial, unsigned char *Trans_chr, int Size)
{
    write(Serial, Trans_chr, Size);
}

void Send_Serial_Char(int Serial, unsigned char *Trans_chr)
{
    write(Serial, Trans_chr, 1);
}

int Read_Serial_Char(int Serial, unsigned char *Recei_chr)
{
    if(read(Serial, Recei_chr, 1) > 0)
        return 1;

    return -1;
}


void rev_func(const uxa_serial_msgs::transmit::ConstPtr &msg)
{    
    ROS_INFO("Serial receive msg : 0x%x to Send",msg->tx_data);
    *msg_buf = msg->tx_data;
    Send_Serial_Char(Serial, msg_buf);
}

void dashboard_msg_func(const uxa_serial_msgs::transmit::ConstPtr &msg)
{
    ROS_INFO("Dashboard send message : 0x%x to SAM",msg->tx_data);
    *msg_buf = msg->tx_data;
    Send_Serial_Char(Serial, msg_buf);
}

bool response_func(uxa_sam_msgs::sam_response::Request  &req,
         uxa_sam_msgs::sam_response::Response &res)
{
    unsigned char Recev_chr[_SERIAL_BUFF_SIZE];
    unsigned char Trans_chr[_SERIAL_BUFF_SIZE];
    unsigned char cnt = 0;

    Trans_chr[cnt++] = 0xFF;
    Trans_chr[cnt++] = (unsigned char)(req.samid| (5 << 5));
    Trans_chr[cnt++] = 0x00;
    Trans_chr[cnt++] = (Trans_chr[1]^Trans_chr[2]) & 0x7F;
    Send_Serial_String(Serial, Trans_chr, cnt);

//    while(read(Serial, Recev_chr, 2) <= 0)
//    {
//        res.data1 = Recev_chr[0];
//        res.data2 = Recev_chr[1];
//        ROS_INFO("Request: ID=%d",(uint)req.samid);
//        ROS_INFO("Response byte: [%d], [%d]", (uint8_t)res.data1, (uint8_t)res.data2);

//    }
    if(read(Serial, Recev_chr, 2) > 0)
    {
        res.data1 = Recev_chr[0];
        res.data2 = Recev_chr[1];
        ROS_INFO("Request: ID=%d",(uint)req.samid);
        ROS_INFO("Response byte: [%d], [%d]", (uint8_t)res.data1, (uint8_t)res.data2);
    }
    else
    {
        res.data1 = 0x00;
        res.data2 = 0x00;
        ROS_INFO("Request: ID=%d",(uint8_t)req.samid);
        ROS_INFO("Response byte: nodatata");
    }
    return true;
}
