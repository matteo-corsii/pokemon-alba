Galloro_MapScripts::
	.byte 0

Galloro_EventScript_Sudowoodo::
	lock
	msgbox Galloro_Text_Sudowoodo, MSGBOX_DEFAULT
	release
	end

Galloro_EventScript_Custode::
	lock
	faceplayer
	goto_if_set FLAG_GALLORO_LA_MEMORIA_COMPLETED, Galloro_EventScript_CustodePostQuest
	setflag FLAG_GALLORO_LA_MEMORIA_SEEN
	msgbox Galloro_Text_CustodeIntro, MSGBOX_DEFAULT
	msgbox Galloro_Text_SpiritIntro, MSGBOX_DEFAULT
	msgbox Galloro_Text_SpiritManifest, MSGBOX_DEFAULT
	msgbox Galloro_Text_CustodeReply, MSGBOX_DEFAULT
	msgbox Galloro_Text_SpiritGreeting, MSGBOX_DEFAULT
	msgbox Galloro_Text_MemoryProblem, MSGBOX_DEFAULT
	checkitem ITEM_WAILMER_PAIL
	goto_if_eq VAR_RESULT, FALSE, Galloro_EventScript_MemoryNoPail
	msgbox Galloro_Text_AskWater, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Galloro_EventScript_MemoryDeclined
	goto Galloro_EventScript_WaterSudowoodo

Galloro_EventScript_WaterSudowoodo::
	lock
	faceplayer
	goto_if_set FLAG_GALLORO_LA_MEMORIA_COMPLETED, Galloro_EventScript_CustodePostQuest
	special DoWateringBerryTreeAnim
	waitse
	playse SE_SUDOWOODO_SHAKE
	applymovement LOCALID_GALLORO_SUDOWOODO, Galloro_Movement_SudowoodoShake
	waitmovement 0
	msgbox Galloro_Text_CustodeAfterWater, MSGBOX_DEFAULT
	msgbox Galloro_Text_SpiritRewardIntro, MSGBOX_DEFAULT
	checkitemspace ITEM_SPELL_TAG
	goto_if_eq VAR_RESULT, FALSE, Galloro_EventScript_MemoryBagFull
	giveitem ITEM_SPELL_TAG
	msgbox Galloro_Text_CustodeReward, MSGBOX_DEFAULT
	msgbox Galloro_Text_SpiritReward, MSGBOX_DEFAULT
	setflag FLAG_GALLORO_LA_MEMORIA_COMPLETED
	release
	end

Galloro_EventScript_MemoryNoPail::
	msgbox Galloro_Text_MemoryNoPail, MSGBOX_DEFAULT
	release
	end

Galloro_EventScript_MemoryDeclined::
	msgbox Galloro_Text_MemoryDeclined, MSGBOX_DEFAULT
	release
	end

Galloro_EventScript_MemoryBagFull::
	msgbox Galloro_Text_MemoryBagFull, MSGBOX_DEFAULT
	release
	end

Galloro_EventScript_CustodePostQuest::
	msgbox Galloro_Text_CustodePostQuest, MSGBOX_DEFAULT
	msgbox Galloro_Text_SpiritPostQuest, MSGBOX_DEFAULT
	release
	end

Galloro_Movement_SudowoodoShake:
	face_right
	delay_8
	face_down
	delay_8
	face_left
	delay_8
	face_down
	delay_8
	face_right
	delay_8
	face_down
	step_end

Galloro_EventScript_CustodeLegacy::
	msgbox Galloro_Text_Custode, MSGBOX_DEFAULT
	release
	end

Galloro_EventScript_Viaggiatore::
	msgbox Galloro_Text_Viaggiatore, MSGBOX_NPC
	end

Galloro_EventScript_Residente::
	msgbox Galloro_Text_Residente, MSGBOX_NPC
	end

Galloro_EventScript_Botanica::
	msgbox Galloro_Text_Botanica, MSGBOX_NPC
	end

Galloro_EventScript_ApprendistaElio::
	msgbox Galloro_Text_ApprendistaElio, MSGBOX_NPC
	end

Galloro_EventScript_Pellegrino::
	msgbox Galloro_Text_Pellegrino, MSGBOX_NPC
	end

