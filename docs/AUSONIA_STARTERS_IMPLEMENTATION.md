# Implementazione degli starter di Ausonia

Questo documento descrive l'architettura tecnica verificata nella versione corrente di `pokeemerald-expansion` e la strategia append-only scelta per i nove starter originali di Pokémon Alba.

## Architettura reale delle specie

- `include/constants/species.h` contiene l'enumerazione `SPECIES_*`, il limite dell'intervallo personalizzato, `SPECIES_EGG` e `NUM_SPECIES`.
- `include/constants/pokedex.h` contiene i numeri del Pokédex nazionale e `NATIONAL_DEX_COUNT`.
- `src/data/pokemon/species_info.h` contiene `gSpeciesInfo`: nomi, dati base, tipi, statistiche, abilità, gruppi Uova, rapporto sessi, crescita, evoluzioni, dati Pokédex e riferimenti grafici e sonori.
- Il learnset per livello è una tabella di `struct LevelUpMove` collegata da `gSpeciesInfo`. Ogni linea usa tabelle separate ma identiche per le tre forme, così ogni evoluzione potrà divergere in futuro senza modificare l'identità della specie.
- `src/data/pokemon/all_learnables.json` alimenta il generatore delle compatibilità MT e tutor. `src/data/pokemon/teachable_learnsets.h` è generato durante la build e non va modificato manualmente.
- `src/data/pokemon/egg_moves.h` contiene le mosse Uovo.
- `src/data/pokemon/pokedex_orders.h` contiene gli ordinamenti alfabetico, per peso e per altezza del Pokédex nazionale.
- Front sprite, back sprite, palette, icone, impronte, animazioni, coordinate, ombre e cry sono riferimenti di `gSpeciesInfo`; le risorse binarie sono dichiarate nelle tabelle grafiche già incluse dal progetto.
- `test/species.c` e `test/text.c` verificano rispettivamente i dati delle specie e la resa dei nomi e delle voci Pokédex. Gli strumenti in `tools/learnset_helpers/` generano le tabelle insegnabili a partire dai JSON.

La struttura `SpeciesInfo` corrente non contiene campi per habitat o forma corporea. Queste categorie non vengono quindi inventate; per i prototipi vengono registrati soltanto i colori corporei supportati più vicini.

## Strategia degli identificatori

L'ultimo identificatore ufficiale verificato è `SPECIES_GLIMMORA_MEGA = 1572`. L'intervallo personalizzato inizia subito dopo `SPECIES_CUSTOM_START`, senza inserimenti fra le specie ufficiali. La prima linea usa pertanto:

| Specie | ID specie | Numero Pokédex nazionale |
| --- | ---: | ---: |
| Cingerm | 1573 | 1026 |
| Rovasco | 1574 | 1027 |
| Selvazanna | 1575 | 1028 |
| Serbrace | 1576 | 1029 |
| Vipercen | 1577 | 1030 |
| Tossivampa | 1578 | 1031 |
| Ardeino | 1579 | 1032 |
| Velairone | 1580 | 1033 |
| Codairone | 1581 | 1034 |

Gli ID del Pokédex seguono in appendice `NATIONAL_DEX_PECHARUNT = 1025`. Nessun numero regionale iniziale viene assegnato.

Gli identificatori vengono assegnati soltanto insieme ai dati completi della relativa linea. L'ordine append-only resta vincolato come segue:

1. Cingerm
2. Rovasco
3. Selvazanna
4. Serbrace
5. Vipercen
6. Tossivampa
7. Ardeino
8. Velairone
9. Codairone

Questa strategia conserva tutti gli ID ufficiali. I salvataggi creati prima dell'aggiunta restano compatibili perché nessuna specie preesistente viene rinumerata; dopo che un ID custom è stato usato in un salvataggio, non dovrà più essere riordinato o riutilizzato.

## Dati e disponibilità dei prototipi

Tutte e tre le linee sono registrate con evoluzioni ai livelli 16 e 36, dati base completi e un learnset per livello comune a ciascuna linea. Le compatibilità MT/tutor e le mosse Uovo restano vuote tramite le tabelle `sNone*`: è una scelta prudente supportata dalla struttura e non richiede di modificare file generati. Statistiche e learnset restano preliminari fino alla validazione automatica e ai test di gioco.

Cingerm, Serbrace e Ardeino sostituiscono rispettivamente Treecko, Torchic e Mudkip nei soli tre slot starter Emerald. La selezione crea la forma base scelta al livello 5 attraverso il normale flusso `GetStarterPokemon`/`ScriptGiveMon`, lasciando al motore natura, sesso, IV, statistiche e mosse iniziali. Bulbasaur, Charmander e Squirtle restano invariati nelle configurazioni FireRed e LeafGreen.

