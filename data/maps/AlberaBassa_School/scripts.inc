AlberaBassa_School_MapScripts::
	.byte 0

AlberaBassa_School_EventScript_Teacher::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_RUNNING_SHOES, AlberaBassa_School_EventScript_TeacherAfterReward
	msgbox AlberaBassa_School_Text_TeacherIntro, MSGBOX_DEFAULT
	playfanfare MUS_OBTAIN_ITEM
	message AlberaBassa_School_Text_ReceivedRunningShoes
	waitfanfare
	setflag FLAG_RECEIVED_RUNNING_SHOES
	setflag FLAG_SYS_B_DASH
	msgbox AlberaBassa_School_Text_RunningTutorial, MSGBOX_DEFAULT
	release
	end

AlberaBassa_School_EventScript_TeacherAfterReward::
	msgbox AlberaBassa_School_Text_TeacherAfterReward, MSGBOX_DEFAULT
	release
	end

AlberaBassa_School_EventScript_StudentOne::
	msgbox AlberaBassa_School_Text_StudentOne, MSGBOX_NPC
	end

AlberaBassa_School_EventScript_StudentTwo::
	msgbox AlberaBassa_School_Text_StudentTwo, MSGBOX_NPC
	end

AlberaBassa_School_Text_TeacherIntro:
	.string "INSEGNANTE: Camminare con calma aiuta\n"
	.string "a osservare cio' che ti circonda.\p"
	.string "Ma quando conosci bene la strada,\n"
	.string "puoi anche muoverti piu' in fretta.\p"
	.string "Tieni queste SCARPE DA CORSA.$"

AlberaBassa_School_Text_ReceivedRunningShoes:
	.string "{PLAYER} riceve le\n"
	.string "SCARPE DA CORSA!$"

AlberaBassa_School_Text_RunningTutorial:
	.string "Tieni premuto B mentre cammini\n"
	.string "all'aperto per correre.$"

AlberaBassa_School_Text_TeacherAfterReward:
	.string "INSEGNANTE: Non correre dove non\n"
	.string "conosci ancora la strada.$"

AlberaBassa_School_Text_StudentOne:
	.string "STUDENTE: A piedi noto molti piu'\n"
	.string "particolari del quartiere.$"

AlberaBassa_School_Text_StudentTwo:
	.string "STUDENTESSA: Le Scarpe da Corsa sono\n"
	.string "comode quando hai gia' deciso dove andare.$"
