BoscoDelRomitorio_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, BoscoDelRomitorio_OnTransition
	.byte 0

BoscoDelRomitorio_OnTransition:
	call_if_eq VAR_STARTER_MON, 0, BoscoDelRomitorio_EventScript_HideCingerm
	call_if_eq VAR_STARTER_MON, 1, BoscoDelRomitorio_EventScript_HideSerbrace
	call_if_eq VAR_STARTER_MON, 2, BoscoDelRomitorio_EventScript_HideArdeino
	end

BoscoDelRomitorio_EventScript_HideCingerm::
	setflag FLAG_HIDE_BOSCO_DEL_ROMITORIO_CINGERM
	return

BoscoDelRomitorio_EventScript_HideArdeino::
	setflag FLAG_HIDE_BOSCO_DEL_ROMITORIO_ARDEINO
	return

BoscoDelRomitorio_EventScript_HideSerbrace::
	setflag FLAG_HIDE_BOSCO_DEL_ROMITORIO_SERBRACE
	return

BoscoDelRomitorio_EventScript_Cingerm::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_CINGERM, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setwildbattle SPECIES_CINGERM, 15
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	dowildbattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, BoscoDelRomitorio_EventScript_CingermDone
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, BoscoDelRomitorio_EventScript_CingermAvailable
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, BoscoDelRomitorio_EventScript_CingermAvailable

BoscoDelRomitorio_EventScript_CingermDone::
	setflag FLAG_HIDE_BOSCO_DEL_ROMITORIO_CINGERM
	goto Common_EventScript_RemoveStaticPokemon

BoscoDelRomitorio_EventScript_CingermAvailable::
	release
	end

BoscoDelRomitorio_EventScript_Ardeino::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_ARDEINO, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setwildbattle SPECIES_ARDEINO, 15
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	dowildbattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, BoscoDelRomitorio_EventScript_ArdeinoDone
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, BoscoDelRomitorio_EventScript_ArdeinoAvailable
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, BoscoDelRomitorio_EventScript_ArdeinoAvailable

BoscoDelRomitorio_EventScript_ArdeinoDone::
	setflag FLAG_HIDE_BOSCO_DEL_ROMITORIO_ARDEINO
	goto Common_EventScript_RemoveStaticPokemon

BoscoDelRomitorio_EventScript_ArdeinoAvailable::
	release
	end

BoscoDelRomitorio_EventScript_Serbrace::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_SERBRACE, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setwildbattle SPECIES_SERBRACE, 15
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	dowildbattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, BoscoDelRomitorio_EventScript_SerbraceDone
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, BoscoDelRomitorio_EventScript_SerbraceAvailable
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, BoscoDelRomitorio_EventScript_SerbraceAvailable

BoscoDelRomitorio_EventScript_SerbraceDone::
	setflag FLAG_HIDE_BOSCO_DEL_ROMITORIO_SERBRACE
	goto Common_EventScript_RemoveStaticPokemon

BoscoDelRomitorio_EventScript_SerbraceAvailable::
	release
	end

BoscoDelRomitorio_EventScript_EntranceSign::
	msgbox BoscoDelRomitorio_Text_EntranceSign, MSGBOX_DEFAULT
	end

BoscoDelRomitorio_EventScript_RetreatSign::
	msgbox BoscoDelRomitorio_Text_RetreatSign, MSGBOX_DEFAULT
	end

BoscoDelRomitorio_EventScript_Hermit::
	lock
	faceplayer
	msgbox BoscoDelRomitorio_Text_Hermit, MSGBOX_DEFAULT
	release
	end

BoscoDelRomitorio_Text_EntranceSign:
	.string "BOSCO DEL ROMITORIO\n"
	.string "Il ROMITORIO si trova a nord.$"

BoscoDelRomitorio_Text_RetreatSign:
	.string "ROMITORIO\n"
	.string "Antico luogo di preghiera.$"

BoscoDelRomitorio_Text_Hermit:
	.string "Un tempo questo era un antico\n"
	.string "luogo di preghiera.\p"
	.string "Qui vivevano degli eremiti.\p"
	.string "Nel ROMITORIO si possono ancora\n"
	.string "vedere i resti della torre\n"
	.string "campanaria.$"
