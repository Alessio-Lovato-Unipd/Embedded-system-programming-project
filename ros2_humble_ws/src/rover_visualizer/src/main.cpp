#include "rover_visualizer/Visualizer.h"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<rover::Visualizer>());
  rclcpp::shutdown();

  exit(EXIT_SUCCESS);
}
