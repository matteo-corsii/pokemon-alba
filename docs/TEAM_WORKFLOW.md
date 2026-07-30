# Flusso di lavoro del team di Pokémon Alba

Questo documento definisce come Matteo, `dsalvagno1994-bot` e Codex collaborano
senza sovrapporre modifiche o trasformare un incarico tecnico in una decisione
di canone. Si applica al repository pubblico `matteo-corsii/pokemon-alba`.

## A. Scopo

- **Matteo** è il responsabile del progetto: approva canone, priorità, map brief,
  risultati dei playtest e merge.
- **`dsalvagno1994-bot`** è il collaboratore dedicato principalmente alla
  realizzazione e rifinitura delle mappe assegnate tramite un brief approvato.
- **Codex** esegue audit del repository, implementazioni circoscritte, test,
  documentazione, preparazione delle pull request e analisi della CI.

## B. Responsabilità di Matteo

Matteo decide canone, storia, progressione e bilanciamento; assegna i task,
approva visivamente le mappe, esegue la review e autorizza il merge. Gestisce
inoltre build private e playtest finali quando previsti da un incarico separato.

## C. Responsabilità del collaboratore

Di norma il collaboratore realizza layout, collisioni, decorazioni, warp e
connessioni già concordati, spazi per eventi, NPC e oggetti, test tecnici, draft
della pull request e screenshot utili alla review.

Il collaboratore non decide autonomamente:

- canone o ordine degli eventi;
- flag, variabili o stati narrativi;
- squadre, incontri o fauna;
- connessioni non previste dal brief;
- dialoghi o testi mostrati al giocatore.

Qualunque elemento mancante in queste aree deve essere riportato a Matteo prima
di procedere.

## D. Pipeline parallela

Matteo e Codex integrano la milestone giocabile corrente mentre il collaboratore
può preparare la mappa della milestone successiva. La sequenza di riferimento è
Via Verdi → Albèra Storica → Anfiteatro Romano → Via Consolare → Lago di Albèra.

Per ciascun task:

1. Matteo approva canone, obiettivo e confini.
2. Codex esegue l'audit tecnico e prepara il brief verificabile.
3. Il collaboratore lavora sulla mappa assegnata e su un branch dedicato.
4. Codex controlla diff, file coinvolti, collisioni, riferimenti e scope.
5. Matteo effettua il playtest o la revisione visiva richiesta.
6. La pull request viene aperta, sottoposta a CI e unita solo dopo approvazione.

Task indipendenti possono procedere in parallelo soltanto quando non condividono
file e non dipendono da una decisione ancora aperta.

## E. Proprietà temporanea dei file

Ogni task assegna una proprietà temporanea dei file al suo autore. Due persone
non devono modificare contemporaneamente la stessa directory
`data/maps/<NomeMappa>/`, neppure se intendono lavorare su file diversi al suo
interno. Chi necessita della stessa mappa attende il merge o concorda un
passaggio di consegne esplicito.

Documentazione, dati e grafica seguono lo stesso principio quando le modifiche
si sovrappongono semanticamente. I file non assegnati restano fuori scope. Ogni
task deve indicare responsabile, branch, file consentiti e vietati, dipendenze e
criteri di completamento.

### Brief obbligatorio del task

Ogni incarico deve indicare almeno:

- repository, branch di base e commit atteso;
- obiettivo e stato: implementato, approvato oppure pianificato;
- file o directory consentiti;
- file e contenuti vietati;
- comportamento atteso e criteri di accettazione;
- identificatori tecnici da conservare;
- dipendenze e decisioni canoniche già approvate;
- controlli statici, CI e playtest richiesti;
- messaggio di commit, destinazione della PR e regola di merge;
- condizione che impone di fermarsi e chiedere una decisione.

## F. Workflow Git

Avvio ordinario:

```bash
git switch develop
git pull --ff-only origin develop
git switch -c <branch>
```

- La base ordinaria è `origin/develop`, sincronizzata prima di iniziare.
- Ogni task usa un branch nuovo e circoscritto.
- Prefissi branch: `map/<nome-mappa>-layout`, `feature/<funzionalita>`,
  `fix/<problema>`, `docs/<argomento>` e `test/<argomento>`.
- Prefissi commit: `map:`, `feat:`, `fix:`, `docs:` e `test:`.
- I commit devono essere piccoli, descrittivi e privi di file estranei.
- Non si usa force push e non si inviano modifiche a `upstream`.
- Non si lavora direttamente su `master` o `develop`.
- Una pull request deve avere base `develop`, descrivere scope e verifiche e non
  viene unita automaticamente.
- Non si modifica o elimina il branch di un altro collaboratore senza un
  passaggio di consegne esplicito.
- La working tree deve essere pulita prima di cambiare task.

## G. File e azioni vietati salvo incarico esplicito

- ROM, file `.gba`, `.elf`, `.map`, salvataggi, save state e screenshot;
- output generati dalla compilazione o conversione;
- file temporanei e archivi ZIP del sorgente;
- segreti, credenziali e configurazioni personali;
- workflow GitHub, protezioni dei branch, permessi e collaboratori;
- repository privati o cartelle di build;
- file fuori dal perimetro indicato nel brief.

La compilazione, una build privata o l'avvio di un emulatore richiedono sempre
un incarico che li preveda espressamente.

## H. Passaggio di consegne

Il resoconto di handoff deve riportare:

- branch e commit finale;
- file modificati e motivazione;
- cosa è completo e cosa resta provvisorio;
- controlli eseguiti e relativi risultati;
- eventuali dubbi, rischi o decisioni mancanti;
- istruzioni di playtest riproducibili;
- stato di push, PR e CI;
- conferma dell'assenza di file generati o fuori scope.

## I. Modello di map brief

```text
Mappa e directory:
Nome visibile:
Identificatore interno:
Branch di base / commit:
Scopo narrativo:
Stato del contenuto: implementato | approvato | pianificato
Dimensioni e tileset consentiti:
Ingressi, uscite e connessioni approvate:
Punti obbligatori:
Percorsi facoltativi:
Spazi per eventi, NPC e oggetti:
Stile visivo:
Collisioni ed elevazioni richieste:
Object event consentiti:
Script, flag, variabili e dialoghi: vietati oppure specificati da Matteo
Fauna, incontri e squadre: vietati oppure specificati da Matteo
File modificabili:
File vietati:
Fuori scope:
Criteri di accettazione:
Screenshot o playtest richiesti:
Condizioni di arresto:
```

## J. Regole operative per Codex

Codex deve eseguire l'audit prima di modificare file, rispettare lo scope e non
inventare decisioni narrative o tecniche mancanti. Se trova una contraddizione
sostanziale, si ferma e la presenta a Matteo. Non amplia autonomamente il task,
non modifica branch altrui e non dichiara implementato ciò che è soltanto
approvato o pianificato.

Ogni consegna deve includere un riepilogo di diff, test, CI, stato Git e rischi.
Codex non effettua il merge automatico: l'approvazione finale resta a Matteo.
