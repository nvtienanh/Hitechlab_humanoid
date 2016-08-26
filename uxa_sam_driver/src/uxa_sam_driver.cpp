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
    uxa_sam_msgs::sam_response samclient;

    while(ros::ok())
    {

        samclient.request.samid = 24;

        if (sam_driver_client.call(samclient))
        {
          ROS_INFO("Data 1: %d", (uint8_t)samclient.response.data1);
          ROS_INFO("Data 2: %d", (uint8_t)samclient.response.data2);

        }
        else
        {
          ROS_ERROR("Failed to call service sam_cmd");

        }
        loop_rate.sleep();
        ros::spinOnce();        
    }


    cout << endl;
    cout << "SAM_DRIVER : " << "uxa_sam_driver node terminate." << endl;
    return 0;
}
