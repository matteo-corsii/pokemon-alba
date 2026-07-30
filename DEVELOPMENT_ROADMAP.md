# Roadmap di sviluppo di Pokémon Alba

Questa roadmap descrive una sequenza iniziale di attività. Ogni fase deve produrre un risultato verificabile prima di passare alla successiva. I dettagli tecnici e gli eventuali comandi saranno definiti solo dopo essere stati verificati sulla documentazione e sull’ambiente effettivamente utilizzato.

**Stato delle milestone:** è in validazione il primo segmento canonico giocabile di Albèra: anomalia domestica, convocazione di Lauro, scelta del trio nel laboratorio, distribuzione a Nico e Lia, battaglia con Nico, incarico e checkpoint su Route 101. FireRed e LeafGreen restano compatibili e conservano i propri starter. Tutte le nove specie possiedono front animato, back, icona e palette normale originali; shiny, cry, impronte e overworld non sono ancora definitivi. Le 46 mosse uniche dei nove learnset per livello restano localizzate senza cambiamenti tecnici.

## Prossima milestone approvata: Via Verdi — Prima indagine sulle sorgenti

Questa milestone è **approvata ma non ancora implementata**. `Route101` conserva
identificatori e directory tecniche; **Via Verdi** sarà il nome mostrato al
giocatore, scelto per il richiamo a Giuseppe Verdi e al paesaggio verde.

Il segmento collegherà Albèra Bassa a Porta Pretoria con un percorso verde e
collinare segnato da sorgenti, canaletti, terreno umido e canalizzazioni
antiche. Lauro consegnerà esattamente **10 Poké Ball**. Le catture saranno
facoltative e non è previsto un tutorial lungo o obbligatorio. Lia guiderà
l'indagine; Nico comparirà brevemente senza una seconda battaglia.

La progressione comprenderà tre punti concettuali di rilevazione: un canale o
una sorgente quasi asciutta con terreno ancora umido; un'area attraversata o
evitata in modo anomalo dai Pokémon; un'antica canalizzazione vicina a Porta
Pretoria. Nella conclusione, un suono profondo e un lieve tremore accompagneranno
una variazione o un arresto del flusso, seguiti dalla ripresa improvvisa
dell'acqua. Non sono ancora approvati coordinate, testi, flag, variabili o
script. Sono esclusi antagonisti, leggendari, disastri dichiarati e la prima
Palestra, che appartiene alla successiva Albèra Storica.

Progressione di livello approvata:

- starter iniziale: livello 5;
- Via Verdi: livelli 5–8;
- preparazione ad Albèra Storica e prima Palestra: livelli 8–10;
- prima Palestra: media livelli 9–10, asso livello 11;
- dopo la Medaglia: livelli 11–12;
- Via Consolare e Lago di Albèra: livelli 12–16;
- prima evoluzione al livello 16 e seconda evoluzione al livello 36.

Il bilanciamento non deve imporre grinding, contatori obbligatori, catture o
Allenatori necessari: incontri e sfide aggiuntive devono restare facoltativi.

## 1. Verificare la compilazione pulita del progetto base

**Esito della verifica:** la compilazione pulita è stata verificata con successo tramite GitHub Actions; tutti i job previsti dal workflow ufficiale sono riusciti e nessuna ROM è stata pubblicata come artefatto.

**Obiettivo:** stabilire una base tecnica affidabile, senza modifiche funzionali rispetto a `pokeemerald-expansion`.

**Risultato verificabile:** il progetto base completa la compilazione prevista e il relativo risultato viene registrato senza aggiungere contenuti di gioco.

**Rischi principali:** ambiente Windows incompleto, versioni degli strumenti incompatibili, configurazione locale diversa da quella documentata dal progetto.

**Condizione per procedere:** ottenere una compilazione ripetibile oppure documentare con precisione un impedimento esterno che deve essere risolto.

## 2. Configurare la compilazione automatica tramite GitHub Actions

**Obiettivo:** rendere la verifica di compilazione ripetibile e indipendente dal singolo computer locale.

**Risultato verificabile:** una workflow avviata su GitHub completa con successo la compilazione del progetto base.

**Rischi principali:** configurazione errata della workflow, strumenti non disponibili nell’ambiente del runner, gestione impropria di materiali protetti.

**Condizione per procedere:** la workflow deve essere stabile, usare soltanto input leciti e non pubblicare ROM.

