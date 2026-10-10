Laricia_Gym_TrainerRoom4_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Laricia_Gym_TrainerRoom4_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, Laricia_Gym_TrainerRoom4_OnTransition
	.byte 0

Laricia_Gym_TrainerRoom4_OnLoad::
	call Laricia_Gym_TrainerRoom4_EventScript_UpdateTorch
	end

Laricia_Gym_TrainerRoom4_OnTransition::
	call Laricia_Gym_TrainerRoom4_EventScript_UpdateTorch
	end

Laricia_Gym_TrainerRoom4_EventScript_UpdateTorch::
	goto_if_defeated TRAINER_LARICIA_GYM_TRAINER_4, Laricia_Gym_TrainerRoom4_EventScript_TorchOn
	setmetatile 9, 2, METATILE_LariciaGym_TorchTopOff, TRUE
	setmetatile 9, 3, METATILE_LariciaGym_TorchCenterOff, TRUE
	setmetatile 9, 4, METATILE_LariciaGym_TorchBase, TRUE
	drawwholemapview
	return
Laricia_Gym_TrainerRoom4_EventScript_TorchOn::
	setmetatile 9, 2, METATILE_LariciaGym_TorchTopOn, TRUE
	setmetatile 9, 3, METATILE_LariciaGym_TorchCenterOn, TRUE
	setmetatile 9, 4, METATILE_LariciaGym_TorchBase, TRUE
	drawwholemapview
	return

Laricia_Gym_TrainerRoom4_EventScript_Trainer::
	trainerbattle_single TRAINER_LARICIA_GYM_TRAINER_4, Laricia_Gym_TrainerRoom4_Text_TrainerIntro, Laricia_Gym_TrainerRoom4_Text_TrainerDefeat, Laricia_Gym_TrainerRoom4_EventScript_TrainerDefeated
	msgbox Laricia_Gym_TrainerRoom4_Text_TrainerPostBattle, MSGBOX_AUTOCLOSE
	end

Laricia_Gym_TrainerRoom4_EventScript_TrainerDefeated::
	call Laricia_Gym_TrainerRoom4_EventScript_UpdateTorch
	return

Laricia_Gym_TrainerRoom4_Text_TrainerIntro:
	.string "Custodisco una fiamma antica.\n"
	.string "Mostrami la tua tempra!$"
Laricia_Gym_TrainerRoom4_Text_TrainerDefeat:
	.string "La tua fiamma e piu luminosa.$"
Laricia_Gym_TrainerRoom4_Text_TrainerPostBattle:
	.string "Il calore della tradizione passa\n"
	.string "La fiamma della tradizione resiste.$"
