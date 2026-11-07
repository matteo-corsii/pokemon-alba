Laricia_Gym_LeaderRoom_MapScripts::
	.byte 0

Laricia_Gym_LeaderRoom_EventScript_Augusto::
	trainerbattle_single TRAINER_AUGUSTO, Laricia_Gym_LeaderRoom_Text_AugustoIntro, Laricia_Gym_LeaderRoom_Text_AugustoDefeat, Laricia_Gym_LeaderRoom_EventScript_AugustoDefeated
	goto_if_unset FLAG_RECEIVED_TM_FLAME_CHARGE, Laricia_Gym_LeaderRoom_EventScript_GiveFlameCharge
	msgbox Laricia_Gym_LeaderRoom_Text_AugustoPostBattle, MSGBOX_AUTOCLOSE
	end

Laricia_Gym_LeaderRoom_EventScript_AugustoDefeated::
	setflag FLAG_BADGE03_GET
	message Laricia_Gym_LeaderRoom_Text_ReceivedFocolareBadge
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	msgbox Laricia_Gym_LeaderRoom_Text_ExplainFocolareBadge, MSGBOX_DEFAULT
	goto Laricia_Gym_LeaderRoom_EventScript_GiveFlameCharge

Laricia_Gym_LeaderRoom_EventScript_GiveFlameCharge::
	msgbox Laricia_Gym_LeaderRoom_Text_GiveFlameCharge, MSGBOX_DEFAULT
	giveitem ITEM_TM_FLAME_CHARGE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_TM_FLAME_CHARGE
	msgbox Laricia_Gym_LeaderRoom_Text_ExplainFlameCharge, MSGBOX_DEFAULT
	release
	end

Laricia_Gym_LeaderRoom_Text_AugustoIntro:
	.string "Sono Augusto, custode di questa\n"
	.string "fiamma e della storia che custodisce.\n"
	.string "Mostrami la tua forza.$"
Laricia_Gym_LeaderRoom_Text_AugustoDefeat:
	.string "Hai saputo trasformare il calore in\n"
	.string "forza. La MEDAGLIA FOCOLARE e tua.$"
Laricia_Gym_LeaderRoom_Text_ReceivedFocolareBadge:
	.string "Hai ricevuto la MEDAGLIA FOCOLARE!$"
Laricia_Gym_LeaderRoom_Text_ExplainFocolareBadge:
	.string "La MEDAGLIA FOCOLARE rende piu salda\n"
	.string "la volonta dei tuoi Pokemon.$"
Laricia_Gym_LeaderRoom_Text_GiveFlameCharge:
	.string "Prendi anche questa MT:\n"
	.string "NITROCARICA.$"
Laricia_Gym_LeaderRoom_Text_ExplainFlameCharge:
	.string "NITROCARICA aumenta la Velocita del\n"
	.string "Pokemon che la usa.$"
Laricia_Gym_LeaderRoom_Text_AugustoPostBattle:
	.string "Il vero fuoco non consuma il passato:\n"
	.string "lo trasforma in memoria.$"
