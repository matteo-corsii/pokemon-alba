ViaConsolare_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ViaConsolare_OnTransition
	.byte 0

ViaConsolare_OnTransition::
	goto_if_unset FLAG_CISTERNONI_AUREA_ENCOUNTER_COMPLETE, ViaConsolare_OnTransition_Hide
	goto_if_set FLAG_VIA_CONSOLARE_EMISSARIO_LEAD_COMPLETE, ViaConsolare_OnTransition_Hide
	clearflag FLAG_HIDE_VIA_CONSOLARE_LIA
	clearflag FLAG_HIDE_VIA_CONSOLARE_NICO
	end

ViaConsolare_OnTransition_Hide::
	setflag FLAG_HIDE_VIA_CONSOLARE_LIA
	setflag FLAG_HIDE_VIA_CONSOLARE_NICO
	end

ViaConsolare_EventScript_LiaEmissario::
	msgbox ViaConsolare_Text_LiaEmissario, MSGBOX_NPC
	end

ViaConsolare_EventScript_NicoEmissario::
	msgbox ViaConsolare_Text_NicoEmissario, MSGBOX_NPC
	end

ViaConsolare_EventScript_StartEmissarioLead::
	lockall
	goto_if_set FLAG_VIA_CONSOLARE_EMISSARIO_LEAD_COMPLETE, ViaConsolare_EventScript_EndEmissarioLead
	goto_if_unset FLAG_CISTERNONI_AUREA_ENCOUNTER_COMPLETE, ViaConsolare_EventScript_EndEmissarioLead
	msgbox ViaConsolare_Text_LiaEmissarioLead, MSGBOX_DEFAULT
	msgbox ViaConsolare_Text_NicoEmissarioLead, MSGBOX_DEFAULT
	setflag FLAG_VIA_CONSOLARE_EMISSARIO_LEAD_COMPLETE
	applymovement LOCALID_VIA_CONSOLARE_LIA_EMISSARIO, ViaConsolare_Movement_LiaLeavesNorth
	waitmovement 0
	applymovement LOCALID_VIA_CONSOLARE_NICO_EMISSARIO, ViaConsolare_Movement_NicoLeavesNorth
	waitmovement 0
	setflag FLAG_HIDE_VIA_CONSOLARE_LIA
	setflag FLAG_HIDE_VIA_CONSOLARE_NICO
	removeobject LOCALID_VIA_CONSOLARE_LIA_EMISSARIO
	removeobject LOCALID_VIA_CONSOLARE_NICO_EMISSARIO
ViaConsolare_EventScript_EndEmissarioLead::
	releaseall
	end

ViaConsolare_Movement_LiaLeavesNorth:
	walk_up
	walk_up
	step_end

ViaConsolare_Movement_NicoLeavesNorth:
	walk_up
	walk_up
	step_end

ViaConsolare_EventScript_Custode::
	msgbox ViaConsolare_Text_Custode, MSGBOX_NPC
	end

ViaConsolare_EventScript_Olivicoltrice::
	msgbox ViaConsolare_Text_Olivicoltrice, MSGBOX_NPC
	end

ViaConsolare_EventScript_Caposquadra::
	msgbox ViaConsolare_Text_Caposquadra, MSGBOX_NPC
	end

ViaConsolare_EventScript_Livio::
	trainerbattle_single TRAINER_VIA_CONSOLARE_LIVIO, ViaConsolare_Text_LivioIntro, ViaConsolare_Text_LivioDefeat
	msgbox ViaConsolare_Text_LivioPostBattle, MSGBOX_AUTOCLOSE
	end

ViaConsolare_EventScript_Elio::
	trainerbattle_single TRAINER_VIA_CONSOLARE_ELIO, ViaConsolare_Text_ElioIntro, ViaConsolare_Text_ElioDefeat
	msgbox ViaConsolare_Text_ElioPostBattle, MSGBOX_AUTOCLOSE
	end

