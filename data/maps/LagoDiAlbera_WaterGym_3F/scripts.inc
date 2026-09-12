LagoDiAlbera_WaterGym_3F_MapScripts::
	.byte 0

LagoDiAlbera_WaterGym_3F_EventScript_Trainer3::
	trainerbattle_single TRAINER_LAGO_WATER_GYM_NEREO, LagoDiAlbera_WaterGym_3F_Text_NereoIntro, LagoDiAlbera_WaterGym_3F_Text_NereoDefeat
	msgbox LagoDiAlbera_WaterGym_3F_Text_NereoPostBattle, MSGBOX_AUTOCLOSE
	end
LagoDiAlbera_WaterGym_3F_Text_NereoIntro:
	.string "Il lago non concede due volte\n"
	.string "la stessa corrente. Si parte!$"
LagoDiAlbera_WaterGym_3F_Text_NereoDefeat:
	.string "Hai letto bene l'acqua.$"
LagoDiAlbera_WaterGym_3F_Text_NereoPostBattle:
	.string "MARINA affronta ogni sfida\n"
	.string "come una corrente nuova.$"
