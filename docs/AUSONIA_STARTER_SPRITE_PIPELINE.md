# Pipeline degli sprite degli starter di Ausonia

## Stato delle nove specie

Cingerm, Rovasco, Selvazanna, Serbrace, Vipercen, Tossivampa, Ardeino, Velairone e Codairone usano asset grafici originali del progetto Pokémon Alba: front sprite animato, back sprite, icona, palette normale e palette shiny provvisoria. Cingerm, Serbrace e Ardeino sono rispettivamente gli starter Erba, Fuoco e Acqua del prototipo Emerald. Cry, footprint, sprite overworld, ombra e relative palette continuano temporaneamente a riutilizzare i modelli ufficiali già documentati. FireRed e LeafGreen non cambiano starter; dati di gameplay, evoluzioni, learnset, mappe, incontri e squadre restano invariati.

## File sorgente

Gli asset importati sono conservati nelle nove directory omonime sotto `graphics/pokemon/`: `cingerm/`, `rovasco/`, `selvazanna/`, `serbrace/`, `vipercen/`, `tossivampa/`, `ardeino/`, `velairone/` e `codairone/`. Ogni directory segue la stessa struttura:

- `anim_front.png`: PNG indicizzato 64×128, due frame 64×64 verticali;
- `back.png`: PNG indicizzato 64×64;
- `icon.png`: PNG indicizzato 32×64, due frame 32×32 verticali;
- `normal.pal`: palette JASC-PAL 0100 da 16 voci;
- `shiny.pal`: palette JASC-PAL 0100 da 16 voci, ancora provvisoria.

L'indice 0 è trasparente e gli asset usano soltanto gli indici 0–15. I file PNG devono mantenere dimensioni multiple di 8. Concept art, anteprime ad alta risoluzione, manifest e note del pacchetto sorgente non appartengono al repository.

## Conversione

Le dichiarazioni `INCGFX` in `src/data/graphics/pokemon.h` attivano la pipeline Make esistente:

1. `tools/gbagfx` converte PNG e JASC-PAL in `.4bpp` e `.gbapal`;
2. `tools/compresSmol` comprime front e back in `.4bpp.smol`;
3. icona e palette restano nei formati GBA previsti dal motore.

I file generati `*.4bpp`, `*.gbapal`, `*.smol`, `build/`, ROM, ELF e MAP non devono essere modificati manualmente né aggiunti a Git.

## Coordinate e animazione

Per Cingerm i bounding box misurati sono:

- front frame 0 e 1: `x=3`, `y=12`, `58×48`;
- back: `x=5`, `y=10`, `54×50`;
- icona frame 0: `x=4`, `y=1`, `24×30`;
- icona frame 1: `x=4`, `y=2`, `24×30`.

Il record usa `MON_COORDS_SIZE(64, 48)` per il front, `MON_COORDS_SIZE(56, 56)` per il back e offset verticali pari a 4. L'animazione mostra brevemente il frame 1 e ritorna al frame 0 senza ciclo rapido.

Per Serbrace i bounding box misurati sono:

- front frame 0: `x=2`, `y=6`, `60×54`;
- front frame 1: `x=2`, `y=6`, `59×54`;
- back: `x=3`, `y=9`, `58×51`;
- icona frame 0 e 1: `x=2`, `y=3`, `28×28`.

Il record di Serbrace usa `MON_COORDS_SIZE(64, 56)` per front e back e offset verticali pari a 4. L'animazione passa brevemente al frame 1 per 12 tick, torna al frame 0 per 8 tick e termina senza ciclo.

Per Ardeino i bounding box rimisurati direttamente sugli asset sono:

- front frame 0: `x=7`, `y=3`, `48×58`;
- front frame 1: `x=5`, `y=3`, `54×58`;
- back: `x=17`, `y=3`, `33×57`;
- icona frame 0: `x=1`, `y=6`, `30×25`;
- icona frame 1: `x=1`, `y=7`, `30×25`.

Questi valori reali correggono le dimensioni indicative del manifest sorgente. Il record usa `MON_COORDS_SIZE(56, 64)` per il front, `MON_COORDS_SIZE(40, 64)` per il back, offset front pari a 3 e offset back pari a 4. L'animazione mostra il frame 1 per 12 tick e torna al frame 0 per 8 tick, senza ciclo.

