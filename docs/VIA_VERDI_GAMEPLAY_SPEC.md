# Via Verdi — specifica gameplay

**Stato:** APPROVATO MA NON IMPLEMENTATO

## 1. Scopo della milestone

La milestone **Via Verdi — Prima indagine sulle sorgenti** inizia dopo
l'incarico affidato dal Professor Lauro e termina con un checkpoint stabile
vicino a Porta Pretoria. `Route101` resta l'identificatore tecnico della mappa;
**Via Verdi** sarà il nome visibile al giocatore.

Sono già approvati il ruolo investigativo di Lia, la presenza breve di Nico
senza una seconda battaglia, i tre rilevamenti, le 10 Poké Ball e la
destinazione Porta Pretoria. Posizioni, testi definitivi, fauna, allenatori,
strumenti e regia puntuale richiedono ancora approvazione o il blockout del
mapper.

## 2. Flusso del giocatore

1. Lauro consegna esattamente 10 Poké Ball dopo aver affidato l'incarico.
2. Il giocatore entra su Via Verdi e Lia introduce il metodo di rilevamento.
3. Il primo punto mostra un canale quasi asciutto.
4. Si apre una fase libera di esplorazione e cattura.
5. Il secondo punto mostra un comportamento anomalo dei Pokémon.
6. Nico compare brevemente, con tono competitivo ma senza combattere.
7. Una deviazione facoltativa offre esplorazione e una ricompensa.
8. Il terzo punto evidenzia canalizzazioni antiche.
9. Un'anomalia finale collega i tre indizi senza spiegarne ancora la causa.
10. Il giocatore raggiunge Porta Pretoria e attiva un checkpoint stabile.

La libertà di cattura, gli allenatori e la deviazione non devono bloccare il
percorso essenziale.

## 3. Consegna delle Poké Ball

- Quantità atomica: **10 Poké Ball**, una sola volta.
- La consegna avviene nel flusso canonico del laboratorio dopo l'incarico e
  l'eventuale consegna del Pokédex, prima di marcare conclusa l'assegnazione.
- Prima di `giveitem ITEM_POKE_BALL, 10` deve essere eseguito
  `checkitemspace ITEM_POKE_BALL, 10`: `giveitem` da solo può aggiungere una
  quantità parziale quando lo spazio è insufficiente.
- Con borsa piena o spazio parziale non si consegna nulla, si mostra un
  messaggio italiano e si lascia disponibile un nuovo tentativo.
- Lo stato persistente viene avanzato solo dopo la consegna completa. Reload e
  interazioni ripetute non devono duplicare gli oggetti.
- La ricompensa non dipende dall'esito della battaglia con Nico: vittoria e
  sconfitta convergono già nello stesso flusso.

`FLAG_ALBERA_WATER_RESEARCH_STARTED` indica oggi che l'incarico è iniziato, ma
non distingue una consegna riuscita da una borsa piena. È quindi consigliato un
flag dedicato alla consegna, oppure uno stato sequenziale equivalente. L'ID
numerico sarà assegnato solo dopo l'audit definitivo degli slot Emerald e FRLG.
Il dialogo definitivo non è ancora approvato.

## 4. Tre rilevamenti

### Rilevamento 1 — canale quasi asciutto

- **Scopo narrativo:** trasformare l'anomalia domestica in un problema
  osservabile sul territorio.
- **Informazione:** la portata è molto inferiore al normale, senza stabilirne
  la causa.
- **Spazio:** bordo leggibile del canale, area sicura per giocatore e Lia e una
  via libera dopo la scena.
- **Personaggi:** Lia e giocatore.
- **Trigger:** coordinata o breve linea trasversale al percorso principale,
  definita dopo il blockout.
- **Stato:** da incarico/consegna completati a rilevamento 1 concluso.
- **Salvataggio:** possibile subito dopo il rilascio dei controlli.
- **Ripetibilità:** scena una sola volta; interazione successiva ridotta a un
  promemoria opzionale.
- **Rischi:** trigger aggirabile, Lia su una cella bloccata, stato aggiornato
  troppo presto o movimento incompatibile con più direzioni d'arrivo.
- **Dipendenza:** geometria del canale, corridoio e collisioni del mapper.

### Rilevamento 2 — comportamento anomalo dei Pokémon

- **Scopo narrativo:** mostrare che il fenomeno influenza anche la fauna.
- **Informazione:** comportamento insolito e localizzato, non aggressione né
  spiegazione definitiva.
- **Spazio:** radura o margine d'erba con linea visiva, spazio per Lia e per la
  comparsa/uscita di Nico.
