LagoDiAlbera_StiltHouse1_MapScripts::
	.byte 0

LagoDiAlbera_StiltHouse1_EventScript_Elder::
	msgbox LagoDiAlbera_StiltHouse1_Text_Elder, MSGBOX_NPC
	end

LagoDiAlbera_StiltHouse1_EventScript_Resident::
	msgbox LagoDiAlbera_StiltHouse1_Text_Resident, MSGBOX_NPC
	end

LagoDiAlbera_StiltHouse1_Text_Elder:
	.string "Queste case sull'acqua sembrano strane\n"
	.string "ai forestieri. Ma qui si costruiva così\n"
	.string "molto prima dei nostri nonni.$"

LagoDiAlbera_StiltHouse1_Text_Resident:
	.string "Quando l'acqua si abbassa si vedono pezzi\n"
	.string "di legno scuro sotto la riva. Papà dice\n"
	.string "di non toccarli.$"
