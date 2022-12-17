#ifndef ROBOT_H
#define ROBOT_H

#include "Mappa.h"
#include <iostream>

typedef float distanza;
//first == distanza, second == potenziale
typedef pair<distanza, float> dati_cella;
typedef map<posizione, dati_cella> mappa_potenziali;


class Robot {
public:
    /*Costruttore classe
	  Parametri:
	  	- robot: posizione (x,y) alla quale si trova il robot
		- obbiettivo: posizione (x,y) alla quale deve arrivare il robot
		- mappa riferimento: mappa alla quale riferirsi
		- [facoltativo] raggio_robot: dimensione del robot (essendo cilindrico il raggio) in metri
	*/
    Robot(const posizione &robot, Mappa &mappa_riferimento, const float raggio_robot = 0.5f);

	/*Funzione per spostare il robot alla cella circostante con potenziale minore rispetto all'obbiettivo
	  Parametri:
	  	- mappa_condivisa: mappa sulla quale fanno riferimento i robot
		- potenziali_celle: mappa contenente le informazioni delle celle adiacenti alla posizione centrale del robot
	  Output:
	  	Ritorna true se il posizionamento è stato eseguito correttamente, altrimenti ritorna false e stampa l'errore
	*/
    bool sposta_su_cella_successiva(Mappa &mappa_condivisa, mappa_potenziali &potenziali_celle);

	/*Funzione per calcolare il potenziale delle celle adiacenti alla posizione centrale attuale del robot. Il calcolo
	  viene eseguito solo rispetto agli ostacoli e non ai robot.
	*/
    mappa_potenziali calcola_potenziali_celle_adiacenti() ;
	/*Assegno un nuovo obbiettivo al robot
	  Parametri:
		- nuovo_obbiettivo: nuova posizione da raggiungere (verrà normalizzata alla griglia)
	  Output:
		Ritorna true se è stato possibile aggiornare l'obbiettivo, altrimenti false e stampa un errore
	*/
	bool nuovo_obbiettivo(const posizione &nuovo_obbiettivo);

	/*Funzioni accesso dati*/
	// Ritorna l'elenco delle celle occupate dal robot
    set<posizione> posizione_attuale() const {return celle_occupate_;};
	// Ritorna la posizione centrale delle celle occupate deal robot
    posizione posizione_centrale() const {return robot_celle_;};
	// Ritorna la posizione ce il robot deve raggiungere
    posizione obbiettivo() const {return obbiettivo_celle_;};
	// Ritorna true se una delle celle occupate dal robot contiene l'obbiettivo, altrimenti false
    bool obbiettivo_raggiunto() const {return celle_occupate_.contains(obbiettivo_celle_);};
	/* Fine funzioni accesso dati */

private:
    posizione robot_celle_; //Posizione centrale del robot nella mappa
    posizione obbiettivo_celle_; // Posizione dell'obbiettio nella mappa
    Mappa mappa_; //copia della mappa condivisa con gli altri robot
    const double raggio_; //Dimensione del raggio del robot
    set<posizione> posizioni_precedenti; //Elenco delle posizioni precedentemente percorse dal robot
    set<posizione> celle_occupate_; //Elenco delle celle occupate dal robot
	bool obbiettivo_stabilito_{false}; //Verifica che sia stato assegnato un obbiettivo al robot

	/*Funzione per modificare le dimensioni della mappa nel caso la posizione passata per argomento
	  non fosse già all'interno della mappa. Se le dimensioni vengono modificate, aggiungo celle ai bordi modificati.
	  Parametri:
	  	- posizione_necessaria: posizione da inserire nella mappa
	*/
    void incrementa_mappa(const posizione &posizione_necessaria);

	/*Funzione per il calcolo del campo attrattivo tra due posizioni
	  Parametri:
	  	- partenza: prima posizione da cui calcolare il potenziale
		- arrivo: seconda posizione alla quale è riferito il potenziale
	*/
    float campo_attrattivo(const posizione &partenza, const posizione &arrivo) const;

	/*Funzione per il calcolo del campo repulsivo tra due posizioni
	  Parametri:
	  	- partenza: prima posizione da cui calcolare il potenziale
		- arrivo: seconda posizione alla quale è riferito il potenziale
	*/
	float campo_repulsivo(const posizione &partenza, const posizione &arrivo) const;

	/*Funzione per il calcolo dell'oggetto più vicino ad una determinata posizione tra un elenco di posizioni occupate
	  Parametri:
	  	- attuale: posizione sulla quale calcolare l'oggetto più vicino
		- inizio_set: iteratore costante all'inizio del set contenente i punti da analizzare
		- fine_set: iteratore costante alla fine del set contenente i punti da analizzare
	*/
    pair<posizione, distanza> oggetto_piu_vicino(const posizione &attuale, const set<posizione>::const_iterator &inizio_set,
													const set<posizione>::const_iterator &fine_set);

	/*Funzione per ricalcolare il campo di potenziale su una cella rispetto ad un nuovo ostacolo o robot
	  Parametri:
	  	- cella da calcolare: posizione della cella sulla quale effettuare il ricalcolo
		- robot_ostacolo: posizione dell'ostacolo che modifica il potenziale
	*/
    float ricalcolo_potenziale_cella (const posizione &cella_da_ricalcolare, const posizione &robot_ostacolo) const;

	/*Funzione per ricalcolare il potenziale delle celle adiacenti alla posizione attuale (centrale) considerando i robot
	  presenti nella mappa. La lista dei robot è aggiornata all'inizio della funzione con la lista della mappa condivisa.
	  Parametri:
	  	-celle con potenziali: mappa con le celle calcolate in precedenza tramite la funzione 'calcola_potenziali_celle_adaicenti()'
	*/
    void aggiorna_campi_potenziale(mappa_potenziali &celle_con_potenziali);

    /*Funzione per verificare che il robot non faccia uno spostamento diagonale se adiacente ad
    un altro robot o ad un ostacolo.
    Parametri:
    - Mappa che rappresenta i possibili spostamenti futuri
    */
    void limita_spostamenti(mappa_potenziali &potenziali_celle);

    /*Funzione per calcolare la posizione delle celle adiacenti occupate dal robot
      rispetto alla posizione centrale
    Parametri: 
        - attuale : posizione centrale rispetto a cui calcolare le celle
    Output:
        - set<posizione>: set di variabili posizione
    */
    set<posizione> celle_adiacenti(const posizione &attuale) const;

    /*Funzione che restituisce true se in una determinata cella si ha collisione con
      altro robot o ostacolo, altrimenti false
    Parametri: 
        - prossima_cella : cella di cui si vuole calcolare la possibile collisione
    */
    bool collisione(const posizione &cella) const;
};

	/*Funzione per il calcolo della distanza euclidea tra due posizioni
	  Parametri:
	  	- partenza: prima posizione da cui calcolare la distanza
		- arrivo: seconda posizione alla quale è riferita la distanza
	*/
    distanza calcolo_distanza(const posizione &partenza, const posizione &arrivo);

#endif