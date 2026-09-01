# Emissario del Lago e primo Eco

**Stato:** CANONICAL DESIGN e TECHNICAL AUDIT approvati — STRUCTURAL BLOCKOUT IMPLEMENTED

Questo documento fissa la progressione narrativa successiva alla Palestra delle
Macine, il blockout della nuova camera dell'Emissario e il comportamento del
primo Eco. Il solo blockout strutturale della camera e la coppia di warp con il
Lago sono implementati. Scene, animazioni, flag, Allenatori e modifiche alla
lotta restano intenzionalmente fuori da questo batch.

Le coordinate, gli ID numerici e i nomi dei simboli proposti nella sezione
tecnica devono essere ricontrollati sul `HEAD` del branch di implementazione
prima di essere assegnati.

## 1. Esito dell'audit canonico

Il repository conteneva già tre decisioni pertinenti:

- la progressione geografica passa da Via Consolare prima di arrivare al Lago
  di Albèra;
- Cisternide è escluso dagli incontri selvatici del Lago ed è riservato al
  futuro dungeon dell'Emissario;
- la Forma Riflesso è una futura trasformazione legata allo starter originale
  e al rapporto con il protagonista.

Non esistevano invece una mappa dell'Emissario, una seconda recluta Aurea, una
regola del primo Eco o una soluzione tecnica già implementata. Il presente
documento aggiunge questi elementi senza modificare la Forma Riflesso completa
descritta in `PROJECT_CONTEXT.md`.

Sono superati da questo design:

- il vecchio concetto dell'Emissario composto da tre vasche;
- il dialogo post-Palestra in cui Lauro afferma che l'Emissario deve restare
  chiuso anche dopo la consegna di Surf.

La nuova versione è una singola camera circolare. Il primo Eco è un'anticipazione
della Forma Riflesso, non la sua attivazione.

## 2. Ordine narrativo canonico

1. Il giocatore conclude la scena dei Cisternoni e incontra nuovamente Nico e
   Lia lungo la progressione verso Via Consolare.
2. Via Consolare viene attraversata prima del Lago di Albèra.
3. Vicino all'uscita nord, Lia rileva che la pressione converge verso
   l'Emissario e decide di precedere il gruppo.
4. Nico si dirige alla Palestra per sfidare Marina.
5. Il giocatore raggiunge il Lago. Prima della Medaglia Macina può incontrare
   Lia vicino all'Emissario, ma la camera non è ancora accessibile.
6. Il giocatore batte Marina e riceve Surf dal Professor Lauro.
7. Lauro comunica che le letture di Lia sono cambiate e indirizza il giocatore
   all'Emissario.
8. Nico, Lia e il giocatore entrano insieme nella camera.
9. La seconda recluta del Team Aurea affronta il giocatore. Quando manda in
   campo Cisternide avviene il primo Eco.
10. Dopo la lotta, il condotto verso nord introduce il futuro collegamento
    subacqueo al Lago di Nemi e indirizza la ricerca verso il bosco e gli
    antichi resti del Romitorio.

L'evento dell'Emissario è quindi **post-Palestra**, anche se Lia può raggiungere
l'ingresso prima che il giocatore ottenga la Medaglia.

## 3. Passaggi narrativi

I testi seguenti fissano intenzione, informazioni e tono. La conversione in
stringhe di gioco dovrà rispettare font, larghezza delle finestre e terminatori
del progetto senza cambiarne il significato.

### 3.1 Bivio su Via Consolare

La scena avviene presso l'uscita nord, prima della transizione al Lago.

> **LIA:** Aspettate. Qui la pressione non si disperde più.
>
> Dai Cisternoni seguiva le condotte. Ora converge tutta nella stessa
> direzione: l'Emissario del lago.
>
> Vado avanti a controllare l'ingresso.

> **NICO:** E io vado a controllare Marina!
>
> Se quella Palestra è davvero costruita sull'acqua, voglio vederla con i miei
> occhi.

Lia procede verso l'Emissario; Nico verso la Palestra. Il giocatore mantiene
il controllo e può esplorare liberamente il Lago.

### 3.2 Lia prima della Palestra

> **LIA:** Le letture arrivano da questa camera, ma il flusso cambia troppo in
> fretta.
>
> Non entriamo finché la pressione non si stabilizza. Io resto qui a misurare.
>
> Nico è già andato verso la Palestra.

