# Stato della localizzazione italiana

## Ambito verificato

Questa milestone localizza il percorso usato dal prototipo di Pokémon Alba dal menu di pausa alla gestione della squadra:

- voci ordinarie del menu aperto con START (`BORSA`, `SALVA`, `OPZIONI`, `ESCI`);
- menu della squadra e azioni contestuali ordinarie (`RIEPILOGO`, `SPOSTA`, `STRUMENTO`, `DAI`, `PRENDI`, `POSTA`, `LEGGI`, `ANNULLA`);
- richieste e messaggi principali per selezione, spostamento e gestione degli strumenti dei Pokémon;
- titoli ed etichette delle pagine informazioni, statistiche e mosse del riepilogo;
- nome e descrizione di `MOVE_MUD_SLAP`, visualizzata come `Fangosberla`.

Le stringhe sono definite nell'architettura globale esistente (`src/strings.c`) oppure nelle tabelle locali delle schermate (`src/data/party_menu.h` e `src/pokemon_summary_screen.c`). Non esiste, in questa versione del progetto, un catalogo separato per la lingua italiana. Le stringhe globali tradotte sono quindi condivise dalle configurazioni Emerald, FireRed e LeafGreen e devono restare valide in tutte e tre le build.

## Scelte di interfaccia

Le traduzioni mantengono i nomi propri `POKéMON`, `POKéDEX` e `POKéNAV`. Nel riepilogo sono usate abbreviazioni italiane soltanto dove la finestra contiene anche valori numerici: `ATT.SP.`, `DIF.SP.`, `VEL.`, `PREC.` e `STAT.`. I test misurano le etichette con le utility e i font reali del progetto.

`Fangosberla` conserva potenza, precisione, PP, tipo, categoria, bersaglio, priorità, riduzione della precisione e animazione di `MOVE_MUD_SLAP`. Cingerm continua a impararla al livello 7.

## Testi inglesi mantenuti intenzionalmente

La milestone non costituisce una traduzione completa del gioco. Restano intenzionalmente fuori ambito:

- nomi propri e marchi dell'interfaccia, incluso il nome del giocatore;
- nomi e descrizioni delle altre mosse, degli strumenti e delle abilità ancora provenienti dall'upstream;
- nature, testo biografico dell'Allenatore originale, messaggi delle Uova e altre parti narrative del riepilogo;
- azioni della squadra visibili soltanto in lotta, gare, strutture speciali, scambi o sistemi opzionali, come `SHIFT`, `SEND OUT`, `ENTER` e `NO ENTRY`;
- menu specializzati per forme, fusioni e funzioni opzionali non usati dal prototipo;
- contenuto interno delle schermate BORSA, SALVA e OPZIONI oltre alle rispettive voci del menu di pausa;
- identificatori C, enum, commenti tecnici e API, che rimangono in inglese.

Una milestone futura dovrà eseguire un audit completo schermata per schermata e definire un glossario italiano stabile prima di estendere la localizzazione agli altri sottosistemi.
