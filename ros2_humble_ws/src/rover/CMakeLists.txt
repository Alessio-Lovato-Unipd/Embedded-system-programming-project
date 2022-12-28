cmake_minimum_required(VERSION 3.8)
project(rover)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)

find_package(rover_visualizer REQUIRED)

#impostazione verione di c++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

add_executable(${PROJECT_NAME}_node
    src/main.cpp
    src/publisher.cpp
    src/Gestore_robot.cpp
    src/Mappa.cpp
    src/robot.cpp
    # Add your cpp files here
)

set_target_properties(${PROJECT_NAME}_node
                      PROPERTIES OUTPUT_NAME
                      ${PROJECT_NAME}
                      PREFIX
                      "")

target_include_directories(${PROJECT_NAME}_node PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

ament_target_dependencies(${PROJECT_NAME}_node
    rclcpp
    std_msgs
    geometry_msgs
    rover_visualizer
)

install(TARGETS
  ${PROJECT_NAME}_node
  DESTINATION lib/${PROJECT_NAME})

ament_package()
