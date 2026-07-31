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
- **BALANCE PROVISIONAL:** livello evolutivo, statistiche, abilità o altri
  parametri tecnici ancora da confermare.
- **PLACEMENT CANDIDATE:** area generale approvata come direzione, ma
  coordinate, tassi e tabelle di incontro ancora da definire.
- **PLACEMENT TBD:** collocazione non approvata.
- **RETIRED WORKING NAME:** vecchio nome conservato soltanto come riferimento
  storico e non conteggiato come specie attiva.
- **POSSIBLE CUT:** concetto conservato ma potenzialmente eliminabile.

Le specie diverse dai nove starter restano proposte salvo quando sono marcate
esplicitamente **CANONICAL DESIGN**. Anche in quel caso, la presenza nel
catalogo non equivale a implementazione o disponibilità nel gameplay.
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

- Ghepio → Tinuncol → Peregrinus è una famiglia approvata di tre specie;
- Gazzuola → Brillazza → Gazzombra aggiunge una nuova famiglia di tre specie;
- Borgotto → Pastufo e Miciolo → Felivates aggiungono due famiglie comuni a
  due stadi;
- Foliarva → Crisalvia → Infiorala aggiunge una famiglia Coleottero iniziale a
  tre stadi;
- Paludix → Sanguilex aggiunge una famiglia Coleottero specializzata a due
  stadi;
- Carpulus → Carpulutum, Lucinus → Umbracius e Cisternide → Calcistern
  aggiungono tre nuove famiglie a due stadi e sei specie attive;
- Glicidra, già censita come concetto a stadio singolo, viene formalizzata
  come stadio finale di Gliciscia → Pergoluber → Glicidra: la famiglia
  esistente riceve due nuove specie senza essere duplicata;
- Lumella, Luscinco e Tritino aggiungono uno stadio alle rispettive famiglie
  già censite;
- Brinix e Glaciterno sono due concetti separati e potrebbero formare una linea,
  senza che il legame sia approvato;
- Porchemecha o Bronzoverro è un solo concetto con due nomi alternativi;
- Salampolla → Alchimandra sostituisce Saladoct come concetto attivo;
  Saladoct resta soltanto un'etichetta storica;
- le altre specie singole potrebbero ricevere stadi evolutivi futuri.

Il catalogo censisce **32 famiglie o concetti**. Contiene **60 etichette
nominali**, includendo Porchemecha e Bronzoverro come alternative e Saladoct
come vecchio nome di lavorazione. Le etichette corrispondono ad almeno **58
specie attive distinte attualmente nominate**: Porchemecha/Bronzoverro conta
una volta e Saladoct non conta come specie attiva. Le tre famiglie nuove
aggiungono sei specie; l'ampliamento del concetto esistente di Glicidra ne
aggiunge due. Il totale finale resta **TBD** perché linee come
Brinix/Glaciterno non sono deliberate, diversi concetti singoli possono
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

## Other regional concepts

### Ghepio → Tinuncol → Peregrinus

- **Temporary family ID:** `AUS-FAM-FALCON`
- **Species/stages:** Ghepio → Tinuncol → Peregrinus.
- **Current status:** CANONICAL DESIGN; non implementata; PLACEMENT CANDIDATE;
  parametri tecnici BALANCE PROVISIONAL.
- **Proposed typing:** Ghepio Volante; Tinuncol Volante; Peregrinus
  Volante/Lotta.
- **Core concept:** linea di falconidi prevalentemente diurna, offensiva,
  rapida e basata sulla picchiata.
- **Real-world inspiration:** gheppio, *Falco tinnunculus* e falco pellegrino;
  Peregrinus deriva direttamente da *Falco peregrinus*.
- **Evolution structure:** linea canonica a tre stadi; livelli evolutivi
  BALANCE PROVISIONAL.
- **Primary habitat candidate:** Via Verdi durante il giorno per Ghepio;
  PLACEMENT CANDIDATE, senza rendere l'orario un'esclusiva assoluta.
- **Secondary habitat candidates:** campagne, borghi e alture; PLACEMENT
  CANDIDATE.
- **Earliest story band candidate:** Via Verdi per il primo stadio.
- **Gameplay role candidate:** attaccante rapido basato sulla picchiata;
  statistiche e learnset BALANCE PROVISIONAL.
- **Narrative importance:** ordinaria; nessun ruolo narrativo approvato.
- **Name review:** linea e nomi approvati.
- **Type review:** tipi approvati per tutti e tre gli stadi.
- **Placement review:** frequenze, orari esatti e presenza degli stadi evoluti
  allo stato selvatico restano TBD.
- **Open questions:** livelli evolutivi, abilità, statistiche, learnset,
  frequenze, orari esatti e distribuzione degli stadi evoluti.

### Gazzuola → Brillazza → Gazzombra

- **Temporary family ID:** `AUS-FAM-MAGPIE`
- **Species/stages:** Gazzuola → Brillazza → Gazzombra.
- **Current status:** CANONICAL DESIGN; non implementata; PLACEMENT CANDIDATE;
  parametri tecnici BALANCE PROVISIONAL.
