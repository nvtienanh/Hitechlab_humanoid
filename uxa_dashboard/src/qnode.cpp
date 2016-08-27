/**
 * @file /src/qnode.cpp
 *
 * @brief Ros communication central!
 *
 * @date February 2011
 **/

/*****************************************************************************
** Includes
*****************************************************************************/

//#include <ros/ros.h>
//#include <ros/network.h>
//#include <string>
//#include <std_msgs/String.h>
//#include <sstream>
#include "../include/uxa_dashboard/qnode.hpp"

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace uxa_dashboard {

/*****************************************************************************
** Implementation
*****************************************************************************/

QNode::QNode(int argc, char** argv ) :
	init_argc(argc),
	init_argv(argv)
	{}

QNode::~QNode() {
    if(ros::isStarted()) {
      ros::shutdown(); // explicitly needed since we use ros::start();
      ros::waitForShutdown();
    }
	wait();
}

bool QNode::init() {    
	ros::init(init_argc,init_argv,"uxa_dashboard");
	if ( ! ros::master::check() ) {
		return false;
	}
	ros::start(); // explicitly needed since our nodehandle is going out of scope.
    ros::NodeHandle n;
	// Add your ros communications here.
	chatter_publisher = n.advertise<std_msgs::String>("chatter", 1000);
    // UIC Message
    dashboard_pub = n.advertise<uxa_uic_msgs::remocon>("uic_driver_remocon", 100);
    motion_pub = n.advertise<uxa_uic_msgs::motion>("uic_driver_motion", 100);
    // SAM Driver Message
    std_pos_move_pub = n.advertise<uxa_sam_msgs::std_position_move>("sam_driver_std_position_move", 100);
    pos_move_pub = n.advertise<uxa_sam_msgs::position_move>("sam_driver_position_move", 100);
    // Serial Publisher
    dashboard_to_serial_pub = n.advertise<uxa_serial_msgs::transmit>("dashboard_to_serial_sub", _MSG_BUFF_SIZE);
    uxa_serial_pub = n.advertise<uxa_serial_msgs::receive>("uxa_serial_publisher", _MSG_BUFF_SIZE);
//    uxa_serial_sub = n.subscribe<uxa_serial_msgs::transmit>("uxa_serial_subscriber", _MSG_BUFF_SIZE, &QNode::rev_func,this);
    // Sam service client
    sam_driver_client = n.serviceClient<uxa_sam_msgs::sam_response>("sam_cmd");
	start();

	return true;
}

bool QNode::init(const std::string &master_url, const std::string &host_url) {
	std::map<std::string,std::string> remappings;
	remappings["__master"] = master_url;
	remappings["__hostname"] = host_url;
	ros::init(remappings,"uxa_dashboard");
	if ( ! ros::master::check() ) {
		return false;
	}
	ros::start(); // explicitly needed since our nodehandle is going out of scope.
	ros::NodeHandle n;
    // Add your ros communications here.
    chatter_publisher = n.advertise<std_msgs::String>("chatter", 1000);
    // UIC Message
    dashboard_pub = n.advertise<uxa_uic_msgs::remocon>("uic_driver_remocon", 100);
    motion_pub = n.advertise<uxa_uic_msgs::motion>("uic_driver_motion", 100);
    // SAM Driver Message
    std_pos_move_pub = n.advertise<uxa_sam_msgs::std_position_move>("sam_driver_std_position_move", 100);
    pos_move_pub = n.advertise<uxa_sam_msgs::position_move>("sam_driver_position_move", 100);
    // Serial Publisher
    dashboard_to_serial_pub = n.advertise<uxa_serial_msgs::receive>("dashboard_to_serial", _MSG_BUFF_SIZE);
    uxa_serial_pub = n.advertise<uxa_serial_msgs::receive>("uxa_serial_subscriber", _MSG_BUFF_SIZE);
//    uxa_serial_sub = n.subscribe<uxa_serial_msgs::transmit>("uxa_serial_subscriber", _MSG_BUFF_SIZE, &QNode::rev_func,this);
    // Sam service client
    sam_driver_client = n.serviceClient<uxa_sam_msgs::sam_response>("sam_cmd");

    start();

	return true;
}

void QNode::run() {
    ros::Rate loop_rate(100);
//	int count = 0;
	while ( ros::ok() ) {        

//		ss << "hello world " << count;
//		msg.data = ss.str();
//		chatter_publisher.publish(msg);
//        log(Info,std::string("I sent: ")+msg.data);
//        get_position(24);

		ros::spinOnce();
		loop_rate.sleep();
//		++count;
	}
	std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;
	Q_EMIT rosShutdown(); // used to signal the gui for a shutdown (useful to roslaunch)
}


void QNode::log( const LogLevel &level, const std::string &msg) {
	logging_model.insertRows(logging_model.rowCount(),1);
	std::stringstream logging_model_msg;
	switch ( level ) {
		case(Debug) : {
				ROS_DEBUG_STREAM(msg);
                logging_model_msg << "[DEBUG]: " << msg;
				break;
		}
		case(Info) : {
				ROS_INFO_STREAM(msg);
                logging_model_msg << "[INFO]: " << msg;
				break;
		}
		case(Warn) : {
				ROS_WARN_STREAM(msg);
                logging_model_msg << "[INFO]: " << msg;
				break;
		}
		case(Error) : {
				ROS_ERROR_STREAM(msg);
                logging_model_msg << "[ERROR]: " << msg;
				break;
		}
		case(Fatal) : {
				ROS_FATAL_STREAM(msg);
                logging_model_msg << "[FATAL]: " << msg;
				break;
		}
	}
	QVariant new_row(QString(logging_model_msg.str().c_str()));
	logging_model.setData(logging_model.index(logging_model.rowCount()-1),new_row);
	Q_EMIT loggingUpdated(); // used to readjust the scrollbar
}

void QNode::console( const LogLevel &level, const std::string &msg) {
    console_model.insertRows(console_model.rowCount(),1);
    std::stringstream console_model_msg;
    switch ( level ) {
        case(Debug) : {
                ROS_DEBUG_STREAM(msg);
                console_model_msg << "[DEBUG]: " << msg;
                break;
        }
        case(Info) : {
                ROS_INFO_STREAM(msg);
                console_model_msg << "[INFO]: " << msg;
                break;
        }
        case(Warn) : {
                ROS_WARN_STREAM(msg);
                console_model_msg << "[INFO]: " << msg;
                break;
        }
        case(Error) : {
                ROS_ERROR_STREAM(msg);
                console_model_msg << "[ERROR]: " << msg;
                break;
        }
        case(Fatal) : {
                ROS_FATAL_STREAM(msg);
                console_model_msg << "[FATAL]: " << msg;
                break;
        }
    }
    QVariant new_row(QString(console_model_msg.str().c_str()));
    console_model.setData(console_model.index(console_model.rowCount()-1),new_row);
    Q_EMIT consoleUpdated(); // used to readjust the scrollbar
}

void QNode::send_msg()
{
    unsigned char temp;
    temp = 0xff;
    uic_pub_msg.btn_code = temp;
    dashboard_pub.publish(uic_pub_msg);
}

void QNode::send_msg(unsigned char input)
{
    uic_pub_msg.btn_code = input;
    dashboard_pub.publish(uic_pub_msg);
}

void QNode::send_msg(std::string str)
{
    uic_motion_msg.motion_name = str;
    motion_pub.publish(uic_motion_msg);
}

void QNode::send_std_position(unsigned int pos)
{
    sam_std_pos_move_msg.id = 24;
    sam_std_pos_move_msg.pos14 = pos;
    std_pos_move_pub.publish(sam_std_pos_move_msg);
}

uint *QNode::get_position(uint id)
{
    samclient.request.samid = id;    
    std::stringstream ss;

    if (sam_driver_client.call(samclient))
    {
      ROS_INFO("Data 1: %d", samclient.response.data1);
      ROS_INFO("Data 2: %d", samclient.response.data2);
      ss << "ID: " << unsigned(id) << " Byte[0]: " << unsigned(samclient.response.data1) << " Byte[1]: "<< uint(samclient.response.data2);
      console(Info,ss.str());

    }
    else
    {
      ROS_ERROR("Failed to call service sam_cmd");

    }


}

int QNode::Init_Serial(const char *Serial_Port)
{

    termios Serial_Setting;

    if((Serial = open(Serial_Port, O_RDWR | O_NONBLOCK | O_NOCTTY)) == -1)
    {
        std::cout << "SERIAL : " << Serial_Port << " Device open error" << std::endl;
        std::cout << "SERIAL : " << Serial_Port << " Device permission change progress...." << std::endl;
        for(int temp = 0; temp < 5; temp++)
        {
            if(chmod(Serial_Port, __S_IREAD | __S_IWRITE) == 0){

                std::cout << "SERIAL : " << Serial_Port << " Device permission change complete" << std::endl;
                Serial = open(Serial_Port, O_RDWR | O_NONBLOCK | O_NOCTTY);

                if(Serial == -1)
                {
                    std::cout << "SERIAL : " << Serial_Port << " Device Not Found" << std::endl;
                    return -1;
                }
                else
                    std::cout << "SERIAL : " << Serial_Port <<" Device open" << std::endl;
            }
            else
            {
                std::cout << "SERIAL : " << Serial_Port << " Device permission change error" << std::endl;
                //return -1;
            }
        }

    }

    else
        std::cout << "SERIAL : " << Serial_Port << " Device open" << std::endl;


    memset(&Serial_Setting, 0, sizeof(Serial_Setting));
    Serial_Setting.c_iflag = 0;
    Serial_Setting.c_oflag = 0;
    Serial_Setting.c_cflag = _BAUDRATE | CS8 | CREAD | CLOCAL;
    Serial_Setting.c_lflag = 0;
    Serial_Setting.c_cc[VMIN] = 1;
    Serial_Setting.c_cc[VTIME] = 0;

    cfsetispeed(&Serial_Setting, _BAUDRATE);
    cfsetospeed(&Serial_Setting, _BAUDRATE);

    tcflush(Serial, TCIFLUSH);
    tcsetattr(Serial, TCSANOW, &Serial_Setting);

    return Serial;

}

int QNode::Serial_Test(int Serial, unsigned int Test_size)
{

    char Trans_chr[Test_size];
    char Recei_chr[Test_size];

    memset(Trans_chr, 0b10101010, sizeof(Trans_chr));
    memset(Recei_chr, '\0', sizeof(Recei_chr));

    write(Serial, Trans_chr, sizeof(Trans_chr));

    if(read(Serial, Recei_chr, sizeof(Recei_chr)) > 0)
    {
        std::cout << "Receive" << std::endl;
        if(strcmp(Trans_chr, Recei_chr) != 0)
        {
            close(Serial);
            std::cout << std::endl;
            std::cout << "ERR" << std::endl;
            std::cout << "ERR Receive CHR = " << Recei_chr << std::endl;
            return -1;
        }
        else
        {
            std::cout << std::endl;
            std::cout << "OK" <<std::endl;
            memset(Recei_chr, '\0', sizeof(Recei_chr));
        }

    }
}

void QNode::Send_Serial_String(int Serial, unsigned char *Trans_chr, int Size)
{
    write(Serial, Trans_chr, Size);
}

void QNode::Send_Serial_Char(int Serial, unsigned char *Trans_chr)
{
    write(Serial, Trans_chr, 1);
}

int QNode::Read_Serial_Char(int Serial, unsigned char *Recei_chr)
{
    if(read(Serial, Recei_chr, 1) > 0)
        return 1;

    return -1;
}

//void QNode::rev_func(const uxa_serial_msgs::transmit::ConstPtr &msg)
//{
//    std_msgs::String serialmsg;
//    std::stringstream ss;
//    *msg_buf = msg->tx_data;
////    Send_Serial_Char(Serial, msg_buf);
//    ss << "Dashboard serial transmit: " << std::hex << unsigned(msg_buf[0]);
//    serialmsg.data = ss.str();
//    console(Info,serialmsg.data);
//}

void QNode::get_serial()
{
    unsigned char Trans_chr[6];
    unsigned char cnt = 0;

    Trans_chr[cnt++] = 0xFF;
    Trans_chr[cnt++] = (unsigned char)(7 << 5);
    Trans_chr[cnt++] = 225;
    Trans_chr[cnt++] = 0;
    Trans_chr[cnt++] = 0X07;
    Trans_chr[cnt++] = (Trans_chr[1]^Trans_chr[2]^Trans_chr[3]^Trans_chr[4]) & 0x7F;

    Message_sender(Trans_chr, cnt);
    sleep(1);

}

void QNode::Message_sender(unsigned char *Send_data, int Size)
{
    for(char cnt = 0; cnt < Size; cnt++)
    {
        dashboard_to_serial_pub_msg.tx_data = Send_data[cnt];
        dashboard_to_serial_pub.publish(dashboard_to_serial_pub_msg);
    }
}


}  // namespace uxa_dashboard
