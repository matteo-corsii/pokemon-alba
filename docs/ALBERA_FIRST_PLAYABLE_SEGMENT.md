# Primo segmento giocabile di Albèra

## Flusso canonico

La milestone conserva l'introduzione precedente all'arrivo e modifica soltanto
il flusso successivo:

1. nella casa del protagonista si verifica una breve anomalia della pressione;
2. il familiare comunica la convocazione del Professor Lauro;
3. l'uscita nord di Albèra resta chiusa durante le misurazioni;
4. nel Laboratorio del Cratere sono presenti Lauro, Nico e Lia;
5. il giocatore sceglie Cingerm, Serbrace o Ardeino;
6. Nico riceve lo starter avvantaggiato e Lia quello rimanente;
7. Nico affronta il giocatore in una lotta amichevole;
8. vittoria e sconfitta convergono dopo una sola battuta diversa;
9. Lia rileva una nuova variazione proveniente da Route 101;
10. Lauro affida un sopralluogo e riapre la strada;
11. l'ingresso in Route 101 costituisce il checkpoint stabile finale.

Lauro non conosce la causa dell'anomalia. Non vengono introdotti emergenze,
antagonisti, leggendari, profezie o una battaglia contro Lia.

## Mappe e file principali

- `data/scripts/players_house.inc` avvia lo stato narrativo dopo la scena TV.
- `data/maps/LittlerootTown_BrendansHouse_1F/scripts.inc` contiene i testi
  condivisi dell'anomalia domestica.
- `data/maps/LittlerootTown/scripts.inc` gestisce il blocco diegetico dell'uscita
  e rinvia il vecchio evento obbligatorio del campetto.
- `data/maps/LittlerootTown_ProfessorBirchsLab/map.json` aggiunge gli oggetti
  simultanei di Nico e Lia.
- `data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc` gestisce scelta,
  distribuzione, battaglia, rilevazione e incarico.
- `data/maps/Route101/scripts.inc` registra il checkpoint senza avviare il
  salvataggio vanilla di Lauro.
- `src/battle_setup.c` permette allo selector esistente di ritornare al
  laboratorio senza avviare la battaglia selvaggia originale.

## Stati persistenti

| Identificatore | Valore | Ruolo |
| --- | ---: | --- |
| `FLAG_ALBERA_HOME_ANOMALY_SEEN` | `0x022` | scena domestica completata |
| `FLAG_ALBERA_NICO_BATTLE_COMPLETED` | `0x023` | battaglia introduttiva conclusa |
| `FLAG_HIDE_ALBERA_LAB_NICO` | `0x024` | visibilità di Nico nel laboratorio |
| `FLAG_HIDE_ALBERA_LAB_LIA` | `0x025` | visibilità di Lia nel laboratorio |
| `FLAG_ALBERA_WATER_RESEARCH_STARTED` | `0x021` | incarico assegnato e uscita sbloccata |
| `VAR_ALBERA_OPENING_STATE` | `0x40F7` | progressione da casa (`1`) a checkpoint (`6`) |

Gli slot sono definiti sia per Emerald sia per FRLG per mantenere la
compilazione comune. La storia è raggiungibile soltanto nel percorso Emerald;
FireRed e LeafGreen conservano i rispettivi starter e flussi iniziali.

## Distribuzione degli starter

| Scelta giocatore | Nico | Lia |
| --- | --- | --- |
| Cingerm | Serbrace | Ardeino |
| Serbrace | Ardeino | Cingerm |
| Ardeino | Cingerm | Serbrace |

`VAR_STARTER_MON` resta la fonte unica della scelta. Le squadre iniziali già
registrate di Nico vengono riutilizzate senza modificare specie, livelli o dati
di gioco. Lia riceve narrativamente la terza specie e non combatte.

## Robustezza

- Lo starter viene creato una sola volta durante lo script bloccante.
- Lo stato viene avanzato prima e dopo lo selector per impedire riaperture.
- L'ingresso nel laboratorio lascia il controllo al giocatore; una linea di
  trigger vicina al gruppo normalizza la posizione prima di iniziare la scena.
- Nico avanza verso il giocatore prima della lotta e le orientazioni del gruppo
  vengono ripristinate al ritorno dalla battaglia.
- La battaglia usa il flusso nativo `trainerbattle_earlyrival` con cura dopo
  la sconfitta; lo script cura la squadra dopo la vittoria e converge in
  entrambi gli esiti.
- Il solo flag `RIVAL_BATTLE_HEAL_AFTER` non attiva più
  `BATTLE_TYPE_FIRST_BATTLE`: la lotta resta una normale battaglia tra
  allenatori e non richiama il tutorial di Oak/FRLG.
- L'incarico imposta lo stato di Route 101 a `3`, nasconde borsa, Lauro e
  Pokémon del salvataggio vanilla, e nasconde il rivale alternativo di Route
  103 senza cancellarne gli script.
- Il checkpoint su Route 101 non blocca movimento, salvataggio o ritorno.

## Validazione automatica

Eseguire:

```powershell
./test/validate_albera_first_playable_segment.ps1
./test/validate_full_ausonia_starter_trio.ps1
git diff --check develop...HEAD
```

La CI pubblica deve inoltre completare build Emerald, FireRed, LeafGreen,
release, test e validazione della documentazione.

## Checklist manuale residua

Ripetere il segmento da una nuova partita con tre salvataggi separati:

- Cingerm → Nico Serbrace → Lia Ardeino;
- Serbrace → Nico Ardeino → Lia Cingerm;
- Ardeino → Nico Cingerm → Lia Serbrace.

Per ogni percorso verificare anomalia domestica, blocco dell'uscita, testi e
transizioni del laboratorio, sprite e icona, assenza di duplicazioni, esito di
vittoria e sconfitta, rilevazione di Lia, incarico, accesso a Route 101,
salvataggio/caricamento e ritorno ad Albèra. La verifica manuale resta aperta
fino alla build privata successiva al merge.

La regressione correttiva richiede inoltre tre prove separate della battaglia:
vittoria senza oggetti, vittoria usando una Pozione e sconfitta volontaria. In
tutti i casi verificare che Serbrace non fugga, che i messaggi di uso, recupero
PS e richiamo siano in italiano, che il flusso converga senza crash e che
rientrando nel laboratorio l'evento non riparta.

## Fuori ambito

Restano esclusi evento principale di Route 101, battaglia con Lia, crisi
dichiarata, antagonisti, leggendari, palestre, nuove aree, evoluzioni narrative,
cry, footprint, overworld e shiny definitivi.