- **Proposed typing:** Gazzuola Normale/Volante; Brillazza Buio/Volante;
  Gazzombra Buio/Volante.
- **Core concept:** linea della gazza: curiosità, attrazione per oggetti
  brillanti e infine comportamento furtivo.
- **Real-world inspiration:** gazze e loro comportamento opportunista e
  attrazione per oggetti luminosi.
- **Evolution structure:** linea canonica a tre stadi; livelli evolutivi TBD.
- **Primary habitat candidate:** borghi, giardini, rovine, fontane e strade;
  PLACEMENT CANDIDATE.
- **Secondary habitat candidates:** Via Verdi e Albèra Storica per Gazzuola;
  presenza prevalentemente crepuscolare e notturna, ma non esclusiva.
- **Earliest story band candidate:** Via Verdi per la forma base.
- **Gameplay role candidate:** Gazzuola ha l'abilità approvata Raccolta
  (Pickup); ruolo, statistiche e learnset degli stadi evoluti restano TBD.
- **Narrative importance:** fauna opportunista di borghi e rovine; nessun
  evento narrativo approvato.
- **Name review:** linea e nomi approvati.
- **Type review:** tipi approvati per tutti e tre gli stadi.
- **Placement review:** tassi e fasce orarie precise restano TBD.
- **Open questions:** livelli evolutivi, statistiche, learnset, seconda
  abilità, abilità nascoste e abilità definitive di Brillazza e Gazzombra.

### Borgotto → Pastufo

- **Temporary family ID:** `AUS-FAM-DOG`
- **Species/stages:** Borgotto → Pastufo.
- **Current status:** CANONICAL DESIGN; non implementata; PLACEMENT CANDIDATE;
  evoluzione e parametri tecnici BALANCE PROVISIONAL.
- **Proposed typing:** Borgotto Normale; Pastufo Normale/Terra.
- **Core concept:** cane comune, rurale e protettivo di borghi, cortili e
  campagne, distinto dal molosso templare Molospsy.
- **Real-world inspiration:** Borgotto unisce borgo e cucciolotto; Pastufo
  unisce pastore e tufo.
- **Evolution structure:** linea canonica a due stadi; evoluzione di lavoro al
  livello 23, BALANCE PROVISIONAL.
- **Primary habitat candidate:** Borgotto tra Albèra Bassa, Via Verdi e le
  campagne iniziali; PLACEMENT CANDIDATE.
- **Secondary habitat candidates:** Pastufo tra Via Consolare e l'arco del
  Lago di Albèra; PLACEMENT CANDIDATE.
- **Earliest story band candidate:** Albèra Bassa per Borgotto.
- **Gameplay role candidate:** fisico, resistente e affidabile, con buoni PS e
  Difesa e Velocità non elevata; valori esatti BALANCE PROVISIONAL.
- **Narrative importance:** Borgotto può essere accudito da più abitanti;
  Pastufo protegge orti, vigne, sentieri e viandanti senza essere aggressivo
  senza motivo.
- **Name review:** linea e nomi approvati.
- **Type review:** tipi approvati per entrambi gli stadi.
- **Placement review:** incontri, coordinate, tassi e presenza selvatica di
  Pastufo restano TBD.
- **Open questions:** abilità, statistiche, learnset, frequenze e collocazioni
  esatte; la sensibilità a vibrazioni e terreno umido può farne un indicatore
  ecologico, ma la linea non causa l'anomalia.

### Miciolo → Felivates

- **Temporary family ID:** `AUS-FAM-CAT`
- **Species/stages:** Miciolo → Felivates.
- **Current status:** CANONICAL DESIGN; non implementata; PLACEMENT CANDIDATE;
  evoluzione e parametri tecnici BALANCE PROVISIONAL.
- **Proposed typing:** Miciolo Normale; Felivates Normale/Psico.
- **Core concept:** linea urbana di vicoli, cortili, fontane, giardini e
  rovine, agile e tecnica, basata su osservazione, previsione e precisione.
- **Real-world inspiration:** Miciolo unisce micio e vicolo; Felivates combina
  *felis/feles* con *vates*, veggente o profeta, richiamando l'identità romana
  di Albèra Storica.
- **Evolution structure:** linea canonica a due stadi; evoluzione di lavoro
  tramite amicizia alta salendo di livello, BALANCE PROVISIONAL.
- **Primary habitat candidate:** Miciolo visibile ad Albèra Bassa e catturabile
  soprattutto ad Albèra Storica; PLACEMENT CANDIDATE.
- **Secondary habitat candidates:** ville, muretti e rovine; Felivates
  selvatico solo più avanti, se approvato.
- **Earliest story band candidate:** Albèra Bassa come fauna urbana visibile.
- **Gameplay role candidate:** agile, tecnico e orientato a previsione,
  precisione e controllo; statistiche e learnset BALANCE PROVISIONAL.
- **Narrative importance:** Felivates percepisce intenzioni, pericoli e
  vibrazioni nelle rovine, ma non è sacro né leggendario.
