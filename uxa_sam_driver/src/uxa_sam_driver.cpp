#include "../include/uxa_sam_driver.h"


int main(int argc, char **argv)
{

    ros::init(argc, argv, "uxa_sam_driver");
//    if (argc != 3)
//    {
//        ROS_INFO("usage: add_two_ints_client X Y");
//        return 1;
//    }
    Init_Message();

    ros::Rate loop_rate(1000);

    cout << "SAM_DRIVER : " <<  "SAM DRIVER stand by.." << endl << endl;    

    while(ros::ok())
    {

        loop_rate.sleep();
        ros::spinOnce();        
    }


    cout << endl;
    cout << "SAM_DRIVER : " << "uxa_sam_driver node terminate." << endl;
    return 0;
}
