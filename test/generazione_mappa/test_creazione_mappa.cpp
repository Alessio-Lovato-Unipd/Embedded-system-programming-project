#include "mappa.h"
#include <iostream>

using std::endl;
using std::cerr;
using std::string;
using std::cout;


void crea_vettore_ostacoli(string filename, vector<ostacolo> &vettore){
    string line;
    std::ifstream file(filename, std::ios::in);
   
    while (getline(file, line)) {
        auto virgola{line.find(',')};
        string x1 = line.substr(0, virgola);

        auto virgola_successiva{line.find(',', virgola + 1)};
		    string y1 = line.substr(virgola + 1, virgola_successiva);

        virgola = line.find(',', virgola_successiva + 1);
		    string x2 = line.substr(virgola_successiva + 1, virgola);

        virgola_successiva = line.find(',', virgola + 1);
		    string y2 = line.substr(virgola + 1, virgola_successiva);

        vettore.push_back(ostacolo{stoi(x1), stoi(y1), stoi(x2), stoi(y2)});
    }

    file.close();
}



int main(int argc, char *argv[]) {
  if (argc == 1 || argc >2) {
    cerr << "Deve essere inserito il percorso del file da cui ricavare gli ostacoli e nient'altro." << endl;
    exit(EXIT_FAILURE);
  }

  std::ifstream file_conteggio(argv[1], std::ios::in);
  if (!file_conteggio.is_open()) {
    std::cerr << "Errore nell'apertura di: " << argv[1] << endl;
    exit(EXIT_FAILURE);
  } else {
 
    //numero di ostacoli presenti nel file
    int righe{conta_ostacoli_da_file(file_conteggio)};
    file_conteggio.close();

    //vettore contenente le posizioni degli ostacoli
    vector<ostacolo> ostacoli;
    ostacoli.reserve(righe);
    crea_vettore_ostacoli(argv[1], ostacoli);

    //mappa
    //mappa griglia{ostacoli};

    //stampo il vettore di ostacoli
    std::sort(ostacoli.begin(), ostacoli.end());
    for (auto elemento : ostacoli)
      cout << "x1: " << elemento.posizione_minima().first <<"  y1: " << elemento.posizione_minima().second
           <<"  x2: " << elemento.posizione_massima().first <<"  y2: " << elemento.posizione_massima().second <<endl;
    cout << endl;
  }
    return 0;
}
