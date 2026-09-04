LagoDiAlbera_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, LagoDiAlbera_OnTransition
	.byte 0

LagoDiAlbera_OnTransition::
	goto_if_set FLAG_EMISSARIO_AUREA_ENCOUNTER_COMPLETE, LagoDiAlbera_OnTransition_HideEmissarioParty
	goto_if_unset FLAG_VIA_CONSOLARE_EMISSARIO_LEAD_COMPLETE, LagoDiAlbera_OnTransition_HideEmissarioParty
	goto_if_set FLAG_RECEIVED_HM_SURF, LagoDiAlbera_OnTransition_ShowEmissarioParty
	clearflag FLAG_HIDE_LAGO_DI_ALBERA_LIA_EMISSARIO
	setflag FLAG_HIDE_LAGO_DI_ALBERA_NICO_EMISSARIO
	goto LagoDiAlbera_EventScript_EndTransition
LagoDiAlbera_OnTransition_ShowEmissarioParty::
	clearflag FLAG_HIDE_LAGO_DI_ALBERA_LIA_EMISSARIO
	clearflag FLAG_HIDE_LAGO_DI_ALBERA_NICO_EMISSARIO
	goto LagoDiAlbera_EventScript_EndTransition
LagoDiAlbera_OnTransition_HideEmissarioParty::
	setflag FLAG_HIDE_LAGO_DI_ALBERA_LIA_EMISSARIO
	setflag FLAG_HIDE_LAGO_DI_ALBERA_NICO_EMISSARIO
	goto_if_set FLAG_BADGE02_GET, LagoDiAlbera_EventScript_EndTransition
	setflag FLAG_HIDE_LAGO_DI_ALBERA_LAURO_SURF
LagoDiAlbera_EventScript_EndTransition::
	end

LagoDiAlbera_EventScript_Custode::
	msgbox LagoDiAlbera_Text_Custode, MSGBOX_NPC
	end

LagoDiAlbera_EventScript_Pescatore::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_GOOD_ROD, LagoDiAlbera_EventScript_PescatoreDopo
	msgbox LagoDiAlbera_Text_PescatorePrima, MSGBOX_DEFAULT
	giveitem ITEM_GOOD_ROD
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_GOOD_ROD
	msgbox LagoDiAlbera_Text_PescatoreConsegna, MSGBOX_DEFAULT
	release
	end

LagoDiAlbera_EventScript_PescatoreDopo::
	msgbox LagoDiAlbera_Text_PescatoreDopo, MSGBOX_DEFAULT
	release
	end

LagoDiAlbera_EventScript_Guardaboschi::
	msgbox LagoDiAlbera_Text_Guardaboschi, MSGBOX_NPC
	end

LagoDiAlbera_EventScript_Archeologa::
	msgbox LagoDiAlbera_Text_Archeologa, MSGBOX_NPC
	end

LagoDiAlbera_EventScript_Anziano::
	msgbox LagoDiAlbera_Text_Anziano, MSGBOX_NPC
	end

LagoDiAlbera_EventScript_Carpentiere::
	msgbox LagoDiAlbera_Text_Carpentiere, MSGBOX_NPC
	end

LagoDiAlbera_EventScript_AbitantePalafitte::
	msgbox LagoDiAlbera_Text_AbitantePalafitte, MSGBOX_NPC
	end

LagoDiAlbera_EventScript_Bagnante::
	msgbox LagoDiAlbera_Text_Bagnante, MSGBOX_NPC
	end

LagoDiAlbera_EventScript_Ciclista::
	msgbox LagoDiAlbera_Text_Ciclista, MSGBOX_NPC
	end

LagoDiAlbera_EventScript_Aspirante::
	msgbox LagoDiAlbera_Text_Aspirante, MSGBOX_NPC
	end

LagoDiAlbera_EventScript_Tecnico::
	goto_if_set FLAG_RECEIVED_HM_SURF, LagoDiAlbera_EventScript_TecnicoAfterSurf
	msgbox LagoDiAlbera_Text_Tecnico, MSGBOX_NPC
	end

LagoDiAlbera_EventScript_TecnicoAfterSurf::
	msgbox LagoDiAlbera_Text_TecnicoAfterSurf, MSGBOX_NPC
	end

LagoDiAlbera_EventScript_LiaEmissario::
	goto_if_set FLAG_RECEIVED_HM_SURF, LagoDiAlbera_EventScript_LiaEmissarioAfterSurf
	msgbox LagoDiAlbera_Text_LiaEmissarioBeforeGym, MSGBOX_NPC
	end

LagoDiAlbera_EventScript_LiaEmissarioAfterSurf::
	msgbox LagoDiAlbera_Text_LiaEmissarioAfterSurf, MSGBOX_NPC
	end

LagoDiAlbera_EventScript_NicoEmissario::
	msgbox LagoDiAlbera_Text_NicoEmissarioAfterSurf, MSGBOX_NPC
	end

