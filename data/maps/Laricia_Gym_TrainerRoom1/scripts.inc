Laricia_Gym_TrainerRoom1_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Laricia_Gym_TrainerRoom1_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, Laricia_Gym_TrainerRoom1_OnTransition
	.byte 0

Laricia_Gym_TrainerRoom1_OnLoad::
	call Laricia_Gym_TrainerRoom1_EventScript_UpdateTorch
	end

Laricia_Gym_TrainerRoom1_OnTransition::
	call Laricia_Gym_TrainerRoom1_EventScript_UpdateTorch
	end

Laricia_Gym_TrainerRoom1_EventScript_UpdateTorch::
	goto_if_defeated TRAINER_LARICIA_GYM_TRAINER_1, Laricia_Gym_TrainerRoom1_EventScript_TorchOn
	setmetatile 9, 2, METATILE_LariciaGym_TorchTopOff, TRUE
	setmetatile 9, 3, METATILE_LariciaGym_TorchCenterOff, TRUE
	setmetatile 9, 4, METATILE_LariciaGym_TorchBase, TRUE
	drawwholemapview
	return
Laricia_Gym_TrainerRoom1_EventScript_TorchOn::
	setmetatile 9, 2, METATILE_LariciaGym_TorchTopOn, TRUE
	setmetatile 9, 3, METATILE_LariciaGym_TorchCenterOn, TRUE
	setmetatile 9, 4, METATILE_LariciaGym_TorchBase, TRUE
	drawwholemapview
	return

Laricia_Gym_TrainerRoom1_EventScript_Trainer::
	trainerbattle_single TRAINER_LARICIA_GYM_TRAINER_1, Laricia_Gym_TrainerRoom1_Text_TrainerIntro, Laricia_Gym_TrainerRoom1_Text_TrainerDefeat, Laricia_Gym_TrainerRoom1_EventScript_TrainerDefeated
	msgbox Laricia_Gym_TrainerRoom1_Text_TrainerPostBattle, MSGBOX_AUTOCLOSE
	end

Laricia_Gym_TrainerRoom1_EventScript_TrainerDefeated::
	call Laricia_Gym_TrainerRoom1_EventScript_UpdateTorch
	return

Laricia_Gym_TrainerRoom1_Text_TrainerIntro:
	.string "La brace conserva la memoria del metallo.\n"
	.string "Mostrami la tua tempra!$"
Laricia_Gym_TrainerRoom1_Text_TrainerDefeat:
	.string "Il tuo calore ha superato il mio.$"
Laricia_Gym_TrainerRoom1_Text_TrainerPostBattle:
	.string "La disciplina rende salda ogni fiamma.\n"
	.string "La fiamma della tradizione resiste.$"
