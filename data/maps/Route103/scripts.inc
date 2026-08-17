Route103_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Route103_OnLoad
	.byte 0

Route103_OnLoad:
	call_if_unset FLAG_BADGE01_GET, Route103_EventScript_CloseCisternoniEntrance
	call_if_set FLAG_BADGE01_GET, Route103_EventScript_OpenCisternoniEntrance
	goto_if_unset FLAG_BADGE01_GET, Route103_OnLoad_HideParty
	goto_if_set FLAG_CISTERNONI_AUREA_ENCOUNTER_COMPLETE, Route103_OnLoad_ShowNico
	goto_if_set FLAG_CISTERNONI_LIA_READY, Route103_OnLoad_HideParty
	clearflag FLAG_HIDE_ROUTE103_LIA
	setflag FLAG_HIDE_ROUTE103_NICO
	end

Route103_EventScript_CloseCisternoniEntrance:
	setmetatile 52, 7, METATILE_General_CaveEntrance_Bottom, TRUE
	return

Route103_EventScript_OpenCisternoniEntrance:
	setmetatile 52, 7, METATILE_General_CaveEntrance_Bottom, FALSE
	return

Route103_OnLoad_ShowNico:
	setflag FLAG_HIDE_ROUTE103_LIA
	clearflag FLAG_HIDE_ROUTE103_NICO
	end

Route103_OnLoad_HideParty:
	setflag FLAG_HIDE_ROUTE103_LIA
	setflag FLAG_HIDE_ROUTE103_NICO
	end

Route103_EventScript_LiaBeforeCisternoni::
	lockall
	goto_if_set FLAG_CISTERNONI_LIA_READY, Route103_EventScript_LiaBeforeCisternoniEnd
	goto_if_set FLAG_CISTERNONI_AUREA_ENCOUNTER_COMPLETE, Route103_EventScript_LiaBeforeCisternoniEnd
	msgbox Route103_Text_LiaBeforeCisternoni, MSGBOX_DEFAULT
	setflag FLAG_CISTERNONI_LIA_READY
	setflag FLAG_HIDE_ROUTE103_LIA
	removeobject LOCALID_ROUTE103_LIA
	releaseall
	end

Route103_EventScript_LiaBeforeCisternoniEnd:
	releaseall
	end

Route103_EventScript_NicoAfterCisternoni::
	lockall
	faceplayer
	goto_if_set FLAG_CISTERNONI_NICO_POST_AUREA_TALKED, Route103_EventScript_NicoReminder
	msgbox Route103_Text_NicoAfterCisternoni, MSGBOX_DEFAULT
	setflag FLAG_CISTERNONI_NICO_POST_AUREA_TALKED
	releaseall
	end

Route103_EventScript_NicoReminder:
	msgbox Route103_Text_NicoReminder, MSGBOX_DEFAULT
	releaseall
	end

Route103_EventScript_Marco::
	trainerbattle_single TRAINER_CISTERNONI_MARCO, Route103_Text_MarcoIntro, Route103_Text_MarcoDefeat
	msgbox Route103_Text_MarcoPostBattle, MSGBOX_AUTOCLOSE
	end

Route103_EventScript_Teo::
	trainerbattle_single TRAINER_CISTERNONI_TEO, Route103_Text_TeoIntro, Route103_Text_TeoDefeat
	msgbox Route103_Text_TeoPostBattle, MSGBOX_AUTOCLOSE
	end

Route103_EventScript_Fisherman::
	msgbox Route103_Text_Fisherman, MSGBOX_NPC
	end

Route103_EventScript_Visitor::
	msgbox Route103_Text_Visitor, MSGBOX_NPC
	end

Route103_EventScript_Walker::
	msgbox Route103_Text_Walker, MSGBOX_NPC
	end

Route103_Text_MarcoIntro:
	.string "Queste pietre sono scivolose quando\n"
	.string "l'aria e umida. Fammi vedere il passo!$"

Route103_Text_MarcoDefeat:
	.string "Hai un passo sicuro!$"

Route103_Text_MarcoPostBattle:
	.string "Le vie antiche chiedono rispetto\n"
	.string "a chi le attraversa.$"

Route103_Text_TeoIntro:
	.string "Gli uccelli cambiano rotta qui.\n"
	.string "Forse sentono cio che noi non sentiamo.$"

Route103_Text_TeoDefeat:
	.string "Non ti hanno distratto.$"

Route103_Text_TeoPostBattle:
	.string "Il vento porta segreti lungo\n"
	.string "questa via.$"

Route103_Text_Fisherman:
	.string "L'acqua sembra ferma, ma l'aria e\n"
	.string "piu fresca vicino ai Cisternoni.$"

Route103_Text_Visitor:
	.string "I Cisternoni raccolgono acqua da\n"
	.string "secoli. Dovevo fermarmi a guardarli.$"

Route103_Text_Walker:
	.string "La Via dei Cisternoni porta lontano,\n"
	.string "ma tutti parlano delle sue vasche.$"

Route103_Text_LiaBeforeCisternoni:
	.string "LIA: Nei documenti che ho letto\n"
	.string "c'era un indizio che portava proprio\l"
	.string "ai CISTERNONI.\p"
	.string "Voglio controllare di persona.\n"
	.string "Entriamo insieme?$"

Route103_Text_NicoAfterCisternoni:
	.string "NICO: Una tipa losca nei CISTERNONI?\n"
	.string "Mmh... questa storia mi puzza.\p"
	.string "Meno male che c'eravate voi!\p"
	.string "Io intanto ho battuto LIRIO!\n"
	.string "Ora si va verso la VIA CONSOLARE!$"

Route103_Text_NicoReminder:
	.string "NICO: Direzione VIA CONSOLARE!$"

Route103_EventScript_CisternoniAccessClosed::
	msgbox Route103_Text_CisternoniAccessClosed, MSGBOX_SIGN
	end

Route103_Text_CisternoniAccessClosed:
	.string "L'accesso ai CISTERNONI è\n"
	.string "interdetto.$"
