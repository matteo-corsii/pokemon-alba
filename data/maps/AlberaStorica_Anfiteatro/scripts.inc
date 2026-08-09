AlberaStorica_Anfiteatro_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, AlberaStorica_Anfiteatro_OnTransition
	.byte 0

AlberaStorica_Anfiteatro_OnTransition:
	setvar VAR_ALBERA_GYM_INPUT, 0
	end

AlberaStorica_Anfiteatro_EventScript_Tamburo::
	lockall
	goto_if_eq VAR_ALBERA_GYM_STATE, 0, AlberaStorica_Anfiteatro_EventScript_TamburoStrofaI
	goto_if_eq VAR_ALBERA_GYM_STATE, 2, AlberaStorica_Anfiteatro_EventScript_TamburoStrofaIII
	goto AlberaStorica_Anfiteatro_EventScript_DispositivoNonAttivo

AlberaStorica_Anfiteatro_EventScript_TamburoStrofaI:
	goto_if_eq VAR_ALBERA_GYM_INPUT, 0, AlberaStorica_Anfiteatro_EventScript_TamburoCorrettoI
	goto AlberaStorica_Anfiteatro_EventScript_SequenzaErrata

AlberaStorica_Anfiteatro_EventScript_TamburoCorrettoI:
	playse SE_M_BELLY_DRUM
	setvar VAR_ALBERA_GYM_INPUT, 1
	msgbox AlberaStorica_Anfiteatro_Text_TamburoCerchio, MSGBOX_DEFAULT
	releaseall
	end

AlberaStorica_Anfiteatro_EventScript_TamburoStrofaIII:
	goto_if_eq VAR_ALBERA_GYM_INPUT, 0, AlberaStorica_Anfiteatro_EventScript_TamburoCorrettoIII
	goto AlberaStorica_Anfiteatro_EventScript_SequenzaErrata

AlberaStorica_Anfiteatro_EventScript_TamburoCorrettoIII:
	playse SE_M_BELLY_DRUM
	setvar VAR_ALBERA_GYM_INPUT, 1
	msgbox AlberaStorica_Anfiteatro_Text_TamburoCerchio, MSGBOX_DEFAULT
	releaseall
	end

AlberaStorica_Anfiteatro_EventScript_Corda::
	lockall
	goto_if_eq VAR_ALBERA_GYM_STATE, 0, AlberaStorica_Anfiteatro_EventScript_CordaStrofaI
	goto_if_eq VAR_ALBERA_GYM_STATE, 1, AlberaStorica_Anfiteatro_EventScript_CordaStrofaII
	goto_if_eq VAR_ALBERA_GYM_STATE, 2, AlberaStorica_Anfiteatro_EventScript_CordaStrofaIII
	goto AlberaStorica_Anfiteatro_EventScript_DispositivoNonAttivo

AlberaStorica_Anfiteatro_EventScript_CordaStrofaI:
	goto_if_eq VAR_ALBERA_GYM_INPUT, 1, AlberaStorica_Anfiteatro_EventScript_CompletaStrofaI
	goto AlberaStorica_Anfiteatro_EventScript_SequenzaErrata

AlberaStorica_Anfiteatro_EventScript_CordaStrofaII:
	goto_if_eq VAR_ALBERA_GYM_INPUT, 1, AlberaStorica_Anfiteatro_EventScript_CordaCorrettaII
	goto AlberaStorica_Anfiteatro_EventScript_SequenzaErrata

AlberaStorica_Anfiteatro_EventScript_CordaCorrettaII:
	playse SE_M_STRING_SHOT
	setvar VAR_ALBERA_GYM_INPUT, 2
	msgbox AlberaStorica_Anfiteatro_Text_CordaTreLinee, MSGBOX_DEFAULT
	releaseall
	end

