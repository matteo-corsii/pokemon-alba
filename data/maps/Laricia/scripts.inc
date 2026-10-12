Laricia_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Laricia_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Laricia_OnFrame
	.byte 0

Laricia_OnTransition:
	goto_if_unset FLAG_LARICIA_SAGRA_ACTIVE, Laricia_OnTransition_Preparativi
	clearflag FLAG_HIDE_LARICIA_SAGRA_TRUCK_3
	clearflag FLAG_HIDE_LARICIA_POST_GYM_NICO_LIA
	setflag FLAG_HIDE_LARICIA_POST_GYM_AUGUSTO
	goto Laricia_OnTransition_Done

Laricia_OnTransition_Preparativi:
	setflag FLAG_HIDE_LARICIA_SAGRA_TRUCK_3
	setflag FLAG_HIDE_LARICIA_POST_GYM_AUGUSTO
	setflag FLAG_HIDE_LARICIA_POST_GYM_NICO_LIA

Laricia_OnTransition_Done:
	end

Laricia_OnFrame:
	map_script_2 VAR_TEMP_0, 1, Laricia_EventScript_PostGymScene
	.2byte 0

Laricia_EventScript_SagraBlocker::
	goto_if_set FLAG_LARICIA_SAGRA_ACTIVE, Laricia_EventScript_SagraCamion
	msgbox Laricia_Text_SagraBlocker, MSGBOX_NPC
	end

Laricia_EventScript_SagraCamion::
	msgbox Laricia_Text_SagraCamion, MSGBOX_NPC
	end

Laricia_EventScript_SagraAllestimento::
	msgbox Laricia_Text_SagraAllestimento, MSGBOX_NPC
	end

Laricia_EventScript_SagraVenditore1::
	lock
	faceplayer
	goto_if_unset FLAG_LARICIA_SAGRA_ACTIVE, Laricia_EventScript_SagraVenditore1_Preparativi
	msgbox Laricia_Text_SagraVenditore1, MSGBOX_DEFAULT
	pokemart Laricia_Sagra_Pokemart
	msgbox Laricia_Text_SagraGrazie, MSGBOX_DEFAULT
	release
	end

Laricia_EventScript_SagraVenditore1_Preparativi::
	msgbox Laricia_Text_SagraVenditore1_Preparativi, MSGBOX_DEFAULT
	release
	end

Laricia_EventScript_SagraVenditore2::
	lock
	faceplayer
	goto_if_unset FLAG_LARICIA_SAGRA_ACTIVE, Laricia_EventScript_SagraVenditore2_Preparativi
	msgbox Laricia_Text_SagraVenditore2, MSGBOX_DEFAULT
	pokemart Laricia_Sagra_Pokemart
	msgbox Laricia_Text_SagraGrazie, MSGBOX_DEFAULT
	release
	end

Laricia_EventScript_SagraVenditore2_Preparativi::
	msgbox Laricia_Text_SagraVenditore2_Preparativi, MSGBOX_DEFAULT
	release
	end

Laricia_EventScript_SagraVenditore3::
	lock
	faceplayer
	goto_if_unset FLAG_LARICIA_SAGRA_ACTIVE, Laricia_EventScript_SagraVenditore3_Preparativi
	msgbox Laricia_Text_SagraVenditore3, MSGBOX_DEFAULT
	pokemart Laricia_Sagra_Pokemart
	msgbox Laricia_Text_SagraGrazie, MSGBOX_DEFAULT
	release
	end

Laricia_EventScript_SagraVenditore3_Preparativi::
	msgbox Laricia_Text_SagraVenditore3_Preparativi, MSGBOX_DEFAULT
	release
	end

Laricia_EventScript_PostGymAugusto::
	msgbox Laricia_Text_PostGymAugusto, MSGBOX_NPC
	end

Laricia_EventScript_PostGymNico::
	msgbox Laricia_Text_PostGymNico, MSGBOX_NPC
	end

Laricia_EventScript_PostGymLia::
	msgbox Laricia_Text_PostGymLia, MSGBOX_NPC
	end