Questo dialogo non deve fingere che Surf sia già disponibile e non deve
teletrasportare il giocatore.

### 3.3 Consegna di Surf e apertura dell'Emissario

La scena esistente di Lauro conserva la consegna di Surf ma aggiorna
l'informazione narrativa.

> **PROF. LAURO:** La Medaglia Macina abilita Surf fuori dalla lotta.
>
> Con Surf potrai attraversare nuove parti del Lago di Albèra.
>
> Lia ha segnalato un cambiamento all'Emissario. La pressione si è stabilizzata,
> almeno in superficie.
>
> Raggiungetela, ma non entrate separati.

La consegna resta sicura in caso di Borsa piena: l'accesso narrativo richiede
sia `FLAG_BADGE02_GET` sia `FLAG_RECEIVED_HM_SURF`.

### 3.4 Ricongiungimento all'ingresso

> **NICO:** Eccomi! Marina non scherza affatto.
>
> Lia mi ha chiamato appena sono uscito dalla Palestra.

> **LIA:** Il flusso è abbastanza regolare da entrare, ma non sappiamo che cosa
> troveremo oltre l'anello di pietra.
>
> Questa volta andiamo insieme.

Il dialogo non stabilisce ancora se Nico abbia già sconfitto Marina. Evita così
di vincolare prematuramente la sua progressione personale.

### 3.5 Seconda recluta Aurea

La recluta si presenta come una presenza già al lavoro nella camera, non come
la causa dichiarata dell'anomalia.

> **AUREA:** Finalmente. Le letture dei Cisternoni portavano proprio qui.

> **LIA:** Eravate voi a seguire i nostri dati.

> **AUREA:** I dati non bastano. Volevamo vedere come avrebbero reagito i
> Pokémon dentro questa camera.

> **NICO:** Potevi chiedere.

> **AUREA:** E perdere una risposta sincera? No.
>
> Se volete passare, mostratemi che cosa ascolta il vostro Pokémon.

La battaglia usa il normale flusso di una lotta Allenatore inserita in una
scena, sul modello già verificato di `trainerbattle_no_intro` nei Cisternoni.

### 3.6 Entrata di Cisternide e primo Eco

Quando Salampolla viene sconfitto e Cisternide è il prossimo Pokémon avversario,
la recluta può usare una trainer slide breve:

> **AUREA:** Ora ascolta, Cisternide.

Se lo starter originale è vivo e in panchina, prima della normale domanda di
cambio compare:

> **{SOPRANNOME} freme nella sua POKé BALL!**
>
> **Sembra voler scendere in campo.**

Segue la domanda di cambio già usata dal gioco. Non viene creato un secondo
menu. Il giocatore può accettare, rifiutare o aprire il menu e annullare.

Dopo l'eventuale cambio e dopo l'entrata di Cisternide:

> **Un'onda senza sorgente attraversa la sala!**
>
> **Il primo Eco raggiunge {POKÉMON ATTIVO}!**

L'animazione mostra un breve oscuramento del campo e un'increspatura simile a
un riflesso. Se è attivo lo starter originale, il suo riflesso anticipa per un
istante il movimento dello sprite. Non cambia forma, specie, sprite o abilità.

### 3.7 Dopo la lotta

> **AUREA:** Quindi risponde anche a voi...

> **LIA:** Non avete creato il fenomeno. Lo stavate aspettando.

> **AUREA:** La differenza vi consola?

Dopo la fuga della recluta, il dialogo ha una sola variazione breve.

Se lo starter originale era attivo durante il primo Eco:

> **LIA:** Per un istante, il riflesso di {SOPRANNOME} ha anticipato il suo
> movimento.
>
> Non era una trasformazione. Sembrava una risposta al vostro legame.

In ogni altro caso:

> **LIA:** Ha reagito {POKÉMON ATTIVO}, non soltanto uno starter.
>
> Quindi l'Eco non appartiene a una sola specie.

La conclusione comune è:

> **NICO:** Un Eco... ma arrivato prima del suono.

> **LIA:** Il primo Eco che siamo riusciti a vedere, non soltanto a misurare.
>
> Il condotto a nord continua sotto il bosco. Dovremo controllare anche i resti
> del Romitorio.

