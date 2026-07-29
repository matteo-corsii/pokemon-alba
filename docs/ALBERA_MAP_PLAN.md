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
prototipo era quindi pronto per ricevere il nuovo prologo; questa nota descrive
lo stato storico precedente al segmento canonico documentato più sotto.

## Secondo collaudo manuale

Il precedente prologo è stato completato con entrambi i protagonisti. Le scene, i
flag e la progressione sono risultati corretti, senza crash o blocchi
permanenti. Il collaudo ha inoltre evidenziato ulteriori collisioni residue e
frammenti grafici lungo il vecchio confine della mappa: questi residui sono
stati ripuliti, mantenendo invariati eventi, collegamenti e delimitazioni
intenzionali. Quel flusso è stato successivamente sostituito, nelle sole parti
incompatibili dopo l'arrivo, dal primo segmento canonico.

## Primo utilizzo narrativo canonico

Dopo la sequenza di arrivo già esistente, la casa del protagonista introduce
un'interruzione momentanea dell'acqua e un rumore nelle tubature. Il familiare
comunica che il Professor Lauro lo aspetta nel Laboratorio del Cratere. Il
campetto conserva spazi e dialoghi ambientali, ma il suo vecchio evento non è
più il primo segnale obbligatorio.

L'uscita nord resta chiusa con una spiegazione diegetica finché Lauro non ha
completato il briefing. Nel laboratorio sono presenti contemporaneamente Nico
e Lia: il giocatore sceglie Cingerm, Serbrace o Ardeino; Nico riceve lo starter
avvantaggiato e Lia quello rimanente. La prima sfida è sempre contro Nico e la
sconfitta cambia soltanto una battuta, senza whiteout o deviazioni narrative.

Lia individua poi una nuova variazione proveniente da Route 101. Lauro affida
un sopralluogo prudente, consegna il Pokédex se necessario e riapre la strada.
Il salvataggio vanilla di Lauro resta conservato negli script ma viene saltato
dal nuovo stato di progressione; anche la battaglia alternativa di Route 103 è
nascosta. L'ingresso in Route 101 registra il checkpoint stabile della
milestone, senza sviluppare l'evento principale dell'area successiva.

I cartelli di scuola, campetto e strade chiuse restano rinviati finché il
prototipo non disporrà di metatile-segnaletica chiaramente riconoscibili; non
vengono aggiunte interazioni invisibili su celle prive di cartello.