- **Name review:** linea e nomi approvati.
- **Type review:** tipi approvati per entrambi gli stadi.
- **Placement review:** incontri, tassi e disponibilità selvatica di Felivates
  restano TBD.
- **Open questions:** conferma tecnica dell'evoluzione per amicizia, abilità,
  statistiche, learnset e distribuzione esatta. Diversamente da Gazzombra,
  non è incentrato su strumenti, furto e disturbo.

### Foliarva → Crisalvia → Infiorala

- **Temporary family ID:** `AUS-FAM-EARLY-BUG`
- **Species/stages:** Foliarva → Crisalvia → Infiorala.
- **Current status:** CANONICAL DESIGN; non implementata; PLACEMENT CANDIDATE;
  evoluzioni e parametri tecnici BALANCE PROVISIONAL.
- **Proposed typing:** Foliarva Coleottero; Crisalvia Coleottero/Erba;
  Infiorala Coleottero/Erba. Nessuno stadio riceve Folletto.
- **Core concept:** linea Coleottero iniziale a tre stadi, legata a prati,
  foglie, germogli e impollinazione.
- **Real-world inspiration:** Foliarva combina *folia*/foglia e larva;
  Crisalvia unisce crisalide e salvia; Infiorala unisce infiorata e ala in un
  lepidottero impollinatore dalle ali simili a composizioni floreali.
- **Evolution structure:** Foliarva evolve in Crisalvia al livello 7 e
  Crisalvia in Infiorala al livello 13; entrambi i livelli sono BALANCE
  PROVISIONAL.
- **Primary habitat candidate:** Foliarva comune di giorno e Crisalvia rara su
  Via Verdi; PLACEMENT CANDIDATE.
- **Secondary habitat candidates:** Via Consolare; Infiorala è
  particolarmente rappresentativa della futura area ispirata a Genzano.
- **Earliest story band candidate:** fra Albèra e Via Verdi.
- **Gameplay role candidate:** Infiorala orientata in generale ad Attacco
  Speciale, Velocità, polveri e supporto vegetale; valori esatti TBD.
- **Narrative importance:** forte legame tematico futuro con l'area ispirata a
  Genzano; i Pokémon Folletto restano più peculiari, soprattutto rispetto al
  futuro arco del Lago di Nemi.
- **Name review:** linea e nomi approvati.
- **Type review:** tipi approvati per tutti e tre gli stadi.
- **Placement review:** incontri, percentuali e fasce orarie precise restano
  TBD.
- **Open questions:** abilità, statistiche, learnset, distribuzione esatta e
  specie zoologica precisa oltre al concetto di lepidottero impollinatore.

### Paludix → Sanguilex

- **Temporary family ID:** `AUS-FAM-MOSQUITO`
- **Species/stages:** Paludix → Sanguilex.
- **Current status:** CANONICAL DESIGN; non implementata; PLACEMENT CANDIDATE;
  evoluzione e parametri tecnici BALANCE PROVISIONAL.
- **Proposed typing:** Paludix Coleottero/Acqua; Sanguilex
  Coleottero/Veleno. Sanguilex non riceve il tipo Volante.
- **Core concept:** linea della zanzara a due stadi, dalla larva acquatica dei
  ristagni all'adulto rapido orientato alla puntura.
- **Real-world inspiration:** Paludix combina *palus/paludis* e *culex*;
  Sanguilex combina *sanguis* e *culex*.
- **Evolution structure:** evoluzione di lavoro al livello 18, di sera o di
  notte; livello e requisito temporale sono BALANCE PROVISIONAL.
- **Primary habitat candidate:** Paludix raro su Via Consolare e linea
  presente soprattutto nell'arco del Lago di Albèra; PLACEMENT CANDIDATE.
- **Secondary habitat candidates:** canalette interrotte, pozze temporanee e
  ristagni; Sanguilex è più attivo al tramonto e di notte.
- **Earliest story band candidate:** Via Consolare per Paludix; la linea è
  assente da Via Verdi.
- **Gameplay role candidate:** Sanguilex molto veloce e fragile, orientato ad
  assorbimento, punture e alterazioni di stato; il volo è espresso tramite
  design, animazioni e mosse.
- **Narrative importance:** nuovi ristagni e variazioni del flusso possono
  spostare le colonie e fornire indizi a Lia; la linea non causa l'anomalia.
- **Name review:** linea e nomi approvati.
- **Type review:** tipi approvati per entrambi gli stadi; linea distinta da
  Tritino/Tricrest e Salampolla/Alchimandra.
- **Placement review:** incontri, coordinate, tassi e fasce orarie precise
  restano TBD.
- **Open questions:** abilità, statistiche, learnset, frequenze e
  disponibilità selvatica di Sanguilex.

### Carpulus → Carpulutum

- **Temporary family ID:** `AUS-FAM-CARP`
- **Species/stages:** Carpulus → Carpulutum.
- **Current status:** CANONICAL DESIGN; non implementata; PLACEMENT CANDIDATE;
  evoluzione e parametri tecnici BALANCE PROVISIONAL.