Il termine canonico è **il primo Eco**. `Eco riflesso` non è il nome della
meccanica.

## 4. Blockout della camera dell'Emissario

La nuova camera sostituisce integralmente il precedente concetto delle tre
vasche.

| Elemento | Contratto di design |
| --- | --- |
| Forma | Singola sala circolare, leggibile appena entrati |
| Dimensione iniziale | Circa 32×30 metatile, adattabile dopo il blockout |
| Ingresso | A sud, collegato al Lago tramite una normale coppia di warp |
| Pavimento | Anello in pietra percorribile lungo il perimetro |
| Acqua | Bacino centrale navigabile con Surf dopo la Medaglia Macina |
| Sbocco | Apertura idrica nella parete nord |
| Profondità futura | Una macchia d'acqua visivamente distinta presso lo sbocco |
| Compagni | Nico e Lia presenti durante la scena, senza follower engine |
| Incontri selvatici | Nessuno in questa prima versione |
| Uscita | Ritorno sicuro al Lago anche dopo la conclusione dell'evento |

La macchia usa un normale comportamento d'acqua, non Dive, non possiede warp e
non conduce a una mappa subacquea. In futuro collegherà l'Emissario al percorso
sommerso verso il Lago di Nemi.

### Tileset minimo

L'audit dei file attuali dimostra che `gTileset_General` e
`gTileset_Cisternoni` forniscono già acqua, pietra e comportamenti necessari al
blockout. La prima versione deve riusare questa coppia senza modificare
globalmente il tileset dei Cisternoni.

Un eventuale `gTileset_Emissario` dedicato verrà creato soltanto se il blockout
in Porymap dimostrerà l'assenza di metatile necessari alla forma circolare o
alla profondità visiva. Non è una dipendenza iniziale.

### Registrazione della mappa

Il nome tecnico proposto è `Emissario`, con layout `LAYOUT_EMISSARIO`. La mappa
va aggiunta **in coda** a `gMapGroup_Dungeons`: inserirla in mezzo al gruppo
sposterebbe inutilmente i numeri delle mappe già esistenti.

L'audit del blockdata ha confermato che il cartello è a `(77,3)`, mentre la
porta reale è a `(81,3)`, elevazione `3`. Il metatile esistente è `0x291` e usa
già `MB_NON_ANIMATED_DOOR`; il blockout aggiunge quindi soltanto il normale warp
event, senza alterare `LagoDiAlbera/map.bin`. Il ritorno interno è a `(15,29)`,
ultima riga utile del layout, elevazione `3`, su `MB_SOUTH_ARROW_WARP`.

## 5. Squadra della seconda recluta

| Ordine | Pokémon | Livello | Ruolo |
| ---: | --- | ---: | --- |
| 1 | Salampolla | 23 | apertura e controllo |
| 2 | Cisternide | 25 | asso, sempre ultimo |

Specie, ordine e livelli sono canonici per questa scena. Mosse, IV, oggetti e
IA restano **BALANCE PROVISIONAL**. Con i learnset correnti, i set naturali
candidati sono:

- Salampolla: Paralizzante, Velenoshock, Stordiraggio e Gigassorbimento;
- Cisternide: Acquanello, Forzantica, Acquadisale e Protezione.

L'implementazione deve assegnare un ID Allenatore univoco e append-only in
Emerald e FRLG. La classe e la grafica della recluta possono restare
segnaposto coerenti con la prima recluta fino all'approvazione dello sprite
definitivo.

## 6. Regole del primo Eco

### 6.1 Attivazione

Il primo Eco avviene una sola volta per tentativo di battaglia quando sono vere
tutte le condizioni:

- lotta singola contro l'ID della seconda recluta Aurea;
- Cisternide è il Pokémon che sta entrando in campo;
- l'evento non è già stato eseguito nella battaglia corrente.

Non dipende dalla mappa caricata, dall'orario, dal sesso del protagonista o
dalla specie del Pokémon attivo. La configurazione deve essere espressa da una
piccola tabella dati `Allenatore + specie bersaglio + effetto`, così altri Eco
potranno essere aggiunti senza nuovi casi speciali sparsi nel motore.

