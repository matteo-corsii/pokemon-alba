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

Cingerm sostituisce temporaneamente Treecko nello slot Erba della borsa del Professor Lauro e nei soli rami delle squadre di Nico e Lia collegati alla vecchia linea di Treecko. La selezione crea Cingerm al livello 5 attraverso il normale flusso `GetStarterPokemon`/`ScriptGiveMon`; Torchic e Mudkip restano invariati. Cingerm non compare in incontri selvatici, regali aggiuntivi, commerci, Uova, NPC o strumenti di debug, mentre le altre otto specie di Ausonia restano non ottenibili direttamente.

Il prototipo serve esclusivamente a verificare grafica, dati, prima battaglia e progressione del rivale. Non è ancora la configurazione definitiva del prologo: gli starter Fuoco e Acqua di Ausonia non dispongono del pacchetto grafico completo e la selezione manuale del sesso è rinviata. Il sesso di Cingerm viene generato dal comportamento standard del motore.

## Asset provvisori

Non vengono copiati file grafici o audio. `gSpeciesInfo` riusa direttamente asset già presenti:

| Specie | Modello grafico | Cry |
| --- | --- | --- |
| Cingerm | Grafica originale; footprint, overworld e ombra di Lechonk | `CRY_LECHONK` |
| Rovasco | Oinkologne maschio | `CRY_OINKOLOGNE_M` |
| Selvazanna | Mamoswine | `CRY_MAMOSWINE` |
| Serbrace | Ekans | `CRY_EKANS` |
| Vipercen | Arbok | `CRY_ARBOK` |
| Tossivampa | Seviper | `CRY_SEVIPER` |
| Ardeino | Ducklett | `CRY_DUCKLETT` |
| Velairone | Swanna | `CRY_SWANNA` |
| Codairone | Bombirdier | `CRY_BOMBIRDIER` |

I modelli danno silhouette piccole, intermedie e grandi distinguibili per ciascuna linea. Le coordinate, animazioni, ombre, palette, icone, impronte e sprite overworld seguono i rispettivi modelli. Non sono definite differenze grafiche fra sessi dedicate alle specie di Ausonia.

Le concept art di Cingerm, Rovasco, Selvazanna, Serbrace, Vipercen, Tossivampa, Ardeino e Velairone sono approvate. Soltanto Codairone resta da progettare definitivamente. Le immagini di concept non sono conservate nel repository in questa fase e gli asset collegati ai dati restano tutti segnaposto.

Quando saranno disponibili asset originali, i riferimenti andranno sostituiti senza cambiare gli ID. Ogni forma richiederà front e back sprite, palette normale e cromatica, icona, impronta, animazioni, coordinate, ombra, eventuali sprite overworld e cry verificati. I segnaposto non sono direzione artistica definitiva.

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
4. Validare il prototipo temporaneo di Cingerm nello slot Erba — in corso.
5. Completare gli asset originali e integrare i tre starter definitivi nel prologo.
6. Aggiungere eventuali differenze sessuali approvate.
7. Progettare e prototipare la Forma Riflesso senza considerarla ancora definitiva.
