#include "iostream"
#include "ros/ros.h"
#include "std_msgs/Float32.h"
#include "std_msgs/Bool.h"
#include "api_i2c_pwm.h"
PCA9685 *pca9685 = new PCA9685() ;
bool STATUS = 0;

void speed(const std_msgs::Float32::ConstPtr& msg){
	if (!STATUS) return;
	double speed=msg->data;
	if (speed >= 0)
	{
		api_set_FORWARD_control(pca9685, speed);
	}
	else
	{
		api_set_BRAKE_control(pca9685, speed);
	}

	ROS_INFO("new throttle: [%f]",speed);
}
void steer(const std_msgs::Float32::ConstPtr& msg2){
	if (!STATUS) return;
	double steer=msg2->data;
	api_set_STEERING_control( pca9685,steer);
	ROS_INFO("new steer: [%f]",steer);
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "car_controller");
	api_pwm_pca9685_init( pca9685 );
	double spd = 0;
	api_set_FORWARD_control(pca9685, spd);
	ros::NodeHandle n;
	ros::NodeHandle m;
	ros::Subscriber sub1 = n.subscribe("/speed_api", 10, speed);
	ros::Subscriber sub2 = m.subscribe("/steer_api", 10, steer);		
	ros::spin();
	return 0;

}

