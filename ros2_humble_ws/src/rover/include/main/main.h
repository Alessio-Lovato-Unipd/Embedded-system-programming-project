//internal dependencies
#include "gestore_robot/Gestore_robot.h"
#include "rover/publisher.h"

// standard dependecies
#include <thread>
#include <chrono>
#include <vector>

//ROS dependencies
#include "rover_visualizer/msg/rover_position.hpp"

//costanti che rappresentano l'ordine degli argomenti da passare al main
const int dim_celle{1};
const int dim_robot{2};
const int pos_robot{3};
const int ostacoli{4};
const int obbiettivi_1{5};
const int obbiettivi_2{6};
const int percorso_mappa{7};
const int minimo_celle{8};
const int attrattivo{9};
const int repulsivo{10};
const int dist_ostacolo{11};
const int incremento{12};

/*Produttore
  Parametri:
  	- server: monitor per gestire le risorse comuni
	- id: id del satellite
	- file_obbiettivi: percorso del file dal quale ottenere gli obbiettivi
*/
void satellite(Gestore_robot &server, size_t id, const string &file_obbiettivi);

/*Funzione per ottenere le posizioni dei robot da un file di testo
  Parametri:
  	- file posizioni_robot: file dal quale ottenere i dati dei robot
*/
std::vector<posizione> ottieni_posizioni_robot(const std::string &file_posizioni_robot);

/*Funzione per creare mappa in base agli argomenti passati da main
  Parametri:
  	- argc: numero di argomenti passati da terminale
	- *argv[]: paramtri passati da terminale
*/
Mappa genera_mappa(int argc, char *argv[]);