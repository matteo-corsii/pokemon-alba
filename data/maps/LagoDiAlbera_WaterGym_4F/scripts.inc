LagoDiAlbera_WaterGym_4F_MapScripts::
	.byte 0

LagoDiAlbera_WaterGym_4F_EventScript_Slide::
	warp MAP_LAGO_DI_ALBERA_WATER_GYM_1F, 17, 31
	waitstate
	end

LagoDiAlbera_WaterGym_4F_EventScript_Leader::
	msgbox LagoDiAlbera_WaterGym_4F_Text_Preparing, MSGBOX_NPC
	end
LagoDiAlbera_WaterGym_4F_Text_Preparing:
	.string "La Palestra e in preparazione.$"
