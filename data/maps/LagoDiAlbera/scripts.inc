LagoDiAlbera_MapScripts::
	.byte 0

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
	msgbox LagoDiAlbera_Text_Tecnico, MSGBOX_NPC
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
