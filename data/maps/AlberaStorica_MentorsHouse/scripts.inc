AlberaStorica_MentorsHouse_MapScripts::
	.byte 0

AlberaStorica_MentorsHouse_EventScript_Mentor::
	lock
	faceplayer
	goto_if_set FLAG_ALBERA_SECRET_BASES_UNLOCKED, AlberaStorica_MentorsHouse_EventScript_MentorAfterUnlock
	msgbox AlberaStorica_MentorsHouse_Text_MentorIntro, MSGBOX_DEFAULT
	setflag FLAG_ALBERA_SECRET_BASES_UNLOCKED
	msgbox AlberaStorica_MentorsHouse_Text_MentorUnlock, MSGBOX_DEFAULT
	release
	end

AlberaStorica_MentorsHouse_EventScript_MentorAfterUnlock::
	msgbox AlberaStorica_MentorsHouse_Text_MentorAfterUnlock, MSGBOX_DEFAULT
	release
	end

AlberaStorica_MentorsHouse_Text_MentorIntro:
	.string "Un buon rifugio non deve essere grande.\n"
	.string "Deve solo sentirsi tuo.\p"
	.string "Gli alberi e le pareti giuste possono\n"
	.string "diventare luoghi personali.$"

AlberaStorica_MentorsHouse_Text_MentorUnlock:
	.string "Ora sai riconoscere un posto adatto\n"
	.string "per il tuo rifugio.\p"
	.string "Prova l'albero qui vicino.$"

AlberaStorica_MentorsHouse_Text_MentorAfterUnlock:
	.string "Cerca un luogo adatto e trasformalo\n"
	.string "nel tuo rifugio personale.$"
