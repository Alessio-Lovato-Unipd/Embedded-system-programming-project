#include "mappa.h"
#include "gnuplot.h"
#include <iostream>

using std::endl;
using std::cerr;
using std::string;
using std::cout;

void stampa_gnuplot(mappa &map) {
    //stampa dei fati necessari per il grafico
    std::ofstream file("gnuplot_raw.dat", std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Impossibile stampare mappa, il file non è presente!" << std::endl;
        return;
    }
    //ciclo stampa valori
    // 0 -> no ostacoli
    // 1 -> ostacoli
    posizione stampa{map.posizione_minima().first , map.posizione_minima().second};
    while (stampa.second <= map.posizione_massima().second) {
        while(stampa.first <= map.posizione_massima().first) {
          file << stampa.first << " " << stampa.second << " ";
             if (map.ostacolo_in_posizione(stampa) == true)
                file << "1";
            else
                file << "0";
            stampa.first++;
            file << endl;
        }
        file << endl;
        stampa.second++;
        stampa.first = map.posizione_minima().first;
    }
    file.close();

    //stampa grafico
    stampa_grafico(map.posizione_minima().first,map.posizione_minima().second,
                  map.posizione_massima().first, map.posizione_massima().second);
  
}


int main(int argc, char *argv[]) {
  if (argc == 1 || argc >3) {
    cerr << "Deve essere inserito il percorso del file da cui ricavare gli ostacoli e nient'altro." << endl;
    exit(EXIT_FAILURE);
  }

  std::ifstream file_conteggio(argv[1], std::ios::in);
  if (!file_conteggio.is_open()) {
    std::cerr << "Errore nell'apertura di: " << argv[1] << endl;
    exit(EXIT_FAILURE);
  } else {
 
    //numero di ostacoli presenti nel file
    int righe{conta_ostacoli_da_file(file_conteggio)};
    file_conteggio.close();

    //vettore contenente le posizioni degli ostacoli
    vector<ostacolo> ostacoli;
    ostacoli.reserve(righe);
    crea_vettore_ostacoli(argv[1], ostacoli);

    //mappa
    mappa griglia{ostacoli};
    griglia.stampa_ostacoli();

    //stampo il vettore di ostacoli
    stampa_vettore_ostacoli(ostacoli);

    //stampa dimensioni griglia
    cout << "Minimo grigia--> x: " << griglia.posizione_minima().first << "  y: " << griglia.posizione_minima().second << endl
    << "Massimo grigia--> x: " << griglia.posizione_massima().first << "  y: " << griglia.posizione_massima().second << endl;

    //stampa griglia
    griglia.stampa_mappa(argv[2]);

    //inizio stampa gnuplot
    stampa_gnuplot(griglia);

  }
    return 0;
}
