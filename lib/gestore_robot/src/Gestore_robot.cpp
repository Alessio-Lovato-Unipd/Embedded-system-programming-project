#include "Gestore_robot.h"

Gestore_robot::Gestore_robot(int numero_robot, int numero_campioni_per_robot)
				:numero_massimo_obbiettivi_{static_cast<size_t>(numero_robot * numero_campioni_per_robot)}
{}

bool Gestore_robot::assegna_obbiettivo(Robot &robot, const posizione &nuovo_obbiettivo) {
    std::unique_lock blocco_mappa{modifica_mappa_};
    bool assegnazione_riuscita{robot.nuovo_obbiettivo(nuovo_obbiettivo)};
	blocco_mappa.unlock();
    buffer_non_pieno.notify_one();
    if (!obbiettivi.empty())
        buffer_non_vuoto.notify_one();
    return assegnazione_riuscita;
}

posizione Gestore_robot::ottieni_prossimo_obbiettivo(const posizione &posizione_attuale_robot) {
	std::unique_lock blocco_buffer{accesso_buffer_};
    while (obbiettivi.empty()) {
        std::cout << "Attendo che vengano caricati degli obbiettivi \n";
        buffer_non_vuoto.wait(blocco_buffer);
    }
    //calcolo l'obbiettivo con distanza minima
    auto nuovo_obbiettivo{std::min_element(obbiettivi.cbegin(), obbiettivi.cend(),
                        [posizione_attuale_robot] (auto lhs, auto rhs)
                        {return calcolo_distanza(posizione_attuale_robot, lhs) < calcolo_distanza(posizione_attuale_robot, rhs);})};
	auto nuova_posizione{*nuovo_obbiettivo};
    //elimino l'obbiettivo dal buffer
    obbiettivi.erase(nuovo_obbiettivo);
	blocco_buffer.unlock();
	return nuova_posizione;
}

void Gestore_robot::aggiungi_obbiettivo(const posizione &obbiettivo) {
    std::unique_lock blocca_buffer(accesso_buffer_);
    while (obbiettivi.size() == numero_massimo_obbiettivi_) {
        std::cout << "Attendo che vengano eliminati degli obbiettivi" << endl;
        buffer_non_pieno.wait(blocca_buffer);
    }
    obbiettivi.push_back(obbiettivo);
    blocca_buffer.unlock();
    buffer_non_vuoto.notify_one();
    if (obbiettivi.size() != numero_massimo_obbiettivi_)
        buffer_non_pieno.notify_one();
}

Robot Gestore_robot::crea_robot(const posizione &robot, Mappa &mappa_riferimento, const float raggio_robot) {
	std::unique_lock blocco_mappa{modifica_mappa_};
	Robot robot_creato{robot, mappa_riferimento, raggio_robot};
	blocco_mappa.unlock();
	return robot_creato;
}

bool Gestore_robot::sposta_robot(Robot &robot, mappa_potenziali &potenziali, Mappa &mappa, const int ID){
	mappa_potenziali posizioni_possibili{robot.calcola_potenziali_celle_adiacenti()};
	std::unique_lock blocco_mappa{modifica_mappa_};
	auto riuscita{robot.sposta_su_cella_successiva(potenziali)};
    if (ID==1) {
        stampa_gnuplot(mappa);
    }
	blocco_mappa.unlock();
	return riuscita;
}

void Gestore_robot::stampa_buffer() const {
    cout << "Obbiettivi presenti: " << endl;
    for (auto elemento : obbiettivi) {
        cout << elemento.first << "; " <<elemento.second << endl;
    }
    cout << endl;
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