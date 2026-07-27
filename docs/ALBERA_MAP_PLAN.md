# Piano del primo prototipo esterno di Albèra

## Strategia tecnica

Il prototipo adotta la strategia C: mantiene `MAP_LITTLEROOT_TOWN` e
`LAYOUT_LITTLEROOT_TOWN`, ma amplia il layout da 20×20 a 36×32 metatile.
Il nucleo originale resta invariato alle coordinate `x = 0–19`, `y = 0–19`;
il nuovo spazio viene aggiunto esclusivamente verso est e sud.

Restano invariati gli identificatori tecnici, la connessione nord con Route 101,
gli eventi e gli interni esistenti, la musica, il meteo, il tipo di mappa, la
destinazione Fly e le heal location. Il prototipo usa soltanto i tileset
`General` e `Petalburg`.

## Pianta concettuale

```text
NORD / ROUTE 101
┌────────────────────┬────────────────┐
│                    │    SCUOLA      │
│  NUCLEO ORIGINALE  │                │
│      20 × 20       ├────────────────┤
│                    │ VIA DELL'ARMONIA│
│ case, camion,      ├───────┬────────┤
│ laboratorio ed     │ PARCO │CAMPO   │
│ eventi invariati   │       │BASKET  │
├───────────┬────────┴───────┴────────┤
│           │   PIAZZA / SLARGO       │
│           └──────────┬──────────────┤
│ futuro centro       │ percorso verso│
│ storico (bloccato)  │ il lago       │
└─────────────────────┴───────X───────┘
                       LAGO (bloccato)
```

## Zone proposte

- **Via dell'Armonia:** corridoio pedonale principale nell'area
  `x = 20–34`, `y = 9–12`, collegato al margine est già percorribile del
  nucleo originale. Deve consentire il passaggio affiancato del giocatore e di
  futuri NPC e non deve terminare in vicoli ciechi involontari.
- **Scuola:** edificio esterno provvisorio in `x = 24–30`, `y = 3–7`.
  L'ingresso è rivolto a sud in `(28, 7)`, con spazio libero davanti. La porta
  resta chiusa, invalicabile e priva di warp.
- **Piccolo parco:** area verde in `x = 21–27`, `y = 13–19`, con quattro
  alberi perimetrali, ingresso a nord in `x = 23–25` e spazio centrale aperto.
  Non contiene acqua né erba con incontri.
- **Campetto provvisorio:** area in `x = 28–34`, `y = 13–19`, con rettangolo
  pavimentato in `x = 29–33`, `y = 14–18`. Il bordo erboso è temporaneamente
  solido e lascia un ingresso a nord in `(30, 13)`–`(31, 13)`. Non sono
  presenti elementi rappresentati come canestri.
- **Piazza meridionale:** slargo pavimentato in `x = 7–22`, `y = 21–26`,
  collegato al bordo sud percorribile del nucleo originale.
- **Futuro centro storico:** strada in `x = 10–13`, `y = 27–29`, chiusa
  dall'aiuola invalicabile `x = 10–13`, `y = 30`. Non viene creata alcuna
  connessione.
- **Percorso verso il Lago di Albèra:** percorso sud-est in `x = 23–33`,
  `y = 23–25`, chiuso dall'aiuola invalicabile `x = 34`, `y = 23–25`. Non
  vengono aggiunti acqua, warp, connessioni o script.

## Elementi temporaneamente invariati

- Tutto il contenuto del vecchio rettangolo 20×20.
- Coordinate di object events, warp, coordinate events e background events.
- Uscita nord, camion, case, laboratorio e relativi interni.
- Movimenti della madre, evento delle Scarpe da Corsa e trigger del soccorso
  del Professor Lauro.
- Doppio sistema delle case dipendente dal protagonista.
- Bordo 2×2, salvo incompatibilità tecnica riscontrata in Porymap.

## Elementi da ridisegnare in seguito

- Aspetto definitivo di scuola, campetto e arredo urbano.
- Interni della scuola e degli altri nuovi edifici.
- Connessioni verso centro storico e Lago di Albèra.
- Cartelli e relativi testi/script.
- Tileset, grafica, dettagli ambientali e canestri originali.
- Eventuali NPC, eventi narrativi, acqua ed erba con incontri.

## Limiti del primo prototipo

Il prototipo serve esclusivamente a verificare scala, percorribilità e
distribuzione degli spazi. Le nuove strutture sono segnaposto costruiti con
metatile esistenti. Non introduce nuovi warp, connessioni, object events,
coordinate events o script; i cartelli vengono rinviati per evitare modifiche
agli script in questa fase.

## Esito del primo collaudo

Tutte le aree previste sono risultate raggiungibili e le future uscite verso il
centro storico e il Lago di Albèra sono correttamente bloccate. Il prologo
originale non ha subito regressioni. Sono state ripulite le collisioni residue
e completate le parti isolate degli alberi individuate durante il collaudo. Il
prototipo è quindi pronto per ricevere il nuovo prologo; la relativa
riscrittura narrativa non è ancora completata.

## Primo utilizzo narrativo

Dopo l'incontro iniziale nella casa di Nico o Lia, il giocatore deve percorrere
Via dell'Armonia e raggiungere il campetto prima di poter accedere a Route 101.
Qui un giovane del quartiere segnala che la fontanella ha perso nuovamente
pressione e che al mattino usciva soltanto un filo d'acqua.

Nico o Lia, mostrato in base al protagonista scelto, interpreta inizialmente il
problema come il normale guasto di una vecchia tubatura. Un rumore proveniente
dal sottosuolo del campetto interrompe la conversazione e introduce il primo
indizio anomalo della crisi idrica, senza mostrarne ancora la causa. Conclusa
la breve scena, un flag permanente ne impedisce la ripetizione e consente al
normale evento di Route 101 di proseguire verso il salvataggio del Professor
Lauro.

I cartelli di scuola, campetto e strade chiuse restano rinviati finché il
prototipo non disporrà di metatile-segnaletica chiaramente riconoscibili; non
vengono aggiunte interazioni invisibili su celle prive di cartello.