## 3. Creare una modifica testuale minima

**Esito del test manuale:** la prima ROM personale è stata compilata con successo e avviata con mGBA 0.10.5. La frase personalizzata è stata verificata senza caratteri corrotti o testo tagliato; avvio, comandi, audio e scelta del nome funzionano correttamente.

**Esito dell’apertura italiana:** l’interfaccia iniziale e l’introduzione tradotta sono state compilate e collaudate manualmente con successo, senza errori grafici, testi tagliati o problemi funzionali.

**Esito del secondo collaudo del prologo:** il percorso è stato completato con entrambi i protagonisti senza blocchi o problemi di progressione. Il collaudo ha rilevato e corretto il testo tagliato di Via dell'Armonia, i messaggi residui di battaglia, i messaggi globali di consegna degli strumenti e il normale flusso di salvataggio. Le correzioni non modificano le relative meccaniche e attendono la validazione CI e la build privata finale.

**Esito del collaudo finale:** sono stati individuati messaggi residui in inglese nella sequenza di sconfitta e alcuni nomi inglesi fra le mosse iniziali degli starter temporanei. È stata preparata la correzione conclusiva della localizzazione del prologo, in attesa della validazione CI e del collaudo della nuova ROM.

**Obiettivo:** verificare l’intero ciclo di modifica del gioco con un cambiamento semplice e facilmente riconoscibile.

**Risultato verificabile:** una singola stringa scelta per il test appare correttamente nel gioco compilato.

**Rischi principali:** modificare la stringa sbagliata, introdurre problemi di codifica o includere cambiamenti non necessari.

**Condizione per procedere:** il diff deve essere minimo, comprensibile e compilare correttamente.

## 4. Produrre automaticamente una ROM di test come artefatto privato della workflow

**Obiettivo:** rendere disponibile ai soli collaboratori autorizzati un risultato di test della compilazione automatica.

**Risultato verificabile:** la workflow produce l’artefatto previsto con accesso limitato e durata di conservazione definita.

**Rischi principali:** esposizione pubblica involontaria, distribuzione impropria di una ROM completa, conservazione eccessiva degli artefatti.

**Condizione per procedere:** verificare licenze e policy applicabili; confermare che accesso, conservazione e distribuzione siano configurati in modo appropriato. Per la distribuzione futura al pubblico saranno usate soltanto patch, non ROM complete.

## 5. Installare e configurare Porymap su Windows

**Obiettivo:** predisporre l’editor di mappe nell’ambiente Windows scelto per il progetto.

**Risultato verificabile:** Porymap apre correttamente il progetto e legge le mappe esistenti senza alterazioni inattese.

**Rischi principali:** versione incompatibile, percorsi configurati male, salvataggi automatici o cambiamenti di formato non desiderati.

**Condizione per procedere:** apertura e salvataggio di controllo devono essere compresi e deve esistere una procedura sicura per esaminare i diff prodotti.

## 6. Creare la prima mappa di prova

**Obiettivo:** apprendere il flusso di modifica delle mappe con un’area sperimentale priva di contenuti definitivi.

**Risultato verificabile:** una piccola mappa di test è raggiungibile, visualizzata correttamente e priva di blocchi evidenti del movimento.

**Rischi principali:** collegamenti errati, collisioni incoerenti, uso prematuro di risorse definitive.

**Condizione per procedere:** la mappa deve compilare, caricarsi ed essere verificabile separatamente dalle aree narrative definitive.

## 7. Pianificare la mappa del quartiere iniziale

**Obiettivo:** tradurre Via Donizetti, Via Rossini, scuola, campetto, abitazioni e area verde in una geografia GBA semplificata.

**Risultato verificabile:** esiste una planimetria approvata con percorsi, punti di interesse, transizioni e scala di massima.

**Rischi principali:** fedeltà geografica eccessiva, area troppo grande, flusso di gioco poco leggibile, uso improprio di nomi reali.

**Condizione per procedere:** il layout deve sostenere il prologo e restare realizzabile con le risorse disponibili.

## 8. Aggiungere un primo Pokémon temporaneo di test