- **Proposed typing:** Carpulus Acqua; Carpulutum Acqua/Terra.
- **Core concept:** linea comune di carpe del Lago di Albèra, dalle acque basse
  ai fondali fangosi.
- **Real-world inspiration:** carpe di lago, canneti, rive e fondali fangosi.
- **Evolution structure:** linea canonica a due stadi; evoluzione proposta
  intorno al livello 25, BALANCE PROVISIONAL.
- **Primary habitat candidate:** Lago di Albèra; PLACEMENT CANDIDATE.
- **Secondary habitat candidates:** Carpulus presso acque basse, rive, pontili
  e canneti; Carpulutum nei fondali fangosi.
- **Earliest story band candidate:** arco del Lago di Albèra.
- **Gameplay role candidate:** lento e resistente, con molti PS e buona
  Difesa; statistiche esatte BALANCE PROVISIONAL.
- **Narrative importance:** percepisce vibrazioni e mutamenti del fondale e
  reagisce alle anomalie del lago, ma non ne è la causa.
- **Name review:** linea e nomi approvati.
- **Type review:** tipi approvati per entrambi gli stadi.
- **Placement review:** incontri, coordinate, tassi e distribuzione degli
  stadi restano TBD.
- **Open questions:** livello definitivo, abilità, statistiche, learnset,
  frequenze e disponibilità selvatica di Carpulutum.

### Lucinus → Umbracius

- **Temporary family ID:** `AUS-FAM-PIKE`
- **Species/stages:** Lucinus → Umbracius.
- **Current status:** CANONICAL DESIGN; non implementata; PLACEMENT CANDIDATE;
  evoluzione e parametri tecnici BALANCE PROVISIONAL.
- **Proposed typing:** Lucinus Acqua; Umbracius Acqua/Spettro.
- **Core concept:** rara linea di lucci del Lago di Albèra, legata ad agguati
  tra canneti e ombre nelle acque profonde.
- **Real-world inspiration:** lucci, rive isolate, acque quiete e sagome sotto
  la superficie.
- **Evolution structure:** linea canonica a due stadi; evoluzione proposta
  intorno al livello 30 durante la notte, BALANCE PROVISIONAL.
- **Primary habitat candidate:** Lago di Albèra; PLACEMENT CANDIDATE.
- **Secondary habitat candidates:** Lucinus tra canneti, rive isolate e acque
  quiete; Umbracius nelle zone profonde.
- **Earliest story band candidate:** arco del Lago di Albèra.
- **Gameplay role candidate:** attaccante fisico basato su agguati, morsi e
  scatti; statistiche e learnset BALANCE PROVISIONAL.
- **Narrative importance:** reagisce alle anomalie del lago, ma non ne è la
  causa.
- **Name review:** linea e nomi approvati.
- **Type review:** tipi approvati per entrambi gli stadi.
- **Placement review:** incontri, coordinate, tassi, fasce orarie e presenza
  selvatica di Umbracius restano TBD.
- **Open questions:** livello definitivo, abilità, statistiche, learnset e
  rarità. La linea resta distinta da Naufragus, riservato al futuro arco
  ispirato al Lago di Nemi.

### Cisternide → Calcistern

- **Temporary family ID:** `AUS-FAM-CISTERN-CRUSTACEAN`
- **Species/stages:** Cisternide → Calcistern.
- **Current status:** CANONICAL DESIGN; non implementata; PLACEMENT CANDIDATE;
  evoluzione e parametri tecnici BALANCE PROVISIONAL.
- **Proposed typing:** Cisternide Acqua; Calcistern Acqua/Roccia.
- **Core concept:** linea di crostacei cavernicoli dei Cisternoni, dei condotti
  antichi e delle sorgenti sotterranee di Albèra.
- **Real-world inspiration:** piccoli crostacei cavernicoli, cisterne antiche,
  calcare e sedimenti.
- **Evolution structure:** linea canonica a due stadi; evoluzione proposta
  intorno al livello 26, BALANCE PROVISIONAL.
- **Primary habitat candidate:** primo avvistamento nei Cisternoni di Albèra
  Storica; PLACEMENT CANDIDATE.
- **Secondary habitat candidates:** cattura candidata nei condotti sotterranei
  tra Via Consolare e Lago di Albèra.
- **Earliest story band candidate:** Albèra Storica, come avvistamento nei
  Cisternoni.
- **Gameplay role candidate:** lento, difensivo e orientato a guscio,
  pressione e controllo; statistiche e learnset BALANCE PROVISIONAL.
- **Narrative importance:** percepisce variazioni della pressione sotterranea,
  ma non causa l'anomalia.
- **Name review:** linea e nomi approvati.
- **Type review:** tipi approvati per entrambi gli stadi.
- **Placement review:** incontri, accesso ai condotti, coordinate e tassi
  restano TBD.
- **Open questions:** livello definitivo, abilità, statistiche, learnset e
  distribuzione. Cisternide è piccolo e pallido, con occhi ridotti, lunghe
  antenne e carapace morbido; Calcistern sviluppa un guscio mineralizzato.

### Lumella → Omphalux

