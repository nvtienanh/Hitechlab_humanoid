#include <ros/ros.h>
#include <uxa_sam_msgs/std_position_move.h>
#include <uxa_uic_msgs/motion.h>
#include <uxa_uic_msgs/remocon.h>


#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>

#include <stdio.h>
#include <iostream>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <opencv/cv.h>

#include <math.h>


#define __WINDOW_OPEN   1
#define __WINDOW_CLOSE  2
#define __WINDOW_MODE   __WINDOW_OPEN

#define __WEBCAM_ERROR  -2

#define __BALL_TRACKING_ERROR   -1
#define __BALL_TRACKING_END     0
#define __BALL_TRACKING_OK      1


#define __X_RESOLUTION 320
#define __Y_RESOLUTION 240
#define __X_HALF_RESOLUTION __X_RESOLUTION/2
#define __Y_HALF_RESOLUTION __Y_RESOLUTION/2


#define __DEAD_ZONE 0.75

#define __X_AXIS_P_GAIN 0.009
#define __Y_AXIS_P_GAIN 0.008

#define __SAM_CENTER                2048

#define __NECK_PITCH_MIN            1500
#define __NECK_PITCH_MAX            2650

#define __NECK_YAW_MIN              750
#define __NECK_YAW_MAX              4096-__NECK_YAW_MIN

#define __NECK_YAW_LEFT_TURN_AREA_OFFSET    __SAM_CENTER - 150
#define __NECK_YAW_RIGHT_TURN_AREA_OFFSET   __SAM_CENTER + 150

#define __NECK_PITCH_KICK_AREA_OFFSET       __NECK_PITCH_MAX
#define __NECK_YAW_KICK_RIGHT_AREA_OFFSET   __SAM_CENTER + 150
#define __NECK_YAW_KICK_LEFT_AREA_OFFSET    __SAM_CENTER

#define __Y_KICK_AREA_OFFSET __Y_RESOLUTION - 50

#define __NECK_PITCH_WALK_4SETP_AREA_OFFSET __NECK_PITCH_MAX - 150
#define __NECK_PITCH_WALK_6SETP_AREA_OFFSET __NECK_PITCH_MAX - 400



void send_msg(std::string STR);
void send_remocon(unsigned char remocon);
void send_std_position(unsigned char ID, unsigned int POS);

int Motor_Control();

ros::Publisher motion_pub;
ros::Publisher remocon_pub;
ros::Publisher std_pos_move_pub;


uxa_uic_msgs::motion uic_motion_msg;
uxa_uic_msgs::remocon uic_remocon_msg;
uxa_sam_msgs::std_position_move sam_std_pos_move_msg;


int Init_Ball_Tracking(char Window_Open);
int Ball_Tracking_Func(char Window_Open);

typedef struct
{
    double H;
    double S;
    double V;
}HSV_data;

typedef struct
{
    double  X_Axis;
    double  Y_Axis;
    double  Ball_size;
    unsigned char Search_Counter;
    bool    Renewal;
    bool    X_Search_Completed;
    bool    Y_Search_Completed;
}BALL_data;

typedef struct
{
    unsigned char   ID;
    double          Pos;
    double          Pos_Err;
    double          Angle;
}SAM_data;

IplImage* hsv_frame;
IplImage* thresholded;
IplImage* frame;

HSV_data Max, Min;
BALL_data Ball, Ball_Old;
SAM_data Head_Pitch, Head_Yaw;

CvScalar hsv_min;
CvScalar hsv_max;

CvCapture* capture;

unsigned char State = 0;

