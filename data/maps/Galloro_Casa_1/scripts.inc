Galloro_Casa_1_MapScripts::
	.byte 0

Galloro_Casa_1_EventScript_Parent::
	lock
	faceplayer
	msgbox Galloro_Casa_1_Text_Parent, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Galloro_Casa_1_EventScript_ParentNo
	fadescreen FADE_TO_BLACK
	waitstate
	special HealPlayerParty
	fadescreen FADE_FROM_BLACK
	msgbox Galloro_Casa_1_Text_ParentYes, MSGBOX_DEFAULT
	release
	end

Galloro_Casa_1_EventScript_ParentNo::
	msgbox Galloro_Casa_1_Text_ParentNo, MSGBOX_DEFAULT
	release
	end

Galloro_Casa_1_EventScript_YoungTrainer::
	msgbox Galloro_Casa_1_Text_YoungTrainer, MSGBOX_NPC
	end

Galloro_Casa_1_Text_Parent:
	.string "Oh, un viaggiatore!\n"
	.string "Vi fermate a riposare?$"

Galloro_Casa_1_Text_ParentYes:
	.string "Ecco fatto! Ora pero niente corse in casa, eh!$"

Galloro_Casa_1_Text_ParentNo:
	.string "Come vuoi! La porta resta aperta.$"

Galloro_Casa_1_Text_YoungTrainer:
	.string "Ho gia preparato lo zaino per partire.\n"
	.string "Mi mancano solo le Poke Ball e i soldi per comprarle.$"
