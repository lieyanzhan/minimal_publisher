// Copyright 2016 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

#include <chrono>
#include <math.h>
#include <serial/serial.h> //ROS已经内置了的串口包
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include "sensor_msgs/msg/magnetic_field.hpp"
// #include "tf2/LinearMath/Quaternion.h"
// #include "tf2_ros/transform_broadcaster.h"
// #include "tf2_ros/static_transform_broadcaster.h"
// #include "nav_msgs/msg/odometry.hpp"

#include "rclcpp/rclcpp.hpp"
#include "CJY61.h"


using namespace std::chrono_literals;
using namespace std;

/* This example creates a subclass of Node and uses std::bind() to register a
 * member function as a callback from the timer. */

// class MinimalPublisher : public rclcpp::Node
// {
// public:
//   MinimalPublisher()
//   : Node("minimal_publisher"), count_(0)
//   {
//     publisher_ = this->create_publisher<std_msgs::msg::String>("topic", 10);
//     timer_ = this->create_wall_timer(
//       100ms, std::bind(&MinimalPublisher::timer_callback, this));
//   }

// private:
//   void timer_callback()
//   {
//     auto message = std_msgs::msg::String();
//     message.data = "lcq! " + std::to_string(count_++);
//     RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
//     publisher_->publish(message);
//   }
//   rclcpp::TimerBase::SharedPtr timer_;
//   rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
//   size_t count_;
// };

// int main(int argc, char * argv[])
// {
//   rclcpp::init(argc, argv);
//   rclcpp::spin(std::make_shared<MinimalPublisher>());
//   rclcpp::shutdown();
//   return 0;
// }

using namespace std::chrono_literals;

class WitImuNode : public rclcpp::Node
{

public:
    WitImuNode()
    : Node("wit_imu_node")
    {
        
        int output_hz = 100000;
        // 表示多少ms回调一次方法
        timer_ms = std::chrono::milliseconds(output_hz / 1000);
        port = "/dev/ttyUSB0";
        baudrate = 115200;
        try
        {
            ser.setPort(port);
            ser.setBaudrate(baudrate);
            serial::Timeout to = serial::Timeout::simpleTimeout(500);
            ser.setTimeout(to);
            ser.open();
        }
        catch (serial::IOException &e)
        {
            RCLCPP_INFO(this->get_logger(), "Unable to open port ");
            return;
        }

        if (ser.isOpen())
        {
            cout << "Serial is open" << endl;
            RCLCPP_INFO(this->get_logger(), "Serial Port initialized");
        }
        else
        {
            return;
        }

        pub_imu = this->create_publisher<sensor_msgs::msg::Imu>("/imu", 20);
        pub_imu_offline = this->create_publisher<sensor_msgs::msg::Imu>("/imu_offline", 20);
        // imu = CJY901();
        
        ser.flush();
        // 定时执行
        timer_ = this->create_wall_timer(
            timer_ms, std::bind(&WitImuNode::timer_callback, this));
    }

private:
    void timer_callback()
    {
        int count = ser.available();
        if (count != 0)
        {
            //cout << "count: " << count << endl;
            //ROS_INFO_ONCE("Data received from serial port.");
            int num;
            rclcpp::Time now = this->get_clock()->now();
            unsigned char read_buf[count];
            num = ser.read(read_buf, count);
            imu.FetchData((char *)read_buf, num);
            sensor_msgs::msg::Imu imu_data;
            sensor_msgs::msg::Imu imu_offline_data;
            //==============imu data===============
            imu_data.header.stamp = now;
            //imu_data.header.frame_id = imu_frame;
            imu_data.header.frame_id = "imu_frame";

            imu_data.linear_acceleration.x = imu.acc.x;
            imu_data.linear_acceleration.y = imu.acc.y;
            imu_data.linear_acceleration.z = imu.acc.z;
            
            imu_data.angular_velocity.x = imu.angle.r * 180.0 / M_PI;
            imu_data.angular_velocity.y = imu.angle.p * 180.0 / M_PI;
            imu_data.angular_velocity.z = imu.angle.y * 180.0 / M_PI;
            // cout << imu_data.linear_acceleration.x << endl;

            // tf2::Quaternion curr_quater;
            // curr_quater.setRPY(imu.angle.r, imu.angle.p, imu.angle.y); //zyx

            // RCLCPP_INFO(this->get_logger(), "angle: x=%f, y=%f, z=%f", imu.angle.r, imu.angle.p, imu.angle.y);

            // imu_data.orientation.x = curr_quater.x();
            // imu_data.orientation.y = curr_quater.y();
            // imu_data.orientation.z = curr_quater.z();
            // imu_data.orientation.w = curr_quater.w();
            // // RCLCPP_INFO(this->get_logger(), "Quaternion: x=%f, y=%f, z=%f, w=%f", 
            // //   imu_data.orientation.x, imu_data.orientation.y, imu_data.orientation.z, imu_data.orientation.w);

            // //==============imu offline data===============
            // imu_offline_data.header.stamp = now;
            // imu_offline_data.header.frame_id = imu_frame;

            // imu_offline_data.linear_acceleration.x = imu.acc.x;
            // imu_offline_data.linear_acceleration.y = imu.acc.y;
            // imu_offline_data.linear_acceleration.z = imu.acc.z;
            
            // imu_offline_data.angular_velocity.x = imu.gyro.x;
            // imu_offline_data.angular_velocity.y = imu.gyro.y;
            // imu_offline_data.angular_velocity.z = imu.gyro.z;

            // imu_offline_data.orientation.x = curr_quater.x();
            // imu_offline_data.orientation.y = curr_quater.y();
            // imu_offline_data.orientation.z = curr_quater.z();
            // imu_offline_data.orientation.w = curr_quater.w();

            // pub_imu->publish(imu_data);
            // pub_imu_offline->publish(imu_offline_data);
        }
    }
    
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr              pub_imu;
    rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr              pub_imu_offline;
    CJY61 imu;
    std::string port;
    int baudrate;
    serial::Serial ser;
    std::chrono::milliseconds timer_ms;
    std::string imu_topic;
    std::string imu_offline_topic;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<WitImuNode>());
    rclcpp::shutdown();
    return 0;
}