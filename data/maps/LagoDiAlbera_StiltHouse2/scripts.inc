LagoDiAlbera_StiltHouse2_MapScripts::
	.byte 0

LagoDiAlbera_StiltHouse2_EventScript_Resident::
	msgbox LagoDiAlbera_StiltHouse2_Text_Resident, MSGBOX_NPC
	end

LagoDiAlbera_StiltHouse2_EventScript_Child::
	msgbox LagoDiAlbera_StiltHouse2_Text_Child, MSGBOX_NPC
	end

LagoDiAlbera_StiltHouse2_Text_Resident:
	.string "Ogni tanto il lago restituisce pietre\n"
	.string "lavorate e vecchi oggetti. Chissà quante\n"
	.string "persone hanno vissuto qui prima di noi.$"

LagoDiAlbera_StiltHouse2_Text_Child:
	.string "A casa teniamo tutto su mensole alte.\n"
	.string "Qui l'acqua trova sempre un modo per\n"
	.string "arrivare dove non dovrebbe.$"
