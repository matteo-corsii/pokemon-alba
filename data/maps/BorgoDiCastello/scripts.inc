BorgoDiCastello_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, BorgoDiCastello_OnTransition
	.byte 0

BorgoDiCastello_OnTransition::
	goto_if_set FLAG_BORGO_NICO_LIA_INTRO_DONE, BorgoDiCastello_OnTransition_HideIntroParty
	goto_if_unset FLAG_EMISSARIO_AUREA_ENCOUNTER_COMPLETE, BorgoDiCastello_OnTransition_HideIntroParty
	clearflag FLAG_HIDE_BORGO_DI_CASTELLO_NICO_LIA
	setvar VAR_TEMP_0, 0
	end

BorgoDiCastello_OnTransition_HideIntroParty::
	setflag FLAG_HIDE_BORGO_DI_CASTELLO_NICO_LIA
	setvar VAR_TEMP_0, 1
	end

BorgoDiCastello_EventScript_StartNicoLiaIntro::
	lockall
	goto_if_set FLAG_BORGO_NICO_LIA_INTRO_DONE, BorgoDiCastello_EventScript_EndNicoLiaIntro
	goto_if_unset FLAG_EMISSARIO_AUREA_ENCOUNTER_COMPLETE, BorgoDiCastello_EventScript_EndNicoLiaIntro
	msgbox BorgoDiCastello_Text_NicoLiaIntroNicoGreeting, MSGBOX_DEFAULT
	msgbox BorgoDiCastello_Text_NicoLiaIntroLiaMaps, MSGBOX_DEFAULT
	msgbox BorgoDiCastello_Text_NicoLiaIntroNicoFalde, MSGBOX_DEFAULT
	msgbox BorgoDiCastello_Text_NicoLiaIntroLiaEmissario, MSGBOX_DEFAULT
	msgbox BorgoDiCastello_Text_NicoLiaIntroNicoDocuments, MSGBOX_DEFAULT
	msgbox BorgoDiCastello_Text_NicoLiaIntroNicoVilla, MSGBOX_DEFAULT
	setflag FLAG_BORGO_NICO_LIA_INTRO_DONE
	setvar VAR_TEMP_0, 1
	applymovement LOCALID_BORGO_DI_CASTELLO_LIA_INTRO, BorgoDiCastello_Movement_LiaLeavesNorth
	waitmovement 0
	applymovement LOCALID_BORGO_DI_CASTELLO_NICO_INTRO, BorgoDiCastello_Movement_NicoLeavesNorth
	waitmovement 0
	setflag FLAG_HIDE_BORGO_DI_CASTELLO_NICO_LIA
	removeobject LOCALID_BORGO_DI_CASTELLO_LIA_INTRO
	removeobject LOCALID_BORGO_DI_CASTELLO_NICO_INTRO
BorgoDiCastello_EventScript_EndNicoLiaIntro::
	releaseall
	end

BorgoDiCastello_Movement_LiaLeavesNorth:
	walk_up
	step_end

BorgoDiCastello_Movement_NicoLeavesNorth:
	walk_up
	step_end

BorgoDiCastello_EventScript_VillaPapaleSign::
	msgbox BorgoDiCastello_Text_VillaPapaleSign, MSGBOX_SIGN
	end

BorgoDiCastello_Text_VillaPapaleSign:
	.string "Villa Papale\n"
	.string "{UP_ARROW} A nord$"

BorgoDiCastello_EventScript_AnzianoBelvedere::
	msgbox BorgoDiCastello_Text_AnzianoBelvedere, MSGBOX_NPC
	end

BorgoDiCastello_EventScript_TuristaBelvedere::
	msgbox BorgoDiCastello_Text_TuristaBelvedere, MSGBOX_NPC
	end

BorgoDiCastello_EventScript_ResidenteVicoli::
	msgbox BorgoDiCastello_Text_ResidenteVicoli, MSGBOX_NPC
	end

BorgoDiCastello_EventScript_ViaggiatoreLaricia::
	msgbox BorgoDiCastello_Text_ViaggiatoreLaricia, MSGBOX_NPC
	end

BorgoDiCastello_EventScript_Bambino::
	msgbox BorgoDiCastello_Text_Bambino, MSGBOX_NPC
	end

BorgoDiCastello_EventScript_DonnaVilla::
	msgbox BorgoDiCastello_Text_DonnaVilla, MSGBOX_NPC
	end

BorgoDiCastello_EventScript_ViaggiatorePiazza::
	msgbox BorgoDiCastello_Text_ViaggiatorePiazza, MSGBOX_NPC
	end

BorgoDiCastello_EventScript_AbitanteFontana::
	msgbox BorgoDiCastello_Text_AbitanteFontana, MSGBOX_NPC
	end

BorgoDiCastello_Text_AnzianoBelvedere:
	.string "Da quassu il LAGO DI ALBERA\n"
	.string "sembra sempre lo stesso.\p"
	.string "Ma chi vive qui da tanto\n"
	.string "sa che non e proprio cosi.$"

BorgoDiCastello_Text_TuristaBelvedere:
	.string "Che vista!\p"
	.string "Capisco perche questo posto\n"
	.string "attiri gente da ogni parte.$"

BorgoDiCastello_Text_ResidenteVicoli:
	.string "Ultimamente si vedono\n"
	.string "parecchi forestieri.\p"
	.string "Pare che alcuni siano molto\n"
	.string "interessati alla Villa Papale.$"

BorgoDiCastello_Text_ViaggiatoreLaricia:
	.string "Verso est la strada scende\n"
	.string "in direzione di Laricia.\p"
	.string "Piu avanti il paesaggio\n"
	.string "cambia completamente.$"

BorgoDiCastello_Text_Bambino:
	.string "Ogni tanto vedo dei POKEMON\n"
	.string "saltare sui tetti!\p"
	.string "Sono troppo veloci per me!$"

BorgoDiCastello_Text_DonnaVilla:
	.string "La Villa Papale conserva\n"
	.string "documenti molto antichi.\p"
	.string "Non tutto, pero, e accessibile\n"
	.string "ai visitatori.$"

BorgoDiCastello_Text_ViaggiatorePiazza:
	.string "Arrivo dalla zona di Laricia.\p"
	.string "La campagna laggiu e enorme.\n"
	.string "Campi a perdita d'occhio.$"

BorgoDiCastello_Text_AbitanteFontana:
	.string "La fontana e il punto\n"
	.string "d'incontro di tutto il borgo.\p"
	.string "Prima o poi qui passa sempre\n"
	.string "qualcuno che conosci.$"

BorgoDiCastello_Text_NicoLiaIntroNicoGreeting:
	.string "NICO: Eccoti.\n"
	.string "Abbiamo chiesto in giro.$"

BorgoDiCastello_Text_NicoLiaIntroLiaMaps:
	.string "LIA: Alla Villa conservano\n"
	.string "vecchie mappe.\p"
	.string "E documenti sul\n"
	.string "territorio.$"

BorgoDiCastello_Text_NicoLiaIntroNicoFalde:
	.string "NICO: Per capire le falde,\n"
	.string "partiamo da quei documenti.$"

BorgoDiCastello_Text_NicoLiaIntroLiaEmissario:
	.string "LIA: Magari ci aiuteranno\n"
	.string "con l'Emissario...$"

BorgoDiCastello_Text_NicoLiaIntroNicoDocuments:
	.string "NICO: Una cosa alla volta.\p"
	.string "Prima leggiamo\n"
	.string "quei documenti.$"

BorgoDiCastello_Text_NicoLiaIntroNicoVilla:
	.string "NICO: Ci troviamo\n"
	.string "alla Villa.$"