I rami di Nico e Lia collegati alle vecchie linee di Treecko, Torchic e Mudkip usano ora gli stadi equivalenti delle linee Cingerm, Serbrace e Ardeino, conservando livelli e configurazione delle squadre. Le tre forme base non compaiono in incontri selvatici, regali aggiuntivi, commerci, Uova, NPC o strumenti di debug; gli stadi successivi sono raggiungibili soltanto tramite evoluzione o nella progressione del rivale. La selezione manuale del sesso resta rinviata.

## Stato degli asset

Gli asset originali approvati di tutte le nove specie sono conservati nelle rispettive directory sotto `graphics/pokemon/`. Ogni specie dispone di front animato a due frame, back, icona, palette normale e palette shiny provvisoria. Per cry, footprint, overworld e ombra, `gSpeciesInfo` continua a riusare direttamente asset già presenti:

| Specie | Modello grafico | Cry |
| --- | --- | --- |
| Cingerm | Grafica base originale; footprint, overworld e ombra di Lechonk | `CRY_LECHONK` |
| Rovasco | Grafica base originale; footprint, overworld e ombra di Oinkologne maschio | `CRY_OINKOLOGNE_M` |
| Selvazanna | Grafica base originale; footprint, overworld e ombra di Mamoswine | `CRY_MAMOSWINE` |
| Serbrace | Grafica base originale; footprint, overworld e ombra di Ekans | `CRY_EKANS` |
| Vipercen | Grafica base originale; footprint, overworld e ombra di Arbok | `CRY_ARBOK` |
| Tossivampa | Grafica base originale; footprint, overworld e ombra di Seviper | `CRY_SEVIPER` |
| Ardeino | Grafica base originale; footprint, overworld e ombra di Ducklett | `CRY_DUCKLETT` |
| Velairone | Grafica base originale; footprint, overworld e ombra di Swanna | `CRY_SWANNA` |
| Codairone | Grafica base originale; footprint, overworld e ombra di Bombirdier | `CRY_BOMBIRDIER` |

Front, back, icone, palette, coordinate e animazioni sono ora specifici per ciascuna specie. Ombre, impronte, sprite overworld, relative palette e animazioni posteriori compatibili seguono ancora i rispettivi modelli provvisori. Non sono definite differenze grafiche fra sessi dedicate alle specie di Ausonia.

Le concept art di Cingerm, Rovasco, Selvazanna, Serbrace, Vipercen, Tossivampa, Ardeino e Velairone sono approvate. Soltanto Codairone resta da progettare definitivamente. Le immagini di concept non sono conservate nel repository. Tutte le nove specie possiedono front, back, icona e palette normali originali, con shiny ancora provvisorie; cry, footprint, overworld e ombra restano segnaposto. Il trio resta interamente selezionabile e le evoluzioni conservano livelli e disponibilità precedenti.

Le 46 mosse uniche ricavate dai nove learnset per livello sono state sottoposte ad audit. Le voci ancora inglesi hanno ricevuto nome e descrizione italiani; `Fangosberla` è rimasta invariata e `MOVE_SMOKESCREEN` è visualizzata come `Muro di Fumo`. Potenza, precisione, PP, tipo, categoria, effetti e ogni altro campo tecnico sono invariati, così come livelli, ordine e contenuto dei nove learnset. La traduzione globale delle mosse non appartenenti a questi learnset resta fuori ambito.

Le prossime sostituzioni dovranno riguardare soltanto shiny definitive, impronte, ombre, sprite overworld e cry, senza cambiare gli ID o i dati di gioco. I segnaposto residui non sono direzione artistica definitiva.

## Rischi e aggiornamenti upstream

- Un aggiornamento upstream può aggiungere nuovi ID dopo l'attuale limite ufficiale: il conflitto va risolto senza rinumerare specie già distribuite in salvataggi.
- Modifiche a `SpeciesInfo`, ai generatori dei learnset o agli array del Pokédex possono richiedere una migrazione coordinata.
- `NATIONAL_DEX_COUNT` deve continuare a comprendere le specie custom in tutte le configurazioni supportate.
- I riferimenti agli asset segnaposto dipendono dalle famiglie ufficiali abilitate nella configurazione corrente.
- La compatibilità Emerald, FireRed e LeafGreen va mantenuta con dati comuni e senza introdurre numeri nei Pokédex regionali.

## Piano di implementazione

1. Registrare e validare la linea Cingerm — registrazione dati completata.
2. Registrare e validare la linea Serbrace — registrazione dati completata.
3. Registrare e validare la linea Ardeino — registrazione dati completata.
4. Integrare e validare il trio completo negli slot Emerald e nei rami del rivale — completato.
5. Completare front, back, icone e palette normali originali delle nove specie — completato; sostituire in seguito shiny, cry, footprint e overworld ancora provvisori.
6. Aggiungere eventuali differenze sessuali approvate.
7. Progettare e prototipare la Forma Riflesso senza considerarla ancora definitiva.
