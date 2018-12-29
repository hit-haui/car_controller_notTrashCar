#include "iostream"
#include "ros/ros.h"
#include "std_msgs/Float32.h"
#include "std_msgs/Bool.h"
#include "api_i2c_pwm.h"
PCA9685 *pca9685 = new PCA9685() ;
bool STATUS = 0;
int dir = 0, throttle_val = 10;
char key = 0;
int getkey() {
	int character; 
	struct termios orig_term_attr;
	struct termios new_term_attr;

	/* set the terminal to raw mode */
	tcgetattr(fileno(stdin), &orig_term_attr);
	memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
	new_term_attr.c_lflag &= ~(ECHO|ICANON);
	new_term_attr.c_cc[VTIME] = 0;
	new_term_attr.c_cc[VMIN] = 0;
	tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);

	/* read a character from the stdin stream without blocking */
	/*   returns EOF (-1) if no character is available */
	character = fgetc(stdin);

	/* restore the original terminal attributes */
	tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);

	return character;
}

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
void status(const std_msgs::Bool::ConstPtr& msg)
{
	STATUS = msg->data;
	if (!STATUS)
	{
		double speed = 0;
		double steer = 0;
		api_set_FORWARD_control(pca9685, speed);
		api_set_STEERING_control( pca9685, steer);
	}
}
int main(int argc, char **argv)
{
	ros::init(argc, argv, "servo_controller");
	api_pwm_pca9685_init( pca9685 );

	double spd = 0;
	api_set_FORWARD_control(pca9685, spd);

	ros::NodeHandle n;
	ros::NodeHandle m;
	ros::NodeHandle p;
	ros::Subscriber sub1 = n.subscribe("/speed", 1000, speed);
	ros::Subscriber sub2 = m.subscribe("/steerAngle", 1000, steer);		
	ros::Subscriber sub3 = p.subscribe("/status", 1000, status);

	/**
	 * Spin dùng để loop các message
	 */
	ros::spin();

	return 0;

}

