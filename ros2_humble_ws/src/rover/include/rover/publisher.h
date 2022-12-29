#ifndef rover_Publisher_h
#define rover_Publisher_h

//internal dependencies
#include "mappa/Mappa.h"
#include "robot/robot.h"
#include "gestore_robot/Gestore_robot.h"

// ROS dependencies
#include "rclcpp/rclcpp.hpp"
#include "rover_visualizer/msg/rover_position.hpp"

// Standard dependencies
#include <iostream>
#include <condition_variable>
#include <mutex>

extern rclcpp::Node::SharedPtr g_node;
extern rclcpp::Publisher<rover_visualizer::msg::RoverPosition>::SharedPtr g_publisher;

//classe per salvare i dati del robot, contiene posizione e dimensione
class dati_robot {
public:
	//costruttore
	dati_robot(const posizione &posizione_robot, float dimensione) : pos_{posizione_robot}, dimensione_{dimensione} {};
	//accesso dati
	posizione posizione_robot() const {return pos_;};
	float dimensione() const {return dimensione_;};

private:
	posizione pos_; //posizione centrale del robot
	float dimensione_; //raggio del robot in metri
};

class Rover {
 public:
  /*Consumatore
    Parametri:
      - server: monitor per gestire le risorse comuni
    - id: id del robot
    - posizione del robot: classe contentente posizione del robot e dimensione
    - mappa_riferimento: mappa condivisa da tutti i robot
  */
  Rover(Gestore_robot &server, int id, const dati_robot &posizione_robot, Mappa &mappa_riferimento);

  /*Funzione principale che viene eseguita quando l'oggetto
    funzionale è usato in una thread
  */
  void operator() ();

  /* Funzione che sposta il robot di una cella e invia un messaggio
     con topic /rover a ROS2
  */
  void timer_callback();

 private:
  rclcpp::TimerBase::SharedPtr timer_{}; // timer per eseguire la callback
  void nuovo_obbiettivo(); // funzione che raggruppa le funzioni per assegnare un nuovo obbiettivo

  Gestore_robot &server_; // classe monitor che gestisce i robot
  const int id_; // id del robot
  Mappa &mappa_; // riferimento alla mappa condivisa
  Robot robot_; // istanza del robot
};

#endif
