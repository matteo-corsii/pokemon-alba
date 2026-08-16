AlberaStorica_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, AlberaStorica_OnLoad
	.byte 0

AlberaStorica_OnLoad:
	goto_if_unset FLAG_BADGE01_GET, AlberaStorica_OnLoad_HideNico
	goto_if_set FLAG_ALBERA_NICO_GYM_ENCOUNTER_COMPLETE, AlberaStorica_OnLoad_HideNico
	clearflag FLAG_HIDE_ALBERA_STORICA_NICO
	end

AlberaStorica_OnLoad_HideNico:
	setflag FLAG_HIDE_ALBERA_STORICA_NICO
	end

AlberaStorica_EventScript_EnterAnfiteatro::
	warp MAP_ALBERA_STORICA_ANFITEATRO, 7, 10
	waitstate
	end

AlberaStorica_EventScript_OrientationNpc::
	msgbox AlberaStorica_Text_OrientationNpc, MSGBOX_NPC
	end

AlberaStorica_EventScript_ShopNpc::
	msgbox AlberaStorica_Text_ShopNpc, MSGBOX_NPC
	end

AlberaStorica_EventScript_PlazaNpc::
	msgbox AlberaStorica_Text_PlazaNpc, MSGBOX_NPC
	end

AlberaStorica_EventScript_GreenNpc::
	msgbox AlberaStorica_Text_GreenNpc, MSGBOX_NPC
	end

AlberaStorica_EventScript_ArchiveNpc::
	msgbox AlberaStorica_Text_ArchiveNpc, MSGBOX_NPC
	end

AlberaStorica_EventScript_GymNpc::
	msgbox AlberaStorica_Text_GymNpc, MSGBOX_NPC
	end

AlberaStorica_EventScript_NicoPostGym::
	lockall
	faceplayer
	msgbox AlberaStorica_Text_NicoPostGym, MSGBOX_DEFAULT
	setflag FLAG_ALBERA_NICO_GYM_ENCOUNTER_COMPLETE
	setflag FLAG_HIDE_ALBERA_STORICA_NICO
	removeobject LOCALID_ALBERA_STORICA_NICO
	releaseall
	end

AlberaStorica_Text_OrientationNpc:
	.string "Questa è Albèra Storica.\n"
	.string "Se sali verso la piazza trovi l'Archivio\l"
	.string "e l'Anfiteatro.$"

AlberaStorica_Text_ShopNpc:
	.string "Hai già un rifugio tutto tuo?\n"
	.string "Quella bottega vende qualche mobile\l"
	.string "semplice. Per cominciare basta e avanza.$"

AlberaStorica_Text_PlazaNpc:
	.string "Quando c'è movimento all'Anfiteatro,\n"
	.string "mezza Albèra finisce per passare\l"
	.string "da questa piazza.$"

AlberaStorica_Text_GreenNpc:
	.string "Qui il verde è tenuto con cura.\n"
	.string "Fa bene fermarsi un momento\l"
	.string "prima di ripartire.$"

AlberaStorica_Text_ArchiveNpc:
	.string "Ultimamente entrano ed escono tutti\n"
	.string "con mappe e registri sotto il braccio.\p"
	.string "Dev'esserci qualcosa che li preoccupa.$"

AlberaStorica_Text_GymNpc:
	.string "Se sei qui per Lirio, preparati bene.\n"
	.string "Dentro l'Anfiteatro non basta\l"
	.string "avere entusiasmo.$"

AlberaStorica_Text_NicoPostGym:
	.string "Ah, quindi hai già finito?\p"
	.string "Bene. Allora tocca a me.\n"
	.string "Non pensare di prendere troppo vantaggio!\p"
	.string "Ci vediamo più avanti. Prima voglio vedere\n"
	.string "cosa sa fare questo Lirio.$"
