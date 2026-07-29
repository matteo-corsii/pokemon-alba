# Pipeline degli sprite degli starter di Ausonia

## Stato delle tre forme base

Cingerm, Serbrace e Ardeino usano asset grafici originali del progetto Pokémon Alba: front sprite animato, back sprite, icona, palette normale e palette shiny provvisoria. Sono rispettivamente gli starter Erba, Fuoco e Acqua del prototipo Emerald. Cry, footprint, sprite overworld, ombra e relative palette continuano temporaneamente a riutilizzare Lechonk per Cingerm, Ekans per Serbrace e Ducklett per Ardeino. FireRed e LeafGreen non cambiano starter; dati di gameplay e learnset restano invariati.

## File sorgente

Gli asset importati sono conservati in `graphics/pokemon/cingerm/`, `graphics/pokemon/serbrace/` e `graphics/pokemon/ardeino/`; le tre directory seguono la stessa struttura:

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

## Controlli richiesti

Prima dell'importazione di altri sprite occorre verificare dimensioni, formato indicizzato, indici 0–15, trasparenza dell'indice 0, palette JASC da 16 voci, frame non vuoti e conversione con gli strumenti del repository. Dopo la conversione vanno inoltre controllati front e back in battaglia, icona a dimensione reale e resa normale/shiny su sfondi chiari e scuri.

## Passaggi futuri

La sostituzione completa richiederà footprint, cry e overworld originali, palette shiny definitive, grafica originale delle evoluzioni e l'eventuale definizione di differenze sessuali. Il prototipo usa il sesso generato normalmente dal motore e non introduce ancora un selettore; le tre forme base restano ottenibili soltanto dalla scelta iniziale Emerald.
