StradaBorgoCisternoni_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, StradaBorgoCisternoni_OnTransition
	.byte 0

StradaBorgoCisternoni_OnTransition::
	goto_if_set FLAG_STRADA_BORGO_CISTERNONI_NICO_LIA_DONE, StradaBorgoCisternoni_OnTransition_HideNicoLia
	goto_if_unset FLAG_VILLA_PAPALE_ARCHIVIO_DONE, StradaBorgoCisternoni_OnTransition_HideNicoLia
	clearflag FLAG_HIDE_STRADA_BORGO_CISTERNONI_NICO_LIA
	setvar VAR_TEMP_0, 0
	end

StradaBorgoCisternoni_OnTransition_HideNicoLia::
	setflag FLAG_HIDE_STRADA_BORGO_CISTERNONI_NICO_LIA
	setvar VAR_TEMP_0, 1
	end

StradaBorgoCisternoni_EventScript_StartNicoLiaScene::
	lockall
	goto_if_set FLAG_STRADA_BORGO_CISTERNONI_NICO_LIA_DONE, StradaBorgoCisternoni_EventScript_EndNicoLiaScene
	goto_if_unset FLAG_VILLA_PAPALE_ARCHIVIO_DONE, StradaBorgoCisternoni_EventScript_EndNicoLiaScene
	msgbox StradaBorgoCisternoni_Text_NicoDirection, MSGBOX_DEFAULT
	msgbox StradaBorgoCisternoni_Text_NicoLaricia, MSGBOX_DEFAULT
	msgbox StradaBorgoCisternoni_Text_LiaChronicle, MSGBOX_DEFAULT
	msgbox StradaBorgoCisternoni_Text_NicoFacts, MSGBOX_DEFAULT
	msgbox StradaBorgoCisternoni_Text_LiaGo, MSGBOX_DEFAULT
	msgbox StradaBorgoCisternoni_Text_NicoRoute, MSGBOX_DEFAULT
	setflag FLAG_STRADA_BORGO_CISTERNONI_NICO_LIA_DONE
	setvar VAR_TEMP_0, 1
	applymovement LOCALID_STRADA_BORGO_CISTERNONI_NICO, StradaBorgoCisternoni_Movement_NicoLeavesSouth
	waitmovement 0
	applymovement LOCALID_STRADA_BORGO_CISTERNONI_LIA, StradaBorgoCisternoni_Movement_LiaLeavesSouth
	waitmovement 0
	setflag FLAG_HIDE_STRADA_BORGO_CISTERNONI_NICO_LIA
	removeobject LOCALID_STRADA_BORGO_CISTERNONI_NICO
	removeobject LOCALID_STRADA_BORGO_CISTERNONI_LIA

StradaBorgoCisternoni_EventScript_EndNicoLiaScene::
	releaseall
	end

StradaBorgoCisternoni_Movement_NicoLeavesSouth:
	walk_down
	step_end

StradaBorgoCisternoni_Movement_LiaLeavesSouth:
	walk_down
	step_end

StradaBorgoCisternoni_Text_NicoDirection:
	.string "NICO: Le carte indicano\n"
	.string "questa direzione.$"

StradaBorgoCisternoni_Text_NicoLaricia:
	.string "NICO: Se vogliamo capire\n"
	.string "cosa sta succedendo,\p"
	.string "dobbiamo arrivare\n"
	.string "a Laricia.$"

StradaBorgoCisternoni_Text_LiaChronicle:
	.string "LIA: Io continuo a pensare\n"
	.string "a quella cronaca...$"

StradaBorgoCisternoni_Text_NicoFacts:
	.string "NICO: Lo so.\p"
	.string "Ma prima vediamo\n"
	.string "cosa troviamo sul posto.$"

StradaBorgoCisternoni_Text_LiaGo:
	.string "LIA: Allora andiamo.$"

StradaBorgoCisternoni_Text_NicoRoute:
	.string "NICO: Scendiamo fino\n"
	.string "alla Via dei Cisternoni.\p"
	.string "Da li proseguiamo\n"
	.string "verso Laricia.$"
