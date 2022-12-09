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

typedef pair<double, double> posizione; // (first == x, second == y)

class Mappa {
public:
    /*Costruttore classe Mappa
    Parametri:
        - percorso_file_ostacoli: percorso del file csv dal quale ottenere gli ostacoli
        - [facoltativo] dimensione_celle_in_metri: dimensione della cella in metri
        - [facoltativo] fattore_scala_celle_minimo: numero minimo di celle presenti sulla mappa
        - [facoltativo] fattore_scala_campo_attrattivo: fattore di moltiplicazione del campo attrattivo
        - [facoltativo] fattore_scala_campo_repulsivo: fattore di moltiplicazione del campo repulsivo
        - [facoltativo] distanza_minima_ostaoclo_percepito: distanza minima alla wuale un ostacolo influenza il campo
        - [facoltativo] incremento_celle_mappa: numero di celle da inserire nei bordi della mappa oltre gli ostacoli
    */
    Mappa(string percorso_file_ostacoli, double dimensione_celle_in_metri = 1.0, int fattore_scala_celle_minimo = 2, double fattore_scala_campo_attrattivo = 1.5,
            double fattore_scala_campo_repulsivo = 100.0, double distanza_minima_ostacolo_percepito = 5.0, int incremento_celle_mappa = 3);
    
    /*Enumeratore per la funzione "centra_posizione()"
    - Minimo e massimo vengono utilizzati con gli ostacoli per indicare quale punto stiamo modificando
    - Centro serve per le posizioni singole"
    */
    enum class tipo_posizione {minimo, massimo,centro};

    /* Funzioni di accesso ai dati */
    //Ritorna posizione minima celle (x,y)
    const posizione posizione_minima() const {return minimo_mappa_;};
    // Ritorna posizione massima celle (x,y)
    const posizione posizione_massima() const {return massimo_mappa_;};
    // Ritorna dimensione cella
    double dimensione_celle_metri() const {return dimensione_celle_metri_;};
    // Ritorna metà dimenzione cella
    double dimensione_mezza_cella() const {return dimensione_mezza_cella_;};
    // Ritorna iteratore all'inizio delle celle della mappa
    map<posizione, bool>::const_iterator cbegin() const {return spazio_movimento_.cbegin();};
    // Ritorna iteratore alla fine delle celle della mappa
    map<posizione, bool>::const_iterator cend() const {return spazio_movimento_.cend();};
    // Ritorna iteratore all'inizio dell'elenco degli ostacoli nella mappa
    set<posizione>::const_iterator ostacoli_cbegin() const {return ostacoli.cbegin();};
    // Ritorna iteratore alla fine dell'elenco degli ostacoli nella mappa
    set<posizione>::const_iterator ostacoli_cend() const {return ostacoli.cend();};
    // Ritorna iteratore all'inizio dell'elenco delle posizioni occupate da robot nella mappa
    set<posizione>::const_iterator robot_cbegin() const {return robot.cbegin();};
    // Ritorna iteratore alla fine dell'elenco delle posizioni occupate da robot nella mappa
    set<posizione>::const_iterator robot_cend() const {return robot.cend();};
    // Ritorna true se la posizione passata per parametro non è occupata da un ostacolo, altrimenti false
    bool cella_libera(const posizione &pos) const {return spazio_movimento_.at(pos);};
    // Ritorna true se la mappa contiene la posizione passata per parametro, altrimenti false
    bool contiene_cella(const posizione &pos) const {return spazio_movimento_.contains(pos);};
    // Ritorna true se la posizione passata per parametro contiene un robot, altrimenti false
    bool contiene_robot(const posizione &pos) const {return robot.contains(pos);};
    // Ritorna il valore del fattore di scala del campo attrattivo della mappa
    double fattore_scala_campo_attrattivo() const {return fattore_scala_campo_attrattivo_;};
    // Ritorna il valore del fattore di scala del campo repulsivo della mappa
    double fattore_scala_campo_repulsivo() const {return fattore_scala_campo_repulsivo_;};
    // Ritorna il valore della distanza alla quale un ostacolo è percepito nella mappa
    double distanza_minima_ostacolo() const {return distanza_minima_ostacolo_percepito_;};
    /* Fine funzioni accesso dati */