int main(int argc, char **argv)
{

    ros::init(argc, argv, "test_opencv");

    if(Init_Ball_Tracking(__WINDOW_MODE) == __WEBCAM_ERROR) return __WEBCAM_ERROR;

    ros::NodeHandle nh;
    ros::Rate loop_rate(30);

    motion_pub = nh.advertise<uxa_uic_msgs::motion>
            ("uic_driver_motion", 100);
    remocon_pub = nh.advertise<uxa_uic_msgs::remocon>
            ("uic_driver_remocon", 100);

    std_pos_move_pub = nh.advertise<uxa_sam_msgs::std_position_move>
            ("sam_driver_std_position_move", 100);


    sleep(2);
    send_msg("pc_control");
    sleep(1);
    send_msg("stop");
    sleep(7);
    send_msg("basic_motion");
    send_std_position(Head_Yaw.ID, (unsigned int)Head_Yaw.Pos);
    send_std_position(Head_Pitch.ID, (unsigned int)Head_Pitch.Pos);
    sleep(2);

    while(ros::ok())
    {
        loop_rate.sleep();
        Ball.Renewal = false;

        if(Ball_Tracking_Func(__WINDOW_MODE) == __BALL_TRACKING_END)   break;

            if(Motor_Control())
            {

                if((Head_Pitch.Pos == __NECK_PITCH_KICK_AREA_OFFSET) &&
                   (Head_Yaw.Pos > __NECK_YAW_KICK_LEFT_AREA_OFFSET) &&
                   (Head_Yaw.Pos > __NECK_YAW_KICK_RIGHT_AREA_OFFSET))
                {
                    send_msg("walk_right");
                    std::cout << "WALK_RIGHT" << std::endl;
                }

                else if((Head_Pitch.Pos == __NECK_PITCH_KICK_AREA_OFFSET) &&
                       (Head_Yaw.Pos < __NECK_YAW_KICK_LEFT_AREA_OFFSET) &&
                       (Head_Yaw.Pos < __NECK_YAW_KICK_RIGHT_AREA_OFFSET))
                {
                    send_msg("walk_left");
                    std::cout << "WALK_LEFT" << std::endl;
                }


                else if((Head_Pitch.Pos == __NECK_PITCH_KICK_AREA_OFFSET) &&
                        (Head_Yaw.Pos > __NECK_YAW_KICK_LEFT_AREA_OFFSET) &&
                        (Head_Yaw.Pos < __NECK_YAW_KICK_RIGHT_AREA_OFFSET) &&
                        (Ball.Y_Axis > __Y_KICK_AREA_OFFSET))
                {
                        send_msg("kick_right");
                        std::cout << "KICK_RIGHT" << std::endl;
                }

                else
                {
                    if(Head_Yaw.Pos < __NECK_YAW_LEFT_TURN_AREA_OFFSET)
                    {
                        send_msg("turn_left");
                        std::cout << "TURN_LEFT" << std::endl;
                    }
                    else if(Head_Yaw.Pos > __NECK_YAW_RIGHT_TURN_AREA_OFFSET)
                    {
                        send_msg("turn_right");
                        std::cout << "TURN_RIGHT" << std::endl;
                    }
                    else
                    {
                        if(Head_Pitch.Pos < __NECK_PITCH_WALK_6SETP_AREA_OFFSET)
                        {
                           send_msg("walk_foward_6step");
                           std::cout << "WALK_FORWARD_6STEP" << std::endl;
                        }
                        else if(Head_Pitch.Pos < __NECK_PITCH_WALK_4SETP_AREA_OFFSET)
                        {
                           send_msg("walk_foward_4step");
                           std::cout << "WALK_FORWARD_4STEP" << std::endl;
                        }
                        else
                        {
                            send_msg("walk_forward_short");
                            std::cout << "WALK_FORWARD_SHORT" << std::endl;
                        }

                    }
                }
            }

    }

     // Release the capture device housekeeping
     cvReleaseCapture( &capture );
     cvDestroyWindow( "Camera" );
     cvDestroyWindow( "HSV" );
     cvDestroyWindow( "After Color Filtering" );
     return 0;

}

void send_msg(std::string STR)
{
    uic_motion_msg.motion_name = STR;
    motion_pub.publish(uic_motion_msg);
}

void send_remocon(unsigned char remocon)
{
    uic_remocon_msg.btn_code = remocon;
    remocon_pub.publish(uic_remocon_msg);
}

