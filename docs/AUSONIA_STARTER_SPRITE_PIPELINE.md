# Pipeline degli sprite degli starter di Ausonia

## Stato di Cingerm

Cingerm usa i primi asset grafici originali del progetto Pokémon Alba: front sprite animato, back sprite, icona, palette normale e palette shiny provvisoria. È lo starter Erba del prototipo Emerald; Serbrace e Ardeino completano gli slot Fuoco e Acqua ma conservano rispettivamente i segnaposto di Ekans e Ducklett. Cry, footprint, sprite overworld, ombra e relative palette di Cingerm continuano temporaneamente a riutilizzare quelli di Lechonk. FireRed e LeafGreen non cambiano starter.

## File sorgente

Gli asset importabili sono conservati in `graphics/pokemon/cingerm/`:

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

I bounding box misurati sono:

- front frame 0 e 1: `x=3`, `y=12`, `58×48`;
- back: `x=5`, `y=10`, `54×50`;
- icona frame 0: `x=4`, `y=1`, `24×30`;
- icona frame 1: `x=4`, `y=2`, `24×30`.

Il record usa `MON_COORDS_SIZE(64, 48)` per il front, `MON_COORDS_SIZE(56, 56)` per il back e offset verticali pari a 4. L'animazione mostra brevemente il frame 1 e ritorna al frame 0 senza ciclo rapido.

## Palette dell'icona

L'icona usa la palette globale `graphics/pokemon/icon_palettes/pal5.pal`, selezionata perché offre contemporaneamente verdi, marroni/aranci, crema e un contorno scuro. Gli indici originali sono stati rimappati così:

`0→0, 1→15, 2→15, 3→14, 4→10, 5→13, 6→13, 7→6, 8→7, 9→3, 10→4, 11→4, 12→5, 13→5, 14→7, 15→14`.

Non è stata aggiunta alcuna palette globale.

## Controlli richiesti

Prima dell'importazione di altri sprite occorre verificare dimensioni, formato indicizzato, indici 0–15, trasparenza dell'indice 0, palette JASC da 16 voci, frame non vuoti e conversione con gli strumenti del repository. Dopo la conversione vanno inoltre controllati front e back in battaglia, icona a dimensione reale e resa normale/shiny su sfondi chiari e scuri.

## Passaggi futuri

La sostituzione completa richiederà footprint, cry e overworld originali, una palette shiny definitiva e l'eventuale definizione di differenze sessuali. Per Serbrace e Ardeino occorre ancora produrre l'intero pacchetto grafico originale. Il prototipo usa il sesso generato normalmente dal motore e non introduce ancora un selettore; le tre forme base restano ottenibili soltanto dalla scelta iniziale Emerald.
