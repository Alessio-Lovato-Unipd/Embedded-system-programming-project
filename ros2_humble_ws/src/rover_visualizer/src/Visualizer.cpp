// Internal dependencies
#include "rover_visualizer/Visualizer.h"

// ROS2 dependencies
#include "visualization_msgs/msg/marker.hpp"

// Standard dependencies
#include <fstream>
#include <iostream>

using std::placeholders::_1;

rover::Visualizer::Visualizer() : Node("visualizer_tool") {
  declare_parameter<std::string>("obstacle_positions_filename",
                                 "obstacle_positions.txt");
  get_parameter("obstacle_positions_filename", obstacle_positions_filename_);

  readObstaclePositions(obstacle_positions_filename_);

  declare_parameter<double>("rover_radius", 0.);
  get_parameter("rover_radius", rover_radius_);
  rover_radius_ = std::max(rover_radius_, 0.1);

  declare_parameter<std::string>("sub_topic_rover", "rover");
  get_parameter("sub_topic_rover", sub_topic_rover_);

  RCLCPP_INFO_STREAM(this->get_logger(),
                     "Subscriber topic name: " << sub_topic_rover_);
  rover_subscriber_ =
      this->create_subscription<rover_visualizer::msg::RoverPosition>(
          sub_topic_rover_, 10,
          std::bind(&rover::Visualizer::roverCallback, this, _1));

  declare_parameter<std::string>("pub_topic_markers", "markers");
  get_parameter("pub_topic_markers", pub_topic_markers_);
  marker_array_pub_ =
      this->create_publisher<visualization_msgs::msg::MarkerArray>(
          pub_topic_markers_, 10);
}

rover::Visualizer::~Visualizer() {}

void rover::Visualizer::roverCallback(
    const rover_visualizer::msg::RoverPosition::SharedPtr msg) {
  visualization_msgs::msg::MarkerArray marker_array{};
  drawObstacles(marker_array);

  visualization_msgs::msg::Marker marker{};

  marker.header.frame_id = "world";
  marker.header.stamp = this->now();
  marker.ns = "rover";
  marker.type = visualization_msgs::msg::Marker::CYLINDER;
  marker.action = visualization_msgs::msg::Marker::ADD;

  marker.color.a = 1.0f;  // alpha
  marker.color.r = 0.0f;
  marker.color.g = 0.0f;
  marker.color.b = 1.0f;

  marker.pose.orientation.x = 0.0;
  marker.pose.orientation.y = 0.0;
  marker.pose.orientation.z = 0.0;
  marker.pose.orientation.w = 1.0;

  marker.scale.x = 2 * rover_radius_;
  marker.scale.y = 2 * rover_radius_;
  marker.scale.z = 0.5;
  marker.pose.position.z = marker.scale.z / 2.;

  marker.id = 100 + msg->id;
  marker.pose.position.x = msg->position.x;
  marker.pose.position.y = msg->position.y;

  marker_array.markers.push_back(marker);

  marker_array_pub_->publish(marker_array);
}

void rover::Visualizer::readObstaclePositions(const std::string& filename) {
  std::ifstream infile{filename};
  ObstacleSize obstacle{};
  while (!infile.eof()) {
    infile >> obstacle.x_min >> obstacle.y_min >> obstacle.x_max >>
        obstacle.y_max;
    if (infile.eof() || infile.fail() || infile.bad()) {
      std::cerr << "Error in input or eof \n";
      break;
    }
    obstacles_.push_back(obstacle);
  }
}

void rover::Visualizer::drawObstacles(
    visualization_msgs::msg::MarkerArray& marker_array) {
  visualization_msgs::msg::Marker marker{};
  ObstacleSize environment_size = obstacles_.at(0);
  marker_array.markers.clear();
  int id_number = 0;
  marker.header.frame_id = "world";
  marker.header.stamp = this->now();
  marker.ns = "obstacles";
  marker.type = visualization_msgs::msg::Marker::CUBE;
  marker.action = visualization_msgs::msg::Marker::ADD;

  marker.color.a = 1.0f;  // alpha
  marker.color.r = 0.8f;
  marker.color.g = 0.3f;
  marker.color.b = 0.0f;

  marker.pose.orientation.x = 0.0;
  marker.pose.orientation.y = 0.0;
  marker.pose.orientation.z = 0.0;
  marker.pose.orientation.w = 1.0;

  marker.pose.position.z = 0.5;
  marker.scale.z = 1;
  for (const auto& obstacle : obstacles_) {
    environment_size.x_min = std::min(environment_size.x_min, obstacle.x_min);
    environment_size.y_min = std::min(environment_size.y_min, obstacle.y_min);
    environment_size.x_max = std::max(environment_size.x_max, obstacle.x_max);
    environment_size.y_max = std::max(environment_size.y_max, obstacle.y_max);

    marker.id = ++id_number;
    marker.pose.position.x = (obstacle.x_max + obstacle.x_min) / 2;
    marker.pose.position.y = (obstacle.y_max + obstacle.y_min) / 2;
    marker.scale.x = obstacle.x_max - obstacle.x_min;
    marker.scale.y = obstacle.y_max - obstacle.y_min;
    marker_array.markers.push_back(marker);
  }

  marker.id = 0;
  marker.ns = "environment";
  marker.color.a = 1.0f;  // alpha
  marker.color.r = 0.8f;
  marker.color.g = 0.4f;
  marker.color.b = 0.0f;
  marker.pose.position.x =
      (environment_size.x_max + environment_size.x_min) / 2;
  marker.pose.position.y =
      (environment_size.y_max + environment_size.y_min) / 2;
  marker.pose.position.z = -0.1;
  marker.scale.x = (environment_size.x_max - environment_size.x_min) * 1.5;
  marker.scale.y = (environment_size.y_max - environment_size.y_min) * 1.5;
  marker.scale.z = 0.2;
  marker_array.markers.push_back(marker);
}