Il percorso narrativo previsto resta la sconfitta di Salampolla seguita
dall'ingresso dell'asso. Il controllo tecnico deve però osservare il primo
ingresso effettivo di Cisternide, così una mossa che forza il cambio non può
saltare l'Eco. Se Cisternide viene trascinato in campo durante un turno, l'Eco
avviene subito senza aprire un cambio fuori sequenza; il preludio dalla Poké
Ball resta riservato al normale rimpiazzo dopo un KO.

### 6.2 Effetto

Il Pokémon attivo del giocatore riceve:

- Attacco `+1` stadio;
- Attacco Speciale `+1` stadio.

L'effetto non si accumula nella stessa battaglia, rispetta il limite di `+6`,
si perde con il cambio e termina alla fine della lotta. Non concede una nuova
forma, una mossa, un'abilità o un aumento permanente.

L'Eco è un incremento narrativo positivo garantito: aggiunge direttamente uno
stadio a entrambe le statistiche e non viene invertito, raddoppiato o bloccato
da abilità come Inversione o Disinvoltura. La regola mantiene lo stesso
`+1/+1` per qualunque specie possa ricevere l'Eco.

### 6.3 Matrice dello starter

| Stato dello starter originale | Messaggio dalla Poké Ball | Domanda di cambio | Eco |
| --- | --- | --- | --- |
| Attivo e vivo | No | No domanda aggiuntiva | Sullo starter, con anomalia del riflesso |
| Vivo in panchina | Sì, con soprannome | Sì, normale domanda vanilla | Sul Pokémon attivo dopo la scelta |
| Esausto in panchina | No | Solo comportamento normale della modalità di lotta | Sul Pokémon attivo |
| Nel PC o alla Pensione | No | Solo comportamento normale | Sul Pokémon attivo |
| Assente o non identificabile | No | Solo comportamento normale | Sul Pokémon attivo |

In modalità **Cambio**, il messaggio speciale precede la domanda vanilla. In
modalità **Fissa**, quello stesso caso costituisce l'unica eccezione narrativa:
il gioco offre il normale cambio una volta perché è lo starter a chiederlo. Se
lo starter non è vivo in panchina, la modalità Fissa resta invariata.

Il rifiuto non penalizza il giocatore e non impedisce l'Eco. La meccanica
universale è l'Eco; il segnale di legame riservato allo starter anticipa la
Forma Riflesso.

### 6.4 Presentazione obbligatoria

Il primo Eco è un'informazione narrativa, quindi la sua animazione deve essere
mostrata anche quando le animazioni di lotta sono disattivate nelle opzioni.
Deve essere registrata come animazione generale dedicata, non simulata con una
serie di effetti di mossa.

La prima versione può riusare primitive grafiche già presenti per oscuramento,
increspatura e cambio statistiche. Una risorsa originale più complessa può
arrivare in un batch artistico successivo.

## 7. Identità dello starter originale

`VAR_STARTER_MON` conserva soltanto la specie scelta e non distingue
l'individuo ricevuto nel prologo. `IsStarterInParty` controlla soltanto la
specie base ed è insufficiente dopo un'evoluzione. Per la futura Forma Riflesso
serve invece riconoscere lo stesso individuo anche se evoluto, rinominato,
depositato o scambiato e poi restituito.

Il riconoscimento di Cingerm/Rovasco/Selvazanna,
Serbrace/Vipercen/Tossivampa e Ardeino/Velairone/Codairone usa una piccola
tabella delle tre linee associata a `VAR_STARTER_MON`. Non richiede modifiche a
`species_info.h`, alle evoluzioni o ai dati delle nove specie. Il confronto del
PID serve soltanto a distinguere l'individuo originale da un altro esemplare
della stessa linea.

### Nuove partite

Subito dopo `ScriptGiveMon` in `CB2_GiveStarter`:

1. si individua il Pokémon della linea scelta;
2. si legge `MON_DATA_PERSONALITY`;
3. il PID a 32 bit viene salvato in due variabili evento da 16 bit;
4. un flag separato dichiara valido il valore, perché un PID pari a zero è
   tecnicamente possibile.

Non si estendono `SaveBlock1` o `SaveBlock2`; i salvataggi restano compatibili.

### Salvataggi esistenti

Prima dell'evento dell'Emissario si esegue una migrazione conservativa che
esamina squadra, Box e Pensione. Un candidato valido deve:

