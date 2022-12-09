#include "robot.h"

Robot::Robot(const posizione &robot, const posizione &obbiettivo, mappa &mappa_riferimento, const float raggio_robot)
        : robot_celle_{robot}, robot_reale_{robot}, obbiettivo_celle_{obbiettivo}, obbiettivo_reale_{obbiettivo}, mappa_{mappa_riferimento}, raggio_{raggio_robot}
{
	//localizzo robot nelle celle
    mappa_.centra_posizione(robot_celle_.first, mappa::tipo_posizione::centro);
    mappa_.centra_posizione(robot_celle_.second, mappa::tipo_posizione::centro);
	mappa_.centra_posizione(obbiettivo_celle_.first, mappa::tipo_posizione::centro);
    mappa_.centra_posizione(obbiettivo_celle_.second, mappa::tipo_posizione::centro);

    if (!mappa_.contiene_cella(obbiettivo_celle_))
                incrementa_mappa(obbiettivo_celle_);
        else if (!mappa_.cella_libera(obbiettivo_celle_)) {//controllo che l'obbiettivo o il robot non siano in un ostacolo
                std::cerr << "L'obbiettivo è stato inserito in una posizione occupata da un ostacolo" << std::endl;
                exit(EXIT_FAILURE);
    }
	celle_occupate_ = celle_adiacenti(robot_celle_);
    for (auto &elemento : celle_occupate_) {

        mappa_.posiziona_robot_cella(elemento);
		if (!mappa_.contiene_cella(elemento))
					incrementa_mappa(elemento);
			else if (!mappa_.cella_libera(elemento)) {//controllo che l'obbiettivo o il robot non siano in un ostacolo
					std::cerr << "Il robot è stato inserito in una posizione occupata da un ostacolo" << std::endl;
					exit(EXIT_FAILURE);
		}   
	}
    
    //attendo mutex per aggiornare mappa
    aggiorna_mappa(mappa_riferimento);
    //rilascio mutex

}

void Robot::incrementa_mappa(const posizione &posizione_necessaria) {
        //calcolo le nuove posizioni minime
        posizione minima{std::min(posizione_necessaria.first, mappa_.posizione_minima().first),
                         std::min(posizione_necessaria.second, mappa_.posizione_minima().second)};
        posizione massima{std::max(posizione_necessaria.first, mappa_.posizione_massima().first),
                         std::max(posizione_necessaria.second, mappa_.posizione_massima().second)};
        
        mappa_.aggiorna_dimensioni(minima, massima);
        //inserisco le celle mancanti sovrascivendo le precedenti
        mappa_.inserisci_celle(minima, massima);
}

float Robot::campo_attrattivo(const posizione &partenza, const posizione &arrivo) const {
        return ((mappa_.fattore_scala_campo_attrattivo() * pow(calcolo_distanza(partenza, arrivo), 2.0f))/2.0f);
}

distanza Robot::calcolo_distanza(const posizione &partenza, const posizione &arrivo) const {
        return (sqrt(pow((arrivo.first - partenza.first), 2.0f) + pow((arrivo.second - partenza.second), 2.0f)));
}

pair<posizione, distanza> Robot::oggetto_piu_vicino(const posizione &attuale, const set<posizione>::const_iterator &inizio_set, const set<posizione>::const_iterator &fine_set) {
        map<posizione, distanza> distanza_ostacolo;
        std::for_each(inizio_set, fine_set, [this, &attuale, &distanza_ostacolo](auto pos) {distanza_ostacolo[pos] = calcolo_distanza(attuale, pos);});
        auto elemento_minimo {min_element(distanza_ostacolo.cbegin(), distanza_ostacolo.cend(), [](auto &lhs, auto &rhs) { return lhs.second < rhs.second;})};
        return pair{elemento_minimo->first, elemento_minimo->second};
}

float Robot::campo_repulsivo(const posizione &partenza, const posizione &arrivo) const {
        distanza dist{calcolo_distanza(partenza, arrivo)};
        if (dist < mappa_.distanza_minima_ostacolo())
                return ((mappa_.fattore_scala_campo_repulsivo()*pow(((1/dist)+(1/mappa_.distanza_minima_ostacolo())),2.0f))/2.0f);
        else
                return 0.0;
}

