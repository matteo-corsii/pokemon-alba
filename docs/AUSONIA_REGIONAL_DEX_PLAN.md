# Ausonia Regional Dex Plan

Questo documento cataloga le specie originali già implementate e i concetti
proposti per la fauna di Ausonia. Salvo indicazione esplicita, nomi, tipi,
linee evolutive, habitat e collocazioni delle specie non implementate sono
proposte di design e non costituiscono canone definitivo.

Il catalogo è documentale: non assegna dati di gioco, numeri regionali o
collocazioni definitive e non rende ottenibile alcuna nuova specie.

## Status legend

- **IMPLEMENTED:** specie presente nel gioco e approvata.
- **CANONICAL DESIGN:** decisione approvata ma non necessariamente
  implementata.
- **CANDIDATE:** concetto da valutare.
- **NAME REVIEW:** nome da rivedere.
- **LINE REVIEW:** struttura evolutiva da decidere.
- **TYPE REVIEW:** typing da riequilibrare.
- **PLACEMENT TBD:** collocazione non approvata.
- **POSSIBLE CUT:** concetto conservato ma potenzialmente eliminabile.

Tutte le specie diverse dai nove starter partono come proposte. La presenza in
questo catalogo non le promuove automaticamente a canone.
Nessun concetto è marcato **POSSIBLE CUT** in questa prima ricognizione, perché
le fonti non indicano quale eliminare; lo stato resta disponibile per una
futura decisione esplicita di review.

## Verified implementation sources

L'audit dei sorgenti ha verificato:

- identificatori in `include/constants/species.h`;
- numeri Pokédex tecnici in `include/constants/pokedex.h`;
- tipi, statistiche, abilità, evoluzioni, gruppi Uova, testi Pokédex e
  collegamenti ai learnset in `src/data/pokemon/species_info.h`;
- ordinamenti tecnici in `src/data/pokemon/pokedex_orders.h`;
- vincoli e test delle nove specie in `test/species.c`;
- stato funzionale e disponibilità in
  `docs/AUSONIA_STARTERS_IMPLEMENTATION.md`;
- stato grafico in `docs/AUSONIA_STARTER_SPRITE_PIPELINE.md`.

Le tre linee hanno totali statistiche 310, 405 e 530, crescita
Medio-Lenta, rapporto sessi 50/50 e liste MT/tutor/Uovo ancora vuote. I dati
dettagliati e i learnset non vengono duplicati qui. Non sono emerse divergenze
fra documentazione e sorgenti; **Vipercen** è la grafia implementata corretta.

## Count and numbering reconciliation

Il materiale iniziale usa una numerazione concettuale non sempre equivalente
al numero di specie distinte:

- Ghepio → Tinuncol è una famiglia proposta, ma contiene due specie nominate;
- Brinix e Glaciterno sono due concetti separati e potrebbero formare una linea,
  senza che il legame sia approvato;
- Porchemecha o Bronzoverro è un solo concetto con due nomi alternativi;
- le altre specie singole potrebbero ricevere stadi evolutivi futuri.

Il catalogo censisce **24 famiglie o concetti**. Contiene **34 etichette
nominali**, che corrispondono ad almeno **33 specie distinte nominate**:
Porchemecha/Bronzoverro conta una volta, mentre le due forme di ciascuna linea
esplicita contano separatamente. Il totale finale resta **TBD** perché linee
come Brinix/Glaciterno non sono deliberate, diversi concetti singoli possono
ottenere evoluzioni e l'ecosistema regionale presenta ancora lacune. Non sono
assegnati numeri Pokédex regionali definitivi.

## Implemented starter families

### Cingerm → Rovasco → Selvazanna

- **Temporary family ID:** `AUS-FAM-STARTER-1`
- **Species/stages:** Cingerm (base) → Rovasco (stadio 1) → Selvazanna
  (stadio 2).
- **Current status:** IMPLEMENTED; CANONICAL DESIGN per linea e nomi.
- **Proposed typing:** non proposto: typing implementato, riportato nella
  tabella seguente.
- **Core concept:** starter Erba di Ausonia.
- **Real-world inspiration:** cinghiale, germoglio, rovi e bosco, come
  registrato dai dati e dai testi esistenti.
- **Evolution structure:** canonica e implementata ai livelli 16 e 36.
- **Primary habitat candidate:** PLACEMENT TBD; non presente negli incontri
  selvatici.
- **Secondary habitat candidates:** TBD.
- **Earliest story band candidate:** Laboratorio del Cratere, scelta starter
  Emerald al livello 5; implementato.
- **Gameplay role candidate:** starter fisico; il bilanciamento dettagliato
  resta soggetto ai test.
- **Narrative importance:** una delle tre scelte iniziali; Nico e Lia ricevono
  le altre specie secondo il flusso implementato.
- **Name review:** nessuna revisione aperta registrata.
- **Type review:** nessuna revisione aperta registrata.
- **Placement review:** eventuale disponibilità fuori dalla scelta iniziale è
  TBD.
- **Open questions:** disponibilità futura, shiny definitive, cry, footprint e
  overworld originali.
- **Implementation references:** `include/constants/species.h`,
  `include/constants/pokedex.h`, `src/data/pokemon/species_info.h`,
  `src/data/pokemon/pokedex_orders.h`, `src/starter_choose.c`,
  `test/species.c`, `docs/AUSONIA_STARTERS_IMPLEMENTATION.md`.

