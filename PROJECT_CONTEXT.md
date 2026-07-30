# Pokémon Alba

Pokémon Alba è un fangame basato su `pokeemerald-expansion`, ambientato in una nuova regione ispirata all’Italia.

Il nome provvisorio del progetto è **Pokémon Alba**.

## Prima beta

La prima beta sarà ambientata in una reinterpretazione videoludica di Albano Laziale e dei Castelli Romani. La geografia reale dovrà essere semplificata per adattarsi a un gioco Pokémon per GBA, mantenendo però elementi riconoscibili.

La prima area comprenderà:

- una zona ispirata a Via Donizetti;
- una zona ispirata a Via Rossini;
- una scuola ispirata alla scuola di Via Rossini;
- il campetto da basket del quartiere;
- una mini-via che unisce scuola, campetto, abitazioni e area verde;
- il centro storico di Albano;
- vicoli e piazze;
- elementi archeologici romani;
- una reinterpretazione della Via Appia;
- la discesa verso il Lago Albano;
- il lungolago;
- zone sotterranee, cisterne e condutture collegate alla falda.

I nomi reali potranno essere sostituiti da nomi fittizi durante la progettazione definitiva.

## Identità provvisoria del mondo

### Regione di Ausonia

Ausonia è una regione ispirata all’Italia, caratterizzata da:

- città moderne costruite sopra insediamenti antichi;
- laghi vulcanici;
- sistemi idrici sotterranei;
- boschi, coste, campagne e montagne;
- forte contrasto tra tutela del territorio e sviluppo economico;
- tradizioni locali differenti da zona a zona.

### Albèra

Albèra è la città iniziale, ispirata ad Albano Laziale.

La progressione geografica canonica approvata è:

`Albèra Bassa → Via Verdi → Porta Pretoria → Albèra Storica → Anfiteatro Romano / prima Palestra → Via Consolare → Lago di Albèra`.

`Lago di Albèra` è il nome visibile nel gioco; il Lago Albano reale resta il
riferimento geografico e ambientale. La distinzione evita di confondere il
luogo narrativo con quello reale.

**Stato implementato:** Albèra Bassa comprende Via dell’Armonia, la casa del
protagonista, la casa di Lia, la scuola, il campetto, il Laboratorio del Cratere
e i musicisti ambientali. Il primo segmento giocabile termina al checkpoint
tecnico di `Route101`.

**Stato approvato ma non implementato:** `Route101` avrà il nome visibile
**Via Verdi**, in omaggio a Giuseppe Verdi e al paesaggio verde attraversato.
Gli identificatori e le directory tecniche non saranno rinominati. Il nome
visibile dovrà essere applicato in futuro a interfaccia, Town Map, cartelli,
dialoghi e documentazione pertinenti.

**Stato pianificato:** Albèra Storica comprenderà Porta Pretoria, strada e
piazza storiche, l’Anfiteatro Romano con la prima Palestra, servizi cittadini,
riferimenti alla storia idraulica e una futura connessione permanente con il
resto di Albèra.

La sua futura mappa comprenderà:

- Via dell’Armonia;
- scuola del quartiere;
- campetto da basket;
- abitazioni;
- centro storico;
- resti romani;
- Via Consolare;
- discesa verso il Lago di Albèra;
- accessi sotterranei alla falda.

### Professor Lauro

Il Professor Lauro studia:

- adattamento dei Pokémon all’ambiente;
- memoria dei luoghi;
- modificazioni causate dall’acqua e dal territorio;
- legame tra Allenatore e Pokémon;
- fenomeno che in futuro verrà chiamato Forma Riflesso.

Il nome richiama l’alloro, ma il personaggio mantiene temporaneamente sprite e struttura tecnica del Professor Birch.

Dopo la prima sfida amichevole con Nico, Lauro affida al protagonista il primo
incarico di ricerca sulle anomalie idriche. Non conosce ancora la causa: chiede
di osservare pressione, sorgenti e comportamento dei Pokémon usando il Pokédex
anche come riferimento scientifico. L'incarico apre Route 101 e avvia
l'indagine senza dichiarare una crisi o introdurre spiegazioni definitive.

La prosecuzione approvata dell'incarico è **Via Verdi — Prima indagine sulle
sorgenti**. Lauro consegnerà esattamente **10 Poké Ball**; le catture resteranno
facoltative e non sarà introdotto un tutorial lungo o obbligatorio. Lia guiderà
l'osservazione, mentre Nico avrà una presenza breve e senza una seconda
battaglia. Il giocatore non è presentato come un prescelto.

Via Verdi collegherà Albèra Bassa a Porta Pretoria attraverso un ambiente verde
e collinare, con sorgenti, canaletti, terreno umido e tracce di canalizzazioni
antiche. L'indagine comprenderà tre punti concettuali di rilevazione e una
conclusione vicino a Porta Pretoria:

