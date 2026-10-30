Laricia_FraschettaPiazza_MapScripts::
	.byte 0

Laricia_FraschettaPiazza_EventScript_Host::
	msgbox Laricia_FraschettaPiazza_Text_Host, MSGBOX_NPC
	end

Laricia_FraschettaPiazza_EventScript_Waiter::
	msgbox Laricia_FraschettaPiazza_Text_Waiter, MSGBOX_NPC
	end

Laricia_FraschettaPiazza_EventScript_Guest::
	msgbox Laricia_FraschettaPiazza_Text_Guest, MSGBOX_NPC
	end

Laricia_FraschettaPiazza_Text_Host:
	.string "Qui la porchetta si prepara con la ricetta di Laricia.$"

Laricia_FraschettaPiazza_Text_Waiter:
	.string "Durante la Sagra la piazza si riempie fino a sera.$"

Laricia_FraschettaPiazza_Text_Guest:
	.string "Sono arrivato presto per trovare un tavolo vicino alla piazza.$"
