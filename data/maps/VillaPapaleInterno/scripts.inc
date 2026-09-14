VillaPapaleInterno_MapScripts::
	.byte 0

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
