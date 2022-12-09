#include "mappa.h"
#include "gnuplot.h"
#include "robot.h"
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
    // 2 -> robot
    for (double x{map.posizione_minima().first}; x <= map.posizione_massima().first; x += map.dimensione_celle_metri()) {
        for(double y = map.posizione_minima().second; y <= map.posizione_massima().second; y += map.dimensione_celle_metri()) {
          file << x << " " << y << " ";
          if (map.contains_robot(posizione{x,y}))
            file << "2";
          else
            file << map.cella_libera(posizione{x,y});
          file << endl;
        }
        file << endl;
    }

    file.close();

    //stampa grafico
    stampa_grafico(map.posizione_minima().first - map.dimensione_mezza_cella(),map.posizione_minima().second - map.dimensione_mezza_cella(),
                  map.posizione_massima().first + map.dimensione_mezza_cella(), map.posizione_massima().second + map.dimensione_mezza_cella(),
                  map.dimensione_celle_metri());
  
}


int main(int argc, char *argv[]) {
  if (argc <4 || argc >4) {
    cerr << "Deve essere inserito il percorso del file da cui ricavare gli ostacoli e nient'altro." << endl;
    exit(EXIT_FAILURE);
  }

  //controllo se la dimensione inserita è un double
  string dimensione{argv[3]};
  bool primo_punto{true};
 for (auto elemento : dimensione) {
    if ((isdigit(elemento) == 0)){ // il carattere non è un digit
      if (elemento == '.' or elemento == ',') {
        if (!primo_punto) {
          cerr << "Inserito un valore dimensionale con più punti" << endl;
          exit(EXIT_FAILURE);
        } else {
          primo_punto = false;
        }
      } else if (elemento == '-') {
          cerr << "Inserito un valore dimensionale negativo" << endl;
          exit(EXIT_FAILURE);
      } else {
        cerr << "Inserito un valore dimensionale non numerico" << endl;
        exit(EXIT_FAILURE);
      }
    }
 }


    //mappa
    mappa griglia{argv[1], std::stof(argv[3])};
    //griglia.stampa_ostacoli();

    //stampa dimensioni griglia
    cout << "Minimo griglia--> x: " << griglia.posizione_minima().first << "  y: " << griglia.posizione_minima().second << endl
    << "Massimo griglia--> x: " << griglia.posizione_massima().first << "  y: " << griglia.posizione_massima().second << endl;

    Robot robot1{posizione{0, -12.7}, posizione{20, 20}, griglia, 0.8};
    Robot robot2{posizione{8.3, 6.7}, posizione{23,12.7}, griglia};
    griglia.stampa_mappa(argv[2]);
    stampa_gnuplot(griglia);
    while (!robot1.obbiettivo_raggiunto()) {
      robot1.sposta_su_cella_successiva(griglia);
      stampa_gnuplot(griglia);
    }
    cout << "obbiettivo: " <<robot1.obbiettivo().first << " : " << robot1.obbiettivo().second << endl;


    return 0;
}