- **Temporary family ID:** `AUS-CONCEPT-OMPHALUX`
- **Species/stages:** Lumella → Omphalux.
- **Current status:** CANONICAL DESIGN; non implementata; PLACEMENT CANDIDATE;
  evoluzione BALANCE PROVISIONAL.
- **Proposed typing:** Lumella Erba; Omphalux Erba/Elettro.
- **Core concept:** linea notturna di funghi bioluminescenti legati agli olivi.
- **Real-world inspiration:** *Omphalotus*, oliveti e bioluminescenza.
- **Evolution structure:** linea canonica a due stadi; evoluzione di lavoro al
  livello 22 durante la notte, BALANCE PROVISIONAL.
- **Primary habitat candidate:** Via Consolare, oliveti e Lago di Albèra;
  PLACEMENT CANDIDATE.
- **Secondary habitat candidates:** Lumella rara su Via Verdi; PLACEMENT
  CANDIDATE.
- **Earliest story band candidate:** Via Verdi, come incontro raro notturno.
- **Gameplay role candidate:** Omphalux lento, orientato ad Attacco Speciale e
  disturbo; statistiche e learnset restano TBD.
- **Narrative importance:** nessun legame causale approvato con l'anomalia
  dell'acqua.
- **Name review:** linea e nomi approvati.
- **Type review:** tipi approvati per entrambi gli stadi.
- **Placement review:** tassi e fasce orarie precise restano TBD.
- **Open questions:** statistiche, abilità, learnset e distribuzione esatta.

### Luscinco → Luscerp

- **Temporary family ID:** `AUS-CONCEPT-LUSCERP`
- **Species/stages:** Luscinco → Luscerp.
- **Current status:** CANONICAL DESIGN; non implementata; PLACEMENT CANDIDATE;
  evoluzione e abilità BALANCE PROVISIONAL.
- **Proposed typing:** Luscinco Erba; Luscerp Erba/Drago.
- **Core concept:** linea terrestre e prevalentemente diurna ispirata a
  luscengola e scinco.
- **Real-world inspiration:** luscengole, scinchi, rive erbose e muretti.
- **Evolution structure:** linea canonica a due stadi; evoluzione di lavoro al
  livello 24, BALANCE PROVISIONAL; il tipo Drago compare solo in Luscerp.
- **Primary habitat candidate:** rive erbose e muretti del Lago di Albèra;
  PLACEMENT CANDIDATE.
- **Secondary habitat candidates:** Luscinco raro su Via Verdi e più comune su
  Via Consolare; PLACEMENT CANDIDATE.
- **Earliest story band candidate:** Via Verdi, come incontro raro.
- **Gameplay role candidate:** Luscerp fisico e rapido; Muta resta un'abilità
  candidata e BALANCE PROVISIONAL, non approvata definitivamente.
- **Narrative importance:** nessun legame narrativo causale con l'anomalia.
- **Name review:** linea e nomi approvati.
- **Type review:** tipi approvati per entrambi gli stadi.
- **Placement review:** frequenze e orari esatti restano TBD.
- **Open questions:** statistiche, learnset, abilità definitiva e
  distribuzione esatta.

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

### Tritino → Tricrest

- **Temporary family ID:** `AUS-CONCEPT-TRICREST`
- **Species/stages:** Tritino → Tricrest.
- **Current status:** CANONICAL DESIGN; non implementata; PLACEMENT CANDIDATE;
  evoluzione e abilità BALANCE PROVISIONAL.
- **Proposed typing:** Tritino Acqua; Tricrest Acqua/Drago.
- **Core concept:** linea lacustre a due stadi ispirata al tritone crestato.
- **Real-world inspiration:** tritoni crestati, raccolte d'acqua e ambienti
  lacustri.
- **Evolution structure:** linea canonica a due stadi; evoluzione di lavoro al
  livello 27, BALANCE PROVISIONAL.
- **Primary habitat candidate:** Lago di Albèra; PLACEMENT CANDIDATE.
- **Secondary habitat candidates:** Tritino molto raro presso raccolte d'acqua
  di Via Consolare e assente dagli incontri normali di Via Verdi; Tricrest
  selvatico candidato solo per aree più avanzate o profonde.
- **Earliest story band candidate:** Via Consolare per Tritino, molto raro.
- **Gameplay role candidate:** Tritino resistente e più orientato al lato
  speciale; Assorbacqua è candidata principale ma non definitiva.
- **Narrative importance:** reagisce alle anomalie di pressione, ma non ne è
  la causa.
- **Name review:** linea e nomi approvati.
- **Type review:** tipi approvati per entrambi gli stadi.
- **Placement review:** tassi e posizione degli stadi evoluti restano TBD.
- **Open questions:** statistiche, learnset, abilità definitiva e frequenze.

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

### Salampolla → Alchimandra

- **Temporary family ID:** `AUS-CONCEPT-SALADOCT`
- **Species/stages:** Salampolla → Alchimandra. Saladoct è un RETIRED WORKING
  NAME e non una specie attiva separata.
- **Current status:** CANONICAL DESIGN; non implementata; PLACEMENT CANDIDATE;
  evoluzione BALANCE PROVISIONAL.