LagoDiAlbera_EventScript_StartEmissarioReunion::
	lockall
	goto_if_set FLAG_EMISSARIO_AUREA_ENCOUNTER_COMPLETE, LagoDiAlbera_EventScript_EndEmissarioReunion
	goto_if_unset FLAG_RECEIVED_HM_SURF, LagoDiAlbera_EventScript_EndEmissarioReunion
	msgbox LagoDiAlbera_Text_EmissarioReunion, MSGBOX_DEFAULT
	setflag FLAG_HIDE_LAGO_DI_ALBERA_LIA_EMISSARIO
	setflag FLAG_HIDE_LAGO_DI_ALBERA_NICO_EMISSARIO
	removeobject LOCALID_LAGO_DI_ALBERA_LIA_EMISSARIO
	removeobject LOCALID_LAGO_DI_ALBERA_NICO_EMISSARIO
LagoDiAlbera_EventScript_EndEmissarioReunion::
	releaseall
	end

LagoDiAlbera_EventScript_Viandante::
	msgbox LagoDiAlbera_Text_Viandante, MSGBOX_NPC
	end

LagoDiAlbera_EventScript_CasaMaestro::
	msgbox LagoDiAlbera_Text_CasaMaestro, MSGBOX_SIGN
	end

LagoDiAlbera_EventScript_NegozioBiciclette::
	msgbox LagoDiAlbera_Text_NegozioBiciclette, MSGBOX_SIGN
	end

LagoDiAlbera_EventScript_Palestra::
	msgbox LagoDiAlbera_Text_Palestra, MSGBOX_SIGN
	end

LagoDiAlbera_EventScript_Emissario::
	msgbox LagoDiAlbera_Text_Emissario, MSGBOX_SIGN
	end

LagoDiAlbera_EventScript_BottegaRifugi::
	msgbox LagoDiAlbera_Text_BottegaRifugi, MSGBOX_SIGN
	end

LagoDiAlbera_EventScript_SalitaBorgo::
	msgbox LagoDiAlbera_Text_SalitaBorgo, MSGBOX_SIGN
	end

LagoDiAlbera_EventScript_LauroSurf::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_HM_SURF, LagoDiAlbera_EventScript_LauroSurfAfter
	msgbox LagoDiAlbera_Text_LauroWaits, MSGBOX_DEFAULT
	release
	end

LagoDiAlbera_EventScript_LauroSurfAfter::
	msgbox LagoDiAlbera_Text_LauroAfter, MSGBOX_DEFAULT
	release
	end

LagoDiAlbera_EventScript_LauroSurfScene::
	lockall
	goto_if_unset FLAG_BADGE02_GET, LagoDiAlbera_EventScript_LauroSurfSceneEnd
	goto_if_set FLAG_RECEIVED_HM_SURF, LagoDiAlbera_EventScript_LauroSurfSceneEnd
	turnobject LOCALID_PLAYER, DIR_WEST
	turnobject LOCALID_LAGO_DI_ALBERA_LAURO_SURF, DIR_EAST
	msgbox LagoDiAlbera_Text_LauroSurfIntro, MSGBOX_DEFAULT
	giveitem ITEM_HM_SURF
	goto_if_eq VAR_RESULT, FALSE, LagoDiAlbera_EventScript_LauroSurfBagFull
	setflag FLAG_RECEIVED_HM_SURF
	setflag FLAG_HIDE_LAGO_DI_ALBERA_LAURO_SURF
	msgbox LagoDiAlbera_Text_LauroSurfGiven, MSGBOX_DEFAULT
	releaseall
	end

LagoDiAlbera_EventScript_LauroSurfBagFull::
	msgbox LagoDiAlbera_Text_LauroSurfBagFull, MSGBOX_DEFAULT
	releaseall
	end

LagoDiAlbera_EventScript_LauroSurfSceneEnd::
	releaseall
	end

LagoDiAlbera_Text_Custode:
	.string "Benvenuto al LAGO DI ALBERA.\n"
	.string "Il sentiero gira attorno al bacino.\p"
	.string "A est trovi le MACINE,\n"
	.string "a nord il bosco e l'EMISSARIO.$"

LagoDiAlbera_Text_PescatorePrima:
	.string "Nel lago vivono POKEMON che una\n"
	.string "canna corta non raggiunge.\p"
	.string "Prendi questa: qui ti servira.$"

LagoDiAlbera_Text_PescatoreConsegna:
	.string "E una CANNA BUONA.\n"
	.string "Provala da rive e pontili!$"

LagoDiAlbera_Text_PescatoreDopo:
	.string "La lenza racconta il fondale.\p"
	.string "Cambia sponda e troverai\n"
	.string "POKEMON diversi.$"

LagoDiAlbera_Text_Guardaboschi:
	.string "I sentieri del bosco cambiano\n"
	.string "aspetto con la luce.\p"
	.string "Quello a nord termina vicino\n"
	.string "all'EMISSARIO, ancora chiuso.$"

LagoDiAlbera_Text_Archeologa:
	.string "Queste pietre sono piu antiche\n"
	.string "delle palafitte.\p"
	.string "Il lago era abitato molto prima\n"
	.string "del villaggio attuale.$"