AlberaStorica_Anfiteatro_EventScript_CordaStrofaIII:
	goto_if_eq VAR_ALBERA_GYM_INPUT, 1, AlberaStorica_Anfiteatro_EventScript_CordaCorrettaIIIa
	goto_if_eq VAR_ALBERA_GYM_INPUT, 3, AlberaStorica_Anfiteatro_EventScript_CompletaStrofaIII
	goto AlberaStorica_Anfiteatro_EventScript_SequenzaErrata

AlberaStorica_Anfiteatro_EventScript_CordaCorrettaIIIa:
	playse SE_M_STRING_SHOT
	setvar VAR_ALBERA_GYM_INPUT, 2
	msgbox AlberaStorica_Anfiteatro_Text_CordaTreLinee, MSGBOX_DEFAULT
	releaseall
	end

AlberaStorica_Anfiteatro_EventScript_Voce::
	lockall
	goto_if_eq VAR_ALBERA_GYM_STATE, 1, AlberaStorica_Anfiteatro_EventScript_VoceStrofaII
	goto_if_eq VAR_ALBERA_GYM_STATE, 2, AlberaStorica_Anfiteatro_EventScript_VoceStrofaIII
	goto AlberaStorica_Anfiteatro_EventScript_DispositivoNonAttivo

AlberaStorica_Anfiteatro_EventScript_VoceStrofaII:
	goto_if_eq VAR_ALBERA_GYM_INPUT, 0, AlberaStorica_Anfiteatro_EventScript_VoceCorrettaIIa
	goto_if_eq VAR_ALBERA_GYM_INPUT, 2, AlberaStorica_Anfiteatro_EventScript_CompletaStrofaII
	goto AlberaStorica_Anfiteatro_EventScript_SequenzaErrata

AlberaStorica_Anfiteatro_EventScript_VoceCorrettaIIa:
	playse SE_M_SING
	setvar VAR_ALBERA_GYM_INPUT, 1
	msgbox AlberaStorica_Anfiteatro_Text_VoceOnda, MSGBOX_DEFAULT
	releaseall
	end

AlberaStorica_Anfiteatro_EventScript_VoceStrofaIII:
	goto_if_eq VAR_ALBERA_GYM_INPUT, 2, AlberaStorica_Anfiteatro_EventScript_VoceCorrettaIII
	goto AlberaStorica_Anfiteatro_EventScript_SequenzaErrata

AlberaStorica_Anfiteatro_EventScript_VoceCorrettaIII:
	playse SE_M_SING
	setvar VAR_ALBERA_GYM_INPUT, 3
	msgbox AlberaStorica_Anfiteatro_Text_VoceOnda, MSGBOX_DEFAULT
	releaseall
	end

AlberaStorica_Anfiteatro_EventScript_SequenzaErrata:
	setvar VAR_ALBERA_GYM_INPUT, 0
	goto_if_eq VAR_ALBERA_GYM_STATE, 0, AlberaStorica_Anfiteatro_EventScript_MostraStrofaI
	goto_if_eq VAR_ALBERA_GYM_STATE, 1, AlberaStorica_Anfiteatro_EventScript_MostraStrofaII
	goto AlberaStorica_Anfiteatro_EventScript_MostraStrofaIII

AlberaStorica_Anfiteatro_EventScript_MostraStrofaI:
	msgbox AlberaStorica_Anfiteatro_Text_StrofaI, MSGBOX_DEFAULT
	releaseall
	end

AlberaStorica_Anfiteatro_EventScript_MostraStrofaII:
	msgbox AlberaStorica_Anfiteatro_Text_StrofaII, MSGBOX_DEFAULT
	releaseall
	end

AlberaStorica_Anfiteatro_EventScript_MostraStrofaIII:
	msgbox AlberaStorica_Anfiteatro_Text_StrofaIII, MSGBOX_DEFAULT
	releaseall
	end

AlberaStorica_Anfiteatro_EventScript_DispositivoNonAttivo:
	msgbox AlberaStorica_Anfiteatro_Text_DispositivoNonAttivo, MSGBOX_DEFAULT
	releaseall
	end

