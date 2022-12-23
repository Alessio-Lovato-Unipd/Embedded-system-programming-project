
# Assegnamento PSE 2022/2023 Lovato Alessio

L'assegnamento consiste nello sviluppo di un programma per la generazione di un algoritmo concorrenziale per la gestione della pianificazione del moto di più robot olonomi su una stessa mappa.

## Dati Richiesti
#### Ostacoli
Per poter generare la mappa è necessario disporre di un file .csv contenente le posizioni minime e massime degli ostacoli presenti nella mappa.\
Come visibile dal file 'ostacoli.csv' nella cartella test/generazione_mappa/ il file deve contenere le posizioni (x1, y1, x2, y2) di un unico ostacolo su ogni riga, **divise da virgole**.\
Alcuni esempi sono:\
1,5,4,9 _<--ostacolo 1_\
2,6,1,8 _<--ostacolo 2_\
-1,6,-7,-8 _<--ostacolo 3_\
1,3,5 _<--ostacolo 4 potenzialmente non rilevato, dipende da dimensione cella_\
1,3,5,5 _<--non rilevato_\
1,4,6, _<--verrà generato un errore che interromperà l'esecuzione_\
1,4,6,a _<--verrà generato un errore che interromperà l'esecuzione se presente una lettera o simbolo non numerico_\

_N.B. La disposizione deve essere (x1,y1,x2,y2) ma non necessariamente x1 < x2 e y1 < y2._

#### Posizione robot
Per poter comandare il robot è necessario passare per argomento il file contenente la posizione iniziale del robot.\
Deve essere un file .txt, dove in ogni riga sono inserite _posizione x, posizione y, raggio del robot_ separate da uno spazio.\
Alcuni esempi sono:
1 5.7 0.5 _<--posizione 1_\
6.2 8 1 _<--posizione 2_\
6,2 8 1 _<--posizione 3: errore, non possono essere usate virgole_\
6.2 8 -1 _<--posizione 4: errore, non possono essere usati valori negativi per il raggio_\

**N.B. Ongi riga identifica un robot. Il numero di robot nella mappa corrisponde al numero di righe di questo file**

#### Posizione obbiettivi
Per poter comandare il robot è necessario passare per argomento il file contenente la posizione degli obbiettivi dei robot.\
Deve essere un file .txt, dove in ogni riga sono inserite _posizione x, posizione y, separate da uno spazio.\
Alcuni esempi sono:
1 5.7 _<--posizione 1_\
6.2 8 _<--posizione 2_\
-4.7 -3 _<--posizione 4_\
6,2 8 _<--posizione 3: errore, non possono essere usate virgole_\

## Gnuplot
Per poter generare i grafici, gnuplot ha bisogno di un file intermedio che viene salvato nella cartella di esecuzione dell'eseguibile con il nome di "gnuplot_raw.dat".\
Questo processo è eseguito in automatico dalla funzione "stampa_gnuplot()" nel file sorgente di test.

### Output testuale
Alcuni test e il main salvano nella cartella dell'eseguibile anche un file testuale (nel nostro caso "mappa.txt" ma il nome può essere modificato) dove viene stampata la mappa con i numeri:
* 0 -> posizione libera
* 1 -> nella posizione è presente un ostacolo
* 2 -> nella posizione è presente un robot
Questo processo è eseguito dalla funzione "stampa_mappa()".

***
## App
Un esempio del codice funzionante con la concorrenza è disponibile nella cartella app.\
La cartella contiene al suo interno anche una cartella _data_ con all'interno degli esempi di file con i dati richiesti.\

#### Dipendenze main
Per poter utilizzare il codice è necessario installare gnuplot e avere una versione cmake minima 3.25.0.\
Per poter eseguire il codice senza utilizzare gnuplot:
* Eliminare le righe 42 e 53 nel file _main.cpp_
* Eliminare la riga _#include "gnuplot.h"_ nel file _main.h_
* Eliminare la riga _find_package(Gnuplot 5.2.8 REQUIRED)_ nel file app/CMakeLists.txt

In questo modo l'output può essere solo testuale o anche visivo inserendo il parametro 6


#### Compilare applicazione
1 ) Creare una cartella build all'interno della cartella principale del progetto\
2 ) Entrare nella cartella\
3 ) Eseguire da terminale il comando _cmake .._\
4 ) Eseguire il comando _make_\
5 ) I test si troveranno all'interno della sottocartella _./app_

#### Eseguire applicazione
Gli argomenti dati all'eseguibile sono:\
1 ) Dimensione celle in metri\
2 ) Percorso del file delle posizioni dei robot\
3 ) Percorso del file delle posizioni degli ostacoli\
4 ) Percorso del file degli obbiettivi del primo satellite\
5 ) Percorso del file degli obbiettivi del secondo satellite\
6 ) [facolattivo] Percorso del file di output della griglia\
6 ) [facoltativo] Fattore minimo delle celle (numero minimo di celle nella mappa); numero intero _- default: 2_\
7 ) [facoltativo] Fattore scala del campo attrattivo; numero decimale  _- default: 1.5_\
8 ) [facoltativo] Fattore scala del campo repulsivo; numero decimale  _- default: 100_\
9 ) [facoltativo] distanza minima percezione dell'ostacolo; numero decimale _- default: 5_\
10 ) [facoltativo] numero di celle da incrementare a bordo mappa; numero intero _- default: 3_\

_N.B. I file di esempio verranno copiati nella cartella con l'eseguibile
***
## Test
Esempi di utilizzo possono essere presi dalla cartella test

#### Compilare i test

1 ) Nel file _CMakeLists.txt_ della cartella principale, alla riga 13, sostituire _stable_ con _test_
2 ) Creare una cartella build all'interno della cartella principale del progetto\
3 ) Entrare nella cartella\
4 ) Eseguire da terminale il comando _cmake .._\
5 ) Eseguire il comando _make_\
6 ) I test si troveranno all'interno della sottocartella _./test/_

#### Eseguire un test

Per poter eseguire uno dei test è necessario passare dei dati come argomento:
* Percorso del file contenente gli ostacoli
* Nome del file dove esportare la griglia stampata testualmente (compresa estensione, consigliata .txt)
* Dimensione in metri delle celle della mappa che si vuole generare (il separatore decimale può essere sia punto che virgola)

## Dipendenze test
Per poter utilizzare il codice è necessario installare gnuplot e avere una versione cmake minima 3.25.0.\
Per poter eseguire il codice senza utilizzare gnuplot:
* Eliminare la funzione "stampa_gnuplot()" dai sorgenti dei test
* Eliminare le sue occorrenze di stampa_gnuplot nella funzione dei test nel codice sorgente
* Eliminare la riga _#include "gnuplot.h"_
* Eliminare la riga _find_package(Gnuplot 5.2.8 REQUIRED)_ nel file test/generazione_mappa/CMakeLists.txt