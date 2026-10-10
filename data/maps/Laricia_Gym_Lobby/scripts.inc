Laricia_Gym_Lobby_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Laricia_Gym_Lobby_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, Laricia_Gym_Lobby_OnTransition
	.byte 0

Laricia_Gym_Lobby_OnLoad::
	call Laricia_Gym_Lobby_EventScript_UpdateLeaderDoor
	end

Laricia_Gym_Lobby_OnTransition::
	call Laricia_Gym_Lobby_EventScript_UpdateLeaderDoor
	end

Laricia_Gym_Lobby_EventScript_UpdateLeaderDoor::
	goto_if_not_defeated TRAINER_LARICIA_GYM_TRAINER_1, Laricia_Gym_Lobby_EventScript_LeaderDoorClosed
	goto_if_not_defeated TRAINER_LARICIA_GYM_TRAINER_2, Laricia_Gym_Lobby_EventScript_LeaderDoorClosed
	goto_if_not_defeated TRAINER_LARICIA_GYM_TRAINER_3, Laricia_Gym_Lobby_EventScript_LeaderDoorClosed
	goto_if_not_defeated TRAINER_LARICIA_GYM_TRAINER_4, Laricia_Gym_Lobby_EventScript_LeaderDoorClosed
	setmetatile 12, 1, METATILE_LariciaGym_LeaderDoorOpen0, TRUE
	setmetatile 13, 1, METATILE_LariciaGym_LeaderDoorOpen1, TRUE
	setmetatile 14, 1, METATILE_LariciaGym_LeaderDoorOpen2, TRUE
	setmetatile 12, 2, METATILE_LariciaGym_LeaderDoorOpen3, TRUE
	setmetatile 13, 2, METATILE_LariciaGym_LeaderDoorOpen4, FALSE
	setmetatile 14, 2, METATILE_LariciaGym_LeaderDoorOpen5, TRUE
	special DrawWholeMapView
	return
Laricia_Gym_Lobby_EventScript_LeaderDoorClosed::
	setmetatile 12, 1, METATILE_LariciaGym_LeaderDoorClosed0, TRUE
	setmetatile 13, 1, METATILE_LariciaGym_LeaderDoorClosed1, TRUE
	setmetatile 14, 1, METATILE_LariciaGym_LeaderDoorClosed2, TRUE
	setmetatile 12, 2, METATILE_LariciaGym_LeaderDoorClosed3, TRUE
	setmetatile 13, 2, METATILE_LariciaGym_LeaderDoorClosed4, FALSE
	setmetatile 14, 2, METATILE_LariciaGym_LeaderDoorClosed5, TRUE
	special DrawWholeMapView
	return

Laricia_Gym_Lobby_EventScript_Butler::
	msgbox Laricia_Gym_Lobby_Text_Butler, MSGBOX_DEFAULT
	end

Laricia_Gym_Lobby_Text_Butler:
	.string "Benvenuto nella dimora di Augusto,\n"
	.string "Capopalestra di Laricia.\p"
	.string "Queste sale custodiscono da lungo tempo\n"
	.string "la memoria della citta.\p"
	.string "Qui il fuoco non e soltanto forza.\n"
	.string "E calore, lavoro... e continuita.\p"
	.string "Prima di poter incontrare Augusto,\n"
	.string "dovrai attraversare le quattro sale.\p"
	.string "In ognuna troverai uno dei suoi Allenatori.\n"
	.string "Puoi affrontarli nell'ordine che preferisci.\p"
	.string "Ogni vittoria riaccendera\n"
	.string "uno dei quattro focolari della Palestra.\p"
	.string "Quando tutte e quattro le fiamme\n"
	.string "saranno tornate a brillare...\p"
	.string "la sala di Augusto si aprira.\p"
	.string "Non avere fretta.\n"
	.string "Il fuoco rivela molto di chi sa governarlo.$"
