#include "Gestore_robot.h"
#include "gnuplot.h"
#include <thread>
#include <chrono>
#include <vector>

//costanti che rappresentano l'ordine degli argomenti da passare al main
const int dim_celle{1};
const int pos_robot{2};
const int ostacoli{3};
const int obbiettivi_1{4};
const int obbiettivi_2{5};

//classe per salvare i dati del robot
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

/*Consumatore*/
void robot(Gestore_robot &server, size_t id, dati_robot posizione_robot, Mappa &mappa_riferimento);

/*Produttore*/
void satellite(Gestore_robot &server, size_t id, const string &file_obbiettivi);

/*Funzione per output a schermo della mappa*/
void visualizza_mappa(Gestore_robot &server, const Mappa &mappa, bool &termina_plot);

/*Funzione per stampare i dati necessari e successivamente la mappa*/
void stampa_gnuplot(const Mappa &map);

/*Funzione per ottenere le posizioni dei robot da un file di testo*/
std::vector<dati_robot> ottieni_posizioni_robot(const std::string &file_posizioni_robot);