    /*Funzione per stampare la mappa in un file di testo
      Parametri:
        - nome_file: nome del file che si vuole in uscita
      Output:
        file con estensione determinata dal parametro "nome_file" nel quale è stampata
        una mappa dove le posizioni delle celle sono rappresentate da:
            0 se nella posizione non ho ostacoli
            1 se nella posizione ho ostacoli
            2 se nella posizione ho un robot
    */
    void stampa_mappa(string nome_file) const;

    /*Funzione per impostare il fattore di scala del campo repulsivo
      Parametri:
        - valore: nuovo valore del fattore di scala del campo repulsivo*/
    void imposta_fattore_scala_repulsivo(double valore) {fattore_scala_campo_repulsivo_ = valore;};

    /*Funzione per normalizzare (centrare nella cella) un valore all'interno della mappa
      Parametri: 
        -valore: valore da normalizzare
        -tipo: enumeratore con diversi significati:
                "minimo" se il valore da modificare rappresenta il punto minimo di un ostacolo
                "massimo" se il valore da modificare rappresenta il punto massimo di un ostacolo
                "centro" se bisogna semplicemente centrare un valore all'interno della cella
    */
    void centra_posizione (double &valore, const tipo_posizione tipo) const;

    /*Funzione per aggiornare le celle della mappa e i robot che contiene
      Parametri:
        -riferimento: mappa dalla quale copiare i valori
    */
    void aggiorna_mappa (const Mappa &riferimento);

    /*Funzione per inserire le celle e gli ostacoli in una mappa. Non sovrascrive i dati esistenti
      Parametri:
        - minimo: posizione della cella estrema inferiore della mappa dalla quale partire a inserire i dati
        - massimmo: posizione della cella estrema superiore alla quale fermare l'inserimento dei dati
    */
    void inserisci_celle(const posizione &minimo, const posizione &massimo);

    /*Funzione per aggiornare i valori di minimo e massimo della mappa
      Parametri:
        - minima: nuova posizione minima delle celle della mappa
        - massima: nuova posizione massima delle celle della mappa
    */
    void aggiorna_dimensioni (const posizione &minima, const posizione &massima) {minimo_mappa_ = minima; massimo_mappa_ = massima;};

    /*Funzione per inserire una cella nell'elenco delle posizione occupate da robot
      Parametri: 
        - pos: posizione occupata dal robot nella mappa
    */
    void posiziona_robot_cella(const posizione &pos) {robot.insert(pos);};

    /*Funzione per eliminare una cella dall'elenco delle posizione occupate da robot
      Parametri: 
        - pos: posizione da liberare della mappa
    */
    void libera_cella_robot(const posizione &pos) {robot.erase(pos);};

    /*Funzione per aggioranre l'elenco delle posizioni occupate dai robot nella mappa
    Parametri:
        - riferimento: mappa dalla quale copiare l'elenco delle posizion occupate
    */
    void aggiorna_posizione_robot_mappa (const Mappa &riferimento);

private:
    set<posizione> ostacoli; //Elenco di posizioni occupate da ostacoli
    set<posizione> robot; //Elenco di posizioni occupate da robot
    posizione minimo_mappa_; //Posizione della cella estram minima nella mappa
    posizione massimo_mappa_; //Posizione della cella estram massima nella mappa
    double dimensione_celle_metri_; //Dimensione delle celle
    //Mappa delle posizioni possibili, se bool = false la posizione contiene un ostacolo
    map<posizione, bool> spazio_movimento_;
    const int fattore_minimo_scala_celle_; //Numero minimo celle nella mappa possibili
    const double dimensione_mezza_cella_{dimensione_celle_metri_/2.0}; //Dimensione metà cella
    double fattore_scala_campo_attrattivo_; //Fattore di scala del campo attrattivo
    double fattore_scala_campo_repulsivo_; //Fattore scala del campo repulsivo
    double distanza_minima_ostacolo_percepito_; //Distanza minima alla quale un ostacolo è percepito
    const int incremento_mappa_; //Numero di celle ai bordi esterne agli ostacoli

    /*Funzione per inserire i dati da un file csv all'elenco delle posizioni occupate da ostacoli
      Parametri:
        - file: file csv dal quale ottenere i dati
    N.B. I dati devo essere "x1,y1,x2,y2" e ogni riga rappresenta un ostacolo
    */
    void crea_set_ostacoli(std::ifstream &file);
    
};

#endif