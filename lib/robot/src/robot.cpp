#include "robot.h"

Robot::Robot(const posizione &robot, const posizione &obbiettivo, mappa &mappa_riferimento, const float raggio_robot)
        : robot_celle_{robot}, robot_reale_{robot}, obbiettivo_celle_{obbiettivo}, obbiettivo_reale_{obbiettivo}, mappa_{mappa_riferimento}, raggio_{raggio_robot}
{
        inserisci_dati_robot_su_mappa(robot_celle_, "Il robot");
        inserisci_dati_robot_su_mappa(obbiettivo_celle_, "L'obbiettivo");      
        
        mappa_.posiziona_robot_cella(robot_celle_);

        //attendo mutex per aggiornare mappa
        aggiorna_mappa(mappa_riferimento);

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

void Robot::centra_posizione(distanza &pos) {
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
        else if (!mappa_.cella_libera(pos)) {//controllo che l'obbiettivo o il robot non siano in un ostacolo
                std::cerr << oggetto << " è stato inserito in una posizione occupata da un ostacolo" << std::endl;
                exit(EXIT_FAILURE);
        }

        //incremento in modo che la posizione del robot sia al centro di una cella
        pos.first += (((pos.first <= 0.0) ? (mappa_.dimensione_mezza_cella()) : (-mappa_.dimensione_mezza_cella())));
        pos.second += (((pos.second <= 0.0) ? (mappa_.dimensione_mezza_cella()) : (-mappa_.dimensione_mezza_cella())));       

}

float Robot::campo_attrattivo(const posizione &partenza, const posizione &arrivo) const {
        return ((mappa_.fattore_scala_campo_attrattivo() * pow(calcolo_distanza(partenza, arrivo), 2.0f))/2.0f);
}

distanza Robot::calcolo_distanza(const posizione &partenza, const posizione &arrivo) const {
        return (sqrt(pow((arrivo.first - partenza.first), 2.0f) + pow((arrivo.second - partenza.second), 2.0f)));
}

pair<posizione, distanza> Robot::oggetto_piu_vicino(const posizione &attuale, const set<posizione>::const_iterator &inizio_set, const set<posizione>::const_iterator &fine_set) {
        map<posizione, float> distanza_ostacolo;
        std::for_each(inizio_set, fine_set, [this, &attuale, &distanza_ostacolo](auto pos) {distanza_ostacolo[pos] = calcolo_distanza(attuale, pos);});
        auto elemento_minimo {min_element(distanza_ostacolo.cbegin(), distanza_ostacolo.cend(), [](auto &lhs, auto &rhs) { return lhs.second < rhs.second;})};
        return pair{elemento_minimo->first, elemento_minimo->second};
}

float Robot::campo_repulsivo(const posizione &partenza, const posizione &arrivo) const {
        distanza dist{calcolo_distanza(partenza, arrivo)};
        if (dist < mappa_.distanza_minima_ostacolo())
                return ((mappa_.fattore_scala_campo_repulsivo()*pow(((1/dist)+(1/mappa_.distanza_minima_ostacolo())),2.0f))/2.0f);
        else
                return 0.0;
}

map<posizione, dati_cella> Robot::calcola_potenziali_celle_adiacenti () {
        map<posizione, dati_cella> potenziali;
        posizione attuale{robot_celle_};
        //cancello la mia posizione dagli ostacoli
        mappa_.libera_cella_robot(robot_celle_);
        pair<posizione, distanza> ostacolo{oggetto_piu_vicino(attuale, mappa_.ostacoli_cbegin(), mappa_.ostacoli_cend())};
        //cella +/
        attuale.first += mappa_.dimensione_celle_metri();
        if (mappa_.contiene_cella(attuale) && mappa_.cella_libera(attuale))
                potenziali.insert({attuale, {ostacolo.second, campo_attrattivo(attuale,obbiettivo_celle_) + campo_repulsivo(attuale, ostacolo.first)}});
        //cella ++
        attuale.second += mappa_.dimensione_celle_metri();
        if (mappa_.contiene_cella(attuale) && mappa_.cella_libera(attuale))
                potenziali.insert({attuale, {ostacolo.second, campo_attrattivo(attuale,obbiettivo_celle_) + campo_repulsivo(attuale, ostacolo.first)}});
        //cella /+
        attuale.first -= mappa_.dimensione_celle_metri();
        if (mappa_.contiene_cella(attuale) && mappa_.cella_libera(attuale))
                potenziali.insert({attuale, {ostacolo.second, campo_attrattivo(attuale,obbiettivo_celle_) + campo_repulsivo(attuale, ostacolo.first)}});
        //cella -+
        attuale.first -= mappa_.dimensione_celle_metri();
        if (mappa_.contiene_cella(attuale) && mappa_.cella_libera(attuale))
                potenziali.insert({attuale, {ostacolo.second, campo_attrattivo(attuale,obbiettivo_celle_) + campo_repulsivo(attuale, ostacolo.first)}});
        //cella /-
        attuale.second -= mappa_.dimensione_celle_metri();
        if (mappa_.contiene_cella(attuale) && mappa_.cella_libera(attuale))
                potenziali.insert({attuale, {ostacolo.second, campo_attrattivo(attuale,obbiettivo_celle_) + campo_repulsivo(attuale, ostacolo.first)}});
        //cella --
        attuale.second -= mappa_.dimensione_celle_metri();
        if (mappa_.contiene_cella(attuale) && mappa_.cella_libera(attuale))
                potenziali.insert({attuale, {ostacolo.second, campo_attrattivo(attuale,obbiettivo_celle_) + campo_repulsivo(attuale, ostacolo.first)}});
        //cella /-
        attuale.first += mappa_.dimensione_celle_metri();
        if (mappa_.contiene_cella(attuale) && mappa_.cella_libera(attuale))
                potenziali.insert({attuale, {ostacolo.second, campo_attrattivo(attuale,obbiettivo_celle_) + campo_repulsivo(attuale, ostacolo.first)}});
        //cella +-
        attuale.first += mappa_.dimensione_celle_metri();
        if (mappa_.contiene_cella(attuale) && mappa_.cella_libera(attuale))
                potenziali.insert({attuale, {ostacolo.second, campo_attrattivo(attuale,obbiettivo_celle_) + campo_repulsivo(attuale, ostacolo.first)}});

        return potenziali;
}

float Robot::ricalcolo_potenziale_cella (const posizione &cella_da_ricalcolare, const posizione &robot_ostacolo) const {
        return (campo_attrattivo(cella_da_ricalcolare, robot_ostacolo) + campo_repulsivo(cella_da_ricalcolare, robot_ostacolo));
}

void Robot::aggiorna_campi_potenziale (map<posizione, dati_cella> &celle_con_potenziali) {
        for (auto elemento : celle_con_potenziali) {
                pair<posizione, distanza> ostacolo{oggetto_piu_vicino(elemento.first, mappa_.robot_cbegin(), mappa_.robot_cend())};
                if (elemento.first.second > ostacolo.second) {
                        //associo la nuova distanza alla cella
                        elemento.second.first = ostacolo.second;
                        elemento.second.second = ricalcolo_potenziale_cella(elemento.first, ostacolo.first);
                }
        }
}


void Robot::sposta_su_cella_successiva(mappa &mappa_condivisa){
        //prima faccio i calcoli sulla mappa che conosco, poi aggiorno le posizioni dei robot
        map<posizione, dati_cella> potenziali_celle{calcola_potenziali_celle_adiacenti()};
        
        //attendo mutex per leggere posizioni robot aggiornate
        mappa_.aggiorna_mappa(mappa_condivisa);
        aggiorna_campi_potenziale(potenziali_celle);
        //evito minimi locali
        posizione prossima_cella{std::min_element(potenziali_celle.cbegin(), potenziali_celle.cend(),
                                                        [](auto &lhs, auto &rhs) { return lhs.second.second < rhs.second.second;})->first};
        while (posizioni_precedenti.contains(prossima_cella)) {
                potenziali_celle.erase(prossima_cella);
                if (potenziali_celle.empty()) {
                        std::cerr << "Ci troviamo in un minimo locale, non è possibile muoversi" << endl;
                        exit(EXIT_FAILURE);
                }
                prossima_cella = std::min_element(potenziali_celle.cbegin(), potenziali_celle.cend(),
                                                        [](auto &lhs, auto &rhs) { return lhs.second.second < rhs.second.second;})->first;
        }
        mappa_.libera_cella_robot(robot_celle_);
        robot_celle_ = std::make_pair(prossima_cella.first, prossima_cella.second);
        mappa_.posiziona_robot_cella(prossima_cella);
        mappa_condivisa.aggiorna_mappa(mappa_);
        //rilascio chiave mutex
        posizioni_precedenti.insert(prossima_cella);

}