- **Personaggi:** Lia, giocatore e Nico per un intervento breve.
- **Trigger:** evento sequenziale attivo solo dopo il primo rilevamento.
- **Stato:** rilevamento 1 concluso → rilevamento 2 e Nico visto.
- **Salvataggio:** possibile prima e dopo la scena, mai durante movimenti
  bloccanti.
- **Ripetibilità:** nessuna seconda battaglia e nessuna ripetizione della
  comparsa dopo il reload.
- **Rischi:** Nico che blocca un'uscita, collisioni di NPC, ordine errato o
  riattivazione al rientro.
- **Dipendenza:** radura, percorsi di entrata e uscita e celle di sosta.

### Rilevamento 3 — canalizzazioni antiche

- **Scopo narrativo:** collegare l'indagine alla storia idraulica di Albèra
  Storica.
- **Informazione:** le opere antiche mostrano tracce coerenti con i primi due
  indizi; la causa resta aperta.
- **Spazio:** elemento architettonico leggibile, area di osservazione e
  passaggio verso Porta Pretoria.
- **Personaggi:** Lia e giocatore; Nico non è necessario.
- **Trigger:** evento sequenziale attivo solo dopo il secondo rilevamento.
- **Stato:** rilevamento 2 → rilevamento 3 → anomalia finale → checkpoint.
- **Salvataggio:** consentito tra rilevamento e anomalia solo se entrambi gli
  stati ricostruiscono correttamente la scena; altrimenti si usa una singola
  sequenza breve e atomica.
- **Ripetibilità:** effetti e dialogo principali una sola volta.
- **Rischi:** effetti che ripartono al caricamento, uscita prematura, trigger
  sovrapposti o checkpoint impostato prima che il percorso sia sicuro.
- **Dipendenza:** posizione delle canalizzazioni, accesso finale e confine con
  Porta Pretoria.

Gli script futuri useranno `lockall`, movimenti con `waitmovement`, eventuali
`playse`/`waitse`, camera tramite `LOCALID_CAMERA` e infine `releaseall`. Gli
effetti di camera o tremore sono opzioni tecniche, non una regia già approvata.

## 5. Nico e Lia

**Lia** guida l'indagine con tono calmo e preciso. Insegna un metodo, lascia al
giocatore l'osservazione e non risolve il mistero da sola. Non combatte.

**Nico** compare brevemente e conserva il tono competitivo, ma non innesca una
seconda battaglia e non trasforma la scena in un conflitto. La sua presenza
deve poter essere registrata separatamente dal progresso obbligatorio se il
layout rende la comparsa facoltativa.

## 6. Fauna — requisiti di design

- Livelli iniziali 2–5, o comunque coerenti con uno starter al livello 5.
- Varietà sufficiente per iniziare una squadra, senza catture obbligatorie.
- Almeno una scelta utile ma non indispensabile per ciascuno starter.
- Nessun counter perfetto richiesto e nessuna specie rara troppo forte.
- Coerenza con vegetazione, sorgenti e ambiente urbano-collinare.
- Uscita prevista al livello 7–8 senza grinding.

La tabella attuale di `MAP_ROUTE101` in `src/data/wild_encounters.json` usa
Wurmple, Poochyena e Zigzagoon ai livelli 2–3, solo su terra. Gli incontri per
fascia oraria sono disattivati da `OW_TIME_OF_DAY_ENCOUNTERS`.

### Proposte da approvare

1. **Conservativa:** Wurmple, Poochyena, Zigzagoon e Lotad. Mantiene l'identità
   Hoenn e aggiunge un legame con l'acqua; rischio: varietà tattica limitata.
2. **Sorgenti:** Lotad, Surskit, Seedot e Zigzagoon. Comunica bene il bioma e
   offre ruoli diversi; rischio: concentrazione di tipi Coleottero/Erba/Acqua e
   disponibilità da bilanciare fra gli starter.
3. **Urbano-collinare:** Taillow, Shroomish, Poochyena e Marill. Offre varietà
   e un tono di margine cittadino; rischio: Marill e Shroomish potrebbero essere
   troppo desiderabili o richiedere percentuali contenute.

Nessuna proposta è canonica o autorizza una modifica agli incontri.

## 7. Allenatori — requisiti

Sono previsti due allenatori facoltativi, evitabili grazie a corridoi laterali
o linee di vista leggibili. Ognuno usa 1–2 Pokémon circa ai livelli 3–5, senza
strategie punitive, e insegna rispettivamente lo scouting delle lotte e la
gestione di una squadra iniziale. Devono contribuire al livello 7–8 senza
essere necessari.

Archetipi da approvare:

- giovane naturalista che osserva il canale, con un solo Pokémon comune;
- studentessa in sopralluogo, con due Pokémon deboli e complementari;
- camminatore locale sulla deviazione, con una squadra semplice e aggirabile.

L'implementazione futura dovrà aggiungere costanti nominali in
`include/constants/opponents.h`, squadre in `src/data/trainers.party`, testi
pre/post lotta, script `trainerbattle_single` e object event. Esistono soltanto
nove slot Emerald prima di `MAX_TRAINERS_COUNT_EMERALD`; due nuovi trainer
consumerebbero due slot e richiedono verifica comune Emerald/FRLG. Nessun ID è
assegnato in questa specifica.

## 8. Strumenti — requisiti

Servono almeno una ricompensa evidente e una sulla deviazione facoltativa.
Opzioni da approvare:

- Pozione visibile sul percorso e Antidoto nascosto sulla deviazione;
- Antidoto visibile e Cura Paralisi sulla deviazione;
- Pozione visibile e una seconda Pozione nascosta, soluzione più conservativa.

Le item ball usano un object event con `Common_EventScript_FindItem`; gli
oggetti nascosti usano un `bg_event` di tipo `hidden_item`. Entrambi richiedono
flag univoci, celle raggiungibili e nessun blocco del corridoio obbligatorio.

## 9. Progressione livelli

| Profilo | Attività | Uscita attesa |
| --- | --- | --- |
| Essenziale | Percorso e tre rilevamenti, nessuna lotta facoltativa | circa 5–6 |
| Normale | Alcuni incontri/catture e un allenatore | circa 7 |
| Completista | Deviazione, due allenatori e più incontri | circa 8 |

Il percorso non richiede grinding, catture o lotte obbligatorie. La curva
generale di Ausonia resta 5–16 prima delle evoluzioni al livello 16.

## 10. Stati narrativi

Modello astratto, senza ID numerici:

1. incarico ricevuto;
2. Poké Ball consegnate;
3. rilevamento 1;
4. rilevamento 2;
5. Nico visto;
6. rilevamento 3;
7. anomalia finale;
8. checkpoint Porta Pretoria.

Una variabile sequenziale è adatta agli eventi obbligatori in ordine. Flag
separati sono preferibili per la consegna atomica, la comparsa di Nico se
facoltativa e altri fatti indipendenti. `VAR_ALBERA_OPENING_STATE` arriva già
al checkpoint iniziale di Route101 e non va estesa senza una verifica di
compatibilità. Gli ID saranno scelti solo dopo l'audit definitivo degli slot.

## 11. Errori e casi limite

- Interazioni ripetute con Lauro non duplicano le 10 Poké Ball.
- Borsa piena o parzialmente libera lascia intatta la quantità e consente il
  retry.
- Uscita, rientro e reload ricostruiscono personaggi e stato corretto.
- Il salvataggio fra i rilevamenti non ripete effetti né salta informazioni.
- I rilevamenti non partono fuori ordine e i trigger già consumati restano
  dormienti.
- La sconfitta contro un allenatore facoltativo usa il normale ritorno al
  checkpoint e non altera l'indagine.
- Il ritorno ad Albèra Bassa resta possibile e non riattiva il prologo.
- I salvataggi della ROM stabile precedente richiedono una migrazione esplicita:
  l'assenza del nuovo flag non deve azzerare `VAR_ALBERA_OPENING_STATE` né
  riprodurre automaticamente eventi già completati.
- Gli eventi legacy di Route101 restano conservati ma dormienti nel flusso
  canonico.
- Nessuna cattura effettuata non impedisce la conclusione.

## 12. Criteri di accettazione

- Il nome visibile è Via Verdi; gli identificatori tecnici restano Route101.
- Lauro consegna 10 Poké Ball esatte, atomiche e una sola volta.
- Tutti e tre gli starter e gli esiti precedenti contro Nico portano allo
  stesso flusso.
- I tre rilevamenti avvengono in ordine e persistono attraverso salvataggio e
  caricamento.
- Lia guida senza combattere; Nico compare senza una seconda battaglia.
- Esplorazione, catture, allenatori e deviazione sono facoltativi.
- Nessun trigger, movimento, collisione o rientro causa softlock.
- Fauna e ricompense rispettano la curva 5–8 senza grinding.
- Gli eventi vanilla conservati non si attivano nel percorso canonico.
- Il checkpoint presso Porta Pretoria è stabile e permette ritorno e reload.
- Nuova partita e salvataggio stabile precedente seguono una politica di
  migrazione verificata.

## 13. Dipendenze dal mapper

La draft PR `map/via-verdi-layout`, di proprietà di `dsalvagno1994-bot`, deve
fornire prima della finalizzazione:

