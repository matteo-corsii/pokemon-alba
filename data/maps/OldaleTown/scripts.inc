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

OldaleTown_EventScript_Molospsy::
	lock
	faceplayer
	goto_if_set FLAG_OLDALE_MOLOPSY_SEEN, OldaleTown_EventScript_Molospsy_After
	msgbox OldaleTown_Text_Molospsy, MSGBOX_DEFAULT
	setflag FLAG_OLDALE_MOLOPSY_SEEN
	release
	end

OldaleTown_EventScript_Molospsy_After::
	msgbox OldaleTown_Text_Molospsy_After, MSGBOX_DEFAULT
	release
	end

OldaleTown_EventScript_PortaPretoriaChild::
	msgbox OldaleTown_Text_PortaPretoriaChild, MSGBOX_NPC
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

OldaleTown_Text_Molospsy:
	.string "Queste pietre hanno visto sorgere e\n"
	.string "cadere molte generazioni.\p"
	.string "Gli uomini costruiscono, dimenticano...\n"
	.string "e poi tornano a cercare cio che hanno perduto.\p"
	.string "Ricorda, giovane viaggiatore: la terra\n"
	.string "conserva tracce che il tempo non puo cancellare.$"

OldaleTown_Text_Molospsy_After:
	.string "Ascolta il silenzio delle pietre.\n"
	.string "Hanno ancora molto da raccontare.$"

OldaleTown_Text_PortaPretoriaChild:
	.string "Anche tu riesci a vederlo?\p"
	.string "E sempre li, vicino alle vecchie pietre.\p"
	.string "A volte sembra che stia aspettando qualcuno...$"

OldaleTown_Text_WestGateWorker:
	.string "Il VIALE PRETORIO conduce ad\n"
	.string "ALBÈRA STORICA.$"
