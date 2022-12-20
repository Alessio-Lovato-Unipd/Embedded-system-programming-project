#include "Mappa.h"
#include "gnuplot.h"
#include "robot.h"
#include <iostream>

using std::endl;
using std::cerr;
using std::string;
using std::cout;

void stampa_gnuplot(Mappa &map) {
    //stampa dei fati necessari per il grafico
    std::ofstream file("gnuplot_raw.dat", std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Impossibile stampare Mappa, il file non è presente!" << std::endl;
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
                  map.dimensione_celle_metri(), gnuplot::tipo_stampa::no_persist);
  
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


    //Mappa
    Mappa griglia{argv[1], std::stof(argv[3])};

    //stampa dimensioni griglia
    cout << "Minimo griglia--> x: " << griglia.posizione_minima().first << "  y: " << griglia.posizione_minima().second << endl
    << "Massimo griglia--> x: " << griglia.posizione_massima().first << "  y: " << griglia.posizione_massima().second << endl;

    Robot robot1{posizione{3, -3}, griglia, };
    Robot robot2{posizione{8.3, 6.7}, griglia};
    robot1.nuovo_obbiettivo(posizione{15, 12});
    //l'assgnazione dell'obbiettivo al robot 2 non è necessaria
    robot2.nuovo_obbiettivo(posizione{-10.8, -14.03});
    griglia.stampa_mappa(argv[2]);
    stampa_gnuplot(griglia);
    while (!robot1.obbiettivo_raggiunto()) {
      mappa_potenziali posizioni_possibili{robot1.calcola_potenziali_celle_adiacenti()};
      if (!robot1.sposta_su_cella_successiva(posizioni_possibili))
        exit(EXIT_FAILURE);
      stampa_gnuplot(griglia);
    }
    cout << "obbiettivo: " <<robot1.obbiettivo().first << " : " << robot1.obbiettivo().second << endl;


    return 0;
}