Laricia_EventScript_PostGymScene::
	setvar VAR_TEMP_0, 0
	goto_if_unset FLAG_LARICIA_POST_GYM_PENDING, Laricia_EventScript_PostGymScene_End
	clearflag FLAG_LARICIA_POST_GYM_PENDING
	goto_if_set FLAG_LARICIA_SAGRA_ACTIVE, Laricia_EventScript_PostGymScene_End
	goto_if_unset FLAG_BADGE03_GET, Laricia_EventScript_PostGymScene_End
	goto_if_unset FLAG_RECEIVED_TM_FLAME_CHARGE, Laricia_EventScript_PostGymScene_End
	lockall
	clearflag FLAG_HIDE_LARICIA_POST_GYM_AUGUSTO
	clearflag FLAG_HIDE_LARICIA_POST_GYM_NICO_LIA
	addobject LOCALID_LARICIA_POST_GYM_AUGUSTO
	addobject LOCALID_LARICIA_POST_GYM_NICO
	addobject LOCALID_LARICIA_POST_GYM_LIA
	msgbox Laricia_Text_PostGymAugustoInvite, MSGBOX_DEFAULT
	applymovement LOCALID_LARICIA_POST_GYM_AUGUSTO, Laricia_Movement_AugustoToPiazza
	waitmovement 0
	msgbox Laricia_Text_PostGymNicoJoin, MSGBOX_DEFAULT
	msgbox Laricia_Text_PostGymLiaJoin, MSGBOX_DEFAULT
	msgbox Laricia_Text_PostGymMayor, MSGBOX_DEFAULT
	msgbox Laricia_Text_PostGymTerraFutura, MSGBOX_DEFAULT
	msgbox Laricia_Text_PostGymAugustoSagra, MSGBOX_DEFAULT
	msgbox Laricia_Text_PostGymMayorClosing, MSGBOX_DEFAULT
	setflag FLAG_LARICIA_SAGRA_ACTIVE
	clearflag FLAG_HIDE_LARICIA_SAGRA_TRUCK_3
	addobject LOCALID_LARICIA_SAGRA_CAMION_3
	setflag FLAG_HIDE_LARICIA_POST_GYM_AUGUSTO
	removeobject LOCALID_LARICIA_POST_GYM_AUGUSTO
	releaseall

Laricia_EventScript_PostGymScene_End:
	end

Laricia_Movement_AugustoToPiazza:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_left
	walk_left
	step_end

Laricia_Text_SagraBlocker:
	.string "La strada per Galloro\n"
	.string "e chiusa per ora.\p"
	.string "Stiamo preparando la Sagra\n"
	.string "della Porchetta.$"

Laricia_Text_SagraCamion:
	.string "Il camioncino e pronto per la festa!\p"
	.string "Qui si sente gia il profumo\n"
	.string "della porchetta.$"
Laricia_Text_SagraAllestimento:
	.string "Un elemento dell'allestimento\n"
	.string "della Sagra della Porchetta.$"

Laricia_Text_SagraVenditore1_Preparativi:
	.string "Stiamo ancora sistemando tutto.\p"
	.string "Tra poco la piazza sara pronta.$"
Laricia_Text_SagraVenditore1:
	.string "La nostra ricetta e semplice:\n"
	.string "pane caldo e porchetta locale.$"
Laricia_Text_SagraVenditore2_Preparativi:
	.string "Il fuoco e quasi pronto,\n"
	.string "ma non si vende ancora.$"
Laricia_Text_SagraVenditore2:
	.string "Senti che profumo?\p"
	.string "Un panino appena fatto ti aspetta.$"
Laricia_Text_SagraVenditore3_Preparativi:
	.string "Quando aprira la Sagra\n"
	.string "qui ci sara posto per tutti.$"
Laricia_Text_SagraVenditore3:
	.string "Benvenuto alla festa!\p"
	.string "Prendi un panino e goditi la piazza.$"
Laricia_Text_SagraGrazie:
	.string "Grazie e buona festa!$"

Laricia_Text_PostGymAugustoInvite:
	.string "Hai acceso tutte le fiamme della palestra.\p"
	.string "Ora vieni con me: Laricia deve festeggiare\n"
	.string "la tua vittoria.$"
Laricia_Text_PostGymNicoJoin:
	.string "NICO: Una festa in piazza?\p"
	.string "Allora veniamo anche noi.$"
Laricia_Text_PostGymLiaJoin:
	.string "LIA: Dopo tutta quella storia,\n"
	.string "un po' di festa ci vuole proprio.$"
Laricia_Text_PostGymMayor:
	.string "SINDACO: Cittadini di Laricia,\n"
	.string "oggi celebriamo una nuova fiamma\n"
	.string "per la nostra citta.$"
Laricia_Text_PostGymTerraFutura:
	.string "TERRA FUTURA: La tradizione locale\n"
	.string "e una risorsa da custodire con cura.\p"
	.string "Sosterremo questa festa\n"
	.string "nel rispetto della comunita.$"
