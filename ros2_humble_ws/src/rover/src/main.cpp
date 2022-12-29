#include "main/main.h"

#include "rclcpp/rclcpp.hpp"

// variabili globali ROS
rclcpp::Node::SharedPtr g_node;
rclcpp::Publisher<rover_visualizer::msg::RoverPosition>::SharedPtr g_publisher;

int main(int argc, char *argv[])
{
    if (argc < 7) {
        std::cerr << "Gli argomenti passati al programma devono essere:" << endl
                  << "- dimensione celle (metri)" << endl
				  << "- dimensione robot (metri)" << endl
                  << "- file posizioni robot" << endl
                  << "- file posizioni ostacoli" << endl
                  << "- file obbiettivi satellite 1" << endl
                  << "- file obbiettivi satellite 2" << endl
				  << "- [facoltativo] file output mappa" << endl
				  << "- [facoltativo] fattore scala celle minimo (numero minimo di celle nella mappa); numero intero - default: 2" << endl
				  << "- [facoltativo] fattore scala campo attrattivo; numero decimale  - default: 1.5" << endl
				  << "- [facoltativo] fattore scala campo repulsivo; numero decimale - default: 100" << endl
				  << "- [facoltativo] distanza minima ostacolo percepito; numero decimale - default: 5" << endl
				  << "- [facoltativo] numero di celle da incrementare a bordo mappa; numero intero - default: 3" << endl;
        exit(EXIT_FAILURE);
    }
	//controllo che la dimensione della cella sia un numero
	try {
		std::stod(argv[dim_celle]);
	} catch (std::invalid_argument &err) {
		std::cerr << "La dimensione della cella non era un numero" << endl;
		exit (EXIT_FAILURE);
	}
	try {
		std::stod(argv[dim_robot]);
	} catch (std::invalid_argument &err) {
		std::cerr << "La dimensione del robot non era un numero" << endl;
		exit (EXIT_FAILURE);
	}

	rclcpp::init(argc, argv);

  	g_node = rclcpp::Node::make_shared("rover_publisher");
 	g_publisher = g_node->create_publisher<rover_visualizer::msg::RoverPosition>(
    	"rover", 10);

    //ottengo le posizioni dei robot e il numero di robot
    std::vector<posizione> posizioni_robot{ottieni_posizioni_robot(argv[pos_robot])};
    const size_t numero_robot{posizioni_robot.size()};
    const size_t numero_satelliti{2};
    Gestore_robot server{static_cast<int>(numero_robot), static_cast<int>(numero_satelliti)};

	//genero mappa in base agli argomenti passati al main
	Mappa mappa{genera_mappa(argc, argv)};
	//creo thread robot
	std::vector<std::thread> thread_robot;
	thread_robot.reserve(numero_robot);
    for (int id{0}; id < static_cast<int>(numero_robot); id++)
	    thread_robot.push_back(std::thread {Rover(server, id, dati_robot{posizioni_robot.at(id), std::stof(argv[dim_robot])}, mappa)});
    //creo le thread dei satelliti
    std::thread sat_1{satellite, std::ref(server), 1, argv[obbiettivi_1]};
	std::thread sat_2{satellite, std::ref(server), 2, argv[obbiettivi_2]};

	rclcpp::spin(g_node);
    //attendo che tutti le thread finiscano
	sat_1.join();
	sat_2.join();
	for (auto thrd{thread_robot.begin()}; thrd != thread_robot.end(); ++thrd)
		thrd->join();

  	rclcpp::shutdown();
	return 0;
}

void satellite(Gestore_robot &server, size_t id, const string &file_obbiettivi) {
	std::ifstream infile{file_obbiettivi};
	std::queue<posizione> nuovi_obbiettivi;
    while (!infile.eof()) {
        double x, y;
        infile >> x >> y;
        if (infile.fail() || infile.bad()) {
			std::osyncstream satellite_a_cerr(std::cerr);
            satellite_a_cerr << "Errore nell'input file per il satellite" << std::to_string(id) << endl;
			infile.close();
            break;
        }
		nuovi_obbiettivi.push(posizione{x,y});
		std::osyncstream satellite_a_cout(std::cout);
		satellite_a_cout << "satellite " << std::to_string(id) << " ha letto posizione {" << std::to_string(nuovi_obbiettivi.back().first)
		<< "; " << std::to_string(nuovi_obbiettivi.back().second) << "}\n";
    }
	while (!nuovi_obbiettivi.empty()) {
		server.aggiungi_obbiettivo(nuovi_obbiettivi.front());
		std::osyncstream satellite_a_cout(std::cout);
		satellite_a_cout << "Satellite " << std::to_string(id) << " ha aggiunto obbiettivo: {"
		<< std::to_string(nuovi_obbiettivi.front().first) << ", " << std::to_string(nuovi_obbiettivi.front().second) << "}" << endl;
		nuovi_obbiettivi.pop();
	}
	server.fine_obbiettivi_satellite();
	std::cout << "Finita scrittura satellite " << std::to_string(id) << endl;
}

