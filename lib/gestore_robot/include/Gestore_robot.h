#ifndef GESTORE_ROBOT_H
#define GESTORE_ROBOT_H

#include "robot.h"
#include <mutex>
#include <condition_variable>
#include <list>
#include <queue>
#include <syncstream>

void stampa_gnuplot(const Mappa &map);

class Gestore_robot {
public:
	Gestore_robot(int numero_robot, int numero_campioni_per_robot = 2);
	
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

	posizione ottieni_prossimo_obbiettivo(const posizione &posizione_attuale_robot);

	Robot crea_robot(const posizione &robot, Mappa &mappa_riferimento, const float raggio_robot);

	bool sposta_robot(Robot &robot, mappa_potenziali &potenziali);

	void stampa_buffer() const;

	/*Funzioni accesso dati*/
	//Restituisce true se sono ancora presenti campioni da raccogliere
	bool obbiettivi_presenti() const {return !obbiettivi.empty();};
	//ritorna lucchetto mutex mappa
	std::unique_lock<std::mutex> blocca_mappa ();
	//sblocca lucchetto mutex mappa
	void sblocca_mappa(std::unique_lock<std::mutex> &mutex) {mutex.unlock();};
	/*Fine funzioni accesso dati*/



private:
    std::mutex modifica_mappa_; //Mutex per limitare spostamento ad un solo robot alla volta
	std::mutex accesso_buffer_; //Mutex per limitare accesso al buffer posizioni
	std::list<posizione> obbiettivi;
	const size_t numero_massimo_obbiettivi_;
	std::condition_variable buffer_non_vuoto;
	std::condition_variable buffer_non_pieno;
	std::condition_variable aggiornamento_mappa_possibile;
};

#endif