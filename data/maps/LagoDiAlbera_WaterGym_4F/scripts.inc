LagoDiAlbera_WaterGym_4F_MapScripts::
	.byte 0

LagoDiAlbera_WaterGym_4F_EventScript_Slide::
	warp MAP_LAGO_DI_ALBERA_WATER_GYM_1F, 17, 31
	waitstate
	end

LagoDiAlbera_WaterGym_4F_EventScript_Leader::
	trainerbattle_single TRAINER_LAGO_WATER_GYM_MARINA, LagoDiAlbera_WaterGym_4F_Text_MarinaIntro, LagoDiAlbera_WaterGym_4F_Text_MarinaDefeat, LagoDiAlbera_WaterGym_4F_EventScript_MarinaDefeated
	goto_if_unset FLAG_RECEIVED_TM_WATER_PULSE, LagoDiAlbera_WaterGym_4F_EventScript_GiveWaterPulse
	msgbox LagoDiAlbera_WaterGym_4F_Text_MarinaPostBattle, MSGBOX_AUTOCLOSE
	end

LagoDiAlbera_WaterGym_4F_EventScript_MarinaDefeated::
	setflag FLAG_BADGE02_GET
	msgbox LagoDiAlbera_WaterGym_4F_Text_MacinaBadge, MSGBOX_DEFAULT
	goto LagoDiAlbera_WaterGym_4F_EventScript_GiveWaterPulse
	end

LagoDiAlbera_WaterGym_4F_EventScript_GiveWaterPulse::
	msgbox LagoDiAlbera_WaterGym_4F_Text_GiveWaterPulse, MSGBOX_DEFAULT
	giveitem ITEM_TM_WATER_PULSE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_TM_WATER_PULSE
	clearflag FLAG_HIDE_LAGO_DI_ALBERA_LAURO_SURF
	msgbox LagoDiAlbera_WaterGym_4F_Text_WaterPulseExplain, MSGBOX_DEFAULT
	release
	end

LagoDiAlbera_WaterGym_4F_Text_MarinaIntro:
	.string "L'acqua e forte perche sa\n"
	.string "adattarsi a ogni movimento.\p"
	.string "Sulle passerelle hai trovato\n"
	.string "il tuo equilibrio. Ora dimostralo!$"
LagoDiAlbera_WaterGym_4F_Text_MarinaDefeat:
	.string "Hai seguito la corrente senza\n"
	.string "lasciarti trascinare!$"
LagoDiAlbera_WaterGym_4F_Text_MacinaBadge:
	.string "Ricevi la MEDAGLIA MACINA!\p"
	.string "Con questa MEDAGLIA, SURF puo essere\n"
	.string "usata fuori dalla lotta.$"
LagoDiAlbera_WaterGym_4F_Text_GiveWaterPulse:
	.string "Prendi anche questa MT:\n"
	.string "IDROPULSAR.$"
LagoDiAlbera_WaterGym_4F_Text_WaterPulseExplain:
	.string "IDROPULSAR confonde le correnti\n"
	.string "e puo confondere anche il nemico.$"
LagoDiAlbera_WaterGym_4F_Text_MarinaPostBattle:
	.string "Continua a muoverti con il lago:\n"
	.string "la forza nasce dall'adattarsi.$"
