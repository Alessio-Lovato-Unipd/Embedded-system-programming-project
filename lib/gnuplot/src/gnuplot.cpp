#include "gnuplot.h"
#include <unistd.h>

gnuplot::gnuplot(const tipo_stampa stampa)
{
    if (stampa == tipo_stampa::persist)
        gnuplotpipe = popen("gnuplot -persist", "w");
    else
        gnuplotpipe = popen("gnuplot", "w");

    if (!gnuplotpipe)
        std::cerr << ("Gnuplot non trovato!");
}

gnuplot::~gnuplot() {
    fprintf(gnuplotpipe, "exit\n");
    pclose(gnuplotpipe);
}

void gnuplot::operator() (const std::string &command) {
    fprintf(gnuplotpipe, "%s\n", command.c_str());
    fflush(gnuplotpipe);
}

void stampa_grafico(double min_x, double min_y, double max_x, double max_y, double dimensione_celle, const gnuplot::tipo_stampa tipo_output) {
    gnuplot p(tipo_output);
    p("set palette defined (0 \"black\", 1 \"white\", 2 \"dark-green\")");
    std::string comando {"set title \"Mappa degli ostacoli - dimesione celle: "};
    comando.append(std::to_string(dimensione_celle)).append(" [m]\"");
    p(comando);
    p("set xlabel \"X [m]\"");
    p("set ylabel \"Y [m]\"");
    //unità misura in metri
    comando.clear();
    comando = "set xrange[";
    comando.append(std::to_string(min_x)).append(":").append(std::to_string(max_x)).append("] noreverse nowriteback");
    p(comando);
    comando.clear();
    comando = "set yrange[";
    comando.append(std::to_string(min_y)).append(":").append(std::to_string(max_y)).append("] noreverse nowriteback");
    p(comando);
    p("unset colorbox");
    p("set xtics 1");
    p("set ytics 1");
    comando.clear();
    //disegno celle
    comando = "set x2tics ";
    comando.append(std::to_string(min_x)).append(",").append(std::to_string(dimensione_celle)).append(",").append(std::to_string(max_x)).append("format \"\" scale 0 ");
    p(comando);
    comando.clear();
    comando = "set y2tics ";
    comando.append(std::to_string(min_y)).append(",").append(std::to_string(dimensione_celle)).append(",").append(std::to_string(max_y)).append("format \"\" scale 0 ");
    p(comando);
    p("set style line 12 lc rgb 'dark-orange' lt 1 lw 2");
    p("set style line 13 lc rgb 'red' lt 1 lw 1");
    p("set grid front x2tics y2tics ls 13");
    p("set grid front  ");
    if (tipo_output == gnuplot::tipo_stampa::file) {
        p("set terminal png size 1920,1080 enhanced font \"Helvetica,10\"");
        p("set out \"mappa.png\"");
    }
    p("plot \"gnuplot_raw.dat\" with image notitle");
    if (tipo_output == gnuplot::tipo_stampa::no_persist)
        sleep(1);
}