- coordinate e direzioni d'ingresso dei trigger;
- corridoi obbligatori e percorsi alternativi;
- collisioni ed elevation;
- spazi sicuri per Lia, Nico, giocatore e camera;
- aree d'erba e distribuzione degli incontri;
- forma e ricompensa della deviazione;
- entrata da Albèra Bassa e uscita verso Porta Pretoria;
- posizione e leggibilità dei tre rilevamenti;
- conseguenze del resize previsto a 36×20 su connessioni, eventi legacy e
  coordinate esistenti.

Finché questi dati non sono disponibili, nessuna coordinata, trainer, oggetto
o movimento deve essere considerato definitivo.

## Audit tecnico di integrazione

Nel laboratorio, il punto d'inserimento consigliato è fra
`LittlerootTown_ProfessorBirchsLab_EventScript_LiaDetectsAnomaly` e
`LittlerootTown_ProfessorBirchsLab_EventScript_CompleteAlberaOpening`: prima
del flag dell'incarico, dello stato `5` e dell'uscita di Nico e Lia. Il vecchio
fallback `LittlerootTown_ProfessorBirchsLab_EventScript_GivePokedex` consegna
invece 5 Poké Ball ed è un flusso legacy, non il modello canonico da copiare.
Le macro e i messaggi standard sono in `asm/macros/event.inc` e
`data/scripts/obtain_item.inc`; va scelta una variante italiana già valida o
aggiunto in futuro un solo messaggio locale.

Il checkpoint attuale è esplicito: `Route101_OnTransition` trasforma
`VAR_ALBERA_OPENING_STATE` da `5` a `6`. Il laboratorio imposta
`VAR_ROUTE101_STATE` a `3` e nasconde borsa, Lauro, Zigzagoon e rivale legacy;
per questo i trigger vanilla ai valori `1` e `2` restano conservati ma
dormienti. Il resize 36×20 dovrà rivalidare connessioni, bordi, trigger,
movimenti e oggetti senza cambiare questa garanzia.

| Area | File e simboli | Dipendenza/rischio | Decisione necessaria | Azione futura consigliata |
| --- | --- | --- | --- | --- |
| Incarico | `data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc`, `LittlerootTown_ProfessorBirchsLab_EventScript_LiaDetectsAnomaly`, `LittlerootTown_ProfessorBirchsLab_EventScript_CompleteAlberaOpening` | `giveitem` può consegnare parzialmente; lo stato attuale non distingue la ricompensa | flag dedicato o nuovo gradino sequenziale; testo di borsa piena | preflight con `checkitemspace`, stato dedicato e retry italiano |
| Nome | `src/data/region_map/region_map_sections.json`, `MAPSEC_ROUTE_101`; `data/maps/Route101/scripts.inc`, `Route101_Text_RouteSign`; `src/region_map.c` | rinominare simboli romperebbe riferimenti e Town Map | insieme esatto dei testi visibili da localizzare | cambiare popup/Town Map/cartello; mantenere `MAP_ROUTE101` e `MAPSEC_ROUTE_101` |
| Incontri | `src/data/wild_encounters.json`, `gRoute101`; `include/config/overworld.h` | curva troppo bassa/monotona o fauna troppo forte | composizione, percentuali e livelli approvati | calibrare gli slot dopo la consegna del layout |
| Trainer | `include/constants/opponents.h`; `src/data/trainers.party`; esempi `data/maps/Route102/` | spazio ID Emerald limitato e compatibilità FRLG | archetipi, squadre e consumo di due slot | allocare ID solo dopo audit e usare trainer evitabili |
| Oggetti | `Common_EventScript_FindItem`; object/bg event in Route102/Route104; `include/constants/flags*.h` | flag duplicati o oggetto irraggiungibile | coppia di ricompense e tipo visibile/nascosto | assegnare flag univoci e validare collisioni dopo il blockout |
| Rilevamenti | coord event `trigger` in `map.json`; `lockall`, `applymovement`, `LOCALID_CAMERA`, `playse` | ordine, reload e movimenti dipendono dalle celle reali | coordinate, regia ed effetti da usare | sequenza persistente, scene brevi e ricostruzione OnTransition |
| Checkpoint | `data/maps/Route101/scripts.inc`, `Route101_OnTransition`, `Route101_EventScript_SetAlberaCheckpoint`; `data/scripts/prof_birch.inc`, `ProfBirch_EventScript_UpdateLocation` | salvataggi esistenti, eventi legacy e resize 36×20 | politica di migrazione dei salvataggi stabili | mantenere stati legacy dormienti e ribasare solo col mapper |
