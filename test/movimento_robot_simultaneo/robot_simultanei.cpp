#include "Gestore_robot.h"
#include <thread>

Gestore_robot monitor{2};

void robot(int id, const posizione &posizione_robot, Mappa &mappa_riferimento, const float raggio_robot = 0.5){
	Robot robot{monitor.crea_robot(posizione_robot, mappa_riferimento, raggio_robot)};
	while (monitor.obbiettivi_presenti())
	{
		posizione nuovo_obbiettivo{monitor.ottieni_prossimo_obbiettivo(robot.posizione_centrale())};
		monitor.assegna_obbiettivo(robot, nuovo_obbiettivo);
		while (!robot.obbiettivo_raggiunto()) {
			mappa_potenziali posizioni_possibili{robot.calcola_potenziali_celle_adiacenti()};
			monitor.sposta_robot(robot, posizioni_possibili);
			std::osyncstream robot_a_cout(std::cout);
			robot_a_cout << "Robot " << std::to_string(id) << ": {" << robot.posizione_centrale().first << ", "
			<< robot.posizione_centrale().second << "}" << endl;
		}
	}
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

//int main(int argc, char *argv[])
int main()
{
	const std::string file_1{"obbiettivi_1.txt"};
	const std::string file_2{"obbiettivi_2.txt"};
	std::thread sat_2{satellite, 2, file_2};
	std::thread sat_1{satellite, 1, file_1};
	sat_1.join();
	sat_2.join();
	monitor.stampa_buffer();
	return 0;
}