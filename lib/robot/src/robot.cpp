#include "robot.h"

Robot::Robot(const posizione &robot, const posizione &obbiettivo, mappa &mappa_riferimento, const float raggio_robot)
        : robot_{robot}, obbiettivo_{obbiettivo}, mappa_{mappa_riferimento}, raggio_{raggio_robot}
{
        //centro il robot in una cella
        centra_posizione(robot_.first);
        centra_posizione(robot_.second);

        if (!mappa_.contiene_cella(robot_))
                incrementa_mappa(robot_);

        //incremento in modo che la posizione del robot sia al centro di una cella
        robot_.first += (((robot_.first <= 0.0) ? (mappa_.dimensione_celle_metri()/2) : (-mappa_.dimensione_celle_metri()/2)));
        robot_.second += (((robot_.second <= 0.0) ? (mappa_.dimensione_celle_metri()/2) : (-mappa_.dimensione_celle_metri()/2)));       
        
        //centro l'obbiettivo in una cella
        //.......
        //controllo che l'obbiettivo o il robot non siano in un ostacolo

        mappa_.rendi_cella_ostacolo(robot_);
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