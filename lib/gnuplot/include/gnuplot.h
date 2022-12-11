#ifndef GNUPLOT_H
#define GNUPLOT_H

#include <iostream>
#include <string>

class gnuplot {
public:
    //classe di supporto alla stampa della mappa
    enum class tipo_stampa{file, persist, no_persist};

    /*Costruttore classe
      Parametri:
        stampa: enumeratore per impostare il tipo di output
                file: non viene mostrato la mappa ma nella cartella di output verrà salvata con nel file 'mappa.png'
                persist: non viene generato nessun file ma viene aperto una finestra di gnuplot dove vedere la mappa
                no_persist: indicato per visualizzare mappa in aggionramento, mostra la finestra di gnuplot con la mappa per 1 secondo

    */
    gnuplot(const tipo_stampa stampa = tipo_stampa::no_persist);

    //Distruttore classe
    ~gnuplot();

    //Funzione per inviare un comando a gnuplot
    void operator () (const std::string &command);

protected:
    //pipe del processo gnuplot
    FILE *gnuplotpipe;
};

/*Funzione per output grafico mappa
  Parametri:
    - min_x: poisione minima sull'asse x della mappa
    - min_y: posizione minima sull'asse y della mappa
    - max_x: poisione massima sull'asse x della mappa
    - max_y: posizione massima sull'asse y della mappa
    - tipo_output: tipologia di output da utilizzare
        file: non viene mostrato la mappa ma nella cartella di output verrà salvata con nel file 'mappa.png'
        persist: non viene generato nessun file ma viene aperto una finestra di gnuplot dove vedere la mappa
        no_persist: indicato per visualizzare mappa in aggionramento, mostra la finestra di gnuplot con la mappa per 1 secondo
*/
void stampa_grafico(double min_x, double min_y, double max_x, double max_y, double dimensione_celle, const gnuplot::tipo_stampa tipo_output);

#endif