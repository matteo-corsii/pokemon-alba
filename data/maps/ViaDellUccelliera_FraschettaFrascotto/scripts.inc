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
	setflag FLAG_UCCELLIERA_CHEF_SEEN
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

ViaDellUccelliera_FraschettaFrascotto_EventScript_Cliente::
	lock
	faceplayer
#if !IS_FRLG
	goto_if_set FLAG_UCCELLIERA_CHEF_DEFEATED, ViaDellUccelliera_FraschettaFrascotto_EventScript_ClienteAfter
#endif
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_Cliente, MSGBOX_NPC
	release
	end

#if !IS_FRLG
ViaDellUccelliera_FraschettaFrascotto_EventScript_ClienteAfter::
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_ClienteAfter, MSGBOX_NPC
	release
	end
#endif

ViaDellUccelliera_FraschettaFrascotto_EventScript_Turista::
	lock
	faceplayer
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_Turista, MSGBOX_NPC
	release
	end

ViaDellUccelliera_FraschettaFrascotto_EventScript_Affamato::
	lock
	faceplayer
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_Affamato, MSGBOX_NPC
	release
	end

ViaDellUccelliera_FraschettaFrascotto_EventScript_Amica::
	lock
	faceplayer
#if !IS_FRLG
	goto_if_set FLAG_UCCELLIERA_CHEF_DEFEATED, ViaDellUccelliera_FraschettaFrascotto_EventScript_AmicaAfter
#endif
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_Amica, MSGBOX_NPC
	release
	end

#if !IS_FRLG
ViaDellUccelliera_FraschettaFrascotto_EventScript_AmicaAfter::
	msgbox ViaDellUccelliera_FraschettaFrascotto_Text_AmicaAfter, MSGBOX_NPC
	release
	end
#endif

ViaDellUccelliera_FraschettaFrascotto_Text_Cliente:
	.string "Conosco Frascotto da una vita!\n"
	.string "Quando si mette ai fornelli,\n"
	.string "devi vedere che spettacolo!\p"
	.string "L'unico problema è convincerlo\n"
	.string "a smettere di cucinare!$"

ViaDellUccelliera_FraschettaFrascotto_Text_ClienteAfter:
	.string "Frascotto è partito, ma\n"
	.string "Fraschietto sta facendo un\n"
	.string "gran lavoro! La fraschetta\n"
	.string "è in ottime mani!$"

ViaDellUccelliera_FraschettaFrascotto_Text_Turista:
	.string "Ho scoperto che un tempo\n"
	.string "le fraschette esponevano\n"
	.string "una frasca per indicare\n"
	.string "che si vendeva vino!\p"
	.string "Ecco perché si chiamano\n"
	.string "così!$"

ViaDellUccelliera_FraschettaFrascotto_Text_Affamato:
	.string "Ho ordinato un panino con\n"
	.string "la porchetta...\p"
	.string "Poi ho visto passare la\n"
	.string "carbonara. Ora non so più\n"
	.string "cosa scegliere!$"

ViaDellUccelliera_FraschettaFrascotto_Text_Amica:
	.string "Fraschietto è cresciuto tra\n"
	.string "questi tavoli. Vedrai, un\n"
	.string "giorno sarà lui a mandare\n"
	.string "avanti la fraschetta!\p"
	.string "Ha preso tutto dal padre...\n"
	.string "tranne la pazienza!$"

ViaDellUccelliera_FraschettaFrascotto_Text_AmicaAfter:
	.string "Guarda un po' Fraschietto!\n"
	.string "È diventato il padrone di\n"
	.string "casa. Suo padre dev'essere\n"
	.string "proprio orgoglioso!$"
