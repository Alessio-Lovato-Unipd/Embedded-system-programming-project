#include "Gestore_robot.h"
#include "gnuplot.h"
#include <thread>
#include <chrono>

Gestore_robot monitor{2,2};

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
	std::thread sat_2{satellite, 2, file_2};
	std::thread sat_1{satellite, 1, file_1};
	sat_1.join();
	sat_2.join();
	return 0;
}