1. un canale o una sorgente quasi asciutta, con terreno ancora umido;
2. un'area attraversata o evitata in modo anomalo dai Pokémon;
3. un'antica canalizzazione vicina a Porta Pretoria.

Nell'evento finale concettuale, un suono profondo e un lieve tremore
accompagneranno una variazione o un arresto del flusso, seguiti dalla ripresa
improvvisa dell'acqua. Coordinate, dialoghi, flag e script non sono ancora
decisi.

### Nico e Lia

Nico e Lia esistono contemporaneamente e hanno ruoli complementari,
indipendenti dal protagonista selezionato.

- Nico è diretto, energico e competitivo senza essere ostile. Riceve lo starter
  avvantaggiato contro quello del giocatore ed è sempre il primo rivale da
  battaglia. In Via Verdi compare brevemente e non combatte di nuovo.
- Lia è calma, precisa e attenta ai dettagli. Riceve lo starter rimanente,
  segue le misurazioni ambientali, guida la prima indagine sulle sorgenti e non
  combatte in questa fase.

Entrambi sono cresciuti nel territorio di Albèra. La loro importanza narrativa
deriva rispettivamente dall'azione e dall'osservazione, non dal sesso scelto per
il protagonista.

## Tema della storia

Il livello dell’acqua dei laghi dei Castelli Romani sta diminuendo. Nel mondo narrativo del gioco, il Lago Albano e il Lago di Nemi sono alimentati da un grande sistema sotterraneo condiviso.

Le cause principali della crisi sono:

- crescente cementificazione del territorio;
- impermeabilizzazione del terreno;
- riduzione dell’acqua che raggiunge la falda;
- eccessivo sfruttamento delle risorse idriche;
- prelievi nascosti o illegali;
- interessi economici e politici;
- aggravamento causato da siccità e cambiamenti climatici.

La storia dovrà mostrare il problema attraverso ambienti, eventi e dialoghi, non soltanto spiegarlo con testi lunghi.

Esempi ambientali:

- fontanelle con poca pressione;
- sorgenti prosciugate;
- pontili lontani dall’acqua;
- rive ritirate;
- terreni trasformati in cantieri;
- parcheggi e nuove costruzioni;
- ville e strutture che consumano grandi quantità d’acqua;
- tubature e pompe nascoste;
- Pokémon acquatici costretti a cambiare habitat.

## Conflitto narrativo

La storia principale tradizionale comprenderà il viaggio, le Palestre, le Medaglie, la Lega Pokémon e l’obiettivo di diventare il miglior Allenatore.

Parallelamente sarà presente una storia più adulta, ma adatta al tono Pokémon, basata sul conflitto tra:

- cittadini;
- comunità locali;
- persone che difendono il territorio;
- imprese;
- amministratori corrotti;
- gruppi criminali;
- reti di interessi economici.

I personaggi non dovranno essere divisi semplicemente in buoni e cattivi. Alcuni cittadini sosterranno i nuovi progetti perché promettono lavoro e servizi. Alcuni lavoratori dell’organizzazione antagonista potrebbero non conoscere le attività illegali dei dirigenti.

## Organizzazione antagonista

Non devono essere utilizzati nomi di aziende, amministrazioni o persone reali come responsabili di reati. L’organizzazione sarà completamente fittizia e ispirata genericamente a una società idrica para-pubblica o municipalizzata.

Il nome provvisorio è **AQUILA – Azienda Qualità Urbana e Idrica Laziale** e potrà essere cambiato in seguito.

Pubblicamente l’organizzazione si occupa di:

- distribuzione idrica;
- manutenzione;
- riqualificazione urbana;
- scuole;
- impianti sportivi;
- iniziative ambientali.

Segretamente alcuni dirigenti collaborano con politici, imprenditori e intermediari criminali per controllare la falda e sfruttare l’energia legata al Pokémon leggendario.

## Pokémon leggendario

Un Pokémon leggendario vive nel sistema profondo del Lago Albano e della falda vulcanica.

Il leggendario:

- protegge l’equilibrio naturale;
- percepisce le alterazioni della falda;
- non appartiene agli esseri umani;
- non distingue semplicemente persone buone e cattive;
- reagisce quando qualcuno tenta di controllare completamente l’acqua;
- sarà raggiungibile tramite la storia parallela.

Il design definitivo, il nome e il tipo non sono ancora decisi.

## Starter

Saranno presenti tre nuovi starter, ciascuno con tre stadi evolutivi. I nomi provvisori approvati sono:

- Cingerm, Rovasco e Selvazanna per la linea Erba;
- Serbrace, Vipercen e Tossivampa per la linea Fuoco;
- Ardeino, Velairone e Codairone per la linea Acqua.

