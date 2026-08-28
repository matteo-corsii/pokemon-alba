LagoDiAlbera_RefugeMastersHouse_2F_MapScripts::
	.byte 0

LagoDiAlbera_RefugeMastersHouse_2F_EventScript_Master::
	lock
	faceplayer
	goto_if_set FLAG_LAGO_REFUGE_MASTER_EXPLAINED_STONE_BASES, LagoDiAlbera_RefugeMastersHouse_2F_EventScript_MasterAfter
	msgbox LagoDiAlbera_RefugeMastersHouse_2F_Text_Master, MSGBOX_DEFAULT
	setflag FLAG_LAGO_REFUGE_MASTER_EXPLAINED_STONE_BASES
	release
	end
LagoDiAlbera_RefugeMastersHouse_2F_EventScript_MasterAfter::
	goto_if_set FLAG_LAGO_REFUGE_AMULET_COIN_RECEIVED, LagoDiAlbera_RefugeMastersHouse_2F_EventScript_MasterDone
	goto_if_set FLAG_LAGO_REFUGE_FIRST_MISSION_STARTED, LagoDiAlbera_RefugeMastersHouse_2F_EventScript_MasterMission
	msgbox LagoDiAlbera_RefugeMastersHouse_2F_Text_MasterAssistant, MSGBOX_NPC
	end
LagoDiAlbera_RefugeMastersHouse_2F_EventScript_MasterMission::
	msgbox LagoDiAlbera_RefugeMastersHouse_2F_Text_MasterMission, MSGBOX_NPC
	end
LagoDiAlbera_RefugeMastersHouse_2F_EventScript_MasterDone::
	msgbox LagoDiAlbera_RefugeMastersHouse_2F_Text_MasterDone, MSGBOX_NPC
	end
LagoDiAlbera_RefugeMastersHouse_2F_EventScript_RockExample::
	msgbox LagoDiAlbera_RefugeMastersHouse_2F_Text_Rock, MSGBOX_SIGN
	end
LagoDiAlbera_RefugeMastersHouse_2F_EventScript_TreeExample::
	msgbox LagoDiAlbera_RefugeMastersHouse_2F_Text_Tree, MSGBOX_SIGN
	end
LagoDiAlbera_RefugeMastersHouse_2F_Text_Master:
	.string "I rifugi non si trovano soltanto\nnegli alberi.\pAnche la pietra puo custodire uno spazio\nsicuro.\pOsserva i due esempi alle mie spalle.$"
LagoDiAlbera_RefugeMastersHouse_2F_Text_Rock:
	.string "Una parete di roccia apparentemente\ncompatta.\pCon l'esperienza giusta puo diventare\nl'ingresso di un rifugio.$"
LagoDiAlbera_RefugeMastersHouse_2F_Text_Tree:
	.string "Un grande albero adattato senza\ndanneggiarlo.\pE la forma piu tradizionale di rifugio\nnaturale.$"
LagoDiAlbera_RefugeMastersHouse_2F_Text_MasterAssistant:
	.string "Il mio aiutante ti mostrera due luoghi\nadatti.\pLo trovi al piano inferiore.$"
LagoDiAlbera_RefugeMastersHouse_2F_Text_MasterMission:
	.string "Legno e pietra offrono rifugi diversi,\nma entrambi possono diventare una casa.\pScegli il luogo che senti piu adatto.$"
LagoDiAlbera_RefugeMastersHouse_2F_Text_MasterDone:
	.string "Il tuo primo rifugio e pronto.\pCol tempo potrai renderlo davvero tuo.$"
