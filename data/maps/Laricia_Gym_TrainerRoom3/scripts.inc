Laricia_Gym_TrainerRoom3_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Laricia_Gym_TrainerRoom3_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, Laricia_Gym_TrainerRoom3_OnTransition
	.byte 0

Laricia_Gym_TrainerRoom3_OnLoad::
	call Laricia_Gym_TrainerRoom3_EventScript_UpdateTorch
	end

Laricia_Gym_TrainerRoom3_OnTransition::
	call Laricia_Gym_TrainerRoom3_EventScript_UpdateTorch
	end

Laricia_Gym_TrainerRoom3_EventScript_UpdateTorch::
	goto_if_defeated TRAINER_LARICIA_GYM_TRAINER_3, Laricia_Gym_TrainerRoom3_EventScript_TorchOn
	setmetatile 9, 2, METATILE_LariciaGym_TorchTopOff, TRUE
	setmetatile 9, 3, METATILE_LariciaGym_TorchCenterOff, TRUE
	setmetatile 9, 4, METATILE_LariciaGym_TorchBase, TRUE
	special DrawWholeMapView
	return
Laricia_Gym_TrainerRoom3_EventScript_TorchOn::
	setmetatile 9, 2, METATILE_LariciaGym_TorchTopOn, TRUE
	setmetatile 9, 3, METATILE_LariciaGym_TorchCenterOn, TRUE
	setmetatile 9, 4, METATILE_LariciaGym_TorchBase, TRUE
	special DrawWholeMapView
	return

Laricia_Gym_TrainerRoom3_EventScript_Trainer::
	trainerbattle_single TRAINER_LARICIA_GYM_TRAINER_3, Laricia_Gym_TrainerRoom3_Text_TrainerIntro, Laricia_Gym_TrainerRoom3_Text_TrainerDefeat, Laricia_Gym_TrainerRoom3_EventScript_TrainerDefeated
	msgbox Laricia_Gym_TrainerRoom3_Text_TrainerPostBattle, MSGBOX_AUTOCLOSE
	end

Laricia_Gym_TrainerRoom3_EventScript_TrainerDefeated::
	call Laricia_Gym_TrainerRoom3_EventScript_UpdateTorch
	return

Laricia_Gym_TrainerRoom3_Text_TrainerIntro:
	.string "Il fuoco forgia il ferro e l'animo.\n"
	.string "Mostrami la tua tempra!$"
Laricia_Gym_TrainerRoom3_Text_TrainerDefeat:
	.string "Una tempra davvero incandescente.$"
Laricia_Gym_TrainerRoom3_Text_TrainerPostBattle:
	.string "La fiamma non e distruzione: e\n"
	.string "La fiamma della tradizione resiste.$"
