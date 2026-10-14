ValleLaricia_Casale1_MapScripts::
	.byte 0

ValleLaricia_Casale1_EventScript_Resident::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_WAILMER_PAIL, ValleLaricia_Casale1_EventScript_CheckOran
	checkitem ITEM_WAILMER_PAIL
	goto_if_eq VAR_RESULT, TRUE, ValleLaricia_Casale1_EventScript_RegisterPail
	msgbox ValleLaricia_Casale1_Text_PailIntro, MSGBOX_DEFAULT
	giveitem ITEM_WAILMER_PAIL
	goto_if_eq VAR_RESULT, FALSE, ValleLaricia_Casale1_EventScript_BagFull
	setflag FLAG_RECEIVED_WAILMER_PAIL
	goto ValleLaricia_Casale1_EventScript_CheckOran

ValleLaricia_Casale1_EventScript_RegisterPail::
	setflag FLAG_RECEIVED_WAILMER_PAIL

ValleLaricia_Casale1_EventScript_CheckOran::
	goto_if_set FLAG_RECEIVED_VALLE_LARICIA_ORAN_BERRIES, ValleLaricia_Casale1_EventScript_After
	msgbox ValleLaricia_Casale1_Text_OranIntro, MSGBOX_DEFAULT
	checkitemspace ITEM_ORAN_BERRY, 2
	goto_if_eq VAR_RESULT, FALSE, ValleLaricia_Casale1_EventScript_BagFull
	giveitem ITEM_ORAN_BERRY, 2
	goto_if_eq VAR_RESULT, FALSE, ValleLaricia_Casale1_EventScript_BagFull
	setflag FLAG_RECEIVED_VALLE_LARICIA_ORAN_BERRIES
	msgbox ValleLaricia_Casale1_Text_After, MSGBOX_DEFAULT
	release
	end

ValleLaricia_Casale1_EventScript_After::
	msgbox ValleLaricia_Casale1_Text_After, MSGBOX_DEFAULT
	release
	end

ValleLaricia_Casale1_EventScript_BagFull::
	msgbox ValleLaricia_Casale1_Text_BagFull, MSGBOX_DEFAULT
	release
	end

ValleLaricia_Casale1_Text_PailIntro:
	.string "Quegli appezzamenti fuori\nsono terra buona per le Bacche.\p"
	.string "Per piantarle basta una Bacca.\nPer curarle, invece, serve questo.$"

ValleLaricia_Casale1_Text_OranIntro:
	.string "Comincia con queste.\nPiantane una e tieni l'altra.$"

ValleLaricia_Casale1_Text_After:
	.string "Una Bacca piantata con pazienza\npuo diventare una piccola scorta.\p"
	.string "Se il terreno ne ha bisogno,\nricordati di annaffiarla.$"

ValleLaricia_Casale1_Text_BagFull:
	.string "Prima fai un po' di spazio nello zaino.$"
