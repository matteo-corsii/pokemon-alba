# Stato della localizzazione italiana

## Ambito verificato

Questa milestone localizza il percorso usato dal prototipo di Pokémon Alba dal menu di pausa alla gestione della squadra:

- voci ordinarie del menu aperto con START (`BORSA`, `SALVA`, `OPZIONI`, `ESCI`);
- menu della squadra e azioni contestuali ordinarie (`RIEPILOGO`, `SPOSTA`, `STRUMENTO`, `DAI`, `PRENDI`, `POSTA`, `LEGGI`, `ANNULLA`);
- richieste e messaggi principali per selezione, spostamento e gestione degli strumenti dei Pokémon;
- titoli ed etichette delle pagine informazioni, statistiche e mosse del riepilogo;
- intestazioni `ABILITÀ` e `MEMO ALLENATORE`, nomi delle 25 Nature e modelli dinamici del Memo Allenatore;
- testi del riepilogo delle Uova e sigla `AO/` dell'Allenatore Originale;
- nomi e descrizioni di Erbaiuto, Agonismo, Aiutofuoco, Corrosione, Acquaiuto e Idratazione, usate dalle tre linee starter di Ausonia;
- nomi e descrizioni delle 46 mosse uniche presenti nei learnset per livello delle nove specie starter di Ausonia; `Fangosberla` è conservata e `MOVE_SMOKESCREEN` è visualizzata come `Muro di Fumo`.

Le stringhe sono definite nell'architettura globale esistente (`src/strings.c`) oppure nelle tabelle locali delle schermate (`src/data/party_menu.h` e `src/pokemon_summary_screen.c`). Non esiste, in questa versione del progetto, un catalogo separato per la lingua italiana. Le stringhe globali tradotte sono quindi condivise dalle configurazioni Emerald, FireRed e LeafGreen e devono restare valide in tutte e tre le build.

## Scelte di interfaccia

Le traduzioni mantengono i nomi propri `POKéMON`, `POKéDEX` e `POKéNAV`. Nel riepilogo sono usate abbreviazioni italiane soltanto dove la finestra contiene anche valori numerici: `ATT.SP.`, `DIF.SP.`, `VEL.`, `PREC.` e `STAT.`. I test misurano le etichette con le utility e i font reali del progetto.

Le intestazioni inglesi `ABILITY` e `TRAINER MEMO`, incorporate nella tilemap upstream, vengono neutralizzate in memoria e sostituite con testo runtime `ABILITÀ` e `MEMO ALLENATORE` usando il font stretto già incluso. Nessun asset grafico è stato modificato. La Natura è mostrata nel memo come `Natura {NATURA}`; placeholder, colori, livelli e luoghi dinamici restano invariati.

`Fangosberla` conserva potenza, precisione, PP, tipo, categoria, bersaglio, priorità, riduzione della precisione e animazione di `MOVE_MUD_SLAP`. Cingerm continua a impararla al livello 7. `Muro di Fumo` conserva integralmente i dati tecnici di `MOVE_SMOKESCREEN` e descrive in italiano la riduzione della Precisione del bersaglio. L'audit ha lasciato invariati livelli, ordine e contenuto di tutti e nove i learnset; soltanto nome e descrizione dei record globali previsti sono stati modificati.

La milestone grafica delle sei evoluzioni non modifica stringhe, nomi di mosse, descrizioni, abilità o learnset. Le 46 mosse già verificate, comprese `Fangosberla` e `Muro di Fumo`, conservano lo stato italiano documentato.

## Testi inglesi mantenuti intenzionalmente

La milestone non costituisce una traduzione completa del gioco. Restano intenzionalmente fuori ambito:

- nomi propri e marchi dell'interfaccia, incluso il nome del giocatore;
- nomi e descrizioni delle mosse esterne ai nove learnset esaminati, degli strumenti e delle abilità ancora provenienti dall'upstream;
- testi del riepilogo estranei a Nature, provenienza, Uova e alle sei abilità delle linee starter;
- azioni della squadra visibili soltanto in lotta, gare, strutture speciali, scambi o sistemi opzionali, come `SHIFT`, `SEND OUT`, `ENTER` e `NO ENTRY`;
- menu specializzati per forme, fusioni e funzioni opzionali non usati dal prototipo;
- contenuto interno delle schermate BORSA, SALVA e OPZIONI oltre alle rispettive voci del menu di pausa;
- identificatori C, enum, commenti tecnici e API, che rimangono in inglese.

Una milestone futura dovrà completare l'audit di tutte le mosse, delle abilità e delle schermate prima di estendere la localizzazione agli altri sottosistemi. Il gioco non è ancora completamente tradotto.
