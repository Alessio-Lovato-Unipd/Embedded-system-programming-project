#include "mappa.h"
#include <fstream>
#include <iostream>

using std::endl;
using std::cerr;

int main(int argc, char *argv[]) {
  if (argc == 1 || argc >2) {
    cerr << "Deve essere inserito il percorso del file da cui ricavare gli ostacoli e nient'altro." << endl;
    exit(EXIT_FAILURE);
  }

  std::ifstream file(argv[1], std::ios::in);
  if (!file.is_open()) {
    std::cerr << "Errore nell'apertura di: " << argv[1] << endl;
    exit(EXIT_FAILURE);
  } else {

  }
    return 0;
}