AlberaStorica_Anfiteatro_EventScript_CompletaStrofaI:
	playse SE_M_STRING_SHOT
	setvar VAR_ALBERA_GYM_INPUT, 0
	msgbox AlberaStorica_Anfiteatro_Text_DarioIntro, MSGBOX_DEFAULT
	trainerbattle_single TRAINER_ALBERA_DARIO, AlberaStorica_Anfiteatro_Text_DarioBattle, AlberaStorica_Anfiteatro_Text_DarioDefeat
	setvar VAR_ALBERA_GYM_STATE, 1
	setvar VAR_ALBERA_GYM_INPUT, 0
	msgbox AlberaStorica_Anfiteatro_Text_DarioAfter, MSGBOX_DEFAULT
	releaseall
	end

AlberaStorica_Anfiteatro_EventScript_CompletaStrofaII:
	playse SE_M_SING
	setvar VAR_ALBERA_GYM_INPUT, 0
	msgbox AlberaStorica_Anfiteatro_Text_MaraIntro, MSGBOX_DEFAULT
	trainerbattle_single TRAINER_ALBERA_MARA, AlberaStorica_Anfiteatro_Text_MaraBattle, AlberaStorica_Anfiteatro_Text_MaraDefeat
	setvar VAR_ALBERA_GYM_STATE, 2
	setvar VAR_ALBERA_GYM_INPUT, 0
	msgbox AlberaStorica_Anfiteatro_Text_MaraAfter, MSGBOX_DEFAULT
	releaseall
	end

AlberaStorica_Anfiteatro_EventScript_CompletaStrofaIII:
	playse SE_M_STRING_SHOT
	setvar VAR_ALBERA_GYM_INPUT, 0
	msgbox AlberaStorica_Anfiteatro_Text_ElioIntro, MSGBOX_DEFAULT
	trainerbattle_single TRAINER_ALBERA_ELIO, AlberaStorica_Anfiteatro_Text_ElioBattle, AlberaStorica_Anfiteatro_Text_ElioDefeat
	setvar VAR_ALBERA_GYM_STATE, 3
	setvar VAR_ALBERA_GYM_INPUT, 0
	msgbox AlberaStorica_Anfiteatro_Text_ElioAfter, MSGBOX_DEFAULT
	releaseall
	end

AlberaStorica_Anfiteatro_EventScript_Dario::
	lock
	faceplayer
	goto_if_ge VAR_ALBERA_GYM_STATE, 1, AlberaStorica_Anfiteatro_EventScript_DarioDone
	msgbox AlberaStorica_Anfiteatro_Text_DarioGuide, MSGBOX_DEFAULT
	release
	end

AlberaStorica_Anfiteatro_EventScript_DarioDone:
	msgbox AlberaStorica_Anfiteatro_Text_DarioAfter, MSGBOX_DEFAULT
	release
	end

AlberaStorica_Anfiteatro_EventScript_Mara::
	lock
	faceplayer
	goto_if_lt VAR_ALBERA_GYM_STATE, 1, AlberaStorica_Anfiteatro_EventScript_MaraWait
	goto_if_ge VAR_ALBERA_GYM_STATE, 2, AlberaStorica_Anfiteatro_EventScript_MaraDone
	msgbox AlberaStorica_Anfiteatro_Text_MaraGuide, MSGBOX_DEFAULT
	release
	end

AlberaStorica_Anfiteatro_EventScript_MaraWait:
	msgbox AlberaStorica_Anfiteatro_Text_MaraWaitText, MSGBOX_DEFAULT
	release
	end

AlberaStorica_Anfiteatro_EventScript_MaraDone:
	msgbox AlberaStorica_Anfiteatro_Text_MaraAfter, MSGBOX_DEFAULT
	release
	end

AlberaStorica_Anfiteatro_EventScript_Elio::
	lock
	faceplayer
	goto_if_lt VAR_ALBERA_GYM_STATE, 2, AlberaStorica_Anfiteatro_EventScript_ElioWait
	goto_if_ge VAR_ALBERA_GYM_STATE, 3, AlberaStorica_Anfiteatro_EventScript_ElioDone
	msgbox AlberaStorica_Anfiteatro_Text_ElioGuide, MSGBOX_DEFAULT
	release
	end