Laricia_Text_PostGymAugustoSagra:
	.string "AUGUSTO: Il fuoco della palestra\n"
	.string "e anche il calore delle persone.\p"
	.string "Che questa Sagra resti un incontro\n"
	.string "fra memoria e futuro.$"
Laricia_Text_PostGymMayorClosing:
	.string "SINDACO: La Sagra della Porchetta\n"
	.string "e ufficialmente aperta!\p"
	.string "Le strade sono libere: benvenuti\n"
	.string "a Laricia.$"

	.align 2
Laricia_Sagra_Pokemart:
	.2byte ITEM_PANINO_CON_PORCHETTA
	pokemartlistend

Laricia_EventScript_AnzianoPiazza::
	goto_if_set FLAG_LARICIA_SAGRA_ACTIVE, Laricia_EventScript_SindacoSagra
	msgbox Laricia_Text_AnzianoPiazza, MSGBOX_NPC
	end
Laricia_EventScript_SindacoSagra::
	msgbox Laricia_Text_PostGymMayor, MSGBOX_NPC
	end
Laricia_EventScript_VisitatorePalazzo::
	goto_if_set FLAG_LARICIA_SAGRA_ACTIVE, Laricia_EventScript_TerraFuturaSagra
	msgbox Laricia_Text_VisitatorePalazzo, MSGBOX_NPC
	end
Laricia_EventScript_TerraFuturaSagra::
	msgbox Laricia_Text_PostGymTerraFutura, MSGBOX_NPC
	end

Laricia_EventScript_SagraSpettatore::
	msgbox Laricia_Text_SagraSpettatore, MSGBOX_NPC
	end

Laricia_EventScript_ClienteFraschetta::
	msgbox Laricia_Text_ClienteFraschetta, MSGBOX_NPC
	end
Laricia_EventScript_RagazzoCentro::
	msgbox Laricia_Text_RagazzoCentro, MSGBOX_NPC
	end
Laricia_EventScript_AbitanteMart::
	msgbox Laricia_Text_AbitanteMart, MSGBOX_NPC
	end
Laricia_EventScript_ResidenteVicoli::
	msgbox Laricia_Text_ResidenteVicoli, MSGBOX_NPC
	end
Laricia_EventScript_ContadinoValle::
	msgbox Laricia_Text_ContadinoValle, MSGBOX_NPC
	end
Laricia_EventScript_ViandanteUccelliera::
	msgbox Laricia_Text_ViandanteUccelliera, MSGBOX_NPC
	end
Laricia_EventScript_VisitatoreSagra::
	msgbox Laricia_Text_VisitatoreSagra, MSGBOX_NPC
	end

Laricia_Text_AnzianoPiazza:
	.string "A Laricia il sole batte forte.\p"
	.string "Ma quando arriva sera,\n"
	.string "la piazza resta piena di vita.$"
Laricia_Text_VisitatorePalazzo:
	.string "Da vicino il Palazzo Chigi\n"
	.string "e ancora piu imponente.\p"
	.string "Domina tutta la piazza.$"
Laricia_Text_ClienteFraschetta:
	.string "A Laricia ognuno ha\n"
	.string "la sua fraschetta preferita.\p"
	.string "Prova a chiedere quale sia\n"
	.string "la migliore... e vedrai!$"
Laricia_Text_RagazzoCentro:
	.string "Faccio avanti e indietro\n"
	.string "per Laricia tutto il giorno.\p"
	.string "Qui e difficile stare fermi!$"
Laricia_Text_AbitanteMart:
	.string "Con tutta la gente che arriva\n"
	.string "per la Sagra, conviene fare\n"
	.string "scorta per tempo.$"
Laricia_Text_ResidenteVicoli:
	.string "Le strade grandi portano\n"
	.string "tutti verso la piazza.\p"
	.string "Nei vicoli trovi la Laricia\n"
	.string "che conosciamo noi.$"
Laricia_Text_ContadinoValle:
	.string "La valle sotto Laricia\n"
	.string "e piena di campi.\p"
	.string "Con il sole giusto,\n"
	.string "qui cresce di tutto.$"
Laricia_Text_ViandanteUccelliera:
	.string "Via dell'Uccelliera sale\n"
	.string "da questa parte.\p"
	.string "Da lassu si vede tutta\n"
	.string "la campagna di Laricia.$"
Laricia_Text_VisitatoreSagra:
	.string "Per la Sagra arriva gente\n"
	.string "da tutta la zona.\p"
	.string "E il profumo si sente prima\n"
	.string "ancora di arrivare in piazza!$"
Laricia_Text_SagraSpettatore:
	.string "Che bella festa in piazza!\p"
	.string "Aspetto l'inizio del discorso.$"
