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
typedef pair<float, float> posizione; // corrisponderà a (x,y)

const float incremento_mappa = 2;

//classe che identifica uan posizione
/*class posizione {
    public:
    posizione(int x, int y);
    int x(){return x_;};
    int y(){return y_;};

    private:
    int x_;
    int y_;
};*/



void stampa_vettore_ostacoli(const set<posizione> &ostacoli);

float arrotonda_valore(float numero, bool minimo);

//classe che definisce l'intera mappa
class mappa {
public:
    mappa(string percorso_file_ostacoli, float dimensione_celle_in_metri = 1.0);
    const posizione posizione_minima() const {return minimo_mappa_;};
    const posizione posizione_massima() const {return massimo_mappa_;};
    float dimensione_celle_metri() const {return dimensione_celle_metri_;} ;
    void rendi_cella_ostacolo(posizione pos) {spazio_movimento_[pos] = false;};
    void rendi_cella_libera(posizione pos) {spazio_movimento_[pos] = true;};
    void stampa_ostacoli() const {stampa_vettore_ostacoli(ostacoli);};
    //stampo una matrice contenete 0 se non ho ostacoli e 1 se li ha,
    //inoltre ai lati sono presenti delle cornici per identificare la posizione delle celle
    void stampa_mappa(string filename);
    bool ostacolo_in_posizione(const posizione &pos) const {return spazio_movimento_.at(pos);};
    void crea_set_ostacoli(std::ifstream &file);

private:
    set<posizione> ostacoli;
    posizione minimo_mappa_;
    posizione massimo_mappa_;
    float dimensione_celle_metri_;
    map<posizione, bool> spazio_movimento_; //se booleano==false contiene robot o ostacolo
    void inserisci_celle(const posizione &minimo, const posizione &massimo);
};


#endif