#include <ros/ros.h>
#include <std_msgs/Bool.h>
#include <string>
#include <boost/asio.hpp>

const std::string serial_port = "/dev/ttyACM0";
const int baud_rate = 9600;
std_msgs::Bool servo_controll_servo;

void ControllState_cb(const std_msgs::Bool::ConstPtr &state)
{
    servo_controll_servo = *state;
    ROS_DEBUG("sub");
    ROS_DEBUG_STREAM("state : " << servo_controll_servo.data);
    if(servo_controll_servo.data)
    {
        try {
            // 创建串口对象
            boost::asio::io_service io;
            boost::asio::serial_port serial(io, serial_port);
            serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
            int count = 0;
            while(count < 1000)//因为发送信号的时间太短了，所以就让他一直发，加循环，不用让他再转回去了
            {
                char data_to_send = '1'; // 发送数据到Arduino（此处示例发送字符 '1'）
                boost::asio::write(serial, boost::asio::buffer(&data_to_send, 1)); // 将数据发送到Arduino
                std::cout << "Sent data: " << data_to_send << std::endl;
                ++count;
            }
            serial.close(); // 关闭串口连接
        } catch (const boost::system::system_error& e) {
            ROS_ERROR_STREAM("Error: " << e.what());
        }
    }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "serialServoControl");
    ros::NodeHandle nh;
    ros::Subscriber servo_controll_servo_sub = nh.subscribe<std_msgs::Bool>
            ("/ServoControl", 10, ControllState_cb);
    servo_controll_servo.data = false;
    ros::spin();
    return 0;
}

