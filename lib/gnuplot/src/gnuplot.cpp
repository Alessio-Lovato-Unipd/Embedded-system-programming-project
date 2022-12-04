#include "gnuplot.h"

gnuplot::gnuplot()
        : gnuplotpipe {popen ("gnuplot -persit", "w")} {
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

void stampa_grafico(int min_x, int min_y, int max_x, int max_y) {
    gnuplot p;
    p("set palette defined (0 \"white\", 1 \"black\")");
    p("set title \"Mappa degli ostacoli\"");
    std::string comando{"set xrange["};
    comando.append(std::to_string(min_x)).append(".5:").append(std::to_string(max_x)).append(".5] noreverse nowriteback");
    p(comando);
    comando.clear();
    comando = "set yrange[";
    comando.append(std::to_string(min_y)).append(".5:").append(std::to_string(max_y)).append(".5] noreverse nowriteback");
    p(comando);
    p("unset colorbox");
    p("set xtics 1 scale 0"); //imposta distanza di comparsa valori  etichette su asse x
    p("set ytics 1 scale 0"); //imposta distanza di comparsa valori etichette su asse x
    comando.clear();
    comando = "set x2range[";
    comando.append(std::to_string(min_x)).append(":").append(std::to_string(max_x + 1)).append("] noreverse nowriteback");
    p(comando);
    comando.clear();
    comando = "set y2range[";
    comando.append(std::to_string(min_y)).append(":").append(std::to_string(max_y + 1)).append("] noreverse nowriteback");
    p(comando);
    p("set x2tics 1 format \"\" scale 0");
    p("set y2tics 1 format \"\" scale 0");
    p("set linetype 1 lc rgb \"dark-orange\"");
    p("set grid x2tics y2tics front linetype 1");
    p("set out \"mappa.png\"");
    p("plot \"gnuplot_raw.dat\" with image notitle");
}