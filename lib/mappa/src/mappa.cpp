#include "mappa.h"

mappa::mappa(string percorso_file_ostacoli, double dimensione_celle_in_metri)
        : dimensione_celle_metri_{dimensione_celle_in_metri} {

    //verifico che la dimensione sia multiplo o sottomultiplo del metro
    if (dimensione_celle_metri_ == 0.0) {
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
    } 

    //vettore contenente le posizioni degli ostacoli
    crea_set_ostacoli(file);
        
    file.close();

    //incremento i valori per avere dimensioni maggiori nel caso avessi ostacoli alle estremità
    // e rendo i numeri interi così da non aver problemi di mcm nel caso la dimensione degli ostacoli non combaciasse con le celle
    minimo_mappa_ = {min_element(ostacoli.begin(), ostacoli.end(), 
                                [](auto &lhs, auto &rhs) { return lhs.first < rhs.first; })->first - (incremento_mappa * dimensione_celle_metri_),
                    min_element(ostacoli.begin(), ostacoli.end(), 
                                [](auto &lhs, auto &rhs) { return lhs.second < rhs.second; })->second - (incremento_mappa * dimensione_celle_metri_)};
       
    massimo_mappa_ = {max_element(ostacoli.begin(), ostacoli.end(), 
                                [](auto &lhs, auto &rhs) { return lhs.first < rhs.first; })->first + (incremento_mappa * dimensione_celle_metri_),
                    max_element(ostacoli.begin(), ostacoli.end(), 
                                [](auto &lhs, auto &rhs) { return lhs.second < rhs.second; })->second + (incremento_mappa * dimensione_celle_metri_)};


    //creo la mappa 
    if (dimensione_celle_metri_ > ((massimo_mappa_.first - minimo_mappa_.first)/fattore_scala_celle_minimo) ||
        dimensione_celle_metri_ > ((massimo_mappa_.second - minimo_mappa_.second)/fattore_scala_celle_minimo)) {
            std::cerr << "E' stata inserita una dimensione delle celle troppo elevata" << endl;
            exit(EXIT_FAILURE);
        }

    inserisci_celle(minimo_mappa_, massimo_mappa_);
        
}

void mappa::crea_set_ostacoli(std::ifstream &file){
    string line;
   
    while (getline(file, line)) {
        auto virgola{line.find(',')};
        double x1{stof(line.substr(0, virgola))};

        auto virgola_successiva{line.find(',', virgola + 1)};
		    double y1{stof(line.substr(virgola + 1, virgola_successiva))};

        virgola = line.find(',', virgola_successiva + 1);
		    double x2{stof(line.substr(virgola_successiva + 1, virgola))};

        virgola_successiva = line.find(',', virgola + 1);
		    double y2{stof(line.substr(virgola + 1, virgola_successiva))};

        //calcolo celle da inserire
        posizione x{std::minmax(x1,x2)};
        posizione y{std::minmax(y1,y2)};

        //gestione della possibile incongruenza tra dimensione ostacolo e dimensione celle
        posizione max{x.second, y.second};
        posizione min{x.first, y.first};

        centra_posizione(max.first);
        centra_posizione(max.second);
        centra_posizione(min.first);
        centra_posizione(min.second);

        //inserisco ostacoli mettendo un incremento della cella di dimensione/2 in modo da ottenere
        // il riferimento della posizione coincidente con la posizione in metri della cella
        for (posizione pos{min}; pos.first < max.first; pos.first += dimensione_celle_metri_) {
            while (pos.second < max.second) {
                ostacoli.insert(pos);
                pos.second += dimensione_celle_metri_;
            }
            pos.second = min.second;
        }
    }

}

void stampa_vettore_ostacoli(const set<posizione> &ostacoli) {
      for (auto elemento : ostacoli)
        cout << "x: " << elemento.first <<"  y: " << elemento.second << endl;
    cout << endl;
}

void mappa::centra_posizione (double &valore) {
    double limite{valore};
    limite /= dimensione_celle_metri_;
    if (valore < 0.0)
        limite = floor(limite);
    else
        limite = ceil(limite);

    limite *= dimensione_celle_metri_;
    if (valore < 0.0)
        limite -= dimensione_mezza_cella_;
    else
        limite += dimensione_mezza_cella_;

    valore = limite;
}

void mappa::inserisci_celle(const posizione &minimo, const posizione &massimo) {
        posizione casella_corrente{minimo};
        //mi posiziono con i valori al centro della prima cella, che corrisponde alla sua posizione
        casella_corrente.first += dimensione_mezza_cella_;
        casella_corrente.second += dimensione_mezza_cella_;
        while (casella_corrente.first < massimo.first) {
            while (casella_corrente.second < massimo.second)
            {
                if (ostacoli.contains(casella_corrente) == true)
                    spazio_movimento_.insert({casella_corrente, false});
                else 
                    spazio_movimento_.insert({casella_corrente, true});
                casella_corrente.second += dimensione_celle_metri_;
            }
        casella_corrente.first += dimensione_celle_metri_;
        casella_corrente.second = minimo.second + dimensione_mezza_cella_;
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
    posizione stampa{minimo_mappa_.first + dimensione_mezza_cella_, massimo_mappa_.second - dimensione_mezza_cella_};
    while (stampa.second > minimo_mappa_.second) {
        while(stampa.first < massimo_mappa_.first) {
            if (contains_robot(stampa))
                file << "2";
            else
                file << !spazio_movimento_.at(stampa);
            stampa.first += dimensione_celle_metri_;
        }
        file << endl;
        stampa.second -= dimensione_celle_metri_;
        stampa.first = minimo_mappa_.first + dimensione_mezza_cella_;
    }
    file.close();        
}

void mappa::aggiorna_mappa(const mappa &riferimento) {
    std::for_each(riferimento.cbegin(), riferimento.cend(), [this](auto &elemento) {spazio_movimento_[elemento.first] = elemento.second;});
    minimo_mappa_ = riferimento.posizione_minima();
    massimo_mappa_ = riferimento.posizione_massima();
    cancella_robot();
    std::for_each(riferimento.robot_cbegin(), riferimento.robot_cend(), [this](auto &el) {posiziona_robot_cella(el);});
}