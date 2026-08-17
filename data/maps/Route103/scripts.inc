Route103_MapScripts::
	.byte 0

Route103_EventScript_Marco::
	trainerbattle_single TRAINER_CISTERNONI_MARCO, Route103_Text_MarcoIntro, Route103_Text_MarcoDefeat
	msgbox Route103_Text_MarcoPostBattle, MSGBOX_AUTOCLOSE
	end

Route103_EventScript_Teo::
	trainerbattle_single TRAINER_CISTERNONI_TEO, Route103_Text_TeoIntro, Route103_Text_TeoDefeat
	msgbox Route103_Text_TeoPostBattle, MSGBOX_AUTOCLOSE
	end

Route103_EventScript_Fisherman::
	msgbox Route103_Text_Fisherman, MSGBOX_NPC
	end

Route103_EventScript_Visitor::
	msgbox Route103_Text_Visitor, MSGBOX_NPC
	end

Route103_EventScript_Walker::
	msgbox Route103_Text_Walker, MSGBOX_NPC
	end

Route103_Text_MarcoIntro:
	.string "Queste pietre sono scivolose quando\n"
	.string "l'aria e umida. Fammi vedere il passo!$"

Route103_Text_MarcoDefeat:
	.string "Hai un passo sicuro!$"

Route103_Text_MarcoPostBattle:
	.string "Le vie antiche chiedono rispetto\n"
	.string "a chi le attraversa.$"

Route103_Text_TeoIntro:
	.string "Gli uccelli cambiano rotta qui.\n"
	.string "Forse sentono cio che noi non sentiamo.$"

Route103_Text_TeoDefeat:
	.string "Non ti hanno distratto.$"

Route103_Text_TeoPostBattle:
	.string "Il vento porta segreti lungo\n"
	.string "questa via.$"

Route103_Text_Fisherman:
	.string "L'acqua sembra ferma, ma l'aria e\n"
	.string "piu fresca vicino ai Cisternoni.$"

Route103_Text_Visitor:
	.string "I Cisternoni raccolgono acqua da\n"
	.string "secoli. Dovevo fermarmi a guardarli.$"

Route103_Text_Walker:
	.string "La Via dei Cisternoni porta lontano,\n"
	.string "ma tutti parlano delle sue vasche.$"

Route103_EventScript_CisternoniAccessClosed::
	msgbox Route103_Text_CisternoniAccessClosed, MSGBOX_SIGN
	end

Route103_Text_CisternoniAccessClosed:
	.string "L'accesso ai CISTERNONI è\n"
	.string "interdetto.$"