LagoDiAlbera_Text_Anziano:
	.string "Dicono che il bosco ricordi\n"
	.string "chi lo attraversa.\p"
	.string "Io credo che siamo noi\n"
	.string "a ricordare i suoi sentieri.$"

LagoDiAlbera_Text_Carpentiere:
	.string "Le passerelle nuove seguono\n"
	.string "le file dei pali antichi.\p"
	.string "Cambiamo il legno,\n"
	.string "non la strada sull'acqua.$"

LagoDiAlbera_Text_AbitantePalafitte:
	.string "Qui impari presto a riconoscere\n"
	.string "il lago dal rumore sotto casa.\p"
	.string "Ogni vento ha un suono diverso.$"

LagoDiAlbera_Text_Bagnante:
	.string "L'acqua sembra immobile,\n"
	.string "ma i POKEMON arrivano curiosi.\p"
	.string "Meglio non lasciare nulla\n"
	.string "incustodito sulla riva!$"

LagoDiAlbera_Text_Ciclista:
	.string "La strada verso il BORGO\n"
	.string "sale parecchio.\p"
	.string "Con una BICI e tutta un'altra cosa!$"

LagoDiAlbera_Text_Aspirante:
	.string "La PALESTRA DELLE MACINE\n"
	.string "e specializzata nel tipo ACQUA.\p"
	.string "Prima di sfidarla voglio studiare\n"
	.string "ogni corrente del lago.$"

LagoDiAlbera_Text_Tecnico:
	.string "L'EMISSARIO comunica con la stessa\n"
	.string "rete dei CISTERNONI.\p"
	.string "La pressione e ancora instabile.\n"
	.string "Per ora l'accesso resta chiuso.$"

LagoDiAlbera_Text_TecnicoAfterSurf:
	.string "La pressione e tornata stabile.\n"
	.string "LIA e NICO stanno gia studiando\n"
	.string "l'EMISSARIO.$"

LagoDiAlbera_Text_Viandante:
	.string "Questa salita porta al\n"
	.string "BORGO DI CASTELLO.\p"
	.string "Conserva un po' di fiato:\n"
	.string "il tratto piu ripido deve venire.$"

LagoDiAlbera_Text_CasaMaestro:
	.string "Casa del Maestro dei rifugi$"

LagoDiAlbera_Text_NegozioBiciclette:
	.string "Negozio di biciclette$"

LagoDiAlbera_Text_Palestra:
	.string "PALESTRA DELLE MACINE\n"
	.string "Tipo ACQUA$"

LagoDiAlbera_Text_Emissario:
	.string "Entrata dell'EMISSARIO$"

LagoDiAlbera_Text_BottegaRifugi:
	.string "BOTTEGA DEI RIFUGI$"

LagoDiAlbera_Text_SalitaBorgo:
	.string "SALITA VERSO BORGO DI CASTELLO\n"
	.string "{RIGHT_ARROW}$"

LagoDiAlbera_Text_LauroWaits:
	.string "PROF. LAURO: Prima conquista la\n"
	.string "MEDAGLIA MACINA, poi parleremo.$"

LagoDiAlbera_Text_LauroAfter:
	.string "PROF. LAURO: SURF ti permettera di\n"
	.string "esplorare il lago con prudenza.$"

LagoDiAlbera_Text_LauroSurfIntro:
	.string "PROF. LAURO: La MEDAGLIA MACINA\n"
	.string "abilita SURF fuori dalla lotta.\p"
	.string "Con SURF esplorerai nuove parti\n"
	.string "del LAGO DI ALBERA.\p"
	.string "LIA ti aspetta all'EMISSARIO.\n"
	.string "Le sue misure possono chiarire\n"
	.string "cosa cerca il TEAM AUREA.$"

LagoDiAlbera_Text_LauroSurfGiven:
	.string "PROF. LAURO: Usa SURF con attenzione\n"
	.string "e ascolta sempre l'acqua.$"

LagoDiAlbera_Text_LauroSurfBagFull:
	.string "PROF. LAURO: Non hai spazio per SURF.\n"
	.string "Sistema la BORSA e torna qui.$"

LagoDiAlbera_Text_LiaEmissarioBeforeGym:
	.string "LIA: L'EMISSARIO non e ancora\n"
	.string "sicuro. Prima conquista la MEDAGLIA\n"
	.string "MACINA e torna dal PROF. LAURO.$"

LagoDiAlbera_Text_LiaEmissarioAfterSurf:
	.string "LIA: La pressione e stabile.\n"
	.string "NICO e io siamo pronti a entrare.$"

LagoDiAlbera_Text_NicoEmissarioAfterSurf:
	.string "NICO: La PALESTRA mi ha insegnato\n"
	.string "a non sottovalutare l'acqua.\n"
	.string "Vediamo cosa nasconde l'EMISSARIO.$"

LagoDiAlbera_Text_EmissarioReunion:
	.string "LIA: Ci siamo. Entreremo per primi\n"
	.string "e controlleremo il condotto.\p"
	.string "NICO: Seguici quando sei pronto.$"