- appartenere a uno dei tre stadi della linea indicata da `VAR_STARTER_MON`;
- avere l'ID Allenatore del giocatore;
- risultare incontrato al livello 5;
- essere l'unico candidato compatibile.

Se il candidato è unico, il PID viene registrato. Se è assente o ambiguo, il
gioco non deve indovinare: il messaggio dello starter viene omesso e il primo
Eco rimane pienamente funzionante sul Pokémon attivo.

Simboli proposti dopo l'audit del `HEAD` corrente:

| Simbolo | Slot candidato | Ruolo |
| --- | ---: | --- |
| `FLAG_ORIGINAL_STARTER_ID_REGISTERED` | `0x8FD` | validità del PID persistente |
| `VAR_ORIGINAL_STARTER_PERSONALITY_LO` | `0x40FB` | metà bassa del PID |
| `VAR_ORIGINAL_STARTER_PERSONALITY_HI` | `0x40FC` | metà alta del PID |

Questi slot sono liberi al momento dell'audit, ma non sono assegnati finché il
codice non viene implementato e validato anche nelle costanti FRLG.

## 8. Stato persistente e compatibilità narrativa

Simboli proposti:

| Simbolo | Slot candidato | Quando viene impostato |
| --- | ---: | --- |
| `FLAG_VIA_CONSOLARE_EMISSARIO_LEAD_COMPLETE` | `0x8FE` | conclusione della scena sul percorso |
| `FLAG_EMISSARIO_AUREA_ENCOUNTER_COMPLETE` | `0x8FF` | vittoria e conclusione della scena |
| `FLAG_FIRST_ECHO_SEEN` | `0x900` | post-lotta, insieme alla conclusione |
| flag di visibilità Emissario | da `0x901` | secondo i placement definitivi |

L'apertura non richiede un ulteriore flag: deriva da Medaglia Macina e Surf.
La continuità con la prima recluta usa
`FLAG_CISTERNONI_AUREA_ENCOUNTER_COMPLETE`.

Il flag del primo Eco non va scritto durante la battaglia. Viene impostato solo
nel seguito post-vittoria, così una sconfitta o un blackout permettono di
rivedere correttamente l'intera scena al tentativo successivo.

### Catch-up per i salvataggi post-Palestra

I salvataggi creati prima di questo contenuto possono possedere Medaglia Macina
e Surf senza il nuovo flag di Via Consolare. Non devono essere costretti a
tornare indietro.

All'ingresso dell'Emissario, se Medaglia e Surf sono presenti ma la scena di Via
Consolare manca, Lia fornisce una versione condensata:

> **LIA:** Ho seguito fin qui la pressione che convergeva dalla Via Consolare.
>
> Nico era alla Palestra; l'ho chiamato quando il flusso si è stabilizzato.

Poi il normale ricongiungimento prosegue e il flag viene normalizzato. Questo
recupero non altera i salvataggi che hanno già visto la scena completa.

## 9. Integrazione tecnica verificata

L'audit individua questi punti d'inserimento. Mappa, layout e warp indicati qui
sono presenti nel blockout; gli elementi narrativi restano per il futuro batch:

### Mappe e progressione

- `data/maps/ViaConsolare`: scena di separazione presso l'uscita nord;
- `data/maps/LagoDiAlbera`: warp implementato; Lia pre-Palestra,
  ricongiungimento e dialogo aggiornato di Lauro ancora da implementare;
- `data/maps/Emissario`: camera e ritorno al Lago implementati; scena Aurea
  ancora da implementare;
- `data/maps/map_groups.json`: append in `gMapGroup_Dungeons`;
- `data/layouts/layouts.json`: nuovo `LAYOUT_EMISSARIO`;
- `data/wild_encounters.json`: nessuna nuova tabella per la camera.

I validator attuali del Lago controllano quantità esatte di eventi e warp:
devono essere aggiornati deliberatamente insieme alla nuova connessione, non
aggirati.

### Battaglia

Il flusso vanilla pertinente è `BattleScript_FaintedMonTryChoose` in
`data/battle_scripts_1.s`:

1. il gioco sceglie il prossimo Pokémon avversario;
2. controlla modalità Fissa/Cambio;
3. mostra la domanda vanilla;
4. esegue l'eventuale cambio del giocatore;
5. manda in campo il nuovo avversario;
6. applica eventi ed effetti di entrata.

Un `callnative` prima del controllo Fissa può riconoscere l'evento e deviare al
breve preludio dello starter, quindi rientrare nella domanda esistente. Un
secondo `callnative`, dopo che entrambi i cambi sono risolti, attiva Eco e
buff sul Pokémon effettivamente in campo. Non serve un nuovo opcode.

Una variabile runtime in `BattleStruct` impedisce ripetizioni nello stesso
tentativo senza cambiare il formato del salvataggio.

L'animazione generale richiede:

- un nuovo ID in `include/constants/battle_anim.h`;
- registrazione nella tabella `sBattleAnims_General` di `src/battle_anim.c`;
- uno script grafico dedicato;
- inclusione nella whitelist di `PlayAnimation` in
  `src/battle_script_commands.c`, affinché non venga saltata con animazioni
  disattivate.

### Allenatore e testi

- aggiunta append-only dell'ID in `opponents.h` e `opponents_frlg.h`;
- blocco party parallelo in `trainers.party` e `trainers_frlg.party`;
- trainer slide `TRAINER_SLIDE_LAST_SWITCHIN` facoltativa per «Ora ascolta»;
- nickname dinamico preparato con i buffer mon già disponibili nelle stringhe
  di lotta.

## 10. Piano di validazione del futuro batch

### Validator strutturale dedicato

Il blockout usa `test/validate_emissario_structural_blockout.ps1` per verificare:

- registrazione append-only di mappa e layout;
- coppia di warp Lago/Emissario e comportamenti standard delle due uscite;
- forma circolare, anello percorribile, bacino Surf e sbocco nord;
- tileset locale e assenza di modifiche globali a Cisternoni;
- assenza di mappa subacquea, comportamento Dive e incontri selvatici;
- assenza deliberata di oggetti, trigger e script narrativi.

Il futuro `test/validate_emissario_first_echo.ps1` dovrà verificare almeno:

- oggetti, flag e gate post-Palestra;
- seconda recluta con Salampolla 23 e Cisternide 25 ultimo;
- continuità con il primo incontro Aurea;
- testi canonici `primo Eco` e assenza di `eco riflesso`;
- catch-up per salvataggi già post-Palestra.

Aggiornare inoltre i validator esistenti di Via Consolare e Lago che vincolano
eventi, warp o dialoghi interessati.

### Test della logica di lotta

Il futuro file di test deve coprire:

- modalità Cambio e modalità Fissa;
- starter attivo, vivo in panchina, esausto, depositato e assente;
- starter evoluto e soprannominato;
- accettazione, rifiuto e annullamento del menu;
- Echo sullo starter e su un Pokémon generico;
- `+1` Attacco e `+1` Attacco Speciale con cap a `+6`;
- incremento invariato con Inversione e Disinvoltura;
- una sola attivazione per battaglia;
- ingresso forzato anticipato di Cisternide;
- animazioni di lotta disattivate;
- sconfitta e nuovo tentativo;
- migrazione univoca e caso ambiguo di un vecchio salvataggio.

I prompt interattivi e il timing visivo richiedono comunque playtest manuale
oltre ai test automatici.

### Controlli finali

Eseguire parser JSON, validator Cisternoni/Via Consolare/Lago/Emissario,
test battle mirati, build Emerald/FRLG, `git diff --check` e un playtest da:

- salvataggio precedente alla scena di Via Consolare;
- salvataggio post-Palestra già esistente;
- nuova partita con ciascuna delle tre linee starter.

## 11. Fuori ambito

L'implementazione corrente non include:

- implementazione della Forma Riflesso completa;
- cambio di sprite, specie, abilità o statistiche base dello starter;
- percorso Dive o mappa subacquea verso il Lago di Nemi;
- incontri selvatici nell'Emissario;
- dungeon del bosco o cattura degli altri due starter;
- Borgo di Castello, Villa Papale, Ponte di Laricia o Conca di Laricia;
- nuovi tileset globali o casi speciali dipendenti da una singola mappa.

Il prossimo passo è il playtest del **blockout strutturale dell'Emissario** con
andata, ritorno, anello e Surf. La scena Aurea e il primo Eco devono arrivare in
un secondo batch separato, dopo che la camera è confermata percorribile.
