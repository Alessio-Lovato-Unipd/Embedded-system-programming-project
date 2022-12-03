#include "mappa.h"
#include <iostream>


int main(){
    ostacolo elemento{4,3,1,5};
    std::cout << "x1: " << elemento.posizione_minima().first <<"  y1: " << elemento.posizione_minima().second
           <<"  x2: " << elemento.posizione_massima().first <<"  y2: " << elemento.posizione_massima().second << std::endl;
    std::cout << std::endl;
    return 0;
}