#include "../include/uxa_uic_driver.h"

int main(int argc, char **argv)
{

    ros::init(argc, argv, "uxa_uic_driver");
    Init_Message();

    ros::Rate loop_rate(1000);

    cout << "UIC_DRIVER : " <<  "UIC DRIVER stand by." << endl << endl;

    while(ros::ok())
    {

        loop_rate.sleep();
        ros::spinOnce();
    }


    cout << endl;
    cout << "UIC_DRIVER : " << "uxa_uic_driver node terminate." << endl;
    return 0;
}
