#include "mappa.h"

ostacolo::ostacolo(int x1, int y1, int x2, int y2) {
    auto x{std::minmax(x1, x2)};
    auto y{std::minmax(y1, y2)};
    pos_minima_ = std::make_pair(x.first,y.first);
    pos_massima_ = std::make_pair(x.second,y.second);
}


mappa::mappa(const vector<ostacolo> &ostacoli_non_ordinati)
: ostacoli{ostacoli_non_ordinati} {
    ostacoli.shrink_to_fit();
   
    //incremento i valori per avere dimensioni maggiori nel caso avessi ostacoli alle estremità
    
   minimo_mappa_ = {(min_element(ostacoli.begin(), ostacoli.end(), 
                                  [](auto &lhs, auto &rhs) { return lhs.posizione_minima().first < rhs.posizione_minima().first; }))->posizione_minima().first
                                  -incremento_mappa,

                                  (min_element(ostacoli.begin(), ostacoli.end(), 
                                  [](auto lhs, auto rhs) { return lhs.posizione_minima().second < rhs.posizione_minima().second; }))->posizione_minima().second
                                  - incremento_mappa};
    
   massimo_mappa_ = {(max_element(ostacoli.begin(), ostacoli.end(), 
                                  [](auto lhs, auto rhs) { return lhs.posizione_massima().first < rhs.posizione_massima().first; }))->posizione_massima().first
                                  + incremento_mappa,

                                  (max_element(ostacoli.begin(), ostacoli.end(), 
                                  [](auto lhs, auto rhs) { return lhs.posizione_massima().second < rhs.posizione_massima().second; }))->posizione_massima().second
                                  + incremento_mappa};
 
    //creo la mappa 
    inserisci_celle(massimo_mappa_, minimo_mappa_, false);
    //inserisco gli ostacoli nella mappa
    for (auto el : ostacoli) {
        inserisci_celle(el.posizione_minima(), el.posizione_massima(), true);
    }
    
}

void mappa::inserisci_celle(const posizione &minimo, const posizione &massimo, bool ostacolo) {
        posizione casella_corrente{minimo};
        while (casella_corrente.first <= massimo.first) {
            while (casella_corrente.second <= massimo.second)
            {
                spazio_movimento_[casella_corrente] = ostacolo;
                casella_corrente.second++;
            }
        casella_corrente.first++;
        casella_corrente.second = minimo.second;
    }
}

int conta_ostacoli_da_file(std::ifstream &file){
    std::string linea;
    int righe{0};
    while(getline(file, linea))
        righe++;
    return righe;
}

void stampa_vettore_ostacoli(const vector<ostacolo> &ostacoli) {
      for (auto elemento : ostacoli)
      cout << "x1: " << elemento.posizione_minima().first <<"  y1: " << elemento.posizione_minima().second
           <<"  x2: " << elemento.posizione_massima().first <<"  y2: " << elemento.posizione_massima().second <<endl;
    cout << endl;
}