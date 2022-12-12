#include "Mappa.h"

Mappa::Mappa(string percorso_file_ostacoli, double dimensione_celle_in_metri, int fattore_minimo_scala_celle, double fattore_scala_campo_attrattivo,
    double fattore_scala_campo_repulsivo, double distanza_minima_ostacolo_percepito, int incremento_celle_mappa)
        : dimensione_celle_metri_{dimensione_celle_in_metri}, fattore_minimo_scala_celle_{fattore_minimo_scala_celle},
          fattore_scala_campo_attrattivo_{fattore_scala_campo_attrattivo}, fattore_scala_campo_repulsivo_{fattore_scala_campo_repulsivo},
          distanza_minima_ostacolo_percepito_{distanza_minima_ostacolo_percepito}, incremento_mappa_{incremento_celle_mappa} {

    //verifico che la dimensione non sia nulla
    if (dimensione_celle_metri_ == 0.0) {
        std::cerr << "E' stata inserito un valore nullo per la dimensione della cella" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    if (percorso_file_ostacoli.empty()) {
        std::cerr << "Il percorso del file contenente gli ostacoli è nullo" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::ifstream file(percorso_file_ostacoli, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Errore nell'apertura di: " << percorso_file_ostacoli << endl;
        exit(EXIT_FAILURE);
    } 

    crea_set_ostacoli(file);
        
    file.close();

    //Rendo minimo e massimo della mappa pari ai valori minimo e massimo delle posizioni degli ostacoli, con un incremento
    minimo_mappa_ = {min_element(ostacoli.begin(), ostacoli.end(), 
                                [](auto &lhs, auto &rhs) { return lhs.first < rhs.first; })->first - (incremento_mappa_ * dimensione_celle_metri_),
                    min_element(ostacoli.begin(), ostacoli.end(), 
                                [](auto &lhs, auto &rhs) { return lhs.second < rhs.second; })->second - (incremento_mappa_ * dimensione_celle_metri_)};
       
    massimo_mappa_ = {max_element(ostacoli.begin(), ostacoli.end(), 
                                [](auto &lhs, auto &rhs) { return lhs.first < rhs.first; })->first + (incremento_mappa_ * dimensione_celle_metri_),
                    max_element(ostacoli.begin(), ostacoli.end(), 
                                [](auto &lhs, auto &rhs) { return lhs.second < rhs.second; })->second + (incremento_mappa_ * dimensione_celle_metri_)};
    
    //prevengo creazione Mappa senza ostacoli e senza incremento
    if (ostacoli.empty()) {
        minimo_mappa_ = {0.0, 0.0};
        massimo_mappa_ = {fattore_minimo_scala_celle_ * dimensione_celle_metri_, fattore_minimo_scala_celle_ * dimensione_celle_metri_};
    }

    //verifico che il rapporto tra celle e dimensione della Mappa non sia minore di un valore preimpostato
    if (dimensione_celle_metri_ > ((massimo_mappa_.first - minimo_mappa_.first)/fattore_minimo_scala_celle_) ||
        dimensione_celle_metri_ > ((massimo_mappa_.second - minimo_mappa_.second)/fattore_minimo_scala_celle_)) {
            std::cerr << "E' stata inserita una dimensione delle celle troppo elevata" << endl;
            exit(EXIT_FAILURE);
    }
    //creo la mappa
    inserisci_celle();       
}

void Mappa::crea_set_ostacoli(std::ifstream &file){
    string line;
   
    while (getline(file, line)) {
        if (!line.empty()) {
            try {
                auto virgola{line.find(',')};
                double x1{stof(line.substr(0, virgola))};

                auto virgola_successiva{line.find(',', virgola + 1)};
                double y1{stof(line.substr(virgola + 1, virgola_successiva))};

                virgola = line.find(',', virgola_successiva + 1);
                double x2{stof(line.substr(virgola_successiva + 1, virgola))};

                virgola_successiva = line.find(',', virgola + 1);
                double y2{stof(line.substr(virgola + 1, virgola_successiva))};

                //verifica dimensione valori
                posizione x{std::minmax(x1,x2)};
                posizione y{std::minmax(y1,y2)};

                //calcolo i punti estrmi dell'ostacolo
                posizione max{x.second, y.second};
                posizione min{x.first, y.first};

                //discretizzo gli estremi dell'ostacolo in funzione della griglia
                centra_posizione(max.first, Mappa::tipo_posizione::massimo);
                centra_posizione(max.second, Mappa::tipo_posizione::massimo);
                centra_posizione(min.first, Mappa::tipo_posizione::minimo);
                centra_posizione(min.second, Mappa::tipo_posizione::minimo);

                //inserisco tutte le celle che rappresentano l'ostacolo nel set dedicato agli ostacoli
                for (posizione pos{min}; pos.first <= max.first; pos.first += dimensione_celle_metri_) {
                    while (pos.second <= max.second) {
                        ostacoli.insert(pos);
                        pos.second += dimensione_celle_metri_;
                    }
                    pos.second = min.second;
                }
            } catch (std::invalid_argument &e) {
                std::cerr << "Inserito un valore nullo o non numerico" << endl;
                exit(EXIT_FAILURE);
            }
        }
    }

}

void Mappa::centra_posizione (double &valore, const tipo_posizione tipo) const {
    double limite{valore};
    limite /= dimensione_celle_metri_;
        if (tipo == tipo_posizione::centro) {
            if (valore < 0.0)
                limite = floor(limite);
            else
                limite = ceil(limite);

            limite *= dimensione_celle_metri_;
            if (valore < 0.0)
                limite += dimensione_mezza_cella_;
            else
                limite -= dimensione_mezza_cella_;
        } else if (tipo == tipo_posizione::minimo) {
            if (valore < 0.0)
                limite = ceil(limite);
            else
                limite = floor(limite);
            limite *= dimensione_celle_metri_;
            limite += dimensione_mezza_cella_;
        } else {
            if (valore < 0.0)
               limite = floor(limite);
            else
                limite = ceil(limite);
            limite *= dimensione_celle_metri_;
            limite -= dimensione_mezza_cella_;
        }
    valore = limite;
}

void Mappa::inserisci_celle() {
    for (auto x{minimo_mappa_.first}; x <= massimo_mappa_.first; x += dimensione_celle_metri_) {
        for(auto y{minimo_mappa_.second}; y <= massimo_mappa_.second; y += dimensione_celle_metri_) {
            if (ostacoli.contains(posizione{x,y}) == true)
                    spazio_movimento_.insert({posizione{x,y}, false});
                else 
                    spazio_movimento_.insert({posizione{x,y}, true});
        }
    }
}


void Mappa::stampa_mappa(string nome_file) const {
    std::ofstream file(nome_file, std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Impossibile stampare mappa, il file non è presente!" << std::endl;
        return;
        }
    //ciclo stampa valori
    // 0 -> no ostacoli
    // 1 -> ostacoli
    // 2 -> robot
    for (auto y{massimo_mappa_.second}; y >= minimo_mappa_.second; y -= dimensione_celle_metri_) {
        for(auto x{minimo_mappa_.first}; x <= massimo_mappa_.first; x += dimensione_celle_metri_) {
            if (contiene_robot(posizione{x, y}))
                file << "2";
            else
                file << !spazio_movimento_.at(posizione{x, y});
        }
        file << endl;
    }
    file.close();        
}

void Mappa::aggiorna_mappa (const Mappa &riferimento) {
    std::for_each(riferimento.cbegin(), riferimento.cend(), [this](auto &elemento) {spazio_movimento_[elemento.first] = elemento.second;});
    minimo_mappa_ = riferimento.posizione_minima();
    massimo_mappa_ = riferimento.posizione_massima();
    robot.clear();
    std::for_each(riferimento.robot_cbegin(), riferimento.robot_cend(), [this](auto &el) {posiziona_robot_cella(el);});
}

void Mappa::aggiorna_posizione_robot_mappa (const Mappa & riferimento) {
    robot.clear();
    std::for_each(riferimento.robot_cbegin(), riferimento.robot_cend(), [this](auto &el) {posiziona_robot_cella(el);});
}