### Evoluzioni Erba

| Specie | Front frame 0 | Front frame 1 | Back | Icona frame 0 | Icona frame 1 | `frontPicSize` | `backPicSize` | Offset front/back |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Rovasco | `2,7 60×53` | `2,6 60×54` | `9,4 47×56` | `1,4 29×27` | `1,5 29×27` | `64×56` | `48×56` | `4/4` |
| Selvazanna | `7,7 48×57` | `7,7 48×57` | `7,8 48×56` | `1,5 29×21` | `1,4 29×24` | `48×64` | `48×56` | `0/0` |

Le misure di Selvazanna sono state ricavate direttamente dai PNG e sostituiscono i valori indicativi del pacchetto. Rovasco usa un assestamento breve di corpo e foglie; Selvazanna un movimento più pesante. Entrambe le animazioni mostrano il frame 1 per 12 tick e ritornano al frame 0 per 8 tick.

### Evoluzioni Fuoco

| Specie | Front frame 0 | Front frame 1 | Back | Icona frame 0 | Icona frame 1 | `frontPicSize` | `backPicSize` | Offset front/back |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Vipercen | `11,5 42×53` | `14,5 37×51` | `11,5 42×52` | `6,4 20×24` | `6,1 19×21` | `48×56` | `48×56` | `6/7` |
| Tossivampa | `3,2 57×60` | `4,2 55×60` | `6,2 51×60` | `1,2 30×27` | `1,1 30×30` | `64×64` | `56×64` | `2/2` |

La larghezza reale di Vipercen è 42 pixel nei frame più estesi e non 40 come indicato inizialmente. Vipercen usa una lieve tensione del cappuccio; Tossivampa una pulsazione lenta delle fumarole. Entrambe usano la sequenza `frame 1: 12 tick`, `frame 0: 8 tick`, quindi terminano.

### Evoluzioni Acqua

| Specie | Front frame 0 | Front frame 1 | Back | Icona frame 0 | Icona frame 1 | `frontPicSize` | `backPicSize` | Offset front/back |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Velairone | `7,3 48×58` | `7,3 49×58` | `17,3 29×58` | `1,6 30×25` | `1,7 30×25` | `56×64` | `32×64` | `3/3` |
| Codairone | `4,3 55×58` | `5,3 53×58` | `10,4 44×57` | `3,2 25×29` | `4,2 24×29` | `56×64` | `48×64` | `3/3` |

Velairone usa un piccolo movimento delle piume e del codino; Codairone un movimento delicato di piume e coda. Anche queste animazioni usano 12 tick sul frame 1 e 8 tick sul frame 0, senza loop rapido.

## Palette dell'icona

L'icona usa la palette globale `graphics/pokemon/icon_palettes/pal5.pal`, selezionata perché offre contemporaneamente verdi, marroni/aranci, crema e un contorno scuro. Gli indici originali sono stati rimappati così:

`0→0, 1→15, 2→15, 3→14, 4→10, 5→13, 6→13, 7→6, 8→7, 9→3, 10→4, 11→4, 12→5, 13→5, 14→7, 15→14`.

Non è stata aggiunta alcuna palette globale.

L'icona di Serbrace usa `graphics/pokemon/icon_palettes/pal3.pal`: fra le palette globali esistenti è quella che conserva meglio insieme contorno scuro, rosso magma, arancione, giallo e crema. La rimappatura completa è:

`0→0, 1→15, 2→15, 3→15, 4→15, 5→15, 6→15, 7→8, 8→14, 9→8, 10→1, 11→13, 12→2, 13→13, 14→12, 15→3`.

Il compromesso principale è la fusione di più tonalità carbone negli indici scuri 14–15; in cambio rimangono distinguibili ventre rosso, riflessi arancioni, occhio e coda gialli e dettagli crema. Nessuna nuova palette globale è stata aggiunta.

L'icona di Ardeino usa anch'essa `graphics/pokemon/icon_palettes/pal3.pal`, scelta perché conserva contemporaneamente bianco, azzurro, blu scuro, beige del becco e giallo della perlina. La rimappatura completa è:

