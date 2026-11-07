LagoDiAlbera_WaterGym_1F_MapScripts::
	.byte 0

LagoDiAlbera_WaterGym_1F_EventScript_Guide::
	lock
	faceplayer
	goto_if_set FLAG_BADGE02_GET, LagoDiAlbera_WaterGym_1F_EventScript_GuideWon
	msgbox LagoDiAlbera_WaterGym_1F_Text_Guide, MSGBOX_DEFAULT
	release
	end
LagoDiAlbera_WaterGym_1F_EventScript_GuideWon::
	msgbox LagoDiAlbera_WaterGym_1F_Text_GuideWon, MSGBOX_DEFAULT
	release
	end
LagoDiAlbera_WaterGym_1F_EventScript_Trainer1::
	trainerbattle_single TRAINER_LAGO_WATER_GYM_REMO, LagoDiAlbera_WaterGym_1F_Text_RemoIntro, LagoDiAlbera_WaterGym_1F_Text_RemoDefeat
	msgbox LagoDiAlbera_WaterGym_1F_Text_RemoPostBattle, MSGBOX_AUTOCLOSE
	end
LagoDiAlbera_WaterGym_1F_Text_Guide:
	.string "Questa e la PALESTRA DELLE MACINE.\n"
	.string "L'acqua insegna a non restare fermi.\p"
	.string "Segui le passerelle fino a MARINA\n"
	.string "e trova il tuo equilibrio.$"
LagoDiAlbera_WaterGym_1F_Text_GuideWon:
	.string "Hai ottenuto la MEDAGLIA MACINA!\p"
	.string "Ora le correnti del lago ti apriranno\n"
	.string "nuove strade.$"
LagoDiAlbera_WaterGym_1F_Text_RemoIntro:
	.string "Le correnti cambiano all'improvviso.\n"
	.string "Vediamo se sai seguirle!$"
LagoDiAlbera_WaterGym_1F_Text_RemoDefeat:
	.string "Hai tenuto bene il passo!$"
LagoDiAlbera_WaterGym_1F_Text_RemoPostBattle:
	.string "Sulle passerelle conta l'equilibrio,\n"
	.string "non la fretta.$"