- **Proposed typing:** Salampolla Veleno; Alchimandra Veleno/Folletto.
- **Core concept:** linea notturna a due stadi della salamandrina alchemica.
- **Real-world inspiration:** salamandrina e immaginario alchemico.
- **Evolution structure:** linea canonica a due stadi; evoluzione di lavoro al
  livello 28 durante la notte, BALANCE PROVISIONAL.
- **Primary habitat candidate:** sottobosco umido e rive ombrose del Lago di
  Albèra; PLACEMENT CANDIDATE.
- **Secondary habitat candidates:** zone umide e rive ombrose, TBD.
- **Earliest story band candidate:** prima comparsa importante candidata
  nell'arco del Lago di Albèra.
- **Gameplay role candidate:** supporto, veleni, cure e controllo degli stati;
  statistiche e learnset restano TBD.
- **Narrative importance:** nessun legame causale approvato con l'anomalia.
- **Name review:** Salampolla e Alchimandra approvati; Saladoct conservato solo
  come RETIRED WORKING NAME.
- **Type review:** tipi approvati per entrambi gli stadi.
- **Placement review:** tassi e fasce orarie precise restano TBD.
- **Open questions:** statistiche, abilità, learnset e distribuzione esatta.

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

### Gliciscia → Pergoluber → Glicidra

- **Temporary family ID:** `AUS-CONCEPT-GLICIDRA`
- **Species/stages:** Gliciscia → Pergoluber → Glicidra. Glicidra era già
  presente nel catalogo come concetto a stadio singolo e viene ora
  formalizzata come stadio finale, senza creare una seconda famiglia.
- **Current status:** CANONICAL DESIGN; non implementata; PLACEMENT CANDIDATE;
  evoluzioni e parametri tecnici BALANCE PROVISIONAL.
- **Proposed typing:** Gliciscia Erba; Pergoluber Erba/Veleno; Glicidra
  Drago/Veleno. Il tipo Erba viene perso nello stadio finale.
- **Core concept:** linea tardiva di rettili del glicine, dal mimetismo fra i
  tralci a un raro drago sinuoso dei giardini monumentali.
- **Real-world inspiration:** Gliciscia combina glicine e biscia; Pergoluber
  combina pergola e il latino *coluber*; Glicidra combina glicine e idra.
- **Evolution structure:** Gliciscia evolve in Pergoluber intorno al livello
  26 e Pergoluber in Glicidra intorno al livello 44; entrambi i livelli sono
  BALANCE PROVISIONAL.
- **Primary habitat candidate:** Gliciscia nei giardini e nelle ville della
  futura area ispirata ad Ariccia; PLACEMENT CANDIDATE.
- **Secondary habitat candidates:** Gliciscia e Pergoluber fra pergolati e
  coltivazioni della futura area ispirata a Genzano; Glicidra soprattutto
  ottenibile tramite evoluzione.
- **Earliest story band candidate:** soltanto dopo l'arco del Lago di Albèra;
  la linea non compare nel primo arco.
- **Gameplay role candidate:** Gliciscia si mimetizza fra tralci, siepi e
  giardini; Pergoluber è rampicante e sviluppa spine e secrezioni tossiche;
  parametri di battaglia ancora TBD.
- **Narrative importance:** Glicidra è rara e prestigiosa, ma è una specie
  normale, non leggendaria; i tralci attorno al collo sembrano false teste e
  confondono gli avversari.
- **Name review:** linea e nomi approvati.
- **Type review:** tipi approvati per tutti e tre gli stadi.
- **Placement review:** incontri, accesso alle aree future, tassi e
  disponibilità selvatica degli stadi evoluti restano TBD.