`0→0, 1→15, 2→15, 3→4, 4→9, 5→4, 6→4, 7→5, 8→1, 9→9, 10→5, 11→6, 12→6, 13→10, 14→3, 15→3`.

Gli indici sono stati riscritti direttamente in un PNG `Indexed8`, senza quantizzazione o dithering. Il compromesso principale è la fusione di alcune tonalità blu scuro; becco, occhio, codino, perlina e piumaggio restano distinguibili. La palette incorporata nell'icona coincide indice per indice con `pal3` e non è stata aggiunta alcuna palette globale.

Le evoluzioni Erba usano `pal5`, che conserva contorno scuro, verdi, marrone, avorio e riflessi caldi:

- Rovasco: `0→0, 1→15, 2→15, 3→14, 4→14, 5→14, 6→4, 7→6, 8→6, 9→4, 10→6, 11→1, 12→5, 13→7, 14→7, 15→3`;
- Selvazanna: `0→0, 1→15, 2→15, 3→14, 4→14, 5→14, 6→4, 7→6, 8→6, 9→4, 10→5, 11→6, 12→7, 13→8, 14→7, 15→3`.

L'icona sorgente di Selvazanna aveva una palette incorporata incoerente con la palette normale. La rimappatura è stata calcolata sui colori tecnici di `normal.pal`, eliminando il magenta visibile senza quantizzazione o dithering. I compromessi consistono nella fusione di alcune gradazioni marroni e dei contorni più scuri.

Le evoluzioni Fuoco usano `pal3`, che offre contorno scuro, grigi, rosso, arancione, crema e un accento adatto al viola:

- Vipercen: `0→0, 1→15, 2→14, 3→1, 4→8, 5→9, 6→13, 7→8, 8→1, 9→14, 10→9, 11→12, 12→1, 13→11, 14→13, 15→7`;
- Tossivampa: `0→0, 1→15, 2→14, 3→1, 4→1, 5→8, 6→13, 7→7, 8→9, 9→8, 10→9, 11→13, 12→12, 13→7, 14→11, 15→15`.

Le gradazioni carbone vengono compresse nei pochi scuri globali disponibili; fiamme, crepe, fumarole, zanne e dettagli viola restano separati.

Le evoluzioni Acqua usano `pal3`, coerente con Ardeino e adatta a bianco, azzurro, blu scuro, beige e oro:

- Velairone: `0→0, 1→15, 2→15, 3→4, 4→4, 5→5, 6→9, 7→5, 8→4, 9→6, 10→2, 11→6, 12→6, 13→9, 14→2, 15→3`;
- Codairone: `0→0, 1→15, 2→15, 3→4, 4→9, 5→4, 6→14, 7→5, 8→9, 9→5, 10→6, 11→6, 12→10, 13→11, 14→2, 15→3`.

Per tutte e sei le icone gli indici sono stati riscritti direttamente e verificati pixel per pixel. La palette incorporata coincide con la palette globale scelta; non sono state aggiunte palette e non è stato introdotto dithering.

## Controlli richiesti

Il validatore `test/validate_ausonia_starter_evolution_graphics.ps1` controlla le sei evoluzioni: presenza dei cinque asset, dimensioni, formato indicizzato, indici 0–15, trasparenza, palette JASC da 16 voci, frame distinti, bounding box, palette globali, simboli, riferimenti di `SpeciesInfo` e invarianza dei dati di gameplay. La CI verifica inoltre la conversione reale tramite `gbagfx`, la compressione front/back tramite `compresSmol` e le build Emerald, FireRed e LeafGreen. Restano necessari controlli manuali in battaglia, nel menu e sulla resa normale/shiny.

## Passaggi futuri

La sostituzione completa richiederà footprint, cry e overworld originali, palette shiny definitive e l'eventuale definizione di differenze sessuali. Il prototipo usa il sesso generato normalmente dal motore e non introduce ancora un selettore; le tre forme base restano ottenibili soltanto dalla scelta iniziale Emerald e gli stadi successivi soltanto tramite le evoluzioni e le squadre già previste.
