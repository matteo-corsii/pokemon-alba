Laricia_FraschettaVicoli_MapScripts::
	.byte 0

Laricia_FraschettaVicoli_EventScript_Host::
	msgbox Laricia_FraschettaVicoli_Text_Host, MSGBOX_NPC
	end

Laricia_FraschettaVicoli_EventScript_Guest::
	msgbox Laricia_FraschettaVicoli_Text_Guest, MSGBOX_NPC
	end

Laricia_FraschettaVicoli_Text_Host:
	.string "Nei vicoli si mangia con calma, lontano dalla confusione.$"

Laricia_FraschettaVicoli_Text_Guest:
	.string "Qui conoscono tutti la ricetta della porchetta migliore.$"
