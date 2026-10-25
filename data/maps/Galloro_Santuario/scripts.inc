Galloro_Santuario_MapScripts::
	.byte 0

Galloro_Santuario_EventScript_Fedele::
	msgbox Galloro_Santuario_Text_Fedele, MSGBOX_NPC
	end

Galloro_Santuario_EventScript_Visitatore::
	msgbox Galloro_Santuario_Text_Visitatore, MSGBOX_NPC
	end

Galloro_Santuario_Text_Fedele:
	.string "Quando fuori c'e troppo rumore,\n"
	.string "vengo qui.\p"
	.string "Non sempre trovo una risposta.\n"
	.string "Ma almeno riesco a sentire le domande.$"

Galloro_Santuario_Text_Visitatore:
	.string "Pensavo che la parte piu antica di Galloro\n"
	.string "fosse questo Santuario.\p"
	.string "Poi mi hanno parlato della Memoria.\n"
	.string "Pare che certi luoghi non debbano essere grandi\n"
	.string "per attraversare i secoli.$"
