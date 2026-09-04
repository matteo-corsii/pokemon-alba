LagoDiAlbera_WaterGym_2F_MapScripts::
	.byte 0

LagoDiAlbera_WaterGym_2F_EventScript_Trainer2::
	trainerbattle_single TRAINER_LAGO_WATER_GYM_DALIA, LagoDiAlbera_WaterGym_2F_Text_DaliaIntro, LagoDiAlbera_WaterGym_2F_Text_DaliaDefeat
	msgbox LagoDiAlbera_WaterGym_2F_Text_DaliaPostBattle, MSGBOX_AUTOCLOSE
	end
LagoDiAlbera_WaterGym_2F_Text_DaliaIntro:
	.string "L'acqua sembra dolce, ma sa\n"
	.string "cambiare forma in un attimo!$"
LagoDiAlbera_WaterGym_2F_Text_DaliaDefeat:
	.string "Ti sei adattato benissimo!$"
LagoDiAlbera_WaterGym_2F_Text_DaliaPostBattle:
	.string "Chi si adatta al movimento\n"
	.string "non teme le onde.$"
