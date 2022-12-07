#ifndef ROBOT_H
#define ROBOT_H

#include "mappa.h"
#include <iostream>
#include <queue>

using std::queue;
typedef float distanza;
//In dati_cella: first == distanza, second == potenziale
typedef pair<distanza, float> dati_cella;


class Robot
{
public:
    Robot(const posizione &robot, const posizione &obbiettivo, mappa &mappa_riferimento, const float raggio_robot = 0.5);
    void raggiungi_obbiettivo(const posizione &obbiettivo);
    void aggiorna_mappa(mappa &mappa_riferimento) const {mappa_riferimento.aggiorna_mappa(mappa_);};
    void sposta_su_cella_successiva(mappa &mappa_condivisa);
    posizione posizione_attuale() const {return robot_celle_;};
    posizione obbiettivo() const {return obbiettivo_celle_;};

private:
    posizione robot_celle_;
    posizione robot_reale_;
    posizione obbiettivo_celle_;
    posizione obbiettivo_reale_;
    mappa mappa_;
    float raggio_;
    queue<posizione> posizioni_precedenti;

    void incrementa_mappa(const posizione &posizione_necessaria);
    void centra_posizione(distanza &pos);
    void inserisci_dati_robot_su_mappa(posizione &pos, string oggetto);
    float campo_attrattivo(const posizione &partenza, const posizione &arrivo) const;
    distanza calcolo_distanza(const posizione &partenza, const posizione &arrivo) const;
    pair<posizione, distanza> oggetto_piu_vicino(const posizione &attuale, const set<posizione>::const_iterator &inizio_set,
                                             const set<posizione>::const_iterator &fine_set);
    float campo_repulsivo(const posizione &partenza, const posizione &arrivo) const;
    map<posizione, dati_cella> calcola_potenziali_celle_adiacenti();
    float ricalcolo_potenziale_cella (const posizione &cella_da_ricalcolare, const posizione &robot_ostacolo) const;
    void aggiorna_campi_potenziale(map<posizione, dati_cella> &celle_con_potenziali);
};
#endif