// Copyright 2015 Open Source Robotics Foundation, Inc.
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

#include <iostream>
#include <memory>
#include <utility>

// include ROS 1
#ifdef __clang__
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#ifdef __clang__
# pragma clang diagnostic pop
#endif

// include ROS 2
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"


rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr pub;


void TFCallback(boost::shared_ptr<sensor_msgs::LaserScan> ros1_msg)
{
  if (pub->get_subscription_count() == 0)
    return;

  auto ros2_msg = std::make_unique<sensor_msgs::msg::LaserScan>();

  ros2_msg->header.frame_id = ros1_msg->header.frame_id;
  ros2_msg->header.stamp = rclcpp::Time(ros1_msg->header.stamp.toNSec());
  ros2_msg->angle_min = ros1_msg->angle_min;
  ros2_msg->angle_max = ros1_msg->angle_max;
  ros2_msg->angle_increment = ros1_msg->angle_increment;
  ros2_msg->time_increment = ros1_msg->time_increment;
  ros2_msg->scan_time = ros1_msg->scan_time;
  ros2_msg->range_min = ros1_msg->range_min;
  ros2_msg->range_max = ros1_msg->range_max;

  ros2_msg->ranges = std::move(ros1_msg->ranges);
  ros2_msg->intensities = std::move(ros1_msg->intensities);

  pub->publish(std::move(ros2_msg));
}

int main(int argc, char * argv[])
{
  // ROS 2 node and publisher
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("scan_1_to_2");
  pub = node->create_publisher<sensor_msgs::msg::LaserScan>("/scan", 100);

  // ROS 1 node and subscriber
  ros::init(argc, argv, "scan_1_to_2");
  ros::NodeHandle n;
  ros::Subscriber sub = n.subscribe("/scan", 100, TFCallback);

  ros::spin();

  rclcpp::shutdown();

  return 0;
}
