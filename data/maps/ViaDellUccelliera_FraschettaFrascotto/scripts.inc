ViaDellUccelliera_FraschettaFrascotto_MapScripts::
	.byte 0

ViaDellUccelliera_FraschettaFrascotto_EventScript_Frascotto::
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_Frascotto, MSGBOX_NPC
	end

ViaDellUccelliera_FraschettaFrascotto_EventScript_Fraschietto::
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_Fraschietto, MSGBOX_NPC
	end

@ T5.1 preparation: this label is intentionally not called until T5.2
@ integrates the chef battle and its success path.
ViaDellUccelliera_FraschettaFrascotto_EventScript_ChooseChefBerry::
	goto_if_set FLAG_UCCELLIERA_CHEF_BERRY_GIVEN, ViaDellUccelliera_FraschettaFrascotto_EventScript_ChefBerryAlreadyGiven
	lock
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_ChooseChefBerry, MSGBOX_DEFAULT
	fadescreen FADE_TO_BLACK
	closemessage
	special Bag_ChooseBerry
	goto_if_eq VAR_ITEM_ID, 0, ViaDellUccelliera_FraschettaFrascotto_EventScript_CancelChefBerry
	removeitem VAR_ITEM_ID
	goto_if_eq VAR_RESULT, FALSE, ViaDellUccelliera_FraschettaFrascotto_EventScript_ChefBerryRemovalFailed
	setflag FLAG_UCCELLIERA_CHEF_BERRY_GIVEN
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_ChefBerryGiven, MSGBOX_DEFAULT
	release
	end

ViaDellUccelliera_FraschettaFrascotto_EventScript_CancelChefBerry::
	release
	end

ViaDellUccelliera_FraschettaFrascotto_EventScript_ChefBerryAlreadyGiven::
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_ChefBerryAlreadyGiven, MSGBOX_DEFAULT
	release
	end

ViaDellUccelliera_FraschettaFrascotto_EventScript_ChefBerryRemovalFailed::
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_ChefBerryRemovalFailed, MSGBOX_DEFAULT
	release
	end

@ T5.2 will call this only after the chef battle is won.
ViaDellUccelliera_FraschettaFrascotto_EventScript_SetChefDefeated::
	setflag FLAG_UCCELLIERA_CHEF_DEFEATED
	end

ViaDellUccelliera_FraschettaFrascotto_Text_Frascotto:
	.string "Benvenuto nella\n"
	.string "nostra fraschetta!\n"
	.string "Qui la cucina è\n"
	.string "una cosa seria!$"

ViaDellUccelliera_FraschettaFrascotto_Text_Fraschietto:
	.string "Io sono\n"
	.string "Fraschietto!\n"
	.string "Do una mano a\n"
	.string "papà in sala!$"

ViaDellUccelliera_FraschettaFrascotto_Text_ChooseChefBerry:
	.string "Scegli una bacca da\n"
	.string "portare allo chef.$"

ViaDellUccelliera_FraschettaFrascotto_Text_ChefBerryGiven:
	.string "Grazie! Lo chef potrà\n"
	.string "ora prepararsi.$"

ViaDellUccelliera_FraschettaFrascotto_Text_ChefBerryAlreadyGiven:
	.string "La bacca è già stata\n"
	.string "consegnata allo chef.$"

ViaDellUccelliera_FraschettaFrascotto_Text_ChefBerryRemovalFailed:
	.string "Non è stato possibile\n"
	.string "consegnare la bacca.$"
