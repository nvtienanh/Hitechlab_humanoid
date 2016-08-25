/**
 * @file /include/uxa_dashboard/qnode.hpp
 *
 * @brief Communications central!
 *
 * @date February 2011
 **/
/*****************************************************************************
** Ifdefs
*****************************************************************************/

#ifndef uxa_dashboard_QNODE_HPP_
#define uxa_dashboard_QNODE_HPP_

/*****************************************************************************
** Includes
*****************************************************************************/
#include <iostream>
#include <ros/ros.h>
#include <ros/network.h>
#include <string>
#include <std_msgs/String.h>
#include <sstream>
#include <QThread>
#include <QStringListModel>
#include <fcntl.h>
#include <termio.h>
#include <sys/stat.h>
#include <uxa_uic_msgs/remocon.h>
#include <uxa_uic_msgs/motion.h>
#include <uxa_sam_msgs/std_position_move.h>
#include <uxa_sam_msgs/position_move.h>
#include <uxa_serial_msgs/receive.h>
#include <uxa_serial_msgs/transmit.h>

#define _SERIAL_PORT "/dev/ttyUSB0"
#define _SERIAL_BUFF_SIZE    20
#define _BAUDRATE    B115200
#define _MSG_BUFF_SIZE  20

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace uxa_dashboard {

/*****************************************************************************
** Class
*****************************************************************************/

class QNode : public QThread {
    Q_OBJECT
public:
	QNode(int argc, char** argv );
	virtual ~QNode();
	bool init();
	bool init(const std::string &master_url, const std::string &host_url);
	void run();

	/*********************
	** Logging
	**********************/
	enum LogLevel {
	         Debug,
	         Info,
	         Warn,
	         Error,
	         Fatal
	 };

	QStringListModel* loggingModel() { return &logging_model; }
	void log( const LogLevel &level, const std::string &msg);
    QStringListModel* consoleModel() { return &console_model; }
    void console( const LogLevel &level, const std::string &msg);

        void send_msg();
        void send_msg(unsigned char input);
        void send_msg(std::string str);
        void send_std_position(unsigned int pos);
        void get_position();

        int Serial;
//        unsigned char *msg_buf=new unsigned char [_SERIAL_BUFF_SIZE];
        unsigned char msg_buf[_MSG_BUFF_SIZE];
        int Init_Serial(const char *Serial_Port);
        int Serial_Test(int Serial, unsigned int Test_size);
        void Send_Serial_String(int Serial, unsigned char *Trans_chr, int Size);
        void Send_Serial_Char(int Serial, unsigned char *Trans_chr);
        int Read_Serial_Char(int Serial, unsigned char *Recei_chr);
        void rev_func(const uxa_serial_msgs::transmit::ConstPtr &msg);

        void Test_Sendata();

Q_SIGNALS:
        void loggingUpdated();
        void consoleUpdated();
        void rosShutdown();


private:
        int init_argc;
        char** init_argv;

        ros::Publisher chatter_publisher;
        ros::Publisher dashboard_pub;
        ros::Publisher motion_pub;
        ros::Publisher std_pos_move_pub;
        ros::Publisher pos_move_pub;
        ros::Publisher uxa_serial_pub;
        ros::Subscriber uxa_serial_sub;

        uxa_uic_msgs::remocon uic_pub_msg;
        uxa_uic_msgs::motion uic_motion_msg;
        uxa_sam_msgs::std_position_move sam_std_pos_move_msg;
        uxa_sam_msgs::position_move sam_pos_move_msg;
        uxa_serial_msgs::transmit serial_pub_msg;

        QStringListModel logging_model;
        QStringListModel console_model;
};

}  // namespace uxa_dashboard

#endif /* uxa_dashboard_QNODE_HPP_ */