map<posizione, dati_cella> Robot::calcola_potenziali_celle_adiacenti () {
        map<posizione, dati_cella> potenziali;
        posizione attuale{robot_celle_};
        //cancello la mia posizione dalla mappa
        for (auto &elemento : celle_occupate_) {
            mappa_.libera_cella_robot(elemento);
		}
        pair<posizione, distanza> ostacolo{oggetto_piu_vicino(attuale, mappa_.ostacoli_cbegin(), mappa_.ostacoli_cend())};
        //cella +/
        attuale.first += mappa_.dimensione_celle_metri();
        if (mappa_.contiene_cella(attuale) && mappa_.cella_libera(attuale))
                potenziali.insert({attuale, {ostacolo.second, campo_attrattivo(attuale,obbiettivo_celle_) + campo_repulsivo(attuale, ostacolo.first)}});
        //cella ++
        attuale.second += mappa_.dimensione_celle_metri();
        if (mappa_.contiene_cella(attuale) && mappa_.cella_libera(attuale))
                potenziali.insert({attuale, {ostacolo.second, campo_attrattivo(attuale,obbiettivo_celle_) + campo_repulsivo(attuale, ostacolo.first)}});
        //cella /+
        attuale.first -= mappa_.dimensione_celle_metri();
        if (mappa_.contiene_cella(attuale) && mappa_.cella_libera(attuale))
                potenziali.insert({attuale, {ostacolo.second, campo_attrattivo(attuale,obbiettivo_celle_) + campo_repulsivo(attuale, ostacolo.first)}});
        //cella -+
        attuale.first -= mappa_.dimensione_celle_metri();
        if (mappa_.contiene_cella(attuale) && mappa_.cella_libera(attuale))
                potenziali.insert({attuale, {ostacolo.second, campo_attrattivo(attuale,obbiettivo_celle_) + campo_repulsivo(attuale, ostacolo.first)}});
        //cella /-
        attuale.second -= mappa_.dimensione_celle_metri();
        if (mappa_.contiene_cella(attuale) && mappa_.cella_libera(attuale))
                potenziali.insert({attuale, {ostacolo.second, campo_attrattivo(attuale,obbiettivo_celle_) + campo_repulsivo(attuale, ostacolo.first)}});
        //cella --
        attuale.second -= mappa_.dimensione_celle_metri();
        if (mappa_.contiene_cella(attuale) && mappa_.cella_libera(attuale))
                potenziali.insert({attuale, {ostacolo.second, campo_attrattivo(attuale,obbiettivo_celle_) + campo_repulsivo(attuale, ostacolo.first)}});
        //cella /-
        attuale.first += mappa_.dimensione_celle_metri();
        if (mappa_.contiene_cella(attuale) && mappa_.cella_libera(attuale))
                potenziali.insert({attuale, {ostacolo.second, campo_attrattivo(attuale,obbiettivo_celle_) + campo_repulsivo(attuale, ostacolo.first)}});
        //cella +-
        attuale.first += mappa_.dimensione_celle_metri();
        if (mappa_.contiene_cella(attuale) && mappa_.cella_libera(attuale))
                potenziali.insert({attuale, {ostacolo.second, campo_attrattivo(attuale,obbiettivo_celle_) + campo_repulsivo(attuale, ostacolo.first)}});

        return potenziali;
}

float Robot::ricalcolo_potenziale_cella (const posizione &cella_da_ricalcolare, const posizione &robot_ostacolo) const {
        return (campo_attrattivo(cella_da_ricalcolare, robot_ostacolo) + campo_repulsivo(cella_da_ricalcolare, robot_ostacolo));
}

void Robot::aggiorna_campi_potenziale (map<posizione, dati_cella> &celle_con_potenziali) {
        for (auto elemento : celle_con_potenziali) {
                pair<posizione, distanza> ostacolo{oggetto_piu_vicino(elemento.first, mappa_.robot_cbegin(), mappa_.robot_cend())};
                if (elemento.second.first > ostacolo.second) {
                        //associo la nuova distanza alla cella
                        elemento.second.first = ostacolo.second;
                        elemento.second.second = ricalcolo_potenziale_cella(elemento.first, ostacolo.first);
                }
        }
}

//prima faccio i calcoli sulla mappa che conosco, poi aggiorno le posizioni dei robot
void Robot::sposta_su_cella_successiva(mappa &mappa_condivisa){

        //se il robot è abbastanza vicino all'ostacolo elimino la componente repulsiva del campo di forza
        if (calcolo_distanza(obbiettivo_celle_, robot_celle_) < mappa_.distanza_minima_ostacolo())
                mappa_.imposta_fattore_scala_repulsivo(0);
        map<posizione, dati_cella> potenziali_celle{calcola_potenziali_celle_adiacenti()};
        
        //attendo mutex per leggere posizioni robot aggiornate
        //scarico mappa aggiornata
        mappa_.aggiorna_mappa(mappa_condivisa);
		//cancello la mia posizione dalla mappa
		for (auto &elemento : celle_occupate_)
                mappa_.libera_cella_robot(elemento);
        //aggiorno i campi di potenziale dovuti alla presenza dei robot
        aggiorna_campi_potenziale(potenziali_celle);
        //evito di andare in diagonale se ostacolo presente nelle celle "a croce"
        limita_spostamenti(potenziali_celle);
        //calcolo posizione futura
        posizione prossima_cella;
        if (potenziali_celle.contains(obbiettivo_celle_))
                prossima_cella = {obbiettivo_celle_.first, obbiettivo_celle_.second};
        else
                prossima_cella = std::min_element(potenziali_celle.cbegin(), potenziali_celle.cend(),
                                                        [](auto &lhs, auto &rhs) { return lhs.second.second < rhs.second.second;})->first;
        //evito minimi locali o robot nelle vicinanze
		while (posizioni_precedenti.contains(prossima_cella) || collisione(prossima_cella)) {
			potenziali_celle.erase(prossima_cella);
			if (potenziali_celle.empty()) {
					std::cerr << "Ci troviamo in un minimo locale, non è possibile muoversi" << endl;
					exit(EXIT_FAILURE);
			}
			prossima_cella = std::min_element(potenziali_celle.cbegin(), potenziali_celle.cend(),
													[](auto &lhs, auto &rhs) { return lhs.second.second < rhs.second.second;})->first;
		}
        //salvo la nuova posizione
        robot_celle_ = {prossima_cella.first, prossima_cella.second};
        //inserisco tutte le celle occupate dal robot
		celle_occupate_.clear();
        celle_occupate_ = celle_adiacenti(robot_celle_);
        for (auto &elemento : celle_occupate_)
                mappa_.posiziona_robot_cella(elemento);
        //aggiorno mappa
        mappa_condivisa.aggiorna_mappa(mappa_);
        //rilascio chiave mutex
        //salvo la posizione precedente per evitare minimo locali
        posizioni_precedenti.insert(prossima_cella);

}

