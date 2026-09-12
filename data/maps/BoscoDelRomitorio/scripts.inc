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
	msgbox BoscoDelRomitorio_Text_EntranceSign, MSGBOX_SIGN
	end

BoscoDelRomitorio_EventScript_RetreatSign::
	msgbox BoscoDelRomitorio_Text_RetreatSign, MSGBOX_SIGN
	end

BoscoDelRomitorio_EventScript_ExitWest::
	warp MAP_LAGO_DI_ALBERA, 4, 2
	end

BoscoDelRomitorio_EventScript_ExitEast::
	warp MAP_LAGO_DI_ALBERA, 5, 2
	end

BoscoDelRomitorio_EventScript_Hermit::
	lock
	faceplayer
	msgbox BoscoDelRomitorio_Text_Hermit, MSGBOX_DEFAULT
	release
	end

BoscoDelRomitorio_EventScript_Guardaboschi::
	trainerbattle_single TRAINER_MIKE_1, BoscoDelRomitorio_Text_GuardaboschiIntro, BoscoDelRomitorio_Text_GuardaboschiDefeat
	msgbox BoscoDelRomitorio_Text_GuardaboschiPostBattle, MSGBOX_AUTOCLOSE
	end

BoscoDelRomitorio_EventScript_Esploratrice::
	trainerbattle_single TRAINER_CINDY_2, BoscoDelRomitorio_Text_EsploratriceIntro, BoscoDelRomitorio_Text_EsploratriceDefeat
	msgbox BoscoDelRomitorio_Text_EsploratricePostBattle, MSGBOX_AUTOCLOSE
	end

BoscoDelRomitorio_EventScript_Sorgenti::
	trainerbattle_single TRAINER_ANDREW, BoscoDelRomitorio_Text_SorgentiIntro, BoscoDelRomitorio_Text_SorgentiDefeat
	msgbox BoscoDelRomitorio_Text_SorgentiPostBattle, MSGBOX_AUTOCLOSE
	end

BoscoDelRomitorio_EventScript_Cercarocce::
	trainerbattle_single TRAINER_LUCAS_2, BoscoDelRomitorio_Text_CercarocceIntro, BoscoDelRomitorio_Text_CercarocceDefeat
	msgbox BoscoDelRomitorio_Text_CercaroccePostBattle, MSGBOX_AUTOCLOSE
	end

BoscoDelRomitorio_EventScript_CustodeRovine::
	trainerbattle_single TRAINER_RHETT, BoscoDelRomitorio_Text_CustodeRovineIntro, BoscoDelRomitorio_Text_CustodeRovineDefeat
	msgbox BoscoDelRomitorio_Text_CustodeRovinePostBattle, MSGBOX_AUTOCLOSE
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

BoscoDelRomitorio_Text_GuardaboschiIntro:
	.string "Conosco ogni sentiero di questo bosco.\n"
	.string "Vediamo se sai seguirne le tracce.$"

BoscoDelRomitorio_Text_GuardaboschiDefeat:
	.string "Hai superato la prova del sentiero.$"

BoscoDelRomitorio_Text_GuardaboschiPostBattle:
	.string "Continua a osservare gli alberi e il\n"
	.string "terreno.$"

BoscoDelRomitorio_Text_EsploratriceIntro:
	.string "Sto esplorando ogni angolo del bosco.\n"
	.string "Non mi fermero proprio adesso.$"

BoscoDelRomitorio_Text_EsploratriceDefeat:
	.string "La tua esplorazione e piu attenta.$"

BoscoDelRomitorio_Text_EsploratricePostBattle:
	.string "Segnero questo sentiero sulla mappa.$"

BoscoDelRomitorio_Text_SorgentiIntro:
	.string "Le sorgenti nascoste sono la mia\n"
	.string "passione. Mostrami la tua squadra.$"

BoscoDelRomitorio_Text_SorgentiDefeat:
	.string "L'acqua ha cambiato ancora il percorso.$"

BoscoDelRomitorio_Text_SorgentiPostBattle:
	.string "Segui il rumore dell'acqua tra gli\n"
	.string "alberi.$"

BoscoDelRomitorio_Text_CercarocceIntro:
	.string "Queste rocce conservano tracce\n"
	.string "preziose. Non intralciarmi.$"

BoscoDelRomitorio_Text_CercarocceDefeat:
	.string "Hai trovato la strada migliore.$"

BoscoDelRomitorio_Text_CercaroccePostBattle:
	.string "Cerchero piu a fondo tra queste rocce.$"

BoscoDelRomitorio_Text_CustodeRovineIntro:
	.string "Custodisco questi ruderi da molto\n"
	.string "tempo. Fatti avanti.$"

BoscoDelRomitorio_Text_CustodeRovineDefeat:
	.string "Le rovine hanno visto una bella lotta.$"

BoscoDelRomitorio_Text_CustodeRovinePostBattle:
	.string "Ora puoi proseguire tra i resti\n"
	.string "antichi.$"
