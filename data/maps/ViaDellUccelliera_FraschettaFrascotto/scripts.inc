ViaDellUccelliera_FraschettaFrascotto_MapScripts::
	#if !IS_FRLG
	map_script MAP_SCRIPT_ON_TRANSITION, ViaDellUccelliera_FraschettaFrascotto_OnTransition
	#else
	.byte 0
	#endif

ViaDellUccelliera_FraschettaFrascotto_OnTransition:
	#if !IS_FRLG
	goto_if_unset FLAG_UCCELLIERA_CHEF_DEFEATED, ViaDellUccelliera_FraschettaFrascotto_OnTransition_Done
	removeobject LOCALID_VIA_DELL_UCCELLIERA_FRASCOTTO
	#endif
ViaDellUccelliera_FraschettaFrascotto_OnTransition_Done:
	end

ViaDellUccelliera_FraschettaFrascotto_EventScript_Frascotto::
	lock
	faceplayer
#if !IS_FRLG
	goto_if_set FLAG_UCCELLIERA_CHEF_DEFEATED, ViaDellUccelliera_FraschettaFrascotto_EventScript_FrascottoAfter
	special HasEnoughMonsForDoubleBattle
	goto_if_ne VAR_RESULT, PLAYER_HAS_TWO_USABLE_MONS, ViaDellUccelliera_FraschettaFrascotto_EventScript_ChefBattleNotEnoughMons
	goto_if_set FLAG_UCCELLIERA_CHEF_BERRY_GIVEN, ViaDellUccelliera_FraschettaFrascotto_EventScript_ChefBattle
	goto ViaDellUccelliera_FraschettaFrascotto_EventScript_ChooseChefBerry
#else
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_Frascotto_Common, MSGBOX_NPC
	release
	end
#endif

#if !IS_FRLG
ViaDellUccelliera_FraschettaFrascotto_EventScript_FrascottoAfter::
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_FrascottoAfter, MSGBOX_NPC
	release
	end
#endif

ViaDellUccelliera_FraschettaFrascotto_EventScript_Fraschietto::
#if !IS_FRLG
	goto_if_set FLAG_UCCELLIERA_CHEF_DEFEATED, ViaDellUccelliera_FraschettaFrascotto_EventScript_FraschiettoManager
#endif
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_Fraschietto_Common, MSGBOX_NPC
	end

ViaDellUccelliera_FraschettaFrascotto_Text_Frascotto_Common:
	.string "Benvenuto nella\n"
	.string "nostra fraschetta!\n"
	.string "Qui la cucina è\n"
	.string "una cosa seria!$"

ViaDellUccelliera_FraschettaFrascotto_Text_Fraschietto_Common:
	.string "Io sono\n"
	.string "Fraschietto!\n"
	.string "Do una mano a\n"
	.string "papà in sala!$"

#if !IS_FRLG
ViaDellUccelliera_FraschettaFrascotto_EventScript_FraschiettoManager::
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_FraschiettoManager, MSGBOX_NPC
	end
#endif

#if !IS_FRLG
ViaDellUccelliera_FraschettaFrascotto_EventScript_ChooseChefBerry::
	goto_if_set FLAG_UCCELLIERA_CHEF_BERRY_GIVEN, ViaDellUccelliera_FraschettaFrascotto_EventScript_ChefBerryAlreadyGiven
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_ChooseChefBerry, MSGBOX_DEFAULT
	fadescreen FADE_TO_BLACK
	closemessage
	special Bag_ChooseBerry
	goto_if_eq VAR_ITEM_ID, 0, ViaDellUccelliera_FraschettaFrascotto_EventScript_CancelChefBerry
	removeitem VAR_ITEM_ID
	goto_if_eq VAR_RESULT, FALSE, ViaDellUccelliera_FraschettaFrascotto_EventScript_ChefBerryRemovalFailed
	setflag FLAG_UCCELLIERA_CHEF_BERRY_GIVEN
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_ChefBerryGiven, MSGBOX_DEFAULT
	goto ViaDellUccelliera_FraschettaFrascotto_EventScript_ChefBattle

ViaDellUccelliera_FraschettaFrascotto_EventScript_CancelChefBerry::
	release
	end

ViaDellUccelliera_FraschettaFrascotto_EventScript_ChefBerryAlreadyGiven::
	goto ViaDellUccelliera_FraschettaFrascotto_EventScript_ChefBattle

ViaDellUccelliera_FraschettaFrascotto_EventScript_ChefBerryRemovalFailed::
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_ChefBerryRemovalFailed, MSGBOX_DEFAULT
	release
	end

ViaDellUccelliera_FraschettaFrascotto_EventScript_ChefBattle::
	special HasEnoughMonsForDoubleBattle
	goto_if_ne VAR_RESULT, PLAYER_HAS_TWO_USABLE_MONS, ViaDellUccelliera_FraschettaFrascotto_EventScript_ChefBattleNotEnoughMons
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_FraschiettoBattle, MSGBOX_DEFAULT
	special PrepareChefBattle
	setwildbattle SPECIES_FRASCOTTO, 31, ITEM_NONE, SPECIES_FRASCHIETTO, 30, ITEM_NONE
	dowildbattle
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, ViaDellUccelliera_FraschettaFrascotto_EventScript_ChefBattleWon
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_ChefBattleLost, MSGBOX_DEFAULT
	release
	end

ViaDellUccelliera_FraschettaFrascotto_EventScript_ChefBattleNotEnoughMons::
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_ChefBattleNotEnoughMons, MSGBOX_DEFAULT
	release
	end

ViaDellUccelliera_FraschettaFrascotto_EventScript_ChefBattleWon::
	setflag FLAG_UCCELLIERA_CHEF_DEFEATED
	removeobject LOCALID_VIA_DELL_UCCELLIERA_FRASCOTTO
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_ChefBattleWon, MSGBOX_DEFAULT
	release
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

ViaDellUccelliera_FraschettaFrascotto_Text_ChefBattleNotEnoughMons:
	.string "Servono almeno due Pokémon\n"
	.string "in grado di lottare.$"

ViaDellUccelliera_FraschettaFrascotto_Text_FrascottoAfter:
	.string "Lo chef ha superato la prova.\n"
	.string "Ora posso dedicarmi alla cucina!$"

ViaDellUccelliera_FraschettaFrascotto_Text_FraschiettoBattle:
	.string "Preparati! Quando papà si mette ai fornelli,\n"
	.string "la sala diventa bollente!$"

ViaDellUccelliera_FraschettaFrascotto_Text_FraschiettoManager:
	.string "Papà si è riposato dopo la sfida.\n"
	.string "Ora mi occupo io della sala!$"

ViaDellUccelliera_FraschettaFrascotto_Text_ChefBattleLost:
	.string "Riprova quando sarai pronto.\n"
	.string "La bacca e ancora conservata.$"

ViaDellUccelliera_FraschettaFrascotto_Text_ChefBattleWon:
	.string "La sfida e finita!\n"
	.string "Frascotto, ora puoi riposarti.$"
#endif
