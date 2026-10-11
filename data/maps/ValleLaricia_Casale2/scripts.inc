ValleLaricia_Casale2_MapScripts::
	.byte 0

ValleLaricia_Casale2_EventScript_Resident::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_VALLE_LARICIA_PECHA_BERRIES, ValleLaricia_Casale2_EventScript_After
	msgbox ValleLaricia_Casale2_Text_Intro, MSGBOX_DEFAULT
	checkitemspace ITEM_PECHA_BERRY, 2
	goto_if_eq VAR_RESULT, FALSE, ValleLaricia_Casale2_EventScript_BagFull
	giveitem ITEM_PECHA_BERRY, 2
	goto_if_eq VAR_RESULT, FALSE, ValleLaricia_Casale2_EventScript_BagFull
	setflag FLAG_RECEIVED_VALLE_LARICIA_PECHA_BERRIES
	msgbox ValleLaricia_Casale2_Text_After, MSGBOX_DEFAULT
	release
	end

ValleLaricia_Casale2_EventScript_After::
	msgbox ValleLaricia_Casale2_Text_After, MSGBOX_DEFAULT
	release
	end

ValleLaricia_Casale2_EventScript_BagFull::
	msgbox ValleLaricia_Casale2_Text_BagFull, MSGBOX_DEFAULT
	release
	end

ValleLaricia_Casale2_Text_Intro:
	.string "Nei campi conviene avere sempre\nqualche Bacca a portata di mano.\p"
	.string "Prendi queste.\nUna puoi usarla, l'altra coltivarla.$"

ValleLaricia_Casale2_Text_After:
	.string "Se continui a ripiantarne una,\nla scorta non finisce facilmente.$"

ValleLaricia_Casale2_Text_BagFull:
	.string "Prima fai un po' di spazio nello zaino.$"