AlberaStorica_Anfiteatro_EventScript_ElioWait:
	msgbox AlberaStorica_Anfiteatro_Text_ElioWaitText, MSGBOX_DEFAULT
	release
	end

AlberaStorica_Anfiteatro_EventScript_ElioDone:
	msgbox AlberaStorica_Anfiteatro_Text_ElioAfter, MSGBOX_DEFAULT
	release
	end

AlberaStorica_Anfiteatro_EventScript_Lirio::
	lockall
	goto_if_eq VAR_ALBERA_GYM_STATE, 4, AlberaStorica_Anfiteatro_EventScript_LirioPostVictory
	goto_if_eq VAR_ALBERA_GYM_STATE, 3, AlberaStorica_Anfiteatro_EventScript_LirioBattle
	msgbox AlberaStorica_Anfiteatro_Text_LirioBlocked, MSGBOX_DEFAULT
	releaseall
	end

AlberaStorica_Anfiteatro_EventScript_LirioBattle:
	trainerbattle_single TRAINER_LEADER_LIRIO, AlberaStorica_Anfiteatro_Text_LirioIntro, AlberaStorica_Anfiteatro_Text_LirioDefeat, AlberaStorica_Anfiteatro_EventScript_LirioDefeated, NO_MUSIC
	msgbox AlberaStorica_Anfiteatro_Text_LirioPostVictoryText, MSGBOX_DEFAULT
	releaseall
	end

AlberaStorica_Anfiteatro_EventScript_LirioDefeated:
	call Common_EventScript_PlayGymBadgeFanfare
	setflag FLAG_BADGE01_GET
	setvar VAR_ALBERA_GYM_STATE, 4
	setvar VAR_ALBERA_GYM_INPUT, 0
	msgbox AlberaStorica_Anfiteatro_Text_MedagliaBallata, MSGBOX_DEFAULT
	return

AlberaStorica_Anfiteatro_EventScript_LirioPostVictory:
	msgbox AlberaStorica_Anfiteatro_Text_LirioPostVictoryText, MSGBOX_DEFAULT
	releaseall
	end

AlberaStorica_Anfiteatro_Text_TamburoCerchio:
	.string "TAMBuro: CERCHIO.$"

AlberaStorica_Anfiteatro_Text_CordaTreLinee:
	.string "CORDA: TRE LINEE.$"

AlberaStorica_Anfiteatro_Text_VoceOnda:
	.string "VOCE: ONDA.$"

AlberaStorica_Anfiteatro_Text_StrofaI:
	.string "STROFA I - RITMO\n"
	.string "CERCHIO -> TRE LINEE$"

AlberaStorica_Anfiteatro_Text_StrofaII:
	.string "STROFA II - VOCE\n"
	.string "ONDA -> TRE LINEE -> ONDA$"

AlberaStorica_Anfiteatro_Text_StrofaIII:
	.string "STROFA III - MEMORIA\n"
	.string "CERCHIO -> TRE LINEE -> ONDA\n"
	.string "-> TRE LINEE$"

AlberaStorica_Anfiteatro_Text_DispositivoNonAttivo:
	.string "Completa prima la Strofa corrente.$"

AlberaStorica_Anfiteatro_Text_DarioGuide:
	.string "PERCUSSIONISTA DARIO: Il ritmo non\n"
	.string "serve a correre. Serve a sapere\n"
	.string "quando fermarsi!\p"
	.string "STROFA I: CERCHIO -> TRE LINEE.$"

AlberaStorica_Anfiteatro_Text_DarioIntro:
	.string "La STROFA I risuona nell'arena.$"

AlberaStorica_Anfiteatro_Text_DarioBattle:
	.string "Il ritmo non serve a correre!$"