| Species | Technical species ID | Technical Pokédex number | Implemented type(s) | Evolution |
| --- | ---: | ---: | --- | --- |
| Cingerm | `SPECIES_CINGERM` = 1573 | `NATIONAL_DEX_CINGERM` = 1026 | Erba | livello 16 → Rovasco |
| Rovasco | `SPECIES_ROVASCO` = 1574 | `NATIONAL_DEX_ROVASCO` = 1027 | Erba | livello 36 → Selvazanna |
| Selvazanna | `SPECIES_SELVAZANNA` = 1575 | `NATIONAL_DEX_SELVAZANNA` = 1028 | Erba/Buio | stadio finale |

### Serbrace → Vipercen → Tossivampa

- **Temporary family ID:** `AUS-FAM-STARTER-2`
- **Species/stages:** Serbrace (base) → Vipercen (stadio 1) → Tossivampa
  (stadio 2).
- **Current status:** IMPLEMENTED; CANONICAL DESIGN per linea e nomi.
- **Proposed typing:** non proposto: typing implementato, riportato nella
  tabella seguente.
- **Core concept:** starter Fuoco con sviluppo vulcanico e velenoso.
- **Real-world inspiration:** serpente, brace, cenere, fumarole e minerali,
  come registrato dai dati e dai testi esistenti.
- **Evolution structure:** canonica e implementata ai livelli 16 e 36.
- **Primary habitat candidate:** PLACEMENT TBD; non presente negli incontri
  selvatici.
- **Secondary habitat candidates:** TBD.
- **Earliest story band candidate:** Laboratorio del Cratere, scelta starter
  Emerald al livello 5; implementato.
- **Gameplay role candidate:** starter speciale rapido; il bilanciamento
  dettagliato resta soggetto ai test.
- **Narrative importance:** una delle tre scelte iniziali.
- **Name review:** nessuna revisione aperta; Vipercen è la grafia corretta.
- **Type review:** nessuna revisione aperta registrata.
- **Placement review:** eventuale disponibilità fuori dalla scelta iniziale è
  TBD.
- **Open questions:** disponibilità futura, shiny definitive, cry, footprint e
  overworld originali.
- **Implementation references:** `include/constants/species.h`,
  `include/constants/pokedex.h`, `src/data/pokemon/species_info.h`,
  `src/data/pokemon/pokedex_orders.h`, `src/starter_choose.c`,
  `test/species.c`, `docs/AUSONIA_STARTERS_IMPLEMENTATION.md`.

| Species | Technical species ID | Technical Pokédex number | Implemented type(s) | Evolution |
| --- | ---: | ---: | --- | --- |
| Serbrace | `SPECIES_SERBRACE` = 1576 | `NATIONAL_DEX_SERBRACE` = 1029 | Fuoco | livello 16 → Vipercen |
| Vipercen | `SPECIES_VIPERCEN` = 1577 | `NATIONAL_DEX_VIPERCEN` = 1030 | Fuoco | livello 36 → Tossivampa |
| Tossivampa | `SPECIES_TOSSIVAMPA` = 1578 | `NATIONAL_DEX_TOSSIVAMPA` = 1031 | Fuoco/Veleno | stadio finale |

### Ardeino → Velairone → Codairone

- **Temporary family ID:** `AUS-FAM-STARTER-3`
- **Species/stages:** Ardeino (base) → Velairone (stadio 1) → Codairone
  (stadio 2).
- **Current status:** IMPLEMENTED; CANONICAL DESIGN per linea e nomi. La concept art
  definitiva di Codairone resta aperta.
- **Proposed typing:** non proposto: typing implementato, riportato nella
  tabella seguente.
- **Core concept:** starter Acqua aviario legato alle acque calme e ai
  riflessi.
- **Real-world inspiration:** ardeidi, piume, correnti e lago, come registrato
  dai dati e dai testi esistenti.
- **Evolution structure:** canonica e implementata ai livelli 16 e 36.
- **Primary habitat candidate:** PLACEMENT TBD; non presente negli incontri
  selvatici.
- **Secondary habitat candidates:** TBD.
- **Earliest story band candidate:** Laboratorio del Cratere, scelta starter
  Emerald al livello 5; implementato.
- **Gameplay role candidate:** starter speciale resistente; il bilanciamento
  dettagliato resta soggetto ai test.
- **Narrative importance:** una delle tre scelte iniziali.
- **Name review:** nessuna revisione aperta registrata.
- **Type review:** nessuna revisione aperta registrata.
- **Placement review:** eventuale disponibilità fuori dalla scelta iniziale è
  TBD.
- **Open questions:** concept definitiva di Codairone, disponibilità futura,
  shiny definitive, cry, footprint e overworld originali.
- **Implementation references:** `include/constants/species.h`,
  `include/constants/pokedex.h`, `src/data/pokemon/species_info.h`,
  `src/data/pokemon/pokedex_orders.h`, `src/starter_choose.c`,
  `test/species.c`, `docs/AUSONIA_STARTERS_IMPLEMENTATION.md`.

| Species | Technical species ID | Technical Pokédex number | Implemented type(s) | Evolution |
| --- | ---: | ---: | --- | --- |
| Ardeino | `SPECIES_ARDEINO` = 1579 | `NATIONAL_DEX_ARDEINO` = 1032 | Acqua | livello 16 → Velairone |
| Velairone | `SPECIES_VELAIRONE` = 1580 | `NATIONAL_DEX_VELAIRONE` = 1033 | Acqua | livello 36 → Codairone |
| Codairone | `SPECIES_CODAIRONE` = 1581 | `NATIONAL_DEX_CODAIRONE` = 1034 | Acqua/Volante | stadio finale |