ViaConsolare_EventScript_CippoNord::
	msgbox ViaConsolare_Text_CippoNord, MSGBOX_SIGN
	end

ViaConsolare_EventScript_CippoOccidentale::
	msgbox ViaConsolare_Text_CippoOccidentale, MSGBOX_SIGN
	end

ViaConsolare_EventScript_CartelloMansio::
	msgbox ViaConsolare_Text_CartelloMansio, MSGBOX_SIGN
	end

ViaConsolare_EventScript_VascaMansio::
	msgbox ViaConsolare_Text_VascaMansio, MSGBOX_SIGN
	end

ViaConsolare_EventScript_SbarramentoOccidentale::
	msgbox ViaConsolare_Text_SbarramentoOccidentale, MSGBOX_SIGN
	end

ViaConsolare_Text_Custode:
	.string "L'acqua scorre piano, ma porta\n"
	.string "con se terra e foglie.\p"
	.string "Puliamo il canale ogni giorno,\n"
	.string "come facevano quelli prima di noi.$"

ViaConsolare_Text_Olivicoltrice:
	.string "Questi muretti sono piu antichi\n"
	.string "degli olivi che proteggono.\p"
	.string "Li ripariamo ancora con le pietre\n"
	.string "cadute lungo la via.$"

ViaConsolare_Text_Caposquadra:
	.string "Stiamo controllando il tratto\n"
	.string "occidentale della VIA CONSOLARE.\p"
	.string "Finche i lavori non sono conclusi,\n"
	.string "da quella parte non si passa.$"

ViaConsolare_Text_LivioIntro:
	.string "Camminare su pietre tanto antiche\n"
	.string "fa pensare a tutti quelli\n"
	.string "che sono passati di qui!\p"
	.string "Vediamo che segno lasci tu!$"

ViaConsolare_Text_LivioDefeat:
	.string "Questa battaglia restera\n"
	.string "nella mia memoria!$"

ViaConsolare_Text_LivioPostBattle:
	.string "Ogni riparazione del basolato\n"
	.string "racconta un'epoca diversa.$"

ViaConsolare_Text_ElioIntro:
	.string "I POKEMON dei campi crescono\n"
	.string "forti tra il sole e la pietra!\p"
	.string "Te lo faccio vedere!$"

ViaConsolare_Text_ElioDefeat:
	.string "Hai raccolto proprio\n"
	.string "una bella vittoria!$"

ViaConsolare_Text_ElioPostBattle:
	.string "Rispettiamo muretti e canali.\n"
	.string "Senza di loro i campi\n"
	.string "soffrirebbero.$"

ViaConsolare_Text_CippoNord:
	.string "LAGO DI ALBERA - A NORD.$"

ViaConsolare_Text_CippoOccidentale:
	.string "La VIA CONSOLARE\n"
	.string "prosegue verso ovest.$"

ViaConsolare_Text_CartelloMansio:
	.string "MANSIO CONSOLARE\n"
	.string "Ristoro per i viandanti.$"

ViaConsolare_Text_VascaMansio:
	.string "Acqua limpida scorre\n"
	.string "nell'antica vasca.$"

ViaConsolare_Text_SbarramentoOccidentale:
	.string "Lavori di manutenzione.\n"
	.string "Il transito e vietato.$"

ViaConsolare_Text_LiaEmissario:
	.string "LIA: Le misure dei CISTERNONI\n"
	.string "portano tutte verso nord.$"

ViaConsolare_Text_NicoEmissario:
	.string "NICO: Se Lia ha ragione, il LAGO\n"
	.string "potrebbe darci altre risposte.$"

ViaConsolare_Text_LiaEmissarioLead:
	.string "LIA: Ho collegato i dati dei\n"
	.string "CISTERNONI all'EMISSARIO.\p"
	.string "Andiamo al LAGO DI ALBERA.$"

ViaConsolare_Text_NicoEmissarioLead:
	.string "NICO: Ti raggiungo li. Prima voglio\n"
	.string "affrontare la PALESTRA delle MACINE.$"
