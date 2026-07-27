# Implementazione degli starter di Ausonia

Questo documento descrive l'architettura tecnica verificata nella versione corrente di `pokeemerald-expansion` e la strategia append-only scelta per i nove starter originali di Pokémon Alba.

## Architettura reale delle specie

- `include/constants/species.h` contiene l'enumerazione `SPECIES_*`, il limite dell'intervallo personalizzato, `SPECIES_EGG` e `NUM_SPECIES`.
- `include/constants/pokedex.h` contiene i numeri del Pokédex nazionale e `NATIONAL_DEX_COUNT`.
- `src/data/pokemon/species_info.h` contiene `gSpeciesInfo`: nomi, dati base, tipi, statistiche, abilità, gruppi Uova, rapporto sessi, crescita, evoluzioni, dati Pokédex e riferimenti grafici e sonori.
- Il learnset per livello è una tabella di `struct LevelUpMove` collegata da `gSpeciesInfo`. La linea di Cingerm usa tabelle separate ma identiche, così ogni forma potrà divergere in futuro senza modificare l'identità della specie.
- `src/data/pokemon/all_learnables.json` alimenta il generatore delle compatibilità MT e tutor. `src/data/pokemon/teachable_learnsets.h` è generato durante la build e non va modificato manualmente.
- `src/data/pokemon/egg_moves.h` contiene le mosse Uovo.
- `src/data/pokemon/pokedex_orders.h` contiene gli ordinamenti alfabetico, per peso e per altezza del Pokédex nazionale.
- Front sprite, back sprite, palette, icone, impronte, animazioni, coordinate, ombre e cry sono riferimenti di `gSpeciesInfo`; le risorse binarie sono dichiarate nelle tabelle grafiche già incluse dal progetto.
- `test/species.c` e `test/text.c` verificano rispettivamente i dati delle specie e la resa dei nomi e delle voci Pokédex. Gli strumenti in `tools/learnset_helpers/` generano le tabelle insegnabili a partire dai JSON.

La struttura `SpeciesInfo` corrente non contiene campi per habitat o forma corporea. Queste categorie non vengono quindi inventate; per il prototipo viene registrato soltanto il colore corporeo supportato più vicino, marrone.

## Strategia degli identificatori

L'ultimo identificatore ufficiale verificato è `SPECIES_GLIMMORA_MEGA = 1572`. L'intervallo personalizzato inizia subito dopo `SPECIES_CUSTOM_START`, senza inserimenti fra le specie ufficiali. La prima linea usa pertanto:

| Specie | ID specie | Numero Pokédex nazionale |
| --- | ---: | ---: |
| Cingerm | 1573 | 1026 |
| Rovasco | 1574 | 1027 |
| Selvazanna | 1575 | 1028 |

Gli ID del Pokédex seguono in appendice `NATIONAL_DEX_PECHARUNT = 1025`. Nessun numero regionale iniziale viene assegnato.

In questa fase vengono registrati soltanto questi tre ID. Riservare gli altri sei senza dati completi allargherebbe gli array e introdurrebbe specie parziali; il loro ordine futuro resta vincolato come segue:

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

## Dati e disponibilità del primo prototipo

La linea Cingerm viene registrata con evoluzioni ai livelli 16 e 36, dati base completi e un learnset per livello comune. Le compatibilità MT/tutor e le mosse Uovo restano vuote tramite le tabelle `sNone*`: è una scelta prudente supportata dalla struttura e non richiede di modificare file generati.

Le specie non vengono aggiunte a incontri selvatici, regali, commerci, Uova, squadre, mappe, script, borsa degli starter o Pokédex regionale. Sono istanziabili soltanto tramite test e strumenti di debug già esistenti.

## Asset provvisori

Non vengono copiati file grafici o audio. `gSpeciesInfo` riusa direttamente asset già presenti:

| Specie | Modello grafico | Cry |
| --- | --- | --- |
| Cingerm | Lechonk | `CRY_LECHONK` |
| Rovasco | Oinkologne maschio | `CRY_OINKOLOGNE_M` |
| Selvazanna | Mamoswine | `CRY_MAMOSWINE` |

I tre modelli danno silhouette piccola, intermedia e grande distinguibili. Le coordinate, animazioni, ombre, palette, icone e impronte seguono i rispettivi modelli. Non sono definite differenze grafiche fra sessi né sprite overworld dedicati.

Quando saranno disponibili asset originali, i riferimenti andranno sostituiti senza cambiare gli ID. Ogni forma richiederà front e back sprite, palette normale e cromatica, icona, impronta, animazioni, coordinate, ombra, eventuali sprite overworld e cry verificati. I segnaposto non sono direzione artistica definitiva.

## Rischi e aggiornamenti upstream

- Un aggiornamento upstream può aggiungere nuovi ID dopo l'attuale limite ufficiale: il conflitto va risolto senza rinumerare specie già distribuite in salvataggi.
- Modifiche a `SpeciesInfo`, ai generatori dei learnset o agli array del Pokédex possono richiedere una migrazione coordinata.
- `NATIONAL_DEX_COUNT` deve continuare a comprendere le specie custom in tutte le configurazioni supportate.
- I riferimenti agli asset segnaposto dipendono dalle famiglie ufficiali abilitate nella configurazione corrente.
- La compatibilità Emerald, FireRed e LeafGreen va mantenuta con dati comuni e senza introdurre numeri nei Pokédex regionali.

## Piano di implementazione

1. Registrare e validare la linea Cingerm.
2. Registrare e validare la linea Serbrace.
3. Registrare e validare la linea Ardeino.
4. Integrare la selezione degli starter nel prologo.
5. Sostituire i segnaposto con sprite originali.
6. Aggiungere eventuali differenze sessuali approvate.
7. Progettare e prototipare la Forma Riflesso senza considerarla ancora definitiva.
