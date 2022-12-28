#include "rclcpp/rclcpp.hpp"
#include "rover/publisher.h"
#include "rover_visualizer/msg/rover_position.hpp"

rclcpp::Node::SharedPtr g_node;
rclcpp::Publisher<rover_visualizer::msg::RoverPosition>::SharedPtr g_publisher;

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);

  g_node = rclcpp::Node::make_shared("rover_publisher");
  g_publisher = g_node->create_publisher<rover_visualizer::msg::RoverPosition>(
      "rover", 10);

  // example code that spawns 2 rovers
  auto rover0 = Rover(0, 0.2);  // Spawn a rover with id 0 and "speed" 0.4m/s
                                // for each axis (0.2m every 500ms)
  auto rover1 = Rover(1, -0.2);  // Spawn a rover with id 1 and "speed" -0.4m/s
                                 // for each axis (0.2m every 500ms)
  rover1.setX_pos(10);  // Set rover1 x_pos to 10
  rover1.setY_pos(8);   // Set rover1 y_pos to 8

  rclcpp::spin(g_node);
  rclcpp::shutdown();

  exit(EXIT_SUCCESS);
}
