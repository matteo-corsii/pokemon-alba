BorgoDiCastello_RistoranteBelvedere_MapScripts::
	.byte 0

BorgoDiCastello_RistoranteBelvedere_EventScript_Gestore::
	msgbox BorgoDiCastello_RistoranteBelvedere_Text_Gestore, MSGBOX_NPC
	end

BorgoDiCastello_RistoranteBelvedere_EventScript_ClienteAnziano::
	msgbox BorgoDiCastello_RistoranteBelvedere_Text_ClienteAnziano, MSGBOX_NPC
	end

BorgoDiCastello_RistoranteBelvedere_EventScript_ClienteGiovane::
	msgbox BorgoDiCastello_RistoranteBelvedere_Text_ClienteGiovane, MSGBOX_NPC
	end

BorgoDiCastello_RistoranteBelvedere_Text_Gestore:
	.string "Benvenuto al Ristorante\n"
	.string "Belvedere!\p"
	.string "Qui si mangia con calma.\n"
	.string "Il panorama fa il resto.$"

BorgoDiCastello_RistoranteBelvedere_Text_ClienteAnziano:
	.string "Mio nonno raccontava sempre\n"
	.string "che la Valle di Laricia,\n"
	.string "un tempo, era un lago.\p"
	.string "A guardarla oggi,\n"
	.string "sembra impossibile.$"

BorgoDiCastello_RistoranteBelvedere_Text_ClienteGiovane:
	.string "Prima o poi voglio andare\n"
	.string "a Laricia.\p"
	.string "Dicono che gli Allenatori\n"
	.string "laggiu siano tosti!$"
