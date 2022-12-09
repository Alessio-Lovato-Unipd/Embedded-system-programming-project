#ifndef ROBOT_H
#define ROBOT_H

#include "mappa.h"
#include <iostream>

typedef float distanza;
//In dati_cella: first == distanza, second == potenziale
typedef pair<distanza, float> dati_cella;


class Robot
{
public:
    Robot(const posizione &robot, const posizione &obbiettivo, mappa &mappa_riferimento, const float raggio_robot = 0.5f);
    void aggiorna_mappa(mappa &mappa_riferimento) const {mappa_riferimento.aggiorna_mappa(mappa_);};
    void sposta_su_cella_successiva(mappa &mappa_condivisa);
    set<posizione> posizione_attuale() const {return celle_occupate_;};
    posizione posizione_centrale() const {return robot_celle_;};
    posizione obbiettivo() const {return obbiettivo_celle_;};
    bool obbiettivo_raggiunto() const {return celle_occupate_.contains(obbiettivo_celle_);};

private:
    posizione robot_celle_;
    posizione robot_reale_;
    posizione obbiettivo_celle_;
    posizione obbiettivo_reale_;
    mappa mappa_;
    double raggio_;
    set<posizione> posizioni_precedenti;
    set<posizione> celle_occupate_;

    void incrementa_mappa(const posizione &posizione_necessaria);
    float campo_attrattivo(const posizione &partenza, const posizione &arrivo) const;
    distanza calcolo_distanza(const posizione &partenza, const posizione &arrivo) const;
    pair<posizione, distanza> oggetto_piu_vicino(const posizione &attuale, const set<posizione>::const_iterator &inizio_set,
                                             const set<posizione>::const_iterator &fine_set);
    float campo_repulsivo(const posizione &partenza, const posizione &arrivo) const;
    map<posizione, dati_cella> calcola_potenziali_celle_adiacenti();
    float ricalcolo_potenziale_cella (const posizione &cella_da_ricalcolare, const posizione &robot_ostacolo) const;
    void aggiorna_campi_potenziale(map<posizione, dati_cella> &celle_con_potenziali);

    /*Funzione per verificare che il robot non faccia uno spostamento diagonale se adiacente ad
    un altro robot o ad un ostacolo.
    Parametri:
    - mappa che rappresenta i possibili spostamenti futuri
    */
    void limita_spostamenti(map<posizione, dati_cella> &potenziali_celle);

    /*Funzione per calcolare la posizione delle celle adiacenti occupate dal robot
      rispetto alla posizione centrale
    Parametri: 
        - attuale : posizione centrale rispetto a cui calcolare le celle
    Output:
        - set<posizione>: set di variabili posizione
    */
    set<posizione> celle_adiacenti(const posizione &attuale) const;

    /*Funzione verificare se in una determinata cella di ha collisione con
      altro robot o ostacolo
    Parametri: 
        - prossima_cella : cella di cui si vuole calcolare la possibile collisione
    Output:
        - bool : true se avviene collisione, false altrimenti
    */
    bool collisione(const posizione &cella) const;
};
#endif