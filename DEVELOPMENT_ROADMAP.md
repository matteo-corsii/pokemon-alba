# Roadmap di sviluppo di Pokémon Alba

Questa roadmap descrive una sequenza iniziale di attività. Ogni fase deve produrre un risultato verificabile prima di passare alla successiva. I dettagli tecnici e gli eventuali comandi saranno definiti solo dopo essere stati verificati sulla documentazione e sull’ambiente effettivamente utilizzato.

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

**Milestone in preparazione:** è iniziato il pacchetto di identità narrativa. I nomi provvisori di regione, città, Professore e rivali sono stati stabiliti; in questa fase vengono sostituiti soltanto i testi visibili, mantenendo invariati gli identificatori interni originali. La milestone non sarà considerata completa prima della compilazione e del test.

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

**Obiettivo:** verificare il processo tecnico per introdurre una nuova specie usando risorse chiaramente provvisorie.

**Risultato verificabile:** il Pokémon di test è definito, incontrabile in un contesto controllato e non altera specie non correlate.

**Rischi principali:** conflitti negli identificatori, dati incompleti, grafica provvisoria confusa con materiale definitivo, problemi di bilanciamento irrilevanti in questa fase.

**Condizione per procedere:** l’aggiunta deve essere isolata, documentata e sostituibile senza propagare dipendenze non necessarie.

## 9. Progettare gli starter definitivi

**Obiettivo:** definire identità, linee evolutive, tipi, ruoli di gioco e direzione artistica dei tre starter.

**Risultato verificabile:** ciascuna linea possiede una scheda approvata con tre stadi, silhouette, tipi, ruolo e collegamento narrativo alla regione.

**Rischi principali:** design poco distinguibili, ruoli sbilanciati, somiglianze eccessive con creature esistenti, espansione prematura dell’ambito.

**Condizione per procedere:** i tre concept devono essere coerenti tra loro e sufficientemente definiti per pianificare dati e risorse originali.

## 10. Implementare la scelta del sesso dello starter

**Obiettivo:** permettere al giocatore di scegliere specie, sesso e soprannome dello starter durante la sequenza iniziale.

**Risultato verificabile:** tutte le combinazioni previste producono lo starter corretto e la scelta viene conservata senza alterare vantaggi di gioco in base al sesso del protagonista.

**Rischi principali:** percorsi di dialogo incompleti, valori non salvati correttamente, casi limite nell’assegnazione o nel soprannome.

**Condizione per procedere:** i percorsi della scelta devono essere testati e integrarsi con gli eventi successivi del prologo.

## 11. Sviluppare eventi e dialoghi del prologo

**Obiettivo:** introdurre protagonista, quartiere, rivale, starter e primi segnali della crisi idrica attraverso il gioco.

**Risultato verificabile:** il prologo è percorribile dall’inizio alla conclusione prevista, con eventi coerenti e dialoghi revisionati.

**Rischi principali:** esposizione narrativa troppo lunga, blocchi negli eventi, tono inadatto, confusione tra riferimenti reali e finzione.

**Condizione per procedere:** il prologo deve essere comprensibile, giocabile e mostrare il tema anche attraverso l’ambiente.

## 12. Prototipare la Forma Riflesso

**Obiettivo:** valutare fattibilità tecnica, identità visiva e ruolo ludico della meccanica senza impegnarsi subito nell’implementazione definitiva.

**Risultato verificabile:** un prototipo controllato dimostra attivazione, trasformazione, ritorno allo stato normale e un effetto di gioco chiaramente definito.

**Rischi principali:** complessità tecnica elevata, proliferazione delle risorse grafiche, problemi di bilanciamento, somiglianza eccessiva con meccaniche esistenti.

**Condizione per procedere:** il prototipo deve dimostrare valore narrativo e ludico, essere sostenibile per tutti e tre gli starter e avere regole originali documentate.
