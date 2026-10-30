ValleLaricia_CasettaAgricola_MapScripts::
	.byte 0

ValleLaricia_CasettaAgricola_EventScript_Resident::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_VALLE_LARICIA_CHERI_BERRIES, ValleLaricia_CasettaAgricola_EventScript_After
	msgbox ValleLaricia_CasettaAgricola_Text_Intro, MSGBOX_DEFAULT
	checkitemspace ITEM_CHERI_BERRY, 2
	goto_if_eq VAR_RESULT, FALSE, ValleLaricia_CasettaAgricola_EventScript_BagFull
	giveitem ITEM_CHERI_BERRY, 2
	goto_if_eq VAR_RESULT, FALSE, ValleLaricia_CasettaAgricola_EventScript_BagFull
	setflag FLAG_RECEIVED_VALLE_LARICIA_CHERI_BERRIES
	msgbox ValleLaricia_CasettaAgricola_Text_After, MSGBOX_DEFAULT
	release
	end

ValleLaricia_CasettaAgricola_EventScript_After::
	msgbox ValleLaricia_CasettaAgricola_Text_After, MSGBOX_DEFAULT
	release
	end

ValleLaricia_CasettaAgricola_EventScript_BagFull::
	msgbox ValleLaricia_CasettaAgricola_Text_BagFull, MSGBOX_DEFAULT
	release
	end

ValleLaricia_CasettaAgricola_Text_Intro:
	.string "Qui non si butta quasi niente.\p"
	.string "Anche una sola Bacca,\nse la ripianti, puo dare altro raccolto.$"

ValleLaricia_CasettaAgricola_Text_After:
	.string "Una per lo zaino,\nuna per la terra. E si ricomincia.$"

ValleLaricia_CasettaAgricola_Text_BagFull:
	.string "Prima fai un po' di spazio nello zaino.$"
