# Via Verdi — piano di test

**Stato:** PIANIFICATO — NON ESEGUITO

Questo documento descrive la validazione della futura implementazione. Non
registra risultati e non implica che gameplay, mappa o script siano già stati
modificati.

## A. Regressioni del prologo

- [ ] L'anomalia domestica avviene una volta e senza crash.
- [ ] La convocazione di Lauro e l'accesso al laboratorio funzionano.
- [ ] Cingerm, Serbrace e Ardeino sono selezionabili senza duplicazioni.
- [ ] La battaglia contro Nico parte nella posizione corretta.
- [ ] Vittoria e sconfitta convergono nello stesso flusso.
- [ ] Nessun esito causa crash, falsa fuga o ripetizione della lotta.
- [ ] Il vecchio evento del campetto non si attiva in anticipo.
- [ ] Il checkpoint iniziale di Route101 resta raggiungibile e stabile.

## B. 10 Poké Ball

- [ ] Lauro consegna esattamente 10 Poké Ball una sola volta.
- [ ] Interazione ripetuta, uscita/rientro e reload non creano duplicati.
- [ ] Con spazio insufficiente non viene aggiunta una quantità parziale.
- [ ] Dopo aver liberato spazio, il retry consegna tutte e 10 le Poké Ball.
- [ ] Il messaggio di borsa piena è corretto e in italiano.
- [ ] Salvataggio e caricamento conservano lo stato della consegna.
- [ ] Il test passa con tutti e tre gli starter.
- [ ] Il test passa sia dopo vittoria sia dopo sconfitta contro Nico.

## C. Via Verdi

- [ ] Il nome visibile è Via Verdi su popup, Town Map e cartello previsto.
- [ ] Ingresso, uscita e ritorno verso Albèra Bassa sono percorribili.
- [ ] Collisioni, elevation, bordi e connessioni non creano softlock.
- [ ] Gli eventi legacy di Route101 restano dormienti.
- [ ] I tre rilevamenti avvengono nell'ordine previsto.
- [ ] Trigger consumati non si ripetono dopo rientro o reload.
- [ ] Salvare tra i rilevamenti ricostruisce NPC e stato corretti.
- [ ] Lia introduce e accompagna l'indagine senza risolverla da sola.
- [ ] Nico compare una volta e non avvia una seconda battaglia.
- [ ] Deviazione, erba e ricompense restano facoltative.

## D. Fauna e livelli

- [ ] Le specie incontrate corrispondono alla tabella approvata.
- [ ] Livelli e percentuali sono adatti a uno starter al livello 5.
- [ ] Cattura e fuga funzionano per ogni specie e in ogni area d'erba.
- [ ] Nessuna cattura è richiesta per proseguire.
- [ ] Profilo essenziale: percorso completabile senza grinding.
- [ ] Profilo normale: uscita circa al livello 7.
- [ ] Profilo completista: uscita circa al livello 8 senza sovralivellamento.
- [ ] Tutti e tre gli starter dispongono di opzioni utili ma non obbligatorie.

## E. Trainer e oggetti

- [ ] Entrambi gli allenatori sono facoltativi ed evitabili in modo leggibile.
- [ ] Squadre, livelli e IA non sono punitivi.
- [ ] Vittoria e sconfitta non bloccano rilevamenti o corridoi.
- [ ] Le ricompense monetarie sono coerenti con le classi scelte.
- [ ] L'oggetto evidente e quello sulla deviazione sono raggiungibili.
- [ ] Ogni strumento si ottiene una sola volta e persiste dopo reload.
- [ ] Item ball e hidden item non bloccano il passaggio.

## F. Checkpoint finale

- [ ] L'anomalia finale conduce vicino a Porta Pretoria.
- [ ] Il checkpoint si attiva solo dopo i tre rilevamenti.
- [ ] È possibile salvare e caricare senza ripetere la sequenza.
- [ ] Il ritorno su Via Verdi e ad Albèra Bassa conserva lo stato.
- [ ] Nessun NPC o trigger impedisce di proseguire o tornare indietro.

## G. Compatibilità

- [ ] Una nuova partita completa l'intero flusso.
- [ ] Un salvataggio della ROM stabile precedente viene migrato senza perdere
  progresso, duplicare Poké Ball o riattivare eventi legacy.
- [ ] Il test non dipende da savestate; i salvataggi normali sono sufficienti.
- [ ] La CI compila Emerald, FireRed e LeafGreen quando previsto dal workflow.
- [ ] Le nuove costanti comuni, se introdotte, non collidono fra Emerald e
  FRLG.
