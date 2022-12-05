#ifndef GNUPLOT_H
#define GNUPLOT_H

#include <iostream>
#include <string>

class gnuplot {
public:
    gnuplot();
    ~gnuplot();
    void operator () (const std::string &command);

protected:
    FILE *gnuplotpipe;
};

//stampo grafico predefinito della mappa
void stampa_grafico(float min_x, float min_y, float max_x, float max_y, float dimensione_celle);

#endif