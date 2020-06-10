#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>
#include <math.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot

    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    client.call(srv);
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;

    bool ball_in_camera = false;
    // x goes from -1 to +1 depending on the balls position
    // float x = 0.0;
    float drive_side = 0.0;
    float drive_forward = 0.0;


    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera

    for (int i = 0; i < img.height * img.step; i+=3) {
        if( img.data[i] == white_pixel && img.data[i+1] == white_pixel && img.data[i+2] == white_pixel){
            ball_in_camera = true;

            // This is a smoother version of the motor command but I was not sure if it was welcomed so I kept the old version
            // calculates where the ball is in the image x = -1 ball is left; x = 0
            // x = 2.0*(i % img.step) / img.step - 1;
            // Just for debbuging
            //ROS_INFO("img.step is:%i",img.step);
            //ROS_INFO("i mod img.sep is:%i",i % img.step);
            //ROS_INFO("x is:%1.2f",x);
            //drive_side = 0.5 / (1 + exp(3 * x)) - 0.25;
            //drive_forward = 0.25 * exp(-5*pow(x,2));

            //THIS is the old version  
            if( i % img.step <= 0.3*img.step)
               drive_side = 0.2;
            else if ( i % img.step <= 0.7*img.step)
               drive_forward = 0.2;
            else
               drive_side = -0.2;

            break;

        }
    
    }

    // If the ball is in the camera, call drive robot 
    if(ball_in_camera){
        drive_robot(drive_forward, drive_side);
    }
    else {
        drive_robot(0.0,  0.0);
    }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
