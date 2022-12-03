#ifndef MAPPA_H
#define MAPPA_H

#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>

using std::map;
using std::pair;
using std::vector;
using std::cout;
using std::endl;
typedef pair<int, int> posizione; // corrisponderà a (x,y)

const int incremento_mappa = 5;

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

//classe che identifica un ostacolo
class ostacolo {
    public:
    ostacolo(int x1, int y1, int x2, int y2);
    const posizione posizione_minima() {return pos_minima_;};
    const posizione posizione_massima() {return pos_massima_;};
    bool operator<(ostacolo &da_confrontare) {return pos_massima_ < da_confrontare.posizione_massima();};

    private:
    posizione pos_minima_, pos_massima_;
};

int conta_ostacoli_da_file(std::ifstream &file);

void stampa_vettore_ostacoli(const vector<ostacolo> &ostacoli);

//classe che definisce l'intera mappa
class mappa {
    public:
    mappa(const vector<ostacolo> &ostacoli_non_ordinati);
    //mappa(vector<ostacolo> ostacoli_non_ordinati, float dimensione_celle);
    const posizione posizione_minima() {return minimo_mappa_;};
    const posizione posizione_massima() {return massimo_mappa_;};
    void rendi_cella_ostacolo(posizione pos) {spazio_movimento_[pos] = false;};
    void rendi_cella_libera(posizione pos) {spazio_movimento_[pos] = true;};
    void stampa_ostacoli(){stampa_vettore_ostacoli(ostacoli);};
    //stampo una matrice contenete 0 se non ho ostacoli e 1 se li ha,
    //inoltre ai lati sono presenti delle cornici per identificare la posizione delle celle
    void stampa_mappa(std::string filename);
    bool ostacolo_in_posizione(const posizione &pos) {return spazio_movimento_.find(pos)->second;};

    private:
    vector<ostacolo> ostacoli;
    posizione minimo_mappa_;
    posizione massimo_mappa_;
    float dimensione_celle_;
    map<posizione, bool> spazio_movimento_; //se booleano==false contiene robot o ostacolo
    void inserisci_celle(const posizione &minimo, const posizione &massimo, bool ostacolo);
};


#endif