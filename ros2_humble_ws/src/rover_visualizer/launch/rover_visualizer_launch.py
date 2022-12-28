from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
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
                {"rover_radius": 0.5}, # Edit Rover radius
                {"obstacle_positions_filename": "obstacle_positions.txt"} # Edit obstacle_positions.txt path
            ]
        )
    ])
