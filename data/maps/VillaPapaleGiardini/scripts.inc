VillaPapaleGiardini_MapScripts::
	.byte 0

VillaPapaleGiardini_EventScript_Custode::
	lock
	faceplayer
	msgbox VillaPapaleGiardini_Text_Custode, MSGBOX_DEFAULT
	release
	end

VillaPapaleGiardini_EventScript_Giardiniere::
	lock
	faceplayer
	msgbox VillaPapaleGiardini_Text_Giardiniere, MSGBOX_DEFAULT
	release
	end

VillaPapaleGiardini_EventScript_Manutentore::
	lock
	faceplayer
	msgbox VillaPapaleGiardini_Text_Manutentore, MSGBOX_DEFAULT
	release
	end

VillaPapaleGiardini_EventScript_Studiosa::
	lock
	faceplayer
	msgbox VillaPapaleGiardini_Text_Studiosa, MSGBOX_DEFAULT
	release
	end

VillaPapaleGiardini_EventScript_TrainerGiardiniere::
	trainerbattle_single TRAINER_VILLA_PAPALE_GIARDINIERE, VillaPapaleGiardini_Text_TrainerGiardiniereIntro, VillaPapaleGiardini_Text_TrainerGiardiniereDefeat
	msgbox VillaPapaleGiardini_Text_TrainerGiardinierePostBattle, MSGBOX_AUTOCLOSE
	end

VillaPapaleGiardini_EventScript_TrainerManutentore::
	trainerbattle_single TRAINER_VILLA_PAPALE_MANUTENTORE, VillaPapaleGiardini_Text_TrainerManutentoreIntro, VillaPapaleGiardini_Text_TrainerManutentoreDefeat
	msgbox VillaPapaleGiardini_Text_TrainerManutentorePostBattle, MSGBOX_AUTOCLOSE
	end

VillaPapaleGiardini_EventScript_TrainerApprendista::
	trainerbattle_single TRAINER_VILLA_PAPALE_APPRENDISTA, VillaPapaleGiardini_Text_TrainerApprendistaIntro, VillaPapaleGiardini_Text_TrainerApprendistaDefeat
	msgbox VillaPapaleGiardini_Text_TrainerApprendistaPostBattle, MSGBOX_AUTOCLOSE
	end

VillaPapaleGiardini_Text_Custode:
	.string "La Villa e aperta ai visitatori,\n"
	.string "ma alcune aree restano riservate.\p"
	.string "Per gli archivi bisogna rivolgersi\n"
	.string "al personale interno.$"

VillaPapaleGiardini_Text_Giardiniere:
	.string "Qui ogni siepe viene potata\n"
	.string "con grande attenzione.\p"
	.string "Un giardino storico richiede\n"
	.string "molta piu cura di quanto sembri.$"

VillaPapaleGiardini_Text_Manutentore:
	.string "Tra vialetti, fontane e muri,\n"
	.string "c'e sempre qualcosa da sistemare.\p"
	.string "Una villa cosi non riposa mai.$"

VillaPapaleGiardini_Text_Studiosa:
	.string "Le mappe conservate qui coprono\n"
	.string "secoli di storia del territorio.\p"
	.string "Alcune mostrano luoghi molto diversi\n"
	.string "da come li vediamo oggi.$"

VillaPapaleGiardini_Text_TrainerGiardiniereIntro:
	.string "Curare un giardino richiede pazienza.\n"
	.string "Vediamo quanta ne hai tu!$"

VillaPapaleGiardini_Text_TrainerGiardiniereDefeat:
	.string "Hai saputo aspettare il momento giusto.$"

VillaPapaleGiardini_Text_TrainerGiardinierePostBattle:
	.string "Anche nelle lotte serve\n"
	.string "saper aspettare il momento giusto.$"

VillaPapaleGiardini_Text_TrainerManutentoreIntro:
	.string "Una pausa dal lavoro?\n"
	.string "Per una lotta, volentieri!$"

VillaPapaleGiardini_Text_TrainerManutentoreDefeat:
	.string "Questa pausa e stata dura!$"

VillaPapaleGiardini_Text_TrainerManutentorePostBattle:
	.string "Okay, questa pausa e stata\n"
	.string "piu dura del previsto!$"

VillaPapaleGiardini_Text_TrainerApprendistaIntro:
	.string "Conosco ogni angolo dei giardini.\p"
	.string "Vediamo se conosci altrettanto bene\n"
	.string "la tua squadra!$"

VillaPapaleGiardini_Text_TrainerApprendistaDefeat:
	.string "La tua squadra ti conosce bene.$"

VillaPapaleGiardini_Text_TrainerApprendistaPostBattle:
	.string "Direi che la conosci\n"
	.string "molto bene.$"
