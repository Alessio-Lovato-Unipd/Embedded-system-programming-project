#include "robot.h"

Robot::Robot(const posizione &robot, const posizione &obbiettivo, mappa &mappa_riferimento, const float raggio_robot)
        : robot_celle_{robot}, robot_reale_{robot}, obbiettivo_celle_{obbiettivo}, obbiettivo_reale_{obbiettivo}, mappa_{mappa_riferimento}, raggio_{raggio_robot}
{
        inserisci_dati_robot_su_mappa(robot_celle_, "Il robot");
        inserisci_dati_robot_su_mappa(obbiettivo_celle_, "L'obbiettivo");      
        
        mappa_.rendi_cella_ostacolo(robot_celle_);
        mappa_.rendi_cella_ostacolo(obbiettivo_celle_);
        mappa_riferimento.aggiorna_mappa(mappa_);

}

void Robot::incrementa_mappa(const posizione &posizione_necessaria) {
        //calcolo le nuove posizioni minime
        posizione minima{std::min(posizione_necessaria.first, mappa_.posizione_minima().first),
                         std::min(posizione_necessaria.second, mappa_.posizione_minima().second)};
        posizione massima{std::max(posizione_necessaria.first, mappa_.posizione_massima().first),
                         std::max(posizione_necessaria.second, mappa_.posizione_massima().second)};
        //inserisco nuovamente incrementi
        if (minima.first < mappa_.posizione_minima().first)
                minima.first -= mappa_.incremento_mappa_default();
        if (minima.second < mappa_.posizione_minima().second)
                minima.second -= mappa_.incremento_mappa_default();
        if (massima.first > mappa_.posizione_massima().first)
                massima.first += mappa_.incremento_mappa_default();
        if (massima.second > mappa_.posizione_massima().second)
                massima.second += mappa_.incremento_mappa_default();
        
        mappa_.aggiorna_dimensioni(minima, massima);
        //inserisco le celle mancanti sovrascivendo le precedenti
        mappa_.inserisci_celle(minima, massima);
}

void Robot::centra_posizione(float &pos) {
        pos /= mappa_.dimensione_celle_metri();
        pos = arrotonda_valore(pos, (pos <= 0.0) ? true : false);
        pos *= mappa_.dimensione_celle_metri();
}

void Robot::inserisci_dati_robot_su_mappa(posizione &pos, string oggetto) {
        //localizzo robot nelle celle
        centra_posizione(pos.first);
        centra_posizione(pos.second);

        if (!mappa_.contiene_cella(pos))
                incrementa_mappa(pos);
        else if (mappa_.ostacolo_in_posizione(pos)) {//controllo che l'obbiettivo o il robot non siano in un ostacolo
                std::cerr << oggetto << " è stato inserito in una posizione occupata da un ostacolo" << std::endl;
                exit(EXIT_FAILURE);
        }

        //incremento in modo che la posizione del robot sia al centro di una cella
        pos.first += (((pos.first <= 0.0) ? (mappa_.dimensione_mezza_cella()) : (-mappa_.dimensione_mezza_cella())));
        pos.second += (((pos.second <= 0.0) ? (mappa_.dimensione_mezza_cella()) : (-mappa_.dimensione_mezza_cella())));       

}