## Candidate fossil families

### Eleby → Antiquas

- **Temporary family ID:** `AUS-FAM-FOSSIL-ELEPHANT`
- **Species/stages:** Eleby → Antiquas.
- **Current status:** CANDIDATE; Eleby è anche NAME REVIEW.
- **Proposed typing:** Antiquas Roccia/Psico; typing di Eleby TBD.
- **Core concept:** elefante antico.
- **Real-world inspiration:** *Elephas antiquus*.
- **Evolution structure:** linea a due stadi proposta; metodo TBD.
- **Primary habitat candidate:** PLACEMENT TBD; possibile ottenimento fossile,
  non approvato.
- **Secondary habitat candidates:** museo o ricerca paleontologica, TBD.
- **Earliest story band candidate:** premature/TBD.
- **Gameplay role candidate:** fossile regionale, ruolo da definire.
- **Narrative importance:** TBD.
- **Name review:** Eleby da rivedere; Antiquas non è approvato.
- **Type review:** typing di base e bilanciamento della forma evoluta TBD.
- **Placement review:** PLACEMENT TBD.
- **Open questions:** nomi, tipo base, metodo di ottenimento/evoluzione,
  statistiche e collocazione.

### Potamy → Plumbopotam

- **Temporary family ID:** `AUS-FAM-FOSSIL-HIPPO`
- **Species/stages:** Potamy → Plumbopotam.
- **Current status:** CANDIDATE, NAME REVIEW, PLACEMENT TBD.
- **Proposed typing:** Plumbopotam Roccia/Acciaio; typing di Potamy TBD.
- **Core concept:** ippopotamo antico legato al piombo e alla metallurgia
  romana.
- **Real-world inspiration:** ippopotami antichi e lavorazione romana dei
  metalli.
- **Evolution structure:** linea a due stadi proposta; metodo TBD.
- **Primary habitat candidate:** PLACEMENT TBD; possibile ottenimento fossile,
  non approvato.
- **Secondary habitat candidates:** museo, archivio o sito metallurgico, TBD.
- **Earliest story band candidate:** premature/TBD.
- **Gameplay role candidate:** fossile regionale resistente, da definire.
- **Narrative importance:** TBD.
- **Name review:** entrambi i nomi restano candidati.
- **Type review:** typing di base e bilanciamento Roccia/Acciaio TBD.
- **Placement review:** PLACEMENT TBD.
- **Open questions:** nomi, tipo base, metodo di ottenimento/evoluzione,
  statistiche e collocazione.

## Other candidate concepts

### Ghepio → Tinuncol

- **Temporary family ID:** `AUS-FAM-FALCON`
- **Species/stages:** Ghepio → Tinuncol.
- **Current status:** CANDIDATE, LINE REVIEW, PLACEMENT TBD.
- **Proposed typing:** Volante.
- **Core concept:** falco dei borghi e possibile volatile regionale.
- **Real-world inspiration:** gheppio e *Falco tinnunculus*.
- **Evolution structure:** linea a due stadi proposta.
- **Primary habitat candidate:** borghi e margini urbani, da approvare.
- **Secondary habitat candidates:** campagne e alture, TBD.
- **Earliest story band candidate:** possible candidate nelle prime aree.
- **Gameplay role candidate:** volatile regionale comune.
- **Narrative importance:** ordinaria; nessun ruolo narrativo approvato.
- **Name review:** entrambi i nomi restano proposti.
- **Type review:** tipo secondario non deciso.
- **Placement review:** PLACEMENT TBD.
- **Open questions:** nomi, metodo evolutivo, secondo tipo e distribuzione.

### Omphalux

- **Temporary family ID:** `AUS-CONCEPT-OMPHALUX`
- **Species/stages:** Omphalux, singolo stadio provvisorio.
- **Current status:** CANDIDATE, LINE REVIEW, PLACEMENT TBD.
- **Proposed typing:** Erba/Elettro.
- **Core concept:** fungo dell'olivo notturno o bioluminescente.
- **Real-world inspiration:** *Omphalotus* e *lux*.
- **Evolution structure:** TBD; nessuna linea approvata.
- **Primary habitat candidate:** oliveti, possible candidate.
- **Secondary habitat candidates:** boschi notturni, TBD.
- **Earliest story band candidate:** premature/TBD.
- **Gameplay role candidate:** incontro notturno o specie a illuminazione.
- **Narrative importance:** nessuna definita.
- **Name review:** nome candidato, non canonico.
- **Type review:** combinazione proposta da validare.
- **Placement review:** PLACEMENT TBD.
- **Open questions:** linea, meccanica notturna, abilità e distribuzione.

### Luscerp

- **Temporary family ID:** `AUS-CONCEPT-LUSCERP`
- **Species/stages:** Luscerp, stadio non definito.
- **Current status:** CANDIDATE, LINE REVIEW, TYPE REVIEW, PLACEMENT TBD.
- **Proposed typing:** Erba/Drago.
- **Core concept:** luscengola vegetale.
- **Real-world inspiration:** luscengola.
- **Evolution structure:** TBD; Drago potrebbe comparire solo dopo una futura
  evoluzione.