Galloro_EventScript_Escursionista::
	msgbox Galloro_Text_Escursionista, MSGBOX_NPC
	end

Galloro_EventScript_Esploratore::
	trainerbattle_single TRAINER_GALLORO_ESPLORATORE, Galloro_Text_EsploratoreBefore, Galloro_Text_EsploratoreAfter
	msgbox Galloro_Text_EsploratoreRematch, MSGBOX_AUTOCLOSE
	end

Galloro_EventScript_Giardiniera::
	trainerbattle_single TRAINER_GALLORO_GIARDINIERA, Galloro_Text_GiardinieraBefore, Galloro_Text_GiardinieraAfter
	msgbox Galloro_Text_GiardinieraRematch, MSGBOX_AUTOCLOSE
	end

Galloro_EventScript_Pokefan::
	trainerbattle_single TRAINER_GALLORO_POKEFAN, Galloro_Text_PokefanBefore, Galloro_Text_PokefanAfter
	msgbox Galloro_Text_PokefanRematch, MSGBOX_AUTOCLOSE
	end

Galloro_EventScript_LabSign::
	msgbox Galloro_Text_LabSign, MSGBOX_SIGN
	end

Galloro_EventScript_SanctuarySign::
	msgbox Galloro_Text_SanctuarySign, MSGBOX_SIGN
	end

Galloro_EventScript_MemorySign::
	msgbox Galloro_Text_MemorySign, MSGBOX_SIGN
	end

Galloro_EventScript_EntranceSign::
	msgbox Galloro_Text_EntranceSign, MSGBOX_SIGN
	end

Galloro_EventScript_EastSign::
	msgbox Galloro_Text_EastSign, MSGBOX_SIGN
	end

Galloro_Text_Sudowoodo:
	.string "A silent tree watches over Galloro.\n"
	.string "Its story will be revealed later.$"

Galloro_Text_Custode:
	.string "CUSTODE: This place is being prepared.\n"
	.string "Please return when the sanctuary opens.$"
Galloro_Text_CustodeIntro:
	.string "Piano, ragazzo.\n"
	.string "Qui anche il silenzio merita rispetto.\p"
	.string "Questo luogo si chiama la Memoria.\n"
	.string "Lo custodisco da molti anni.\p"
	.string "Ma non sono mai stato solo.$"
Galloro_Text_SpiritIntro:
	.string "Un vecchio amico vive insieme a questo\n"
	.string "Pokemon. E qui da molto prima di me.$"
Galloro_Text_SpiritManifest:
	.string "Vecchio? Parla per te, amico mio.$"
Galloro_Text_CustodeReply:
	.string "Ah! Vedo che non hai perso la voglia\n"
	.string "di contraddirmi.$"
Galloro_Text_SpiritGreeting:
	.string "Un altro viandante...$"
Galloro_Text_MemoryProblem:
	.string "SPIRITO: Un tempo la pioggia raggiungeva\n"
	.string "le radici anche nei giorni piu asciutti.\p"
	.string "Ora molta terra e coperta. L'acqua scorre\n"
	.string "via, senza poter entrare.\p"
	.string "CUSTODE: I sentieri si allargano, il suolo\n"
	.string "si indurisce... E persino gli alberi piu\n"
	.string "antichi ne risentono.\p"
	.string "SPIRITO: Non chiedo che il mondo resti\n"
	.string "immobile. Chiedo soltanto che lasci ancora\n"
	.string "spazio alla vita.\p"
	.string "CUSTODE: Se hai con te qualcosa per\n"
	.string "annaffiare, un po' d'acqua potrebbe aiutarlo.$"
Galloro_Text_AskWater:
	.string "Vuoi annaffiare il Sudowoodo?$"
Galloro_Text_MemoryNoPail:
	.string "Non preoccuparti. Noi siamo qui.\p"
	.string "E, come ama ricordarmi il mio amico,\n"
	.string "sappiamo aspettare.$"
Galloro_Text_MemoryDeclined:
	.string "Va bene. Torna quando vuoi.$"
Galloro_Text_CustodeAfterWater:
	.string "Ecco... Lo vedi? Sta gia meglio.$"
