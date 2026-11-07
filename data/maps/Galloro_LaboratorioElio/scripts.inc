Galloro_LaboratorioElio_MapScripts::
	.byte 0

Galloro_LaboratorioElio_EventScript_Elio::
	lock
	faceplayer
	goto_if_set FLAG_ALBA_BALL_PRESS_UNLOCKED, Galloro_LaboratorioElio_EventScript_ElioAfterUnlock
	msgbox Galloro_LaboratorioElio_Text_ElioIntro, MSGBOX_DEFAULT
	setflag FLAG_ALBA_BALL_PRESS_UNLOCKED
	msgbox Galloro_LaboratorioElio_Text_ElioUnlock, MSGBOX_DEFAULT
	release
	end

Galloro_LaboratorioElio_EventScript_ElioAfterUnlock::
	msgbox Galloro_LaboratorioElio_Text_ElioAfterUnlock, MSGBOX_NPC
	release
	end

Galloro_LaboratorioElio_Text_ElioIntro:
	.string "Ah, finalmente qualcuno che non viene\n"
	.string "a chiedermi di aggiustare il tostapane!\p"
	.string "Benvenuto! Sono Elio, Maestro dei Rifugi.\n"
	.string "Qui cerco di far funzionare le cose\n"
	.string "consumando meno risorse possibili.\p"
	.string "La mia ultima invenzione e la Pressa Ball!$"

Galloro_LaboratorioElio_Text_ElioUnlock:
	.string "Una Baccaliegia diventa una Poke Ball.\n"
	.string "Una Baccafrago, una Mega Ball.\p"
	.string "Ho gia mandato i miei aiutanti\n"
	.string "a installarne una nei tuoi Rifugi!$"

Galloro_LaboratorioElio_Text_ElioAfterUnlock:
	.string "Le idee migliori sono quelle che fanno\n"
	.string "risparmiare senza chiedere niente alla natura.$"