- **Primary habitat candidate:** aree erbose o pietrose, da approvare.
- **Secondary habitat candidates:** campagne e margini boschivi, TBD.
- **Earliest story band candidate:** premature/TBD.
- **Gameplay role candidate:** rettile Erba; ruolo da definire.
- **Narrative importance:** nessuna definita.
- **Name review:** nome candidato, non canonico.
- **Type review:** decidere se e quando assegnare Drago.
- **Placement review:** PLACEMENT TBD.
- **Open questions:** linea evolutiva, typing per stadio e habitat.

### Fragmagma

- **Temporary family ID:** `AUS-CONCEPT-FRAGMAGMA`
- **Species/stages:** Fragmagma, singolo stadio provvisorio.
- **Current status:** CANDIDATE, LINE REVIEW, PLACEMENT TBD.
- **Proposed typing:** Folletto/Fuoco.
- **Core concept:** fragola/vulcanetto.
- **Real-world inspiration:** coltivazioni di fragole e paesaggio vulcanico.
- **Evolution structure:** TBD.
- **Primary habitat candidate:** futura area ispirata a Genzano o Nemi,
  possible candidate.
- **Secondary habitat candidates:** coltivazioni su suolo vulcanico, TBD.
- **Earliest story band candidate:** dopo il Lago di Albèra, premature/TBD.
- **Gameplay role candidate:** specie tematica locale; ruolo da definire.
- **Narrative importance:** nessuna definita.
- **Name review:** nome candidato, non canonico.
- **Type review:** combinazione proposta da validare.
- **Placement review:** PLACEMENT TBD; nessuna area in-game è nominata.
- **Open questions:** linea, area fra Genzano/Nemi e bilanciamento del tipo.

### Solftraver

- **Temporary family ID:** `AUS-CONCEPT-SOLFTRAVER`
- **Species/stages:** Solftraver, singolo stadio provvisorio.
- **Current status:** CANDIDATE, LINE REVIEW, PLACEMENT TBD.
- **Proposed typing:** Roccia/Veleno.
- **Core concept:** solfatara e travertino.
- **Real-world inspiration:** depositi minerali, solfatare e travertino.
- **Evolution structure:** TBD.
- **Primary habitat candidate:** area minerale o vulcanica, possible candidate.
- **Secondary habitat candidates:** cave o sorgenti minerali, TBD.
- **Earliest story band candidate:** premature/TBD.
- **Gameplay role candidate:** specie minerale resistente, da definire.
- **Narrative importance:** nessuna definita.
- **Name review:** nome candidato, non canonico.
- **Type review:** combinazione proposta da validare.
- **Placement review:** PLACEMENT TBD.
- **Open questions:** linea, habitat, rarità e relazione con le sorgenti.

### Tricrest

- **Temporary family ID:** `AUS-CONCEPT-TRICREST`
- **Species/stages:** Tricrest, stadio non definito.
- **Current status:** CANDIDATE, LINE REVIEW, TYPE REVIEW, PLACEMENT TBD.
- **Proposed typing:** Acqua/Drago.
- **Core concept:** tritone crestato.
- **Real-world inspiration:** tritone crestato e zone umide.
- **Evolution structure:** TBD.
- **Primary habitat candidate:** ambiente lacustre o zona umida, da approvare.
- **Secondary habitat candidates:** sorgenti o stagni, TBD.
- **Earliest story band candidate:** premature/TBD.
- **Gameplay role candidate:** specie acquatica rara o intermedia, da definire.
- **Narrative importance:** nessuna definita.
- **Name review:** nome candidato, non canonico.
- **Type review:** decidere se Drago sia appropriato e a quale stadio.
- **Placement review:** PLACEMENT TBD; non è assegnato automaticamente al Lago
  di Albèra.
- **Open questions:** linea, typing, frequenza e habitat definitivo.

### Porchemecha / Bronzoverro

- **Temporary family ID:** `AUS-CONCEPT-FURNACE-BOAR`
- **Species/stages:** un solo concetto; Porchemecha e Bronzoverro sono nomi
  alternativi, non due specie.
- **Current status:** CANDIDATE, NAME REVIEW, LINE REVIEW, PLACEMENT TBD.
- **Proposed typing:** Acciaio/Fuoco.
- **Core concept:** cinghiale-fornace.
- **Real-world inspiration:** fornaci, bronzo e cinghiale.
- **Evolution structure:** TBD.
- **Primary habitat candidate:** area metallurgica o vulcanica, da approvare.
- **Secondary habitat candidates:** cave o rovine produttive, TBD.
- **Earliest story band candidate:** premature/TBD.
- **Gameplay role candidate:** attaccante fisico resistente, da definire.
- **Narrative importance:** nessuna definita.
- **Name review:** scegliere fra Porchemecha e Bronzoverro o sostituirli.
- **Type review:** combinazione proposta da validare.
- **Placement review:** PLACEMENT TBD.
- **Open questions:** nome, struttura evolutiva, habitat e rarità.

### Saladoct

- **Temporary family ID:** `AUS-CONCEPT-SALADOCT`
- **Species/stages:** Saladoct, singolo stadio provvisorio.
- **Current status:** CANDIDATE, NAME REVIEW, LINE REVIEW, PLACEMENT TBD.
- **Proposed typing:** Veleno/Folletto.
- **Core concept:** salamandrina alchemica.
- **Real-world inspiration:** salamandrina e immaginario alchemico.
- **Evolution structure:** TBD.
- **Primary habitat candidate:** zona umida o sito storico-alchemico, da
  approvare.
