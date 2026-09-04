Emissario_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Emissario_OnTransition
	.byte 0

Emissario_OnTransition::
	goto_if_set FLAG_EMISSARIO_AUREA_ENCOUNTER_COMPLETE, Emissario_OnTransition_Hide
	goto_if_set FLAG_VIA_CONSOLARE_EMISSARIO_LEAD_COMPLETE, Emissario_OnTransition_Show
	goto_if_unset FLAG_BADGE02_GET, Emissario_OnTransition_Hide
	goto_if_unset FLAG_RECEIVED_HM_SURF, Emissario_OnTransition_Hide
	setflag FLAG_VIA_CONSOLARE_EMISSARIO_LEAD_COMPLETE
Emissario_OnTransition_Show::
	special TryMigrateOriginalStarterIdentity
	clearflag FLAG_HIDE_EMISSARIO_LIA
	clearflag FLAG_HIDE_EMISSARIO_NICO
	clearflag FLAG_HIDE_EMISSARIO_AUREA_RECRUIT
	end

Emissario_OnTransition_Hide::
	setflag FLAG_HIDE_EMISSARIO_LIA
	setflag FLAG_HIDE_EMISSARIO_NICO
	setflag FLAG_HIDE_EMISSARIO_AUREA_RECRUIT
	end

Emissario_EventScript_Lia::
	msgbox Emissario_Text_Lia, MSGBOX_NPC
	end

Emissario_EventScript_Nico::
	msgbox Emissario_Text_Nico, MSGBOX_NPC
	end

Emissario_EventScript_AureaRecruit::
	lockall
	goto_if_set FLAG_EMISSARIO_AUREA_ENCOUNTER_COMPLETE, Emissario_EventScript_End
	msgbox Emissario_Text_AureaIntro, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_EMISSARIO_AUREA_RECRUIT, Emissario_Text_AureaDefeat
	goto Emissario_EventScript_AureaDefeated
	end

Emissario_EventScript_AureaDefeated::
	msgbox Emissario_Text_AureaRetreat, MSGBOX_DEFAULT
	setflag FLAG_EMISSARIO_AUREA_ENCOUNTER_COMPLETE
	setflag FLAG_FIRST_ECHO_SEEN
	setflag FLAG_HIDE_EMISSARIO_AUREA_RECRUIT
	removeobject LOCALID_EMISSARIO_AUREA_RECRUIT
	special Special_GetFirstEchoActiveMonResult
	goto_if_eq VAR_RESULT, TRUE, Emissario_EventScript_StarterEchoConclusion
	msgbox Emissario_Text_LiaGenericEchoConclusion, MSGBOX_DEFAULT
	goto Emissario_EventScript_CommonConclusion
Emissario_EventScript_StarterEchoConclusion::
	msgbox Emissario_Text_LiaStarterEchoConclusion, MSGBOX_DEFAULT
Emissario_EventScript_CommonConclusion::
	msgbox Emissario_Text_NicoConclusion, MSGBOX_DEFAULT
	setflag FLAG_HIDE_EMISSARIO_LIA
	setflag FLAG_HIDE_EMISSARIO_NICO
	removeobject LOCALID_EMISSARIO_LIA
	removeobject LOCALID_EMISSARIO_NICO
Emissario_EventScript_End::
	releaseall
	end

Emissario_Text_Lia:
	.string "LIA: Le pareti deviano il suono.\n"
	.string "Qualcuno ha lavorato qui di recente.$"
Emissario_Text_Nico:
	.string "NICO: Restiamo vicini. Questo posto\n"
	.string "non e vuoto come sembra.$"
Emissario_Text_AureaIntro:
	.string "AUREA: Siete arrivati troppo presto.\n"
	.string "Il TEAM AUREA non vi lascera\n"
	.string "toccare queste acque.$"
Emissario_Text_AureaDefeat:
	.string "Quell'energia... non era prevista.\n"
	.string "Ma questa sconfitta non cambia nulla.$"
Emissario_Text_AureaRetreat:
	.string "AUREA: Devo riferire. I nostri piani\n"
	.string "sono gia in corso.\p"
	.string "Quello che avete visto e soltanto una\n"
	.string "parte minima di qualcosa di molto piu\n"
	.string "grande.$"
Emissario_Text_LiaStarterEchoConclusion:
	.string "LIA: Per un istante, il riflesso di\n"
	.string "{STR_VAR_1} ha anticipato il movimento.\p"
	.string "Non so ancora che cosa abbiamo visto.\n"
	.string "Dovremo studiarlo con calma.$"
Emissario_Text_LiaGenericEchoConclusion:
	.string "LIA: Anche {STR_VAR_1} ha reagito.\p"
	.string "Non so ancora che cosa abbiamo visto.\n"
	.string "Dovremo studiarlo con calma.$"
Emissario_Text_NicoConclusion:
	.string "NICO: Un Eco... ma arrivato prima\n"
	.string "del suono.\p"
	.string "Non capisco ancora cosa significhi.\n"
	.string "Al BORGO DI CASTELLO ci sono registri\n"
	.string "piu antichi. Andiamo li.$"
