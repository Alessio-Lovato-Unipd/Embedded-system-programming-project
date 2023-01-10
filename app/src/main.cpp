#include "main.h"

int main(int argc, char *argv[])
{
    if (argc < 6) {
        std::cerr << "Gli argomenti passati al programma devono essere:" << endl
                  << "- dimensione celle (metri)" << endl
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

    //ottengo le posizioni dei robot e il numero di robot
    std::vector<dati_robot> posizioni_robot{ottieni_posizioni_robot(argv[pos_robot])};
    const size_t numero_robot{posizioni_robot.size()};
    const size_t numero_satelliti{2};
    Gestore_robot server{static_cast<int>(numero_robot), static_cast<int>(numero_satelliti)};

	//genero mappa in base agli argomenti passati al main
	Mappa mappa{genera_mappa(argc, argv)};
	//creo thread robot
	std::vector<std::thread> thread_robot;
	thread_robot.reserve(numero_robot);
    for (size_t id{0}; id < numero_robot; id++)
	    thread_robot.push_back(std::thread {robot, std::ref(server), id, posizioni_robot.at(id), std::ref(mappa)});
    //creo thread per l'output
	bool termina_plot{false};
	string file_stampa;
	if (argc > 6)
		file_stampa = argv[percorso_mappa];
    std::thread output_visivo(visualizza_mappa, std::ref(server), std::cref(mappa), std::ref(termina_plot), std::cref(file_stampa));
    //creo le thread dei satelliti
    std::thread sat_1{satellite, std::ref(server), 1, argv[obbiettivi_1]};
	std::thread sat_2{satellite, std::ref(server), 2, argv[obbiettivi_2]};
    //attendo che tutti le thread finiscano
	sat_1.join();
	sat_2.join();
	for (auto thrd{thread_robot.begin()}; thrd != thread_robot.end(); ++thrd)
		thrd->join();
	//termino la thread di output
	termina_plot = true;
	output_visivo.join();
	return 0;
}


void robot(Gestore_robot &server, size_t id, dati_robot posizione_robot, Mappa &mappa_riferimento){
	Robot robot{server.crea_robot(posizione_robot.posizione_robot(), mappa_riferimento, posizione_robot.dimensione())};
	bool ho_ancora_obbiettivi{true};
	while (ho_ancora_obbiettivi || server.satelliti_con_dati_presenti()) {
		posizione nuovo_obbiettivo{server.ottieni_prossimo_obbiettivo(robot.posizione_centrale())};
		std::osyncstream robot_a_cout(std::cout);
		//controllo se ho un nuovo obbiettivo ed eventualmente lo assegno
		if (nuovo_obbiettivo.first == robot_.posizione_centrale().first && nuovo_obbiettivo.second == robot_.posizione_centrale().second) {
			server_.assegna_obbiettivo(robot_, nuovo_obbiettivo);
		} else if (server_.assegna_obbiettivo(robot_, nuovo_obbiettivo)) {
				robot_a_cout << "Robot " << std::to_string(id_) << " nuovo obbiettivo: {" << robot_.obbiettivo().first <<
				"; " << robot_.obbiettivo(). second << "}" << endl;
		} else {
			robot_a_cout << "Obbiettivo {" << nuovo_obbiettivo.first <<
			"; " << nuovo_obbiettivo.second << "} non valido, posizionato sopra un ostacolo" << endl;
		}
		//sposto il robot
		while (!robot.obbiettivo_raggiunto()) {
			server.sposta_robot(robot);
			robot_a_cout << "Robot " << std::to_string(id) << " : {" << robot.posizione_centrale().first << ", "
			<< robot.posizione_centrale().second << "}" << endl;
			std::this_thread::sleep_for(std::chrono::microseconds(2));
		}
		if (!server.obbiettivi_presenti())
			ho_ancora_obbiettivi = false;
	}
	std::osyncstream robot_a_cout(std::cout);
	robot_a_cout << "Robot " << std::to_string(id) << " ha finito i task" << endl;
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
	//Nel caso non ci fossero obbiettivi nei file, avvio i robot in modo che concludano thread
	server.attiva_robot();
	std::cout << "Finita scrittura satellite " << std::to_string(id) << endl;
}

void visualizza_mappa(Gestore_robot &server, const Mappa &mappa, bool &termina_plot, const string &file_stampa){
	while(!termina_plot) {
		auto blocco{server.blocca_mappa()};
		stampa_gnuplot(mappa);
		if (!file_stampa.empty())
			mappa.stampa_mappa(file_stampa);
		server.sblocca_mappa(blocco);
		std::this_thread::sleep_for(std::chrono::microseconds(1));
	}
	
}

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

std::vector<dati_robot> ottieni_posizioni_robot(const std::string &file_posizioni_robot) {
    std::ifstream file{file_posizioni_robot};
	std::vector<dati_robot> posizioni;
    while (!file.eof()) {
        double x, y;
		float dim;
        file >> x >> y >> dim;
        if (file.fail() || file.bad()) {
			std::osyncstream err(std::cerr);
            err << "Errore nell'input file delle posizioni dei robot" << endl;
            break;
        }
	    posizioni.push_back(dati_robot{posizione{x,y}, dim});
    }
    file.close();
    if (posizioni.empty()) {
        std::cerr << "Non sono state inserite posizioni per i robot" << endl;
        exit(EXIT_FAILURE);
    }
    return posizioni;
}

Mappa genera_mappa(int argc, char *argv[]) {
	if (argc == 8) {
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
	} else if (argc == 9) {
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
	} else if (argc == 10) {
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
	} else if (argc == 11) {
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
	} else if (argc == 12) {
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
