todo:

- mplementare dimensione minima se non ho ostacoli cella
- gestire sottomultipli del metro
- vedere cos succede se metto ostacolo su unico punto


concorrenza:
-variabile condizione mappa_da_aggiornare
-calcolo percorso
- mutex scarico set robot
- se distanza robot < distanza ostacolo minimo
    - ricalcolo forza repulsiva per le celle
    -ricalcolo cella dove andare
-rilascio mutex

