#ifndef MAPPA_H
#define MAPPA_H

#include <map>
#include <set>
#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>

using std::map;
using std::pair;
using std::set;
using std::string;
using std::cout;
using std::endl;

typedef pair<double, double> posizione; // corrisponderà a (x,y)

void stampa_vettore_ostacoli(const set<posizione> &ostacoli);


const double metro_{1.0f};

//classe che definisce l'intera mappa
class mappa {
public:
    mappa(string percorso_file_ostacoli, double dimensione_celle_in_metri = 1.0);
    const posizione posizione_minima() const {return minimo_mappa_;};
    const posizione posizione_massima() const {return massimo_mappa_;};
    double dimensione_celle_metri() const {return dimensione_celle_metri_;};
    double dimensione_mezza_cella() const {return dimensione_mezza_cella_;};
    void stampa_ostacoli() const {stampa_vettore_ostacoli(ostacoli);};
    //stampo una matrice contenete 0 se non ho ostacoli e 1 se li ha,
    //inoltre ai lati sono presenti delle cornici per identificare la posizione delle celle
    void stampa_mappa(string filename);
    bool cella_libera(const posizione &pos) const {return spazio_movimento_.at(pos);};
    void imposta_fattore_scala_repulsivo(double valore) {fattore_scala_campo_repulsivo_ = valore;};
    void imposta_fattore_scala_attrattivo(double valore) {fattore_scala_campo_attrattivo_ = valore;};
    void imposta_distanza_minima_ostacolo_percepito_(double valore) {distanza_minima_ostacolo_percepito_ = valore;};
    void centra_posizione (double &valore);

    //implementazione per robot
    bool contiene_cella(const posizione &pos) const {return spazio_movimento_.contains(pos);};
    void aggiorna_mappa(const mappa &nuova_mappa);
    map<posizione, bool>::const_iterator cbegin() const {return spazio_movimento_.cbegin();};
    map<posizione, bool>::const_iterator cend() const {return spazio_movimento_.cend();};
    set<posizione>::const_iterator ostacoli_cbegin() const {return ostacoli.cbegin();};
    set<posizione>::const_iterator ostacoli_cend() const {return ostacoli.cend();};
    set<posizione>::const_iterator robot_cbegin() const {return robot.cbegin();};
    set<posizione>::const_iterator robot_cend() const {return robot.cend();};
    double incremento_mappa_default() const {return incremento_mappa;};
    void inserisci_celle(const posizione &minimo, const posizione &massimo);
    void aggiorna_dimensioni (const posizione &minima, const posizione &massima) {minimo_mappa_ = minima; massimo_mappa_ = massima;};
    void posiziona_robot_cella(const posizione &pos) {robot.insert(pos);};
    void libera_cella_robot(const posizione &pos) {robot.erase(pos);};
    bool contains_robot(const posizione &pos) const {return robot.contains(pos);};
    double fattore_scala_campo_attrattivo() const {return fattore_scala_campo_attrattivo_;};
    double fattore_scala_campo_repulsivo() const {return fattore_scala_campo_repulsivo_;};
    double distanza_minima_ostacolo() const {return distanza_minima_ostacolo_percepito_;};

private:
    set<posizione> ostacoli;
    set<posizione> robot;
    posizione minimo_mappa_;
    posizione massimo_mappa_;
    double dimensione_celle_metri_;
    map<posizione, bool> spazio_movimento_; //se booleano==false contiene robot o ostacolo
    const double incremento_mappa = 2; //incremento in metri
    const double fattore_scala_celle_minimo = 2;
    const double metro = 1.0; //unità di riferimento

    const double dimensione_mezza_cella_{dimensione_celle_metri_/2.0};
    double fattore_scala_campo_attrattivo_{1.5};
    double fattore_scala_campo_repulsivo_{100.0};
    double distanza_minima_ostacolo_percepito_{5.0};
    void crea_set_ostacoli(std::ifstream &file);
    void cancella_robot(){robot.clear();};

};


#endif