void send_std_position(unsigned char ID ,unsigned int POS)
{
    sam_std_pos_move_msg.id = ID;
    sam_std_pos_move_msg.pos14 = POS;
    std_pos_move_pub.publish(sam_std_pos_move_msg);
}

int Motor_Control()
{

    double Dead_Zone = __DEAD_ZONE * Ball.Ball_size;



    if(Ball.X_Axis >= __X_HALF_RESOLUTION + Dead_Zone)
    {
        Head_Yaw.Pos_Err = ((Ball.X_Axis - __X_HALF_RESOLUTION + Dead_Zone) * (Ball.Ball_size * __X_AXIS_P_GAIN));
        Head_Yaw.Pos += Head_Yaw.Pos_Err;
        Ball.X_Search_Completed = false;
    }
    else if(Ball.X_Axis <= __X_HALF_RESOLUTION - Dead_Zone)
    {
        Head_Yaw.Pos_Err = ((Ball.X_Axis - __X_HALF_RESOLUTION - Dead_Zone) * (Ball.Ball_size * __X_AXIS_P_GAIN));
        Head_Yaw.Pos += Head_Yaw.Pos_Err;
        Ball.X_Search_Completed = false;
    }
    else
        Ball.X_Search_Completed = true;



    if(Ball.Y_Axis >= __Y_HALF_RESOLUTION + Dead_Zone)
    {
        Head_Pitch.Pos_Err = ((Ball.Y_Axis - __Y_HALF_RESOLUTION + Dead_Zone) * (Ball.Ball_size * __Y_AXIS_P_GAIN));
        Head_Pitch.Pos += Head_Pitch.Pos_Err;
        Ball.Y_Search_Completed = false;
    }
    else if(Ball.Y_Axis <= __Y_HALF_RESOLUTION - Dead_Zone)
    {
        Head_Pitch.Pos_Err = ((Ball.Y_Axis - __Y_HALF_RESOLUTION - Dead_Zone) * (Ball.Ball_size * __Y_AXIS_P_GAIN));
        Head_Pitch.Pos += Head_Pitch.Pos_Err;
        Ball.Y_Search_Completed = false;
    }
    else
        Ball.Y_Search_Completed = true;



    if((Ball.X_Search_Completed == true) &&
       (Ball.Y_Search_Completed == true))
        return 1;



    if((Head_Yaw.Pos > __NECK_YAW_MAX) || (Head_Yaw.Pos < __NECK_YAW_MIN))
    {
        if(Head_Yaw.Pos > __NECK_YAW_MAX)
            Head_Yaw.Pos = __NECK_YAW_MAX;
        else if(Head_Yaw.Pos < __NECK_YAW_MIN)
            Head_Yaw.Pos = __NECK_YAW_MIN;
    }

   if((Head_Pitch.Pos > __NECK_PITCH_MAX) || (Head_Pitch.Pos < __NECK_PITCH_MIN))
   {

        if(Head_Pitch.Pos > __NECK_PITCH_MAX)
            Head_Pitch.Pos = __NECK_PITCH_MAX;
        else if(Head_Pitch.Pos < __NECK_PITCH_MIN)
            Head_Pitch.Pos = __NECK_PITCH_MIN;
   }


    send_std_position(Head_Yaw.ID, (unsigned int)Head_Yaw.Pos);
    send_std_position(Head_Pitch.ID, (unsigned int)Head_Pitch.Pos);

    if((Ball.X_Search_Completed == true) &&
       (Head_Pitch.Pos == __NECK_PITCH_MAX))
        return 1;

    return 0;

}

