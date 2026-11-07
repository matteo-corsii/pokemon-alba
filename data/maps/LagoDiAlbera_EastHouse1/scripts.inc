LagoDiAlbera_EastHouse1_MapScripts::
	.byte 0

LagoDiAlbera_EastHouse1_EventScript_Fisherman::
	msgbox LagoDiAlbera_EastHouse1_Text_Fisherman, MSGBOX_NPC
	end

LagoDiAlbera_EastHouse1_EventScript_Resident::
	msgbox LagoDiAlbera_EastHouse1_Text_Resident, MSGBOX_NPC
	end

LagoDiAlbera_EastHouse1_Text_Fisherman:
	.string "Il livello del lago non è sempre stato così.\n"
	.string "Gli antichi scavarono perfino nella roccia\n"
	.string "per governarne le acque.$"

LagoDiAlbera_EastHouse1_Text_Resident:
	.string "Quando il lago cresce, controlliamo ogni\n"
	.string "passerella prima di uscire di casa.$"
