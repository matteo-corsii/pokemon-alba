Cisternoni_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Cisternoni_OnLoad
	.byte 0

Cisternoni_OnLoad:
	goto_if_unset FLAG_BADGE01_GET, Cisternoni_OnLoad_HideScene
	goto_if_unset FLAG_CISTERNONI_LIA_READY, Cisternoni_OnLoad_HideScene
	goto_if_set FLAG_CISTERNONI_AUREA_ENCOUNTER_COMPLETE, Cisternoni_OnLoad_HideScene
	clearflag FLAG_HIDE_CISTERNONI_LIA
	clearflag FLAG_HIDE_CISTERNONI_AUREA_RECRUIT
	end

Cisternoni_OnLoad_HideScene:
	setflag FLAG_HIDE_CISTERNONI_LIA
	setflag FLAG_HIDE_CISTERNONI_AUREA_RECRUIT
	end

Cisternoni_EventScript_Lia::
	msgbox Cisternoni_Text_LiaReminder, MSGBOX_NPC
	end

Cisternoni_EventScript_AureaRecruit::
	msgbox Cisternoni_Text_AureaCover, MSGBOX_NPC
	end

Cisternoni_EventScript_StartAureaScene::
	lockall
	goto_if_set FLAG_CISTERNONI_AUREA_ENCOUNTER_COMPLETE, Cisternoni_EventScript_EndScene
	applymovement LOCALID_CISTERNONI_LIA, Cisternoni_Movement_LiaApproachWater
	waitmovement 0
	msgbox Cisternoni_Text_LiaMeasurements, MSGBOX_DEFAULT
	applymovement LOCALID_CISTERNONI_AUREA_RECRUIT, Common_Movement_FaceLeft
	applymovement LOCALID_CISTERNONI_LIA, Common_Movement_FaceRight
	waitmovement 0
	msgbox Cisternoni_Text_LiaSuspicious, MSGBOX_DEFAULT
	msgbox Cisternoni_Text_AureaCoverBroken, MSGBOX_DEFAULT
	msgbox Cisternoni_Text_AureaBattleIntro, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_CISTERNONI_AUREA_RECRUIT, Cisternoni_Text_AureaDefeat
	goto Cisternoni_EventScript_AureaDefeated
	end

Cisternoni_EventScript_AureaDefeated:
	msgbox Cisternoni_Text_AureaPostBattle, MSGBOX_DEFAULT
	msgbox Cisternoni_Text_LiaAfterAurea, MSGBOX_DEFAULT
	setflag FLAG_CISTERNONI_AUREA_ENCOUNTER_COMPLETE
	setflag FLAG_HIDE_CISTERNONI_AUREA_RECRUIT
	setflag FLAG_HIDE_CISTERNONI_LIA
	removeobject LOCALID_CISTERNONI_AUREA_RECRUIT
	removeobject LOCALID_CISTERNONI_LIA
	releaseall
	end

Cisternoni_EventScript_EndScene:
	releaseall
	end

Cisternoni_Movement_LiaApproachWater:
	walk_down
	walk_down
	walk_down
	step_end

Cisternoni_Text_LiaReminder:
	.string "Sto confrontando le misure delle\n"
	.string "vasche con quelle di Via Verdi.$"

Cisternoni_Text_AureaCover:
	.string "Le antiche opere idriche sono\n"
	.string "affascinanti, non trovi?$"

Cisternoni_Text_LiaMeasurements:
	.string "LIA: Qui il suono cambia vicino alle\n"
	.string "vasche. Voglio confrontare queste\n"
	.string "misure con quelle di Via Verdi.$"

Cisternoni_Text_LiaSuspicious:
	.string "LIA: Aspetta. Quella donna annota\n"
	.string "le nostre misure, non quelle\n"
	.string "delle vasche.$"

Cisternoni_Text_AureaCoverBroken:
	.string "DONNA: Osservo soltanto. Non c'e\n"
	.string "nulla di strano.$"

Cisternoni_Text_AureaBattleIntro:
	.string "AUREA: Va bene. Sono una recluta\n"
	.string "del TEAM AUREA. I vostri dati\n"
	.string "interessano alla mia organizzazione.$"

Cisternoni_Text_AureaDefeat:
	.string "Non avrei dovuto farmi notare...$"

Cisternoni_Text_AureaPostBattle:
	.string "AUREA: Non importa. Il TEAM AUREA\n"
	.string "seguira ancora queste indagini.$"

Cisternoni_Text_LiaAfterAurea:
	.string "LIA: Qualcuno segue le nostre ricerche.\n"
	.string "Non sappiamo perche, quindi annotiamo\n"
	.string "tutto con cura.$"
