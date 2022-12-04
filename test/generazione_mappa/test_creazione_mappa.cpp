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
    for (float x{map.posizione_minima().first + (map.dimensione_celle_metri()/2)}; x < map.posizione_massima().first; x += map.dimensione_celle_metri()) {
        for(float y = map.posizione_minima().second + (map.dimensione_celle_metri()/2); y < map.posizione_massima().second; y += map.dimensione_celle_metri()) {
          file << x << " " << y << " ";
          file << map.ostacolo_in_posizione(posizione{x,y});
          file << endl;
        }
        file << endl;
    }
    file.close();

    //stampa grafico
    stampa_grafico(map.posizione_minima().first,map.posizione_minima().second,
                  map.posizione_massima().first, map.posizione_massima().second, map.dimensione_celle_metri());
  
}


int main(int argc, char *argv[]) {
  if (argc <4 || argc >4) {
    cerr << "Deve essere inserito il percorso del file da cui ricavare gli ostacoli e nient'altro." << endl;
    exit(EXIT_FAILURE);
  }

    //mappa
    mappa griglia{argv[1], std::stof(argv[3])};
    griglia.stampa_ostacoli();

    //stampo il vettore di ostacoli
    //stampa_vettore_ostacoli(ostacoli);

    //stampa dimensioni griglia
    cout << "Minimo griglia--> x: " << griglia.posizione_minima().first << "  y: " << griglia.posizione_minima().second << endl
    << "Massimo griglia--> x: " << griglia.posizione_massima().first << "  y: " << griglia.posizione_massima().second << endl;

    //stampa griglia
    griglia.stampa_mappa(argv[2]);

    //inizio stampa gnuplot
    stampa_gnuplot(griglia);
    return 0;
}
