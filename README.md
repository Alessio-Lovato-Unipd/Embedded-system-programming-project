
# Assegnamento PSE 2022/2023 Lovato Alessio

L'assegnamento consiste nello sviluppo di un programma per la generazione di un algoritmo concorrenziale per la gestione della pianificazione del moto di più robot olonomi su una stessa mappa.

## Dati Richiesti
Per poter generare la mappa è necessario disporre di un file .csv contenente le posizioni minime e massime degli ostacoli presenti nella mappa.\
Come visibile dal file 'ostacoli.csv' nella cartella test/generazione_mappa/ il file deve contenere le posizioni (x1, y1, x2, y2) di un unico ostacolo su ogni riga, **divise da virgole**.\
Alcuni esempi sono:\
1,5,4,9 _<--ostacolo 1_\
2,6,1,8 _<--ostacolo 2_\
-1,6,-7,-8 _<--ostacolo 3_\
1,3,5 _<--ostacolo 4 potenzialmente non rilevato, dipende da dimensione cella_\
1,3,5,5 _<--non rilevato_\
1,4,6, _<--verrà generato un errore che interromperà l'esecuzione_\
1,4,6,a _<--verrà generato un errore che interromperà l'esecuzione se presente una lettera o simbolo non numerico_

_N.B. La disposizione deve essere (x1,y1,x2,y2) ma non necessariamente x1 < x2 e y1 < y2._
***
## Test
Esempi di utilizzo possono essere presi dalla cartella test

#### Dipendenze
Per poter utilizzare i test è necessario installare gnuplot e avere una versione cmake minima 3.25.0.\
Per poter eseguire i test senza utilizzare gnuplot:
* Eliminare la funzione "stampa_gnuplot()" dai sorgenti dei test
* Eliminare le sue occorrenze di stampa_gnuplot nella funzione dei test nel codice sorgente
* Eliminare la riga _#include "gnuplot.h"_
* Eliminare la riga _find_package(Gnuplot 5.2.8 REQUIRED)_ nel file test/generazione_mappa/CMakeLists.txt

#### Gnuplot
Per poter generare i grafici, gnuplot ha bisogno di un file intermedio che viene salvato nella cartella di esecuzione dell'eseguibile con il nome di "gnuplot_raw.dat",\
Questo processo è eseguito in automatico dalla funzione "stampa_gnuplot()" nel file sorgente di test.

#### Output testuale
I test salvano nella cartella dell'eseguibile anche un file testuale (nel nostro caso "mappa.txt" ma il nome può essere modificato) dove viene stampata la mappa con i numeri:
* 0 -> posizione libera
* 1 -> nella posizione è presente un ostacolo
* 2 -> nella posizione è presente un robot
Questo processo è eseguito dalla funzione "stampa_mappa()".

#### Compilare un test

1 ) Creare una cartella build all'interno della cartella principale del progetto\
2 ) Entrare nella cartella\
3 ) Eseguire da terminale il comando _cmake .._\
4 ) Eseguire il comando _make_\
5 ) I test si troveranno all'interno della sottocartella _./test/generazione_mappa/_

#### Eseguire un test

Per poter eseguire uno dei test è necessario passare dei dati come argomento:
* Percorso del file contenente gli ostacoli
* Nome del file dove esportare la griglia stampata testualmente (compresa estensione, consigliata .txt)
* Dimensione in metri delle celle della mappa che si vuole generare (il separatore decimale può essere sia punto che virgola)

***

### TODO:

idea per concorrenza:
- variabile condizione mappa_da_aggiornare
- calcolo percorso
- mutex scarico set robot
- se distanza robot < distanza ostacolo minimo
    - ricalcolo forza repulsiva per le celle
    - ricalcolo cella dove andare
- rilascio mutex