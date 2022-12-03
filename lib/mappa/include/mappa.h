#ifndef MAPPA_H
#define MAPPA_H

#include <map>
#include <vector>
#include <algorithm>

using std::map;
using std::pair;
using std::vector;
typedef pair<int, int> posizione; // corrisponderà a (x,y)

const int incremento_mappa = 10;

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

    private:
    posizione pos_minima_, pos_massima_;
};

//classe che definisce l'intera mappa
class mappa {
    public:
    mappa(vector<ostacolo> ostacoli_non_ordinati);
    mappa(vector<ostacolo> ostacoli_non_ordinati, float dimensione_celle);
    void rendi_cella_ostacolo(posizione pos) {spazio_movimento_[pos] = false;};
    void rendi_cella_libera(posizione pos) {spazio_movimento_[pos] = true;};
    void stampa_mappa();

    private:
    vector<ostacolo> ostacoli;
    posizione minimo_mappa_;
    posizione massimo_mappa_;
    float dimensione_celle_;
    map<posizione, bool> spazio_movimento_; //se booleano==false contiene robot o ostacolo
    void inserisci_celle(const posizione &minimo, const posizione &massimo, bool ostacolo);
};

#endif