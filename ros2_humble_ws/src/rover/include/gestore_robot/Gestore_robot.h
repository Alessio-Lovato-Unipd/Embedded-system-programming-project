#ifndef GESTORE_ROBOT_H
#define GESTORE_ROBOT_H

#include "robot/robot.h"
#include <mutex>
#include <condition_variable>
#include <list>
#include <queue>
#include <syncstream>
#include <vector>


class Gestore_robot {
public:
	Gestore_robot(int numero_robot, int numero_satelliti, int numero_campioni_per_robot = 2);
	
	/*Funzione per assegnare un nuovo obbiettivo ad un robot
	  Parametri: 
	  	- robot: il robot al quale assegnare l'obbiettivo
	  Output: 
	  Ritorna true se l'assegnazione è andata a buon fine, altrimenti false
	*/
	bool assegna_obbiettivo(Robot &robot, const posizione &nuovo_obbiettivo);

	/*Funzione per inserire un obbiettivo nella lista dei campioni da raccogliere
	  Parametri:
	  	- obbiettivo: la posizione del campione da raccogliere
	*/
	void aggiungi_obbiettivo(const posizione &obbiettivo);

	/*Funzione per ottenere un nuovo obbiettivo per un robot
	  Parametri:
		- posisione_attuale_robot: posizione centrale del robot attuale in modo da
			trovare l'obbiettivo più vicino
	  Output:
	  	Posizione del prossimo obbiettivo del robot
	*/
	posizione ottieni_prossimo_obbiettivo(const posizione &posizione_attuale_robot);

	/*Funzione per creare un nuovo robot garantendo mutua esclusione dalla mappa
	  Parametri:
	  	- robot: posizione in cui mettere il robot
		- mappa_riferimento: mappa condivisa da tutti i robot
		- raggio_robot: dimensione del robot in metri
	  Output:
	  	Ritorna un istanza della classe Robot, ovvero un robot
	*/
	Robot crea_robot(const posizione &robot, Mappa &mappa_riferimento, const float raggio_robot);

	/*Funzione per poter spostare il robot nella cella successiva garantendo la mutua esclusione dalla mappa
	  Parametri:
	  	- robot: robot che si vuole spostare di una cella
		- potenziali: possibili celle su cui spostarsi
	  Output:
		Ritorna true se è stato possibile spostare il robot, altrimenti false
	*/
	bool sposta_robot(Robot &robot);
	
	/*Funzione per attivare un robot che attende i dati
	*/
	void attiva_robot() {buffer_non_vuoto.notify_one();};

	/*Funzioni accesso dati*/
	//Funzione per stampare il contenuto del buffer
	void stampa_buffer() const;
	//Restituisce true se sono ancora presenti campioni da raccogliere
	bool obbiettivi_presenti() const {return !obbiettivi.empty();};
	//ritorna lucchetto mutex mappa
	std::unique_lock<std::mutex> blocca_mappa ();
	//sblocca lucchetto mutex mappa
	void sblocca_mappa(std::unique_lock<std::mutex> &mutex) {mutex.unlock();};
	//elimino un satellite dal vettore, può essere invocata una sola volta per satellite
	void fine_obbiettivi_satellite() {satelliti_con_obbiettivi_--;};
	//ritorna true se alcuni satelliti hanno ancora dati da scrivere nel buffer
	bool satelliti_con_dati_presenti() const {return !(satelliti_con_obbiettivi_ == 0);};
	/*Fine funzioni accesso dati*/



private:
    std::mutex modifica_mappa_; //Mutex per limitare spostamento ad un solo robot alla volta
	std::mutex accesso_buffer_; //Mutex per limitare accesso al buffer posizioni
	std::list<posizione> obbiettivi; // Lista degli obbiettivi che i robot devono raggiungere
	const size_t numero_massimo_obbiettivi_; //Numero massimo di obbiettivi che è possibile salvare nella lista
	std::condition_variable buffer_non_vuoto; //variabile condizione per poter leggere dal buffer
	std::condition_variable buffer_non_pieno; //variabile condizione per poter scrivere nel buffer
	int satelliti_con_obbiettivi_; //array per identificare quanti satelliti hanno ancora dei dati da scrivere nel buffer
	
};

#endif