int Init_Ball_Tracking(char Window_Open)
{

    CvSize size = cvSize(__X_RESOLUTION, __Y_RESOLUTION);
    // Open capture device. 0 is /dev/video0, 1 is /dev/video1, etc.
    capture = cvCaptureFromCAM( 0 );
    if( !capture )
    {
            fprintf( stderr, "ERROR: capture is NULL \n" );
            getchar();
            return __WEBCAM_ERROR;
    }

    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, __X_RESOLUTION);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, __Y_RESOLUTION);
    // Create a window in which the captured images will be presented

    if(Window_Open == __WINDOW_OPEN)
    {
        cvNamedWindow( "Camera", CV_WINDOW_AUTOSIZE );
        cvNamedWindow( "HSV", CV_WINDOW_AUTOSIZE );
        cvNamedWindow( "After Color Filtering", CV_WINDOW_AUTOSIZE );
        printf("Window open mode\n");
    }
    else
        printf("Window close mode\n");

    Max.H = 32;
    Max.S = 95;
    Max.V = 95;

    Min.H = 6;
    Min.S = 60;
    Min.V = 60;

    Max.H /= 2;
    Min.H /= 2;
    Max.S *= 2.55;
    Min.S *= 2.55;
    Max.V *= 2.55;
    Min.V *= 2.55;

    Ball.X_Axis = __X_HALF_RESOLUTION;
    Ball.Y_Axis = __Y_HALF_RESOLUTION;
    Ball.Ball_size = 0;
    Ball.Renewal = false;
    Ball.X_Search_Completed = false;
    Ball.Y_Search_Completed = false;

    Head_Pitch.ID = 24;
    Head_Pitch.Pos = 2350;
    Head_Pitch.Pos_Err = 0;
    Head_Pitch.Angle = 0;


    Head_Yaw.ID = 23;
    Head_Yaw.Pos = 2048;
    Head_Yaw.Pos_Err = 0;
    Head_Yaw.Angle = 0;


    hsv_min = cvScalar(Min.H, Min.S, Min.V, 0);
    hsv_max = cvScalar(Max.H, Max.S, Max.V, 0);

    hsv_frame = cvCreateImage(size, IPL_DEPTH_8U, 3);
    thresholded = cvCreateImage(size, IPL_DEPTH_8U, 1);

    return 1;

}

int Ball_Tracking_Func(char Window_Open)
{


    frame = cvQueryFrame( capture );        // Get one frame

    if( !frame )
    {
            fprintf( stderr, "ERROR: frame is null...\n" );
            getchar();
            return __BALL_TRACKING_ERROR;
    }
    // Covert color space to HSV as it is much easier to filter colors in the HSV color-space.
    cvCvtColor(frame, hsv_frame, CV_BGR2HSV);
    // Filter out colors which are out of range.
    cvInRangeS(hsv_frame, hsv_min, hsv_max, thresholded);
    // Memory for hough circles
    CvMemStorage* storage = cvCreateMemStorage(0);
    // hough detector works better with some smoothing of the image

    cvSmooth(thresholded, thresholded, CV_GAUSSIAN, 17, 17);

    CvSeq* circles = cvHoughCircles(thresholded, storage, CV_HOUGH_GRADIENT, 2,
                                    thresholded->height/2, 100, 45, 1, 80);

    for (int i = 0; i < circles->total; i++)
    {
        float* p = (float*)cvGetSeqElem( circles, i );

        //printf("Ball! x=%f y=%f r=%f\n\r",p[0],p[1],p[2] );

        Ball.X_Axis = p[0];
        Ball.Y_Axis = p[1];
        Ball.Ball_size = p[2];
        Ball.Renewal = true;



        cvCircle( frame, cvPoint(cvRound(p[0]),cvRound(p[1])),
                                3, CV_RGB(0,255,0), -1, 8, 0 );
        cvCircle( frame, cvPoint(cvRound(p[0]),cvRound(p[1])),
                                cvRound(p[2]), CV_RGB(255,0,0), 3, 8, 0 );
    }
    if(Window_Open == __WINDOW_OPEN)
    {
        cvShowImage( "Camera", frame ); // Original stream with detected ball overlay
        cvShowImage( "HSV", hsv_frame); // Original stream in the HSV color space
        cvShowImage( "After Color Filtering", thresholded ); // The stream after color filtering
    }
    cvReleaseMemStorage(&storage);
    // Do not release the frame!
    //If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
    //remove higher bits using AND operator
    if( (cvWaitKey(10) & 255) == 27 ) return __BALL_TRACKING_END;

    return __BALL_TRACKING_OK;

}



