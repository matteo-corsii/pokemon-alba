StradaBorgoCisternoni_Casa_MapScripts::
	.byte 0

StradaBorgoCisternoni_Casa_EventScript_Rilevatore::
	lock
	faceplayer
	checkitem ITEM_ITEMFINDER
	goto_if_eq VAR_RESULT, TRUE, StradaBorgoCisternoni_Casa_EventScript_PostItemfinder
	msgbox StradaBorgoCisternoni_Casa_Text_Intro, MSGBOX_DEFAULT
	checkitemspace ITEM_ITEMFINDER
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	giveitem_msg StradaBorgoCisternoni_Casa_Text_ReceivedItemfinder, ITEM_ITEMFINDER, 1, MUS_OBTAIN_KEY_ITEM
	msgbox StradaBorgoCisternoni_Casa_Text_PostItemfinder, MSGBOX_DEFAULT
	release
	end

StradaBorgoCisternoni_Casa_EventScript_PostItemfinder::
	msgbox StradaBorgoCisternoni_Casa_Text_PostItemfinder, MSGBOX_DEFAULT
	release
	end

StradaBorgoCisternoni_Casa_Text_Intro:
	.string "Chi cammina su queste strade\n"
	.string "impara a guardare bene.\p"
	.string "Ci sono cose che passano\n"
	.string "inosservate ogni giorno.\p"
	.string "Questo potrebbe esserti utile.$"

StradaBorgoCisternoni_Casa_Text_ReceivedItemfinder:
	.string "Hai ricevuto il\n"
	.string "CERCASOGGETTI.$"

StradaBorgoCisternoni_Casa_Text_PostItemfinder:
	.string "Usalo quando hai la sensazione\n"
	.string "che ci sia qualcosa nei dintorni.$"