AlberaStorica_Anfiteatro_Text_DarioDefeat:
	.string "Hai tenuto il tempo anche sotto\n"
	.string "pressione. Non male.$"

AlberaStorica_Anfiteatro_Text_DarioAfter:
	.string "Hai tenuto il tempo anche sotto\n"
	.string "pressione. Non male.$"

AlberaStorica_Anfiteatro_Text_MaraWaitText:
	.string "Completa prima la Strofa del ritmo.$"

AlberaStorica_Anfiteatro_Text_MaraGuide:
	.string "ATTRICE MARA: Una voce può mentire.\n"
	.string "Un'esitazione, molto meno.\p"
	.string "STROFA II: ONDA -> TRE LINEE -> ONDA.$"

AlberaStorica_Anfiteatro_Text_MaraIntro:
	.string "La STROFA II risuona nell'arena.$"

AlberaStorica_Anfiteatro_Text_MaraBattle:
	.string "Una voce può mentire. Un'esitazione,\n"
	.string "molto meno.$"

AlberaStorica_Anfiteatro_Text_MaraDefeat:
	.string "Non hai seguito la mia parte.\n"
	.string "Hai trovato la tua.$"

AlberaStorica_Anfiteatro_Text_MaraAfter:
	.string "Non hai seguito la mia parte.\n"
	.string "Hai trovato la tua.$"

AlberaStorica_Anfiteatro_Text_ElioWaitText:
	.string "Completa prima le prime due Strofe.$"

AlberaStorica_Anfiteatro_Text_ElioGuide:
	.string "CORISTA ELIO: Ogni ritornello cambia\n"
	.string "un poco. Vediamo se riconoscerai\n"
	.string "il mio.\p"
	.string "STROFA III: CERCHIO -> TRE LINEE\n"
	.string "-> ONDA -> TRE LINEE.$"

AlberaStorica_Anfiteatro_Text_ElioIntro:
	.string "La STROFA III risuona nell'arena.$"

AlberaStorica_Anfiteatro_Text_ElioBattle:
	.string "Vediamo se riconoscerai il mio.$"

AlberaStorica_Anfiteatro_Text_ElioDefeat:
	.string "Hai ascoltato ciò che restava uguale,\n"
	.string "non soltanto ciò che cambiava.$"

AlberaStorica_Anfiteatro_Text_ElioAfter:
	.string "Hai ascoltato ciò che restava uguale,\n"
	.string "non soltanto ciò che cambiava.$"

AlberaStorica_Anfiteatro_Text_LirioBlocked:
	.string "LIRIO: Completa le tre Strofe.\n"
	.string "Ogni voce sembra comune, finché\n"
	.string "qualcuno non impara ad ascoltarla.$"

AlberaStorica_Anfiteatro_Text_LirioIntro:
	.string "Passi regolari. Nessuna corsa negli\n"
	.string "ultimi gradini. E hai esitato\n"
	.string "davanti all'arena.\p"
	.string "Io sono LIRIO, Il Bardo dell'Anfiteatro.\n"
	.string "Raccolgo le voci di ALBÈRA e custodisco\n"
	.string "questa Palestra.\p"
	.string "Mostrami la tua voce, Allenatore.$"

AlberaStorica_Anfiteatro_Text_LirioDefeat:
	.string "Ecco. Questa vittoria ti appartiene.$"

AlberaStorica_Anfiteatro_Text_MedagliaBallata:
	.string "{PLAYER} riceve la\n"
	.string "MEDAGLIA BALLATA!$"

AlberaStorica_Anfiteatro_Text_LirioPostVictoryText:
	.string "LIRIO: Una ballata cambia ogni volta\n"
	.string "che viene raccontata.\p"
	.string "L'eco è tornato prima della nota.\p"
	.string "Forse era soltanto il vento.\n"
	.string "Ma il vento non sale dalle cisterne.\p"
	.string "LAURO studia l'acqua. LIA osserva ciò\n"
	.string "che gli altri trascurano. Continua\n"
	.string "ad ascoltare lungo la VIA CONSOLARE.$"
