todo:

-implementare dimensione robot maggiore cella e rimuovere posizioni reali


concorrenza:
-variabile condizione mappa_da_aggiornare
-calcolo percorso
- mutex scarico set robot
- se distanza robot < distanza ostacolo minimo
    - ricalcolo forza repulsiva per le celle
    -ricalcolo cella dove andare
-rilascio mutex

