from launch import LaunchDescription
from launch_ros.actions import Node

import os
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

    configuration_file = os.path.join(
        get_package_share_directory("rover_visualizer"),
        "config/mars.rviz"
        )

    return LaunchDescription([
        Node(
            package="rover",
            executable="rover",
            name="rover",
            namespace="",
            output="screen",
            emulate_tty=True,
        ),
        Node(
            package="rover_visualizer",
            executable="rover_visualizer",
            name="rover_visualizer",
            namespace="",
            output="screen",
            emulate_tty=True,
            parameters=[
                {"sub_topic_rover": "rover"},
                {"pub_topic_markers": "markers"},
                {"rover_radius": 0.2}, # Edit Rover radius
                {"obstacle_positions_filename": "obstacle_positions.txt"} # Edit obstacle_positions.txt path
            ]
        ),
        Node(
            package="rviz2",
            executable="rviz2",
            name="rviz_mars",
            output="screen",
            emulate_tty=True,
            arguments=["-d" + configuration_file]
        )
    ])