Le tre linee completano la prima registrazione dati. Nel prototipo Emerald Cingerm, Serbrace e Ardeino occupano rispettivamente gli slot Erba, Fuoco e Acqua e sono ottenibili soltanto tramite la selezione iniziale nel laboratorio. Nico riceve sempre lo starter avvantaggiato e Lia quello rimanente; soltanto Nico usa la squadra della prima battaglia. FireRed e LeafGreen conservano Bulbasaur, Charmander e Squirtle. Tutte le nove specie usano front sprite animato, back sprite, icona e palette normale originali; le palette shiny sono ancora provvisorie. Cry, impronta, overworld e ombra continuano a riutilizzare i segnaposto documentati per ciascuna specie.

Le 46 mosse uniche presenti nei learnset per livello delle nove specie sono state sottoposte ad audit. I nomi e le descrizioni ancora inglesi sono stati localizzati, `Fangosberla` è stata conservata e `Smokescreen` è ora `Muro di Fumo`; dati tecnici, livelli, ordine e contenuto dei learnset sono rimasti invariati. La localizzazione completa di tutte le mosse del gioco resta una milestone futura.

Le concept art di Cingerm, Rovasco, Selvazanna, Serbrace, Vipercen, Tossivampa, Ardeino e Velairone sono approvate. Soltanto Codairone deve ancora ricevere una concept art definitiva; statistiche e learnset restano preliminari fino alla validazione automatica e ai test di gioco.

### Starter Erba

- cinghiale;
- tipo iniziale Erba;
- tipo finale Erba/Buio;
- ispirato ai boschi dei Castelli Romani;
- fisico, robusto e protettivo;
- il tipo Buio rappresenta astuzia, comportamento notturno e imboscate.

### Starter Fuoco

- serpente;
- tipo iniziale Fuoco;
- tipo finale Fuoco/Veleno;
- ispirato al sottosuolo vulcanico, alle fumarole e alle sostanze presenti nella falda;
- rapido e orientato all’Attacco Speciale;
- capace narrativamente di percepire condotte, pozzi e alterazioni del terreno.

### Starter Acqua

- uccello acquatico simile a un airone;
- lunga piuma o codino sulla parte posteriore della testa;
- tipo iniziale Acqua;
- tipo finale Acqua/Volante;
- ispirato alla fauna del Lago Albano;
- elegante, veloce e tecnico.

## Scelta dello starter

Durante la scelta iniziale, il giocatore dovrà poter selezionare:

- la specie;
- il sesso dello starter;
- il soprannome.

Questa funzionalità dovrà essere presente già nella prima beta.

Nel prototipo corrente il motore genera ancora normalmente il sesso dello starter: il selettore dedicato è rinviato. Il trio è funzionale e tutte le nove specie dispongono del pacchetto grafico originale minimo; shiny definitive, cry, impronte e overworld restano da completare.

## Nuova meccanica: Forma Riflesso

**Forma Riflesso** è il nome provvisorio di una trasformazione legata al rapporto tra protagonista e starter. È ispirata concettualmente al fenomeno del legame tra Ash e Greninja, ma dovrà avere funzionamento e design originali.

La trasformazione:

- sarà inizialmente disponibile per le evoluzioni finali dei tre starter;
- cambierà alcuni dettagli estetici del Pokémon;
- riprenderà caratteristiche visive del protagonista scelto;
- potrà usare colori, accessori, capelli, cappello o motivi dell’abbigliamento;
- non dovrà trasformare il Pokémon in un essere umano;
- non dovrà fornire vantaggi differenti in base al sesso del protagonista;
- avrà effetti, abilità o mosse differenti per ciascuno starter;
- sarà collegata narrativamente ai riflessi dell’acqua e alla falda.

Il funzionamento tecnico e il bilanciamento sono ancora da progettare.

## Obiettivi della prima beta

La prima beta dovrà essere una demo verticale, non una regione completa. Dovrà includere progressivamente:

- quartiere iniziale;
- casa del protagonista;
- scuola;
- campetto da basket;
- centro storico;
- tratto ispirato alla Via Appia;
- laboratorio o luogo di scelta dello starter;
- primi Pokémon selvatici;
- primo rivale;
- primi Allenatori;
- primo dungeon sotterraneo;
- introduzione della crisi idrica;
- primi indizi sull’organizzazione antagonista;
- anticipazione del leggendario;
- scelta del sesso dello starter.

La Forma Riflesso completa potrà essere sviluppata dopo aver creato una prima versione giocabile.

## Principi di sviluppo

- Procedere con modifiche piccole e verificabili.
- Mantenere sempre una versione compilabile.
- Non modificare molte funzionalità contemporaneamente.
- Usare branch separati per le funzionalità principali.
- Non effettuare force push.
- Non inviare modifiche a `upstream`.
- Documentare le decisioni importanti.
- Distinguere sempre fatti reali e finzione narrativa.
- Non attribuire reati a persone o aziende reali.
- Utilizzare grafica e musica originali quando possibile.
- Distribuire in futuro soltanto patch e non ROM complete.
