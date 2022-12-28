#ifndef rover_visualizer_Visualizer_h
#define rover_visualizer_Visualizer_h

// ROS dependencies
#include "rclcpp/rclcpp.hpp"
#include "rover_visualizer/msg/rover_position.hpp"
#include "visualization_msgs/msg/marker_array.hpp"

namespace rover {

class Visualizer : public rclcpp::Node {
 public:
  Visualizer();
  ~Visualizer();

 private:
  typedef struct {
    double x_min, x_max, y_min, y_max;
  } ObstacleSize;

  void roverCallback(const rover_visualizer::msg::RoverPosition::SharedPtr msg);

  void readObstaclePositions(const std::string& filename);
  void drawObstacles(visualization_msgs::msg::MarkerArray& marker_array);

  double rover_radius_{};
  std::vector<ObstacleSize> obstacles_{};

  std::string obstacle_positions_filename_{};
  std::string sub_topic_rover_{};
  std::string pub_topic_markers_{};
  rclcpp::Subscription<rover_visualizer::msg::RoverPosition>::SharedPtr
      rover_subscriber_{};
  rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr
      marker_array_pub_{};
};

}  // namespace rover

#endif
