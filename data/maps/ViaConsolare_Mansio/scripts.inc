ViaConsolare_Mansio_MapScripts::
	.byte 0

ViaConsolare_Mansio_EventScript_Oste::
	lock
	faceplayer
	msgbox ViaConsolare_Mansio_Text_OsteIntro, MSGBOX_YESNO
	switch VAR_RESULT
	case NO, ViaConsolare_Mansio_EventScript_OsteDecline
	case YES, ViaConsolare_Mansio_EventScript_OsteHeal
	case MULTI_B_PRESSED, ViaConsolare_Mansio_EventScript_OsteDecline

ViaConsolare_Mansio_EventScript_OsteHeal::
	playfanfare MUS_HEAL
	waitfanfare
	special HealPlayerParty
	msgbox ViaConsolare_Mansio_Text_OsteAfter, MSGBOX_DEFAULT
	release
	end

ViaConsolare_Mansio_EventScript_OsteDecline::
	msgbox ViaConsolare_Mansio_Text_OsteDecline, MSGBOX_DEFAULT
	release
	end

ViaConsolare_Mansio_EventScript_Viandante::
	msgbox ViaConsolare_Mansio_Text_Viandante, MSGBOX_NPC
	end

ViaConsolare_Mansio_EventScript_Manutentore::
	msgbox ViaConsolare_Mansio_Text_Manutentore, MSGBOX_NPC
	end

ViaConsolare_Mansio_EventScript_Studiosa::
	msgbox ViaConsolare_Mansio_Text_Studiosa, MSGBOX_NPC
	end

ViaConsolare_Mansio_Text_OsteIntro:
	.string "Questa casa accoglie viandanti\n"
	.string "da quando la VIA CONSOLARE era\n"
	.string "il cuore dei commerci.\p"
	.string "Vuoi riposare un po'?$"

ViaConsolare_Mansio_Text_OsteAfter:
	.string "Ecco fatto.\n"
	.string "La strada sembra piu breve\n"
	.string "dopo un po' di riposo.$"

ViaConsolare_Mansio_Text_OsteDecline:
	.string "Come preferisci.\n"
	.string "Qui c'e sempre posto\n"
	.string "per chi percorre la via.$"

ViaConsolare_Mansio_Text_Viandante:
	.string "A nord l'aria cambia.\n"
	.string "Si sente gia l'umidita\n"
	.string "del LAGO DI ALBERA.$"

ViaConsolare_Mansio_Text_Manutentore:
	.string "Il basolato regge da secoli,\n"
	.string "ma il tratto a occidente ha\n"
	.string "bisogno di consolidamento.\p"
	.string "Per ora resta chiuso.$"

ViaConsolare_Mansio_Text_Studiosa:
	.string "Il canale lungo la via segue\n"
	.string "la stessa pendenza dei CISTERNONI.\p"
	.string "Potrebbero appartenere\n"
	.string "a un'unica rete sotterranea.$"