void Robot::limita_spostamenti(map<posizione, dati_cella> &potenziali_celle) {
        //cella ++

        posizione da_verificare{robot_celle_.first + mappa_.dimensione_celle_metri(), robot_celle_.second + mappa_.dimensione_celle_metri()};
        if (potenziali_celle.contains(da_verificare)) {
                posizione limite1{da_verificare.first - mappa_.dimensione_celle_metri(), da_verificare.second};
                posizione limite2{da_verificare.first, da_verificare.second - mappa_.dimensione_celle_metri()};
                if (collisione(limite1) || collisione(limite2))
                        potenziali_celle.erase(da_verificare);
        }

        //cella -+
        da_verificare = {robot_celle_.first - mappa_.dimensione_celle_metri(), robot_celle_.second + mappa_.dimensione_celle_metri()};
        if (potenziali_celle.contains(da_verificare)) {
                posizione limite1 = {da_verificare.first + mappa_.dimensione_celle_metri(), da_verificare.second};
                posizione limite2 = {da_verificare.first, da_verificare.second - mappa_.dimensione_celle_metri()};
                if (collisione(limite1) || collisione(limite2))
                        potenziali_celle.erase(da_verificare);
        }
        //cella --
        da_verificare = {robot_celle_.first - mappa_.dimensione_celle_metri(), robot_celle_.second - mappa_.dimensione_celle_metri()};
        if (potenziali_celle.contains(da_verificare)) {
                posizione limite1 = {da_verificare.first + mappa_.dimensione_celle_metri(), da_verificare.second};
                posizione limite2 = {da_verificare.first, da_verificare.second + mappa_.dimensione_celle_metri()};
                if (collisione(limite1) || collisione(limite2))
                        potenziali_celle.erase(da_verificare);
        }
        //cella +-
        da_verificare = {robot_celle_.first + mappa_.dimensione_celle_metri(), robot_celle_.second - mappa_.dimensione_celle_metri()};
        if (potenziali_celle.contains(da_verificare)) {
                posizione limite1 = {da_verificare.first - mappa_.dimensione_celle_metri(), da_verificare.second};
                posizione limite2 = {da_verificare.first, da_verificare.second + mappa_.dimensione_celle_metri()};
                if (collisione(limite1) || collisione(limite2))
                        potenziali_celle.erase(da_verificare);
        }
}

set<posizione> Robot::celle_adiacenti(const posizione &attuale) const {
        int moltiplicatore{static_cast<int>(ceil(raggio_/mappa_.dimensione_mezza_cella()) - 1)};// - 1 per rendere esatto il moltiplicatore, altrimenti incrementa di una cella in più secondo cicli sottostanti
        set<posizione> posizioni;
		for (auto x{attuale.first - (moltiplicatore * mappa_.dimensione_celle_metri())};
         x <= (attuale.first + (moltiplicatore * mappa_.dimensione_celle_metri())); x += mappa_.dimensione_celle_metri()) {
                for (auto y{attuale.second - (moltiplicatore * mappa_.dimensione_celle_metri())};
                        y <= (attuale.second + (moltiplicatore * mappa_.dimensione_celle_metri())); y += mappa_.dimensione_celle_metri()) {
                                posizioni.insert(posizione{x, y});
                        }
         }
		 return posizioni;
}

bool Robot::collisione(const posizione &cella) const{
	set<posizione> prossime_posizioni{celle_adiacenti(cella)};
	for (auto pos{prossime_posizioni.cbegin()}; pos != prossime_posizioni.cend(); pos++){
		if (mappa_.contiene_cella(*pos)) {
			if (mappa_.contains_robot(*pos) || !mappa_.cella_libera(*pos))
				return true;
		}
	}
	return false;
}
