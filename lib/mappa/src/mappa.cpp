#include "mappa.h"

mappa::mappa(string percorso_file_ostacoli, float dimensione_celle_in_metri)
        : dimensione_celle_metri_{dimensione_celle_in_metri} {

    //verifico che la dimensione sia multiplo o sottomultiplo del metro
    if (dimensione_celle_metri_ == 0.0 || ((std::fmod(metro, dimensione_celle_metri_) != 0.0) && (std::fmod(dimensione_celle_metri_, metro)!= 0.0))) {
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
    minimo_mappa_ = {arrotonda_valore(((min_element(ostacoli.begin(), ostacoli.end(), 
                                [](auto &lhs, auto &rhs) { return lhs.first < rhs.first; }))->first - incremento_mappa), true),
                    arrotonda_valore(((min_element(ostacoli.begin(), ostacoli.end(), 
                                [](auto &lhs, auto &rhs) { return lhs.second < rhs.second; }))->second - incremento_mappa), true)};
       
    massimo_mappa_ = {arrotonda_valore(((max_element(ostacoli.begin(), ostacoli.end(), 
                                [](auto &lhs, auto &rhs) { return lhs.first < rhs.first; }))->first + incremento_mappa), false),
                    arrotonda_valore(((max_element(ostacoli.begin(), ostacoli.end(), 
                                [](auto &lhs, auto &rhs) { return lhs.second < rhs.second; }))->second + incremento_mappa), false)};

    //nel caso in cui la dimensione delle celle sia un multiplo del metro, porto la dimensione a un multiplo della dimensione cella
    if(dimensione_celle_metri_ > metro) {
        allinea_a_dim_celle(minimo_mappa_.first, true);
        allinea_a_dim_celle(minimo_mappa_.second, true);
        allinea_a_dim_celle(massimo_mappa_.first, false);
        allinea_a_dim_celle(massimo_mappa_.second, false);
    }

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

        //gestione della possibile incongruenza tra dimensione ostacolo e dimensione celle
        posizione max{x.second, y.second};
        posizione min{x.first, y.first};

        //divido caso multipli o sottomultipli
        if (dimensione_celle_metri_ <= metro) {
            modifica_ostacolo_sottomultipli(max.first, false);
            modifica_ostacolo_sottomultipli(max.second, false);
            modifica_ostacolo_sottomultipli(min.first, true);
            modifica_ostacolo_sottomultipli(min.second, true);
        } else {
            modifica_ostacolo_multipli(max.first, false);
            modifica_ostacolo_multipli(max.second, false);
            modifica_ostacolo_multipli(min.first, true);
            modifica_ostacolo_multipli(min.second, true);
        }
        //inserisco ostacoli mettendo un incremento della cella di dimensione/2 in modo da ottenere
        // il riferimento della posizione coincidente con la posizione in metri della cella
        min = {min.first + (dimensione_mezza_cella()), min.second + (dimensione_mezza_cella())};
        for (posizione pos{min}; pos.first < max.first; pos.first += dimensione_celle_metri_) {
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

float arrotonda_valore(float valore, bool minimo) {
    if (minimo)
        valore = floor(valore);
    else
        valore = ceil(valore);
    return valore;
}

void mappa::allinea_a_dim_celle(float &valore, bool minimo) {
    if (minimo) {
        while (fmod(valore, dimensione_celle_metri_) != 0.0)
            valore--;
    } else {
        while (fmod(valore, dimensione_celle_metri_) != 0.0)
            valore++;
    }
}

void mappa::modifica_ostacolo_sottomultipli (float &valore, bool minimo) {
    if (fmod(valore, dimensione_celle_metri_) == 0.0 )
        return;
    float limite{arrotonda_valore(valore, minimo)};
    if (minimo) {
        while(limite < valore)
            limite += dimensione_celle_metri_;
        limite -= dimensione_celle_metri_;
    } else {
        while(limite > valore)
            limite -= dimensione_celle_metri_;
        limite += dimensione_celle_metri_;
    }
    valore = limite;
}

void  mappa::modifica_ostacolo_multipli (float &valore, bool minimo) {
    if (fmod(valore, dimensione_celle_metri_) == 0.0 )
        return;
    valore = arrotonda_valore(valore, minimo);
    allinea_a_dim_celle(valore, minimo);
}

void mappa::inserisci_celle(const posizione &minimo, const posizione &massimo) {
        posizione casella_corrente{minimo};
        //mi posiziono con i valori al centro della prima cella, che corrisponde alla sua posizione
        casella_corrente.first += dimensione_mezza_cella();
        casella_corrente.second += dimensione_mezza_cella();
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
        casella_corrente.second = minimo.second + dimensione_mezza_cella();
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
    posizione stampa{minimo_mappa_.first + dimensione_mezza_cella(), massimo_mappa_.second - dimensione_mezza_cella()};
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
        stampa.first = minimo_mappa_.first + dimensione_mezza_cella();
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