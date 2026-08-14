OldaleTown_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, OldaleTown_OnTransition
	.byte 0

OldaleTown_OnTransition:
	setflag FLAG_VISITED_OLDALE_TOWN
	call_if_eq VAR_ALBERA_OPENING_STATE, 6, OldaleTown_EventScript_SetPortaPretoriaCheckpoint
	end

OldaleTown_EventScript_SetPortaPretoriaCheckpoint::
	setvar VAR_ALBERA_OPENING_STATE, 7
	return

OldaleTown_EventScript_TownSign::
	msgbox OldaleTown_Text_TownSign, MSGBOX_SIGN
	end

OldaleTown_EventScript_PokemonCenterSign::
	msgbox OldaleTown_Text_PokemonCenterSign, MSGBOX_SIGN
	end

OldaleTown_EventScript_MartSign::
	msgbox OldaleTown_Text_MartSign, MSGBOX_SIGN
	end

OldaleTown_EventScript_Traveler::
	msgbox OldaleTown_Text_Traveler, MSGBOX_NPC
	end

OldaleTown_EventScript_WallResident::
	msgbox OldaleTown_Text_WallResident, MSGBOX_NPC
	end

OldaleTown_EventScript_WestGateWorker::
	msgbox OldaleTown_Text_WestGateWorker, MSGBOX_NPC
	end

OldaleTown_EventScript_SouthEntranceResident::
	msgbox OldaleTown_Text_SouthEntranceResident, MSGBOX_NPC
	end

OldaleTown_EventScript_BlockNorthExit::
	lockall
	msgbox OldaleTown_Text_NorthExitClosed, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, OldaleTown_Movement_StepSouth
	waitmovement 0
	releaseall
	end

OldaleTown_Movement_StepSouth:
	walk_down
	step_end

OldaleTown_Text_TownSign:
	.string "PORTA PRETORIA\n"
	.string "Ingresso orientale di ALBÈRA STORICA.$"

OldaleTown_Text_PokemonCenterSign:
	.string "CENTRO POKéMON\n"
	.string "Assistenza per Allenatori e viaggiatori.$"

OldaleTown_Text_MartSign:
	.string "EMPORIO DI PORTA PRETORIA$"

OldaleTown_Text_SouthEntranceResident:
	.string "VIA VERDI segue gli orti e i canali\n"
	.string "fino a questa porta.$"

OldaleTown_Text_WallResident:
	.string "PORTA PRETORIA proteggeva l'accesso\n"
	.string "orientale alla città antica.$"

OldaleTown_Text_Traveler:
	.string "Ad Albèra Storica vive un esperto di\n"
	.string "campeggi e rifugi.\p"
	.string "Dicono che insegni ai viaggiatori a\n"
	.string "crearsi un posto tutto loro.$"

OldaleTown_Text_WestGateWorker:
	.string "Il VIALE PRETORIO conduce ad\n"
	.string "ALBÈRA STORICA.$"

OldaleTown_Text_NorthExitClosed:
	.string "Il passaggio settentrionale è chiuso\n"
	.string "per lavori sulla rete idrica.$"