- **Open questions:** livelli definitivi, abilità, statistiche, learnset,
  rarità e distribuzione esatta.

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
| Albèra Bassa | Borgotto; Miciolo visibile come fauna urbana | PLACEMENT CANDIDATE | incontri, ruolo della fauna urbana e riuso di Pokémon ufficiali |
| Via Verdi | Ghepio; Gazzuola; Borgotto; Foliarva comune di giorno; Crisalvia rara; Lumella rara; Luscinco raro; possibili specie ufficiali | PLACEMENT CANDIDATE | tassi, fasce orarie non esclusive, curve dei livelli e approvazione degli incontri |
| Porta Pretoria | Molospsy | premature/TBD | funzione delle rovine e spazio della soglia |
| Albèra Storica | Cisternide come primo avvistamento nei Cisternoni; Miciolo; Gazzuola; Molospsy; Lenghelis | PLACEMENT CANDIDATE | accesso ai Cisternoni, tassi, ciclo giorno/notte e tono degli eventi |
| Anfiteatro Romano / prima Palestra | Molospsy | premature/TBD | tipo della Palestra e funzione narrativa non decisi |
| Via Consolare | Cisternide nei condotti sotterranei; Pastufo; linea Foliarva; Paludix raro; Lumella; Luscinco; Tritino molto raro; linea del falco | PLACEMENT CANDIDATE | accesso ai condotti, bioma, tassi e composizione della route non progettati |
| Lago di Albèra | Carpulus/Carpulutum; Lucinus/Umbracius; Pastufo; Paludix/Sanguilex; Lumella/Omphalux; Luscinco/Luscerp; Tritino/Tricrest; Salampolla/Alchimandra | PLACEMENT CANDIDATE | tassi, fasce orarie e presenza degli stadi evoluti; Naufragus non va collocato automaticamente qui |
| Futura area ispirata ad Ariccia | Gliciscia; Fraschietto | strong thematic candidate | nomi in-game, bioma e distribuzione delle linee |
| Futura area ispirata a Genzano o Lago di Nemi | Gliciscia/Pergoluber e Infiorala per Genzano; Fragmagma | strong thematic candidate | scelta dell'area per Fragmagma, nomi in-game e bilanciamento |
| Futura area ispirata a Monte Cavo | Brinix; Glaciterno; Talpmagnet | possible candidate | linea Brinix/Glaciterno, ambiente montano e sottosuolo |
| Futura area ispirata a Lariano | Boletushield | strong thematic candidate | revisione del nome, linea e blockout |
| Futura area ispirata al Regillo | Marmorequus | strong thematic candidate | rarità, ruolo monumentale e nome in-game |
| Aree rurali o vitivinicole future | Vitemosto | possible candidate | geografia e progressione non definite |
| Oliveti o boschi futuri | Lumella/Omphalux; Lenghelis | possible candidate | ciclo giorno/notte e habitat non definiti |
| Aree minerali, vulcaniche o sotterranee future | Solftraver; Porchemecha/Bronzoverro; Talpmagnet | possible candidate | nomi, blockout e distribuzione dei tipi rari |
| Collocazione non ancora associabile | fossili | premature/TBD | ottenimento, narrativa e habitat definitivi |

Questa tabella raccoglie soltanto affinità di design. Non definisce la fauna di
Via Verdi, del Lago di Albèra o di qualunque area futura.

## Current ecosystem gaps

Quattro lacune hanno ora una soluzione progettuale approvata: Borgotto/Pastufo
copre la linea canina comune, Miciolo/Felivates la linea felina urbana,
Foliarva/Crisalvia/Infiorala il Coleottero iniziale a tre stadi e
Paludix/Sanguilex la seconda famiglia Coleottero specializzata. Nessuna di
queste linee è ancora implementata.

Carpulus/Carpulutum offre ora una soluzione progettuale per la linea Acqua
comune non anfibia, mentre Cisternide/Calcistern copre la fauna cavernicola e
da cisterna. Lucinus/Umbracius rafforza la fauna propria del Lago di Albèra.
Anche queste collocazioni restano candidate e non implementate.

Restano aperte:

