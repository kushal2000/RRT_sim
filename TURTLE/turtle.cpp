#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <stdlib.h>
#include <turtlesim/Pose.h>
#include <math.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <sstream> 

using namespace std; 
turtlesim::Pose pos;
float x;
float y;

void position_callback(const turtlesim::Pose::ConstPtr& msg)
{
	pos.x = msg->x;
	pos.y = msg->y;
	pos.theta = msg->theta;   
}

float linear_error(turtlesim::Pose pos, float x , float y)
{
	return sqrt((pos.x-x)*(pos.x-x)+(pos.y-y)*(pos.y-y));
}

float angle(turtlesim::Pose pos , float x , float y)
{
	return atan2f(y-pos.y , x-pos.x)-pos.theta;
}

int main(int argc, char **argv)
{
	// Create a socket
    int sock = socket(AF_INET , SOCK_STREAM, 0);
    if(sock==-1)
        return 1;
    
    // Create a hint structure for the server we are connecting with
    int port;
    cout << "Enter port no.: ";
    cin >> port;
    string IPadd = "127.0.0.1";
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET , IPadd.c_str() , &hint.sin_addr);
    
    // Connect to the socket on the server
    int conn = connect(sock, (sockaddr*)&hint , sizeof(hint));
    if(conn == -1)
        return 1;
    
    char buff[4096];
    memset( buff , 0 , 4096);
    int bytesrecv2 = recv(sock , buff , 4096 , 0);

    memset( buff , 0 , 4096);
    bytesrecv2 = recv(sock , buff , 4096 , 0);
    int size=0;int temp=0;
    stringstream str(string(buff , bytesrecv2));
    str >> size;

    float arr[100][2];
    
    for(int i = 0;i<size;i++)
    {
        for(int j=0;j<2;j++)
        {
            // Wait for response from other client
            memset( buff , 0 , 4096);
            bytesrecv2 = recv(sock , buff , 4096 , 0);
            stringstream str(string(buff , bytesrecv2));
            str >> temp;
            arr[i][j]=temp/60.0;
        }
    }

	ros::init(argc, argv, "pubTsim");		// connect to roscore
	ros::NodeHandle n;													// node object
	
	// register sub to get current position/pose
	ros::Subscriber pos_sub = n.subscribe("/turtle1/pose",5, position_callback);
	// register pub to send twist velocity (cmd_vel)
	ros::Publisher Twist_pub = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 100);
	
	ros::Rate loop_rate(10);											// freq to run loops in (10 Hz)
	float ErrorLin = 0;													// holds linear error
	float ErrorAng = 0;													// holds angular error
	geometry_msgs::Twist CmdVel;
	int count=0;											// holds command velocity to publish
	ROS_INFO("Ready to send position commands");						// let user know we are ready and good
	while (ros::ok() && n.ok() )										// while ros and the node are ok
	{
		ros::spinOnce();
		if(count==size) break;
		ErrorLin = linear_error(pos, arr[count][0],arr[count][1]);					// find linear error
		ErrorAng = angle(pos, arr[count][0],arr[count][1]);				// find angular error
		printf("Error linear: %f, Error angular: %f\n", ErrorLin, ErrorAng);
		if(ErrorAng*ErrorAng<0.01)
		{
			if(ErrorLin<0.1)
				CmdVel.linear.x = 0;
			else
				CmdVel.linear.x = 0.9 * ErrorLin;
			CmdVel.angular.z = 0;
		}
		else							
		{
			CmdVel.angular.z = 0.5 * ErrorAng;
			CmdVel.linear.x = 0;										
		}
		if(CmdVel.linear.x == 0 && CmdVel.angular.z ==0 )
			count++;
		Twist_pub.publish(CmdVel);									// publish to move turtle
		loop_rate.sleep();												// sleep to maintain loop rate
	}
	
    close(sock);           
  
	return 0;
}