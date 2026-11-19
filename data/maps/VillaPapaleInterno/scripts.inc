VillaPapaleInterno_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, VillaPapaleInterno_OnTransition
	.byte 0

VillaPapaleInterno_OnTransition::
	goto_if_unset FLAG_BORGO_NICO_LIA_INTRO_DONE, VillaPapaleInterno_OnTransition_HideArchiveParty
	clearflag FLAG_HIDE_VILLA_PAPALE_ARCHIVIO_PARTY
	end

VillaPapaleInterno_OnTransition_HideArchiveParty::
	setflag FLAG_HIDE_VILLA_PAPALE_ARCHIVIO_PARTY
	end

VillaPapaleInterno_EventScript_Archivista::
	lockall
	faceplayer
	goto_if_unset FLAG_BORGO_NICO_LIA_INTRO_DONE, VillaPapaleInterno_EventScript_ArchivistaNotReady
	goto_if_set FLAG_VILLA_PAPALE_ARCHIVIO_DONE, VillaPapaleInterno_EventScript_ArchivistaPost
	msgbox VillaPapaleInterno_Text_ArchivistaAncientMaps, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_ArchivistaSources, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_NicoCompareToday, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_LiaChangesFast, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_ArchivistaTerritoryChanges, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_ArchivistaTime, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_NicoTooFast, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_ArchivistaLariciaMap, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_ArchivistaLariciaLake, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_LiaLariciaFields, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_ArchivistaTerritory, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_ArchivistaChronicle, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_ArchivistaEarthCall, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_LiaEmissario, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_NicoNotEnough, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_ArchivistaRareWitnesses, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_LiaHearIt, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_NicoAnyMeaning, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_LiaOrNot, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_NicoFacts, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_ArchivistaPreviousVisitor, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_NicoWhoWasIt, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_ArchivistaUnknownVisitor, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_ArchivistaVisitorRequests, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_NicoLaricia, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_NicoLariciaEyes, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_LiaCallPost, MSGBOX_DEFAULT
	msgbox VillaPapaleInterno_Text_NicoConcrete, MSGBOX_DEFAULT
	setflag FLAG_VILLA_PAPALE_ARCHIVIO_DONE
	releaseall
	end

VillaPapaleInterno_EventScript_ArchivistaNotReady::
	msgbox VillaPapaleInterno_Text_ArchivistaNotReady, MSGBOX_DEFAULT
	releaseall
	end

VillaPapaleInterno_EventScript_ArchivistaPost::
	msgbox VillaPapaleInterno_Text_ArchivistaPost, MSGBOX_DEFAULT
	releaseall
	end

VillaPapaleInterno_EventScript_LiaArchivio::
	lock
	faceplayer
	goto_if_set FLAG_VILLA_PAPALE_ARCHIVIO_DONE, VillaPapaleInterno_EventScript_LiaArchivioPost
	msgbox VillaPapaleInterno_Text_LiaArchivioWait, MSGBOX_DEFAULT
	release
	end

VillaPapaleInterno_EventScript_LiaArchivioPost::
	msgbox VillaPapaleInterno_Text_LiaArchivioPost, MSGBOX_DEFAULT
	release
	end

VillaPapaleInterno_EventScript_NicoArchivio::
	lock
	faceplayer
	goto_if_set FLAG_VILLA_PAPALE_ARCHIVIO_DONE, VillaPapaleInterno_EventScript_NicoArchivioPost
	msgbox VillaPapaleInterno_Text_NicoArchivioWait, MSGBOX_DEFAULT
	release
	end

VillaPapaleInterno_EventScript_NicoArchivioPost::
	msgbox VillaPapaleInterno_Text_NicoArchivioPost, MSGBOX_DEFAULT
	release
	end

VillaPapaleInterno_EventScript_Funzionario::
	lock
	faceplayer
	msgbox VillaPapaleInterno_Text_Funzionario, MSGBOX_DEFAULT
	release
	end

VillaPapaleInterno_EventScript_Studioso::
	lock
	faceplayer
	msgbox VillaPapaleInterno_Text_Studioso, MSGBOX_DEFAULT
	release
	end

VillaPapaleInterno_EventScript_Assistente::
	lock
	faceplayer
	msgbox VillaPapaleInterno_Text_Assistente, MSGBOX_DEFAULT
	release
	end

VillaPapaleInterno_Text_Funzionario:
	.string "Benvenuto alla Villa Papale.\p"
	.string "Le sale principali sono aperte,\n"
	.string "mentre gli archivi sono consultabili\n"
	.string "solo con il personale.$"

VillaPapaleInterno_Text_Studioso:
	.string "Confrontare carte di epoche diverse\n"
	.string "e sempre affascinante.\p"
	.string "A volte il territorio cambia piu\n"
	.string "di quanto immaginiamo.$"

VillaPapaleInterno_Text_Assistente:
	.string "Molti documenti sono fragili.\p"
	.string "Per questo vengono consultati\n"
	.string "solo sotto supervisione.$"

VillaPapaleInterno_Text_ArchivistaNotReady:
	.string "Gli archivi si consultano\n"
	.string "con il personale.$"

VillaPapaleInterno_Text_ArchivistaAncientMaps:
	.string "ARCHIVISTA: Mi avete chiesto\n"
	.string "le carte piu antiche.$"

