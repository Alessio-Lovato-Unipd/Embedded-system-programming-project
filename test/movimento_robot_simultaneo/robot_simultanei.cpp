#include "Gestore_robot.h"
#include "gnuplot.h"
#include <thread>
#include <chrono>

Gestore_robot monitor{2};

void stampa_gnuplot(const Mappa &map) {
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


void robot(int id, const posizione &posizione_robot, Mappa &mappa_riferimento, const float raggio_robot = 0.5){
	Robot robot{monitor.crea_robot(posizione_robot, mappa_riferimento, raggio_robot)};
	bool ho_ancora_obbiettivi{true};
	while (ho_ancora_obbiettivi) {
		posizione nuovo_obbiettivo{monitor.ottieni_prossimo_obbiettivo(robot.posizione_centrale())};
		monitor.assegna_obbiettivo(robot, nuovo_obbiettivo);
		std::osyncstream robot_a_cout(std::cout);
		cout << "Robot " << std::to_string(id) << " nuovo obbiettivo: {" << robot.obbiettivo().first <<
		"; " << robot.obbiettivo(). second << "}" << endl;
		while (!robot.obbiettivo_raggiunto()) {
			mappa_potenziali posizioni_possibili{robot.calcola_potenziali_celle_adiacenti()};
			monitor.sposta_robot(robot, posizioni_possibili);
			robot_a_cout << "Robot " << std::to_string(id) << ": {" << robot.posizione_centrale().first << ", "
			<< robot.posizione_centrale().second << "}" << endl;
			if (id==1) {
				auto blocco{monitor.blocca_mappa()};
        		stampa_gnuplot(mappa_riferimento);
				monitor.sblocca_mappa(blocco);
			}
		}
		if (!monitor.obbiettivi_presenti())
			ho_ancora_obbiettivi = false;
	}
	std::osyncstream robot_a_cout(std::cout);
	robot_a_cout << "Robot " << std::to_string(id) << " ha finito i task" << endl;
}

void satellite(int id, const string &file_obbiettivi) {
	std::ifstream infile{file_obbiettivi};
	std::queue<posizione> nuovi_obbiettivi;
    while (!infile.eof()) {
        double x, y;
        infile >> x >> y;
        if (infile.fail() || infile.bad()) {
            std::cerr << "Error in input for satellite" << std::to_string(id) << endl;
			infile.close();
            break;
        }
		nuovi_obbiettivi.push(posizione{x,y});
		std::osyncstream satellite_a_cout(std::cout);
		satellite_a_cout << "satellite " << std::to_string(id) << " ha letto posizione {" << std::to_string(nuovi_obbiettivi.back().first)
		<< "; " << std::to_string(nuovi_obbiettivi.back().second) << "}\n";
    }
	while (!nuovi_obbiettivi.empty()) {
		monitor.aggiungi_obbiettivo(nuovi_obbiettivi.front());
		std::osyncstream satellite_a_cout(std::cout);
		satellite_a_cout << "Satellite " << std::to_string(id) << " ha aggiunto obbiettivo: {"
		<< std::to_string(nuovi_obbiettivi.front().first) << ", " << std::to_string(nuovi_obbiettivi.front().second) << "}" << endl;
		nuovi_obbiettivi.pop();
	}
	std::cout << "Finita scrittura satellite " << std::to_string(id) << endl;
}


int main()
{
	const std::string file_1{"obbiettivi_1.txt"};
	const std::string file_2{"obbiettivi_2.txt"};
	const std::string ostacoli{"ostacoli.csv"};

	Mappa mappa{ostacoli};
	const posizione posizione_iniziale_1{1,1};
	const posizione posizione_iniziale_2{12,7};
	std::thread robot1{robot, 1, std::cref(posizione_iniziale_1), std::ref(mappa), 0.5};
	std::thread robot2{robot, 2, std::cref(posizione_iniziale_2), std::ref(mappa), 0.5};
	//std::thread output_visivo(&Gestore_robot::stampa_mappa, &monitor, std::ref(mappa));
	std::thread sat_2{satellite, 2, file_2};
	std::thread sat_1{satellite, 1, file_1};
	//monitor.stampa_buffer();
	sat_1.join();
	sat_2.join();
	robot1.join();
	robot2.join();
	//output_visivo.join();
	return 0;
}