- fauna urbana aggiuntiva, soltanto se ancora necessaria;
- ulteriore fauna propria del Lago di Albèra, soltanto se necessaria;
- ulteriori specie impollinatrici, soltanto se necessarie;
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
| `AUS-FAM-FALCON` | Ghepio → Tinuncol → Peregrinus | 3 | CANONICAL DESIGN; non implementata; BALANCE PROVISIONAL | Volante → Volante → Volante/Lotta | PLACEMENT CANDIDATE: Via Verdi di giorno | livelli, abilità, frequenze e orari esatti |
| `AUS-FAM-MAGPIE` | Gazzuola → Brillazza → Gazzombra | 3 | CANONICAL DESIGN; non implementata; BALANCE PROVISIONAL | Normale/Volante → Buio/Volante → Buio/Volante | PLACEMENT CANDIDATE: Via Verdi/Albèra Storica | livelli, abilità evolute e tassi |
| `AUS-FAM-DOG` | Borgotto → Pastufo | 2 | CANONICAL DESIGN; non implementata; BALANCE PROVISIONAL | Normale → Normale/Terra | PLACEMENT CANDIDATE: Albèra Bassa/Via Verdi | livello, abilità e distribuzione di Pastufo |
| `AUS-FAM-CAT` | Miciolo → Felivates | 2 | CANONICAL DESIGN; non implementata; BALANCE PROVISIONAL | Normale → Normale/Psico | PLACEMENT CANDIDATE: Albèra Bassa/Albèra Storica | conferma tecnica dell'amicizia e disponibilità di Felivates |
| `AUS-FAM-EARLY-BUG` | Foliarva → Crisalvia → Infiorala | 3 | CANONICAL DESIGN; non implementata; BALANCE PROVISIONAL | Coleottero → Coleottero/Erba → Coleottero/Erba | PLACEMENT CANDIDATE: Via Verdi | livelli, abilità, statistiche e tassi |
| `AUS-FAM-MOSQUITO` | Paludix → Sanguilex | 2 | CANONICAL DESIGN; non implementata; BALANCE PROVISIONAL | Coleottero/Acqua → Coleottero/Veleno | PLACEMENT CANDIDATE: Via Consolare/Lago di Albèra | evoluzione temporale, abilità e tassi |
| `AUS-FAM-CARP` | Carpulus → Carpulutum | 2 | CANONICAL DESIGN; non implementata; BALANCE PROVISIONAL | Acqua → Acqua/Terra | PLACEMENT CANDIDATE: Lago di Albèra | livello, abilità e distribuzione degli stadi |
| `AUS-FAM-PIKE` | Lucinus → Umbracius | 2 | CANONICAL DESIGN; non implementata; BALANCE PROVISIONAL | Acqua → Acqua/Spettro | PLACEMENT CANDIDATE: Lago di Albèra | evoluzione notturna, rarità e distribuzione |
| `AUS-FAM-CISTERN-CRUSTACEAN` | Cisternide → Calcistern | 2 | CANONICAL DESIGN; non implementata; BALANCE PROVISIONAL | Acqua → Acqua/Roccia | PLACEMENT CANDIDATE: Cisternoni di Albèra Storica | accesso ai condotti, livello e abilità |
| `AUS-CONCEPT-OMPHALUX` | Lumella → Omphalux | 2 | CANONICAL DESIGN; non implementata; BALANCE PROVISIONAL | Erba → Erba/Elettro | PLACEMENT CANDIDATE: Via Verdi rara, Via Consolare/oliveti/lago | evoluzione notturna e distribuzione esatta |
| `AUS-CONCEPT-LUSCERP` | Luscinco → Luscerp | 2 | CANONICAL DESIGN; non implementata; BALANCE PROVISIONAL | Erba → Erba/Drago | PLACEMENT CANDIDATE: Via Verdi/Via Consolare/lago | livello, abilità e frequenze |
| `AUS-CONCEPT-FRAGMAGMA` | Fragmagma | 1+ TBD | CANDIDATE | Folletto/Fuoco | possible candidate: Genzano/Nemi | area e linea |
| `AUS-CONCEPT-SOLFTRAVER` | Solftraver | 1+ TBD | CANDIDATE | Roccia/Veleno | possible candidate: area minerale | habitat e linea |
| `AUS-CONCEPT-TRICREST` | Tritino → Tricrest | 2 | CANONICAL DESIGN; non implementata; BALANCE PROVISIONAL | Acqua → Acqua/Drago | PLACEMENT CANDIDATE: Via Consolare rara/Lago di Albèra | livello, abilità e presenza evoluta |
| `AUS-CONCEPT-FURNACE-BOAR` | Porchemecha / Bronzoverro | 1+ TBD | CANDIDATE; NAME REVIEW | Acciaio/Fuoco | possible candidate: area metallurgica | scegliere il nome |
| `AUS-CONCEPT-SALADOCT` | Salampolla → Alchimandra; Saladoct RETIRED WORKING NAME | 2 | CANONICAL DESIGN; non implementata; BALANCE PROVISIONAL | Veleno → Veleno/Folletto | PLACEMENT CANDIDATE: Lago di Albèra | evoluzione notturna, abilità e tassi |
| `AUS-CONCEPT-NAUFRAGUS` | Naufragus | 1+ TBD | CANDIDATE | Acqua/Acciaio | premature/TBD | ruolo narrativo; non assegnarlo automaticamente al lago |
| `AUS-CONCEPT-MOLOSPSY` | Molospsy | 1+ TBD | CANDIDATE; NAME REVIEW | Lotta/Psico | possible candidate: rovine | nome e sito |
| `AUS-CONCEPT-LENGHELIS` | Lenghelis | 1+ TBD | CANDIDATE | Spettro/Folletto | possible candidate: boschi/borghi | habitat e tono folklorico |
| `AUS-CONCEPT-TALPMAGNET` | Talpmagnet | 1+ TBD | CANDIDATE; NAME REVIEW; LINE REVIEW | Terra/Elettro | possible candidate: sottosuolo | nome e linea |
| `AUS-CONCEPT-VITEMOSTO` | Vitemosto | 1+ TBD | CANDIDATE; LINE REVIEW | Erba/Fuoco | possible candidate: area vitivinicola | linea e collocazione |
| `AUS-CONCEPT-GLICIDRA` | Gliciscia → Pergoluber → Glicidra | 3 | CANONICAL DESIGN; non implementata; BALANCE PROVISIONAL | Erba → Erba/Veleno → Drago/Veleno | PLACEMENT CANDIDATE: dopo il Lago, future aree ispirate ad Ariccia/Genzano | livelli, abilità, rarità e distribuzione |
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
- livelli evolutivi definitivi;
- conferma tecnica dell'evoluzione per amicizia di Miciolo;
- statistiche;
- abilità di tutte le nuove specie e ogni abilità non esplicitamente
  approvata;
- mosse e learnset;
- gruppi Uova;
- rarità, tabelle di incontro e fasce orarie precise;
- tassi di cattura;
- sprite, icone, animazioni e altri asset;
- numeri Pokédex regionali;
- ID specie tecnici;
- disponibilità selvatica degli stadi evoluti;
- habitat definitivi;
- fauna definitiva di Via Verdi, Via Consolare e Lago di Albèra;
- metodo di ottenimento dei fossili;
- nomi delle future città e aree;
- specie leggendarie o mitiche;
- coordinate, inserimento negli incontri, squadre o disponibilità nel
  gameplay.