**Stato:** le linee Cingerm, Serbrace e Ardeino sono registrate come prototipi dati. Le tre forme base sono ottenibili soltanto nei rispettivi slot starter Emerald; Nico usa lo starter avvantaggiato nella prima lotta e Lia riceve narrativamente quello rimanente senza combattere in questa milestone. Gli stadi evoluti restano raggiungibili tramite evoluzione o nei rami futuri già registrati; nessuna delle nove specie compare negli incontri selvatici. Tutte usano front animato, back, icona e palette normale originali, con shiny, cry, impronta, overworld e ombra ancora provvisori. I learnset restano invariati.

**Obiettivo:** verificare il processo tecnico per introdurre una nuova specie usando risorse chiaramente provvisorie.

**Risultato verificabile:** il Pokémon di test è definito, incontrabile in un contesto controllato e non altera specie non correlate.

**Rischi principali:** conflitti negli identificatori, dati incompleti, grafica provvisoria confusa con materiale definitivo, problemi di bilanciamento irrilevanti in questa fase.

**Condizione per procedere:** l’aggiunta deve essere isolata, documentata e sostituibile senza propagare dipendenze non necessarie.

## 9. Progettare gli starter definitivi

**Stato:** sono registrati i nomi e i parametri tecnici preliminari delle nove specie. Le concept art di Cingerm, Rovasco, Selvazanna, Serbrace, Vipercen, Tossivampa, Ardeino e Velairone sono approvate; soltanto Codairone deve ancora ricevere una concept art definitiva.

**Obiettivo:** definire identità, linee evolutive, tipi, ruoli di gioco e direzione artistica dei tre starter.

**Risultato verificabile:** ciascuna linea possiede una scheda approvata con tre stadi, silhouette, tipi, ruolo e collegamento narrativo alla regione.

**Rischi principali:** design poco distinguibili, ruoli sbilanciati, somiglianze eccessive con creature esistenti, espansione prematura dell’ambito.

**Condizione per procedere:** i tre concept devono essere coerenti tra loro e sufficientemente definiti per pianificare dati e risorse originali.

## 10. Implementare la scelta del sesso dello starter

**Stato:** rinviata. Il prototipo Cingerm usa il comportamento standard del motore e non aggiunge ancora un selettore del sesso.

**Obiettivo:** permettere al giocatore di scegliere specie, sesso e soprannome dello starter durante la sequenza iniziale.

**Risultato verificabile:** tutte le combinazioni previste producono lo starter corretto e la scelta viene conservata senza alterare vantaggi di gioco in base al sesso del protagonista.

**Rischi principali:** percorsi di dialogo incompleti, valori non salvati correttamente, casi limite nell’assegnazione o nel soprannome.

**Condizione per procedere:** i percorsi della scelta devono essere testati e integrarsi con gli eventi successivi del prologo.

## 11. Sviluppare eventi e dialoghi del prologo

**Stato:** il flusso successivo all'arrivo ad Albèra è stato riallineato al canone. Il primo segnale è domestico; Lauro convoca il protagonista nel laboratorio, assegna i tre starter al giocatore, a Nico e a Lia, e dopo la lotta amichevole con Nico affida il sopralluogo. Lia non combatte e individua la nuova variazione. Route 101 viene sbloccata come checkpoint stabile. Restano da completare CI e collaudo manuale dei tre percorsi.

**Obiettivo:** introdurre protagonista, quartiere, rivale, starter e primi segnali della crisi idrica attraverso il gioco.

**Risultato verificabile:** il prologo è percorribile dall’inizio alla conclusione prevista, con eventi coerenti e dialoghi revisionati.

**Rischi principali:** esposizione narrativa troppo lunga, blocchi negli eventi, tono inadatto, confusione tra riferimenti reali e finzione.

**Condizione per procedere:** il prologo deve essere comprensibile, giocabile e mostrare il tema anche attraverso l’ambiente.

## 12. Prototipare la Forma Riflesso

**Obiettivo:** valutare fattibilità tecnica, identità visiva e ruolo ludico della meccanica senza impegnarsi subito nell’implementazione definitiva.

**Risultato verificabile:** un prototipo controllato dimostra attivazione, trasformazione, ritorno allo stato normale e un effetto di gioco chiaramente definito.

**Rischi principali:** complessità tecnica elevata, proliferazione delle risorse grafiche, problemi di bilanciamento, somiglianza eccessiva con meccaniche esistenti.

**Condizione per procedere:** il prototipo deve dimostrare valore narrativo e ludico, essere sostenibile per tutti e tre gli starter e avere regole originali documentate.