std::vector<posizione> ottieni_posizioni_robot(const std::string &file_posizioni_robot) {
    std::ifstream file{file_posizioni_robot};
	std::vector<posizione> posizioni;
    while (!file.eof()) {
        double x, y;
		float dim;
        file >> x >> y >> dim;
        if (file.fail() || file.bad()) {
			std::osyncstream err(std::cerr);
            err << "Errore nell'input file delle posizioni dei robot" << endl;
            break;
        }
	    posizioni.push_back(posizione{x,y});
    }
    file.close();
    if (posizioni.empty()) {
        std::cerr << "Non sono state inserite posizioni per i robot" << endl;
        exit(EXIT_FAILURE);
    }
    return posizioni;
}

Mappa genera_mappa(int argc, char *argv[]) {
	if (argc == 9) {
		try {
			if (std::stoi(argv[minimo_celle]) <= 0) {
					std::cerr << "Non è possibile inserire valori negativi o nulli" << endl;
					exit(EXIT_FAILURE);
			}
			Mappa mappa{argv[ostacoli],std::stod(argv[dim_celle]), std::stoi(argv[minimo_celle])};
			return mappa;
		} catch (std::invalid_argument &err) {
			std::cerr << "Il fattore scala celle minimo non era un numero" << endl;
			exit (EXIT_FAILURE);
		}	
	} else if (argc == 10) {
		try {
			if ((std::stoi(argv[minimo_celle]) <= 0) || (std::stod(argv[attrattivo]) <= 0)) {
					std::cerr << "Non è possibile inserire valori negativi o nulli" << endl;
					exit(EXIT_FAILURE);
			}
			Mappa mappa{argv[ostacoli],std::stod(argv[dim_celle]), std::stoi(argv[minimo_celle]), std::stod(argv[attrattivo])};
			return mappa;
		} catch (std::invalid_argument &err) {
			std::cerr << "Il fattore di scala del campo attrattivo non era un numero" << endl;
			exit (EXIT_FAILURE);
		}	
	} else if (argc == 11) {
		try {
			if ((std::stoi(argv[minimo_celle]) <= 0) || (std::stod(argv[attrattivo]) <= 0) || (std::stod(argv[repulsivo]) <= 0)) {
					std::cerr << "Non è possibile inserire valori negativi o nulli" << endl;
					exit(EXIT_FAILURE);
			}
			Mappa mappa{argv[ostacoli],std::stod(argv[dim_celle]), std::stoi(argv[minimo_celle]), std::stod(argv[attrattivo]),
					std::stod(argv[repulsivo])};
			return mappa;
		} catch (std::invalid_argument &err) {
			std::cerr << "Il fattore di scala del campo repulsivo non era un numero" << endl;
			exit (EXIT_FAILURE);
		}
	} else if (argc == 12) {
		try {
			if ((std::stoi(argv[minimo_celle]) <= 0) || (std::stod(argv[attrattivo]) <= 0) || (std::stod(argv[repulsivo]) <= 0) ||
				(std::stod(argv[dist_ostacolo]) <= 0)) {
					std::cerr << "Non è possibile inserire valori negativi o nulli" << endl;
					exit(EXIT_FAILURE);
			}
			Mappa mappa{argv[ostacoli],std::stod(argv[dim_celle]), std::stoi(argv[minimo_celle]), std::stod(argv[attrattivo]),
					std::stod(argv[repulsivo]), std::stod(argv[dist_ostacolo])};
			return mappa;
		} catch (std::invalid_argument &err) {
			std::cerr << "La distanza minima a cui viene rilevato un ostacolo non era un numero" << endl;
			exit (EXIT_FAILURE);
		}
	} else if (argc == 13) {
		try {
			if ((std::stoi(argv[minimo_celle]) <= 0) || (std::stod(argv[attrattivo]) <= 0) || (std::stod(argv[repulsivo]) <= 0) ||
				(std::stod(argv[dist_ostacolo]) <= 0) || (std::stod(argv[incremento]) <= 0)) {
					std::cerr << "Non è possibile inserire valori negativi o nulli" << endl;
					exit(EXIT_FAILURE);
			}
			Mappa mappa{argv[ostacoli],std::stod(argv[dim_celle]), std::stoi(argv[minimo_celle]), std::stod(argv[attrattivo]),
					std::stod(argv[repulsivo]), std::stod(argv[dist_ostacolo]), std::stoi(argv[incremento])};
			return mappa;
		} catch (std::invalid_argument &err) {
			std::cerr << "Il numero di celle da incrementare a bordo mappa non era un numero" << endl;
			exit (EXIT_FAILURE);
		}
	} else {
		Mappa mappa{argv[ostacoli],std::stod(argv[dim_celle])};
		return mappa;
	}
}
