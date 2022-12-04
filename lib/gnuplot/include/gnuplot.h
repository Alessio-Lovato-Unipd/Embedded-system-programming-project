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
void stampa_grafico(int min_x, int min_y, int max_x, int max_y);

#endif