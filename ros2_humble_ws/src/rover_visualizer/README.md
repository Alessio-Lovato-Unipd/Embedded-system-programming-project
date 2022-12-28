# rover_visualizer

This version was tested under Ubuntu 22.04 and requires ROS2 Humble <https://docs.ros.org/en/humble/Installation/Ubuntu-Install-Debians.html>

## Descrizione

Per lanciare sia rviz sia il nodo rover_visualizer con il subscriber delle posizioni dei rover:
```
ros2 launch rover_visualizer rviz_launch.py
```

Per lanciare rviz, il nodo rover_visualizer e anche il nodo rover che controlla le posizioni dei rover:
```
ros2 launch rover_visualizer rover_launch.py 
```

Il nome del topic su cui pubblicare le posizioni dei rover è  ```/rover```.

Il messaggio da inviare è di tipo rover_visualizer::msg::RoverPosition:
```
int32 id
geometry_msgs/Point position
```
In ogni messaggio va indicata la posizione aggiornata del rover (position->x, position->y) ed il numero univoco identificativo del rover da inserire nel campo id. (per informazioni su geometry_msgs/Point: https://github.com/ros2/common_interfaces/blob/humble/geometry_msgs/msg/Point.msg)

Nei file xxx_launch.py possono essere modificati il nome del file in input(posizione ostacoli) ed il raggio che definisce le dimensioni dei rover.