Galloro_Text_SpiritRewardIntro:
	.string "L'acqua che mi hai donato ha risvegliato\n"
	.string "ricordi lontani...\p"
	.string "Vorrei affidarti qualcosa che custodisco\n"
	.string "da molto tempo.$"
Galloro_Text_CustodeReward:
	.string "Una Spettrotarga! Un dono davvero raro.\n"
	.string "Il mio vecchio amico deve averti preso\n"
	.string "in simpatia.$"
Galloro_Text_SpiritReward:
	.string "Portala con te, giovane viaggiatore.\p"
	.string "Che possa ricordarti come anche cio che non\n"
	.string "vediamo faccia parte di questo mondo.$"
Galloro_Text_MemoryBagFull:
	.string "La tua Borsa e piena. Torna quando avrai\n"
	.string "spazio per il dono.$"
Galloro_Text_CustodePostQuest:
	.string "Non lo sentivo parlare cosi da molto tempo.\p"
	.string "Non dirglielo, pero. Finirebbe per montarsi\n"
	.string "la testa.$"
Galloro_Text_SpiritPostQuest:
	.string "Ti sento benissimo, vecchio mio.$"

Galloro_Text_Viaggiatore:
	.string "Genzalia? Sempre dritto verso est.\n"
	.string "Con l'Infiorata in corso, pero,\n"
	.string "anche chi ha fretta finisce per fermarsi!$"
Galloro_Text_Residente:
	.string "Mia sorella e a Genzalia da tre giorni.\n"
	.string "Sta preparando i disegni di petali per l'Infiorata.$"
Galloro_Text_Botanica:
	.string "Gli Infiorala hanno un tempismo incredibile.\n"
	.string "Sembrano voler essere loro i protagonisti!$"
Galloro_Text_ApprendistaElio:
	.string "Il Maestro Elio sostiene che nulla vada sprecato.\n"
	.string "Oggi sto cercando di capire quale macchina faccia piu rumore.$"
Galloro_Text_Pellegrino:
	.string "Il Santuario attira molti visitatori.\n"
	.string "Ma il luogo a cui sono piu legato e poco piu a sud: la Memoria.$"
Galloro_Text_Escursionista:
	.string "Qui dentro il silenzio e strano.\n"
	.string "A volte ti sembra che qualcuno stia ascoltando te.$"
Galloro_Text_EsploratoreBefore:
	.string "Ho seguito le tracce per ore.\n"
	.string "Finalmente qualcosa si muove... Ah, sei tu!$"
Galloro_Text_EsploratoreAfter:
	.string "Meglio tornare a studiare le impronte...$"
Galloro_Text_EsploratoreRematch:
	.string "Pensavo di conoscere ogni sentiero.\n"
	.string "Oggi ne ho trovato uno che porta alla sconfitta!$"
Galloro_Text_GiardinieraBefore:
	.string "Le piante crescono meglio con un po' di competizione.\n"
	.string "Vediamo se vale anche per gli Allenatori!$"
Galloro_Text_GiardinieraAfter:
	.string "Mi hai dato una bella potata!$"
Galloro_Text_GiardinieraRematch:
	.string "La prossima volta metto radici e preparo meglio la squadra!$"
Galloro_Text_PokefanBefore:
	.string "I Pokemon di Ausonia sono incredibili!\n"
	.string "Vuoi vedere quanto li conosco?$"
Galloro_Text_PokefanAfter:
	.string "Forse dovrei studiare anche le lotte...$"
Galloro_Text_PokefanRematch:
	.string "La prossima volta ti stupiro. Ho gia preso un sacco di appunti!$"
Galloro_Text_LabSign:
	.string "Laboratorio di Elio - Ricerca e tecnologie sostenibili.$"
Galloro_Text_SanctuarySign:
	.string "Santuario di Galloro.$"
Galloro_Text_MemorySign:
	.string "La Memoria - Rispettate il silenzio del luogo.$"
Galloro_Text_EntranceSign:
	.string "GALLORO - Dove il bosco custodisce\n"
	.string "la memoria.$"
Galloro_Text_EastSign:
	.string "EST: Genzalia$"
