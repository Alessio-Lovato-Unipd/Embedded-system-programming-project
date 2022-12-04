#include "mappa.h"

void crea_set_ostacoli(std::ifstream &file, set<posizione> &set, float dimensione_cella){
    string line;
   
    while (getline(file, line)) {
        auto virgola{line.find(',')};
        float x1{stof(line.substr(0, virgola))};

        auto virgola_successiva{line.find(',', virgola + 1)};
		    float y1{stof(line.substr(virgola + 1, virgola_successiva))};

        virgola = line.find(',', virgola_successiva + 1);
		    float x2{stof(line.substr(virgola_successiva + 1, virgola))};

        virgola_successiva = line.find(',', virgola + 1);
		    float y2{stof(line.substr(virgola + 1, virgola_successiva))};

        //calcolo celle da inserire
        posizione x{std::minmax(x1,x2)};
        posizione y{std::minmax(y1,y2)};
        //mettendo un incremento della cella di dimensione/2 otteniamo che il fiperimento della posizione
        //coinciderà con la posizione in metri della cella
        posizione min{x.first + (dimensione_cella/2), y.first + (dimensione_cella/2)};
        posizione max{x.second, y.second};
        for (auto pos = min; pos.first < max.first; pos.first += dimensione_cella) {
            while (pos.second < max.second) {
                set.insert(pos);
                pos.second += dimensione_cella;
            }
            pos.second = min.second;
        }
    }

    file.close();
}

void stampa_vettore_ostacoli(const set<posizione> &ostacoli) {
      for (auto elemento : ostacoli)
        cout << "x: " << elemento.first <<"  y: " << elemento.second << endl;
    cout << endl;
}

mappa::mappa(const set<posizione> &ostacoli_non_ordinati, float dimensione_celle_in_metri)
        : ostacoli{ostacoli_non_ordinati}, dimensione_celle_metri_{dimensione_celle_in_metri} {
    
    //verifico che la dimensione sia multiplo o sottomultiplo del metro
    if (dimensione_celle_metri_ == 0.0 || ((std::fmod(1.0,dimensione_celle_metri_) != 0.0) && (std::fmod(dimensione_celle_metri_, 1.0)!= 0))) {
        std::cerr << "Non è stato inserito un valore multiplo o sottomultiplo del metro" << std::endl;
        exit(EXIT_FAILURE);
    }
    //incremento i valori per avere dimensioni maggiori nel caso avessi ostacoli alle estremità 
    minimo_mappa_ = {(min_element(ostacoli.begin(), ostacoli.end(), 
                                  [](auto &lhs, auto &rhs) { return lhs.first < rhs.first; }))->first - incremento_mappa,
                     (min_element(ostacoli.begin(), ostacoli.end(), 
                                  [](auto &lhs, auto &rhs) { return lhs.second < rhs.second; }))->second - incremento_mappa};
    
    massimo_mappa_ = {(max_element(ostacoli.begin(), ostacoli.end(), 
                                  [](auto &lhs, auto &rhs) { return lhs.first < rhs.first; }))->first + incremento_mappa,
                     (max_element(ostacoli.begin(), ostacoli.end(), 
                                  [](auto &lhs, auto &rhs) { return lhs.second < rhs.second; }))->second + incremento_mappa};
    //creo la mappa 
    inserisci_celle(minimo_mappa_, massimo_mappa_, ostacoli);

}

void mappa::inserisci_celle(const posizione &minimo, const posizione &massimo, const set<posizione> &ostacoli) {
        posizione casella_corrente{minimo};
        while (casella_corrente.first <= massimo.first) {
            while (casella_corrente.second <= massimo.second)
            {
                if (ostacoli.contains(casella_corrente) == true)
                    spazio_movimento_[casella_corrente] = false;
                else 
                    spazio_movimento_[casella_corrente] = true;
                casella_corrente.second += dimensione_celle_metri_;
            }
        casella_corrente.first += dimensione_celle_metri_;
        casella_corrente.second = minimo.second;
    }
}


void mappa::stampa_mappa(std::string filename) {
    std::ofstream file(filename, std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Impossibile stampare mappa, il file non è presente!" << std::endl;
        return;
        }
    //ciclo stampa valori
    // 0 -> no ostacoli
    // 1 -> ostacoli
    posizione stampa{minimo_mappa_.first, massimo_mappa_.second};
    while (stampa.second >= minimo_mappa_.second) {
        while(stampa.first <= massimo_mappa_.first) {
             if (spazio_movimento_.find(stampa)->second == false)
                file << "1";
            else
                file << "0";
            stampa.first++;
        }
        file << endl;
        stampa.second--;
        stampa.first = minimo_mappa_.first;
    }
    file.close();        
}