- **Secondary habitat candidates:** boschi umidi, cisterne o rovine, TBD.
- **Earliest story band candidate:** premature/TBD.
- **Gameplay role candidate:** specie tecnica basata su stato, da definire.
- **Narrative importance:** TBD.
- **Name review:** obbligatoria.
- **Type review:** combinazione proposta da validare.
- **Placement review:** PLACEMENT TBD.
- **Open questions:** nome, linea, tono alchemico e collocazione.

### Naufragus

- **Temporary family ID:** `AUS-CONCEPT-NAUFRAGUS`
- **Species/stages:** Naufragus, singolo stadio provvisorio.
- **Current status:** CANDIDATE, LINE REVIEW, PLACEMENT TBD.
- **Proposed typing:** Acqua/Acciaio.
- **Core concept:** relitto legato alla memoria di Caligola.
- **Real-world inspiration:** navi, relitti e memoria storica di Caligola.
- **Evolution structure:** TBD.
- **Primary habitat candidate:** sito storico o acquatico da definire; non è
  collocato automaticamente nel Lago di Albèra.
- **Secondary habitat candidates:** museo o arco narrativo, TBD.
- **Earliest story band candidate:** premature/TBD.
- **Gameplay role candidate:** specie rara o narrativa.
- **Narrative importance:** possibile, ma non approvata.
- **Name review:** nome candidato, non canonico.
- **Type review:** combinazione proposta da validare.
- **Placement review:** PLACEMENT TBD, con divieto di assumere il Lago di
  Albèra come collocazione.
- **Open questions:** natura del relitto, rarità, linea e ruolo narrativo.

### Molospsy

- **Temporary family ID:** `AUS-CONCEPT-MOLOSPSY`
- **Species/stages:** Molospsy, singolo stadio provvisorio.
- **Current status:** CANDIDATE, NAME REVIEW, LINE REVIEW, PLACEMENT TBD.
- **Proposed typing:** Lotta/Psico.
- **Core concept:** molosso guardiano del tempio.
- **Real-world inspiration:** molossi e complessi sacri.
- **Evolution structure:** TBD.
- **Primary habitat candidate:** rovine o complesso sacro, possible candidate.
- **Secondary habitat candidates:** siti monumentali, TBD.
- **Earliest story band candidate:** premature/TBD.
- **Gameplay role candidate:** guardiano fisico/mentale, da definire.
- **Narrative importance:** possibile custode, non approvato.
- **Name review:** obbligatoria.
- **Type review:** combinazione proposta da validare.
- **Placement review:** PLACEMENT TBD.
- **Open questions:** nome, linea, sito e importanza narrativa.

### Lenghelis

- **Temporary family ID:** `AUS-CONCEPT-LENGHELIS`
- **Species/stages:** Lenghelis, singolo stadio provvisorio.
- **Current status:** CANDIDATE, LINE REVIEW, PLACEMENT TBD.
- **Proposed typing:** Spettro/Folletto.
- **Core concept:** Lénghelo del folklore locale.
- **Real-world inspiration:** folklore del Lénghelo.
- **Evolution structure:** TBD.
- **Primary habitat candidate:** bosco o area notturna, possible candidate.
- **Secondary habitat candidates:** borghi e case antiche, TBD.
- **Earliest story band candidate:** premature/TBD.
- **Gameplay role candidate:** incontro notturno basato su disturbo, da
  definire.
- **Narrative importance:** possibile episodio folklorico, non approvato.
- **Name review:** nome candidato, non canonico.
- **Type review:** combinazione proposta da validare.
- **Placement review:** PLACEMENT TBD.
- **Open questions:** linea, frequenza, tono folklorico e habitat.

### Talpmagnet

- **Temporary family ID:** `AUS-CONCEPT-TALPMAGNET`
- **Species/stages:** Talpmagnet, stadio non definito.
- **Current status:** CANDIDATE, NAME REVIEW, LINE REVIEW, PLACEMENT TBD.
- **Proposed typing:** Terra/Elettro.
- **Core concept:** talpa vulcanica e magnetica.
- **Real-world inspiration:** talpe, sottosuolo vulcanico e magnetismo.
- **Evolution structure:** TBD.
- **Primary habitat candidate:** sottosuolo o area vulcanica, possible
  candidate.
- **Secondary habitat candidates:** cave, tunnel o cisterne, TBD.
- **Earliest story band candidate:** premature/TBD.
- **Gameplay role candidate:** scavatore con utilità elettrica, da definire.
- **Narrative importance:** nessuna definita.
- **Name review:** obbligatoria.
- **Type review:** combinazione proposta da validare.
- **Placement review:** PLACEMENT TBD.
- **Open questions:** nome, linea, origine magnetica e distribuzione.

### Vitemosto

- **Temporary family ID:** `AUS-CONCEPT-VITEMOSTO`
- **Species/stages:** Vitemosto, stadio non definito.
- **Current status:** CANDIDATE, LINE REVIEW, PLACEMENT TBD.
- **Proposed typing:** Erba/Fuoco.
- **Core concept:** vite e mosto in fermentazione.
- **Real-world inspiration:** viticoltura e fermentazione.
- **Evolution structure:** TBD.
- **Primary habitat candidate:** area rurale o vitivinicola, possible candidate.
- **Secondary habitat candidates:** campagne e cantine, TBD.
- **Earliest story band candidate:** premature/TBD.
- **Gameplay role candidate:** specie rurale legata a fermentazione o stato,
  da definire.
- **Narrative importance:** nessuna definita.
- **Name review:** nome candidato, non canonico.
- **Type review:** combinazione proposta da validare.
- **Placement review:** PLACEMENT TBD.
- **Open questions:** linea, resa del tema Fuoco e collocazione.

### Glicidra

- **Temporary family ID:** `AUS-CONCEPT-GLICIDRA`
- **Species/stages:** Glicidra, singolo stadio provvisorio.
- **Current status:** CANDIDATE, TYPE REVIEW, PLACEMENT TBD.
- **Proposed typing:** Drago/Veleno.
- **Core concept:** drago delle ville ispirato al glicine.
- **Real-world inspiration:** glicine e ville storiche.
- **Evolution structure:** TBD.
- **Primary habitat candidate:** ville o giardini storici, possible candidate.
- **Secondary habitat candidates:** parchi monumentali, TBD.
- **Earliest story band candidate:** premature/TBD.
- **Gameplay role candidate:** specie rara.
- **Narrative importance:** possibile, ma non approvata.
- **Name review:** nome candidato, non canonico.
- **Type review:** Drago/Veleno richiede revisione di rarità e bilanciamento.
- **Placement review:** PLACEMENT TBD.
- **Open questions:** linea, rarità, accesso e rapporto col glicine.

### Fraschietto

- **Temporary family ID:** `AUS-CONCEPT-FRASCHIETTO`
- **Species/stages:** Fraschietto, stadio non definito.
- **Current status:** CANDIDATE, LINE REVIEW, PLACEMENT TBD.
- **Proposed typing:** Lotta/Erba.
- **Core concept:** guerriero della fraschetta.
- **Real-world inspiration:** fraschette e cultura locale.
- **Evolution structure:** TBD.
- **Primary habitat candidate:** futura area ispirata ad Ariccia, possible
  candidate.
- **Secondary habitat candidates:** campagne o borghi, TBD.
- **Earliest story band candidate:** dopo il Lago di Albèra, premature/TBD.
- **Gameplay role candidate:** combattente fisico, da definire.
- **Narrative importance:** nessuna definita.
- **Name review:** nome candidato, non canonico.
- **Type review:** combinazione proposta da validare.
- **Placement review:** PLACEMENT TBD; nessun nome in-game è approvato.
- **Open questions:** linea, tono del riferimento culturale e collocazione.

### Brinix

- **Temporary family ID:** `AUS-CONCEPT-BRINIX`
- **Species/stages:** Brinix, concetto distinto; possibile base di Glaciterno.
- **Current status:** CANDIDATE, LINE REVIEW, PLACEMENT TBD.
- **Proposed typing:** Ghiaccio.
- **Core concept:** piccola niviera.
- **Real-world inspiration:** neviere tradizionali.
- **Evolution structure:** possibile evoluzione in Glaciterno, non approvata.
- **Primary habitat candidate:** area montana o niviera, da approvare.
- **Secondary habitat candidates:** futura area ispirata a Monte Cavo, possible
  candidate.
- **Earliest story band candidate:** premature/TBD.
- **Gameplay role candidate:** specie Ghiaccio accessibile, da definire.
- **Narrative importance:** nessuna definita.
- **Name review:** nome candidato, non canonico.
- **Type review:** typing proposto da validare.
- **Placement review:** PLACEMENT TBD.
- **Open questions:** legame con Glaciterno, metodo evolutivo e area.

### Glaciterno

- **Temporary family ID:** `AUS-CONCEPT-GLACITERNO`
- **Species/stages:** Glaciterno, concetto distinto; possibile evoluzione di
  Brinix.
- **Current status:** CANDIDATE, LINE REVIEW, PLACEMENT TBD.
- **Proposed typing:** Ghiaccio/Terra.
- **Core concept:** spirito della niviera.
- **Real-world inspiration:** neviere, ghiaccio conservato e terreno montano.
- **Evolution structure:** possibile evoluzione di Brinix, non approvata.
- **Primary habitat candidate:** niviera o area montana, da approvare.
- **Secondary habitat candidates:** futura area ispirata a Monte Cavo, possible
  candidate.
- **Earliest story band candidate:** premature/TBD.
- **Gameplay role candidate:** specie Ghiaccio/Terra resistente, da definire.
- **Narrative importance:** possibile presenza folklorica, non approvata.
- **Name review:** nome candidato, non canonico.
- **Type review:** combinazione proposta da validare.
- **Placement review:** PLACEMENT TBD.
- **Open questions:** legame con Brinix, metodo evolutivo, rarità e area.

### Boletushield

- **Temporary family ID:** `AUS-CONCEPT-BOLETUSHIELD`
- **Species/stages:** Boletushield, singolo stadio provvisorio.
- **Current status:** CANDIDATE, NAME REVIEW, LINE REVIEW, PLACEMENT TBD.
- **Proposed typing:** Terra/Acciaio.
- **Core concept:** porcino difensivo di Lariano.
- **Real-world inspiration:** porcini e futura area ispirata a Lariano.
- **Evolution structure:** TBD.
- **Primary habitat candidate:** futura area ispirata a Lariano, strong
  thematic candidate ma non approvata.
- **Secondary habitat candidates:** boschi, TBD.
- **Earliest story band candidate:** premature/TBD.
- **Gameplay role candidate:** difensore fisico, da definire.
- **Narrative importance:** nessuna definita.
- **Name review:** rivedere l'inglese “shield”.
- **Type review:** combinazione proposta da validare.
- **Placement review:** PLACEMENT TBD; nessun nome in-game è approvato.
- **Open questions:** nuovo nome, linea, composizione del corpo e distribuzione.

### Marmorequus

- **Temporary family ID:** `AUS-CONCEPT-MARMOREQUUS`
- **Species/stages:** Marmorequus, singolo stadio provvisorio.
- **Current status:** CANDIDATE, LINE REVIEW, PLACEMENT TBD.
- **Proposed typing:** Normale/Acciaio.
- **Core concept:** destriero monumentale del Regillo.
- **Real-world inspiration:** Lago Regillo, cavalli e scultura in marmo.
- **Evolution structure:** TBD.
- **Primary habitat candidate:** futura area ispirata al Regillo, strong
  thematic candidate ma non approvata.
- **Secondary habitat candidates:** rovine o sito monumentale, TBD.
- **Earliest story band candidate:** premature/TBD.
- **Gameplay role candidate:** specie rara, monumentale o legata a rovine.
- **Narrative importance:** possibile, ma non approvata.
- **Name review:** nome candidato, non canonico.
- **Type review:** combinazione proposta da validare.
- **Placement review:** PLACEMENT TBD; nessun nome in-game è approvato.
- **Open questions:** linea, rarità, accesso e ruolo monumentale.

## Geographic and story bands

La sequenza canonica iniziale è:

1. Albèra Bassa;
2. Via Verdi;
3. Porta Pretoria;
4. Albèra Storica;
5. Anfiteatro Romano / prima Palestra;
6. Via Consolare;
7. Lago di Albèra.

Il Lago di Albèra è un arco importante e precede le aree ispirate ad Ariccia.
Le future aree ispirate ad Ariccia, Genzano, Lago di Nemi, Monte Cavo, Lariano
e Regillo non hanno ancora nomi in-game definitivi. Nomi provvisori come
Pontaria, Fioralia o Nemora non vengono resi canonici da questo documento.

| Story band | Candidate species/families | Confidence | Open dependencies |
| --- | --- | --- | --- |
| Albèra Bassa | nessuna nuova assegnazione | premature/TBD | ruolo della fauna urbana e riuso di Pokémon ufficiali |
| Via Verdi | Ghepio/Tinuncol; possibili specie ufficiali | possible candidate | fauna della route, layout, curve dei livelli e approvazione separata |
| Porta Pretoria | Molospsy | premature/TBD | funzione delle rovine e spazio della soglia |
| Albèra Storica | Molospsy; Lenghelis | possible candidate | blockout, ciclo giorno/notte e tono degli eventi |
| Anfiteatro Romano / prima Palestra | Molospsy | premature/TBD | tipo della Palestra e funzione narrativa non decisi |
| Via Consolare | Ghepio/Tinuncol; Luscerp | possible candidate | bioma e composizione della route non progettati |
| Lago di Albèra | Tricrest; Naufragus | premature/TBD | fauna lacustre non approvata; Naufragus non va collocato automaticamente qui |
| Futura area ispirata ad Ariccia | Fraschietto | strong thematic candidate | nome in-game, bioma e linea evolutiva |
| Futura area ispirata a Genzano o Lago di Nemi | Fragmagma | strong thematic candidate | scelta dell'area, nome in-game e bilanciamento |
| Futura area ispirata a Monte Cavo | Brinix; Glaciterno; Talpmagnet | possible candidate | linea Brinix/Glaciterno, ambiente montano e sottosuolo |
| Futura area ispirata a Lariano | Boletushield | strong thematic candidate | revisione del nome, linea e blockout |
| Futura area ispirata al Regillo | Marmorequus | strong thematic candidate | rarità, ruolo monumentale e nome in-game |
| Aree rurali o vitivinicole future | Vitemosto | possible candidate | geografia e progressione non definite |
| Oliveti o boschi futuri | Omphalux; Lenghelis | possible candidate | ciclo giorno/notte e habitat non definiti |
| Aree minerali, vulcaniche o sotterranee future | Solftraver; Porchemecha/Bronzoverro; Talpmagnet | possible candidate | nomi, blockout e distribuzione dei tipi rari |
| Collocazione non ancora associabile | fossili, Saladoct, Glicidra | premature/TBD | ottenimento, narrativa e habitat definitivi |

Questa tabella raccoglie soltanto affinità di design. Non definisce la fauna di
Via Verdi, del Lago di Albèra o di qualunque area futura.

## Current ecosystem gaps

Il catalogo non copre ancora in modo convincente:

- piccolo mammifero comune;
- linea Coleottero iniziale;
- linea acquatica comune;
- specie urbana;
- specie cavernicola o da cisterna;
- fauna specifica del Lago di Albèra;
- impollinatore o linea floreale;
- possibile scarsità di Normale, Coleottero e Acqua comuni;
- possibile eccesso di Acciaio, Drago e doppi tipi rari.

Alcune lacune possono essere coperte da Pokémon ufficiali: non è necessario
creare una specie originale per ogni ruolo ecologico o di gameplay.

## Catalog summary

| Temporary ID | Family/species | Stages | Status | Proposed type(s) | Earliest candidate area | Main open issue |
| --- | --- | ---: | --- | --- | --- | --- |
| `AUS-FAM-STARTER-1` | Cingerm → Rovasco → Selvazanna | 3 | IMPLEMENTED | Erba → Erba → Erba/Buio | Laboratorio del Cratere | disponibilità futura e asset provvisori residui |
| `AUS-FAM-STARTER-2` | Serbrace → Vipercen → Tossivampa | 3 | IMPLEMENTED | Fuoco → Fuoco → Fuoco/Veleno | Laboratorio del Cratere | disponibilità futura e asset provvisori residui |
| `AUS-FAM-STARTER-3` | Ardeino → Velairone → Codairone | 3 | IMPLEMENTED | Acqua → Acqua → Acqua/Volante | Laboratorio del Cratere | concept definitiva di Codairone e asset residui |
| `AUS-FAM-FOSSIL-ELEPHANT` | Eleby → Antiquas | 2 | CANDIDATE; NAME REVIEW | TBD → Roccia/Psico | premature/TBD | nome base e ottenimento fossile |
| `AUS-FAM-FOSSIL-HIPPO` | Potamy → Plumbopotam | 2 | CANDIDATE; NAME REVIEW | TBD → Roccia/Acciaio | premature/TBD | nomi e ottenimento fossile |
| `AUS-FAM-FALCON` | Ghepio → Tinuncol | 2 | CANDIDATE; LINE REVIEW | Volante | possible candidate: prime route | linea e collocazione |
| `AUS-CONCEPT-OMPHALUX` | Omphalux | 1+ TBD | CANDIDATE | Erba/Elettro | possible candidate: oliveti/boschi | linea e meccanica notturna |
| `AUS-CONCEPT-LUSCERP` | Luscerp | 1+ TBD | CANDIDATE; LINE REVIEW; TYPE REVIEW | Erba/Drago proposto | premature/TBD | assegnazione di Drago per stadio |
| `AUS-CONCEPT-FRAGMAGMA` | Fragmagma | 1+ TBD | CANDIDATE | Folletto/Fuoco | possible candidate: Genzano/Nemi | area e linea |
| `AUS-CONCEPT-SOLFTRAVER` | Solftraver | 1+ TBD | CANDIDATE | Roccia/Veleno | possible candidate: area minerale | habitat e linea |
| `AUS-CONCEPT-TRICREST` | Tricrest | 1+ TBD | CANDIDATE; LINE REVIEW; TYPE REVIEW | Acqua/Drago | premature/TBD: zona umida | typing e linea |
| `AUS-CONCEPT-FURNACE-BOAR` | Porchemecha / Bronzoverro | 1+ TBD | CANDIDATE; NAME REVIEW | Acciaio/Fuoco | possible candidate: area metallurgica | scegliere il nome |
| `AUS-CONCEPT-SALADOCT` | Saladoct | 1+ TBD | CANDIDATE; NAME REVIEW | Veleno/Folletto | premature/TBD | nome e collocazione |
| `AUS-CONCEPT-NAUFRAGUS` | Naufragus | 1+ TBD | CANDIDATE | Acqua/Acciaio | premature/TBD | ruolo narrativo; non assegnarlo automaticamente al lago |
| `AUS-CONCEPT-MOLOSPSY` | Molospsy | 1+ TBD | CANDIDATE; NAME REVIEW | Lotta/Psico | possible candidate: rovine | nome e sito |
| `AUS-CONCEPT-LENGHELIS` | Lenghelis | 1+ TBD | CANDIDATE | Spettro/Folletto | possible candidate: boschi/borghi | habitat e tono folklorico |
| `AUS-CONCEPT-TALPMAGNET` | Talpmagnet | 1+ TBD | CANDIDATE; NAME REVIEW; LINE REVIEW | Terra/Elettro | possible candidate: sottosuolo | nome e linea |
| `AUS-CONCEPT-VITEMOSTO` | Vitemosto | 1+ TBD | CANDIDATE; LINE REVIEW | Erba/Fuoco | possible candidate: area vitivinicola | linea e collocazione |
| `AUS-CONCEPT-GLICIDRA` | Glicidra | 1+ TBD | CANDIDATE; TYPE REVIEW | Drago/Veleno | possible candidate: ville | rarità e typing |
| `AUS-CONCEPT-FRASCHIETTO` | Fraschietto | 1+ TBD | CANDIDATE; LINE REVIEW | Lotta/Erba | strong thematic candidate: Ariccia | linea e tono culturale |
| `AUS-CONCEPT-BRINIX` | Brinix | 1 | CANDIDATE; LINE REVIEW | Ghiaccio | possible candidate: Monte Cavo | possibile legame con Glaciterno |
| `AUS-CONCEPT-GLACITERNO` | Glaciterno | 1 | CANDIDATE; LINE REVIEW | Ghiaccio/Terra | possible candidate: Monte Cavo | possibile legame con Brinix |
| `AUS-CONCEPT-BOLETUSHIELD` | Boletushield | 1+ TBD | CANDIDATE; NAME REVIEW | Terra/Acciaio | strong thematic candidate: Lariano | nome inglese e linea |
| `AUS-CONCEPT-MARMOREQUUS` | Marmorequus | 1+ TBD | CANDIDATE | Normale/Acciaio | strong thematic candidate: Regillo | rarità e ruolo monumentale |

## Decisions not made by this document

Il catalogo non decide:

- numerazione regionale definitiva;
- totale finale delle specie;
- approvazione o esclusione dei concetti;
- nomi definitivi delle specie candidate;
- statistiche;
- abilità;
- mosse e learnset;
- gruppi Uova;
- rarità;
- tassi di cattura;
- sprite e altri asset;
- habitat definitivi;
- fauna definitiva di Via Verdi;
- fauna definitiva del Lago di Albèra;
- metodo di ottenimento dei fossili;
- nomi delle future città e aree;
- specie leggendarie o mitiche;
- coordinate, incontri, squadre o disponibilità nel gameplay.