VillaPapaleInterno_Text_ArchivistaSources:
	.string "Quelle su sorgenti,\n"
	.string "laghi e falde.$"

VillaPapaleInterno_Text_NicoCompareToday:
	.string "NICO: Le confrontiamo con\n"
	.string "quanto accade oggi.$"

VillaPapaleInterno_Text_LiaChangesFast:
	.string "LIA: Qualcosa cambia troppo\n"
	.string "in fretta.$"

VillaPapaleInterno_Text_ArchivistaTerritoryChanges:
	.string "ARCHIVISTA: Il territorio cambia.\n"
	.string "Lo ha sempre fatto.$"

VillaPapaleInterno_Text_ArchivistaTime:
	.string "Ma certe trasformazioni\n"
	.string "richiedono molto tempo.$"

VillaPapaleInterno_Text_NicoTooFast:
	.string "NICO: E qui sembra accadere\n"
	.string "tutto troppo in fretta.$"

VillaPapaleInterno_Text_ArchivistaLariciaMap:
	.string "ARCHIVISTA:\n"
	.string "Guardate questa carta.\p"
	.string "E' molto antica.$"

VillaPapaleInterno_Text_ArchivistaLariciaLake:
	.string "La conca che oggi chiamiamo\n"
	.string "Valle di Laricia, un tempo,\p"
	.string "era occupata da un lago.$"

VillaPapaleInterno_Text_LiaLariciaFields:
	.string "LIA: Un lago?\n"
	.string "Oggi ci sono campi.$"

VillaPapaleInterno_Text_ArchivistaTerritory:
	.string "ARCHIVISTA: Il territorio\n"
	.string "cambia piu di quanto\p"
	.string "immaginiamo.$"

VillaPapaleInterno_Text_ArchivistaChronicle:
	.string "Questa e una cronaca,\n"
	.string "non un testo scientifico.\p"
	.string "Riporta testimonianze\n"
	.string "molto antiche.$"

VillaPapaleInterno_Text_ArchivistaEarthCall:
	.string "Quando la terra viene ferita,\n"
	.string "alcuni Pokémon sembrano\p"
	.string "rispondere al suo richiamo.$"

VillaPapaleInterno_Text_LiaEmissario:
	.string "LIA: Mi ricorda quello\n"
	.string "che abbiamo visto all'Emissario.$"

VillaPapaleInterno_Text_NicoNotEnough:
	.string "NICO: Somiglia.\n"
	.string "Ma non sappiamo abbastanza.$"

VillaPapaleInterno_Text_ArchivistaRareWitnesses:
	.string "Rari erano coloro\n"
	.string "che sostenevano di udire\p"
	.string "quel richiamo insieme\n"
	.string "ai propri Pokémon.$"

VillaPapaleInterno_Text_LiaHearIt:
	.string "LIA: Udirlo?$"

VillaPapaleInterno_Text_NicoAnyMeaning:
	.string "NICO: Potrebbe voler dire\n"
	.string "qualsiasi cosa.$"

VillaPapaleInterno_Text_LiaOrNot:
	.string "LIA: Oppure no.$"

VillaPapaleInterno_Text_NicoFacts:
	.string "NICO: Prima servono\n"
	.string "altri fatti.$"

VillaPapaleInterno_Text_ArchivistaPreviousVisitor:
	.string "ARCHIVISTA: Curioso...\n"
	.string "Non siete i primi\p"
	.string "a chiedere queste carte.$"

VillaPapaleInterno_Text_NicoWhoWasIt:
	.string "NICO: Chi era?$"

VillaPapaleInterno_Text_ArchivistaUnknownVisitor:
	.string "ARCHIVISTA: Non conosco\n"
	.string "l'identita'.$"

VillaPapaleInterno_Text_ArchivistaVisitorRequests:
	.string "Chiese vecchie mappe,\n"
	.string "documenti sulle falde\p"
	.string "e cronache antiche.$"

VillaPapaleInterno_Text_NicoLaricia:
	.string "NICO: Se vogliamo capire\n"
	.string "quanto c'e di vero...$"

VillaPapaleInterno_Text_NicoLariciaEyes:
	.string "dobbiamo vedere Laricia\n"
	.string "con i nostri occhi.$"

VillaPapaleInterno_Text_LiaCallPost:
	.string "LIA: Continuo a pensare\n"
	.string "a quel richiamo.$"

VillaPapaleInterno_Text_NicoConcrete:
	.string "NICO: Ne parliamo quando\n"
	.string "avremo fatti concreti.$"

VillaPapaleInterno_Text_ArchivistaPost:
	.string "Le carte raccontano molto,\n"
	.string "se sappiamo cosa cercare.$"

VillaPapaleInterno_Text_LiaArchivioWait:
	.string "LIA: Le carte potrebbero\n"
	.string "dirci qualcosa.$"

VillaPapaleInterno_Text_LiaArchivioPost:
	.string "LIA: Continuo a pensare\n"
	.string "a quel richiamo.$"

VillaPapaleInterno_Text_NicoArchivioWait:
	.string "NICO: Prima ascoltiamo\n"
	.string "l'Archivista.$"

VillaPapaleInterno_Text_NicoArchivioPost:
	.string "NICO: Prima i fatti.\n"
	.string "A Laricia capiremo di piu.$"
