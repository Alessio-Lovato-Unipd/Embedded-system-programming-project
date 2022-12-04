#include "mappa.h"

mappa::mappa(string percorso_file_ostacoli, float dimensione_celle_in_metri)
        : dimensione_celle_metri_{dimensione_celle_in_metri} {

    //verifico che la dimensione sia multiplo o sottomultiplo del metro
    if (dimensione_celle_metri_ == 0.0 || ((std::fmod(1.0,dimensione_celle_metri_) != 0.0) || (std::fmod(dimensione_celle_metri_, 1.0)!= 0.0))) {
        std::cerr << "Non è stato inserito un valore multiplo o sottomultiplo del metro" << std::endl;
        exit(EXIT_FAILURE);
    }
        if(percorso_file_ostacoli.empty()) {
        std::cerr << "Il percorso del file contenente gli ostacoli è nullo" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::ifstream file(percorso_file_ostacoli, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Errore nell'apertura di: " << percorso_file_ostacoli << endl;
        exit(EXIT_FAILURE);
    } else {
 
        //vettore contenente le posizioni degli ostacoli
        crea_set_ostacoli(file);
        file.close();

        //incremento i valori per avere dimensioni maggiori nel caso avessi ostacoli alle estremità
        // e rendo i numeri interi così da non aver problemi di mcm nel caso la dimensione degli ostacoli non combaciasse con le celle
        minimo_mappa_ = {arrotonda_valore(((min_element(ostacoli.begin(), ostacoli.end(), 
                                    [](auto &lhs, auto &rhs) { return lhs.first < rhs.first; }))->first - incremento_mappa), true),
                        arrotonda_valore(((min_element(ostacoli.begin(), ostacoli.end(), 
                                    [](auto &lhs, auto &rhs) { return lhs.second < rhs.second; }))->second - incremento_mappa), true)};
        
        massimo_mappa_ = {arrotonda_valore(((max_element(ostacoli.begin(), ostacoli.end(), 
                                    [](auto &lhs, auto &rhs) { return lhs.first < rhs.first; }))->first + incremento_mappa), false),
                        arrotonda_valore(((max_element(ostacoli.begin(), ostacoli.end(), 
                                    [](auto &lhs, auto &rhs) { return lhs.second < rhs.second; }))->second + incremento_mappa), false)};
        //creo la mappa 
        inserisci_celle(minimo_mappa_, massimo_mappa_);
        
            
    }
}

void mappa::crea_set_ostacoli(std::ifstream &file){
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
        posizione min{x.first + (dimensione_celle_metri_/2), y.first + (dimensione_celle_metri_/2)};
        posizione max{x.second, y.second};
        for (auto pos = min; pos.first < max.first; pos.first += dimensione_celle_metri_) {
            while (pos.second < max.second) {
                ostacoli.insert(pos);
                pos.second += dimensione_celle_metri_;
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


float arrotonda_valore(float numero, bool minimo) {
    if (minimo)
        numero = floor(numero);
    else
        numero = ceil(numero);
    return numero;
}

void mappa::inserisci_celle(const posizione &minimo, const posizione &massimo) {
        posizione casella_corrente{minimo};
        //mi posiziono con i valori al centro della prima cella, che corrisponde alla sua posizione
        casella_corrente.first += dimensione_celle_metri_/2;
        casella_corrente.second += dimensione_celle_metri_/2;
        while (casella_corrente.first < massimo.first) {
            while (casella_corrente.second < massimo.second)
            {
                if (ostacoli.contains(casella_corrente) == true)
                    spazio_movimento_[casella_corrente] = false;
                else 
                    spazio_movimento_[casella_corrente] = true;
                casella_corrente.second += dimensione_celle_metri_;
            }
        casella_corrente.first += dimensione_celle_metri_;
        casella_corrente.second = minimo.second + dimensione_celle_metri_/2;
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
    posizione stampa{minimo_mappa_.first + dimensione_celle_metri_/2, massimo_mappa_.second - dimensione_celle_metri_/2};
    while (stampa.second > minimo_mappa_.second) {
        while(stampa.first < massimo_mappa_.first) {
            file << !spazio_movimento_.at(stampa);
            stampa.first += dimensione_celle_metri_;
        }
        file << endl;
        stampa.second -= dimensione_celle_metri_;
        stampa.first = minimo_mappa_.first + dimensione_celle_metri_/2;
    }
    file.close();        
}