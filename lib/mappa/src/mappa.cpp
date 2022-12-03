#include "mappa.h"

ostacolo::ostacolo(int x1, int y1, int x2, int y2) {
    auto x{std::minmax(x1,x2)};
    auto y{std::minmax(y1, y2)};
    pos_minima_ = std::make_pair(x.first,y.first);
    pos_massima_ = pos_minima_ = std::make_pair(x.second,y.second);
}


mappa::mappa(vector<ostacolo> ostacoli_non_ordinati)
: ostacoli{ostacoli_non_ordinati} {
    ostacoli.shrink_to_fit();
    //ordino il vettore di ostacoli in modo da trovare le dimensioni della mappa più velocemtnte
    std::sort(ostacoli.begin(), ostacoli.end());
    //incremento i valori per avere dimensioni maggiori nel caso avessi ostacoli alle estremità
    const posizione minimo_mappa_{ostacoli.begin()->posizione_minima().first - incremento_mappa,
                                    ostacoli.begin()->posizione_minima().second - incremento_mappa};
    
    const posizione massimo_mappa_{ostacoli.begin()->posizione_massima().first + incremento_mappa,
                                    ostacoli.begin()->posizione_massima().second + incremento_mappa};

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