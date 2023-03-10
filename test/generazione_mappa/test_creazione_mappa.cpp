#include "Mappa.h"
#include "gnuplot.h"
#include <iostream>

using std::endl;
using std::cerr;
using std::string;
using std::cout;

void stampa_gnuplot(Mappa &map) {
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
          	if (map.contiene_robot(posizione{x,y}))
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
                  map.dimensione_celle_metri(), gnuplot::tipo_stampa::persist);
  
}


int main(int argc, char *argv[]) {
	if (argc <4 || argc >4) {
		cerr << "Deve essere inserito il percorso del file da cui ricavare gli ostacoli e nient'altro." << endl;
		exit(EXIT_FAILURE);
	}

	//controllo se la dimensione inserita è un numero
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
    Mappa griglia{argv[1], std::stof(argv[3])};

    //stampa dimensioni griglia   griglia.stampa_ostacoli();
    cout << "Minimo griglia--> x: " << griglia.posizione_minima().first << "  y: " << griglia.posizione_minima().second << endl
    << "Massimo griglia--> x: " << griglia.posizione_massima().first << "  y: " << griglia.posizione_massima().second << endl;

    //stampa griglia
    griglia.stampa_mappa(argv[2]);

    //inizio stampa gnuplot
    stampa_gnuplot(griglia);
    return 0;
}
