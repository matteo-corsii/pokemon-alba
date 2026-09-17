Laricia_MapScripts::
	.byte 0

Laricia_EventScript_SagraBlocker::
	msgbox Laricia_Text_SagraBlocker, MSGBOX_NPC
	end

Laricia_EventScript_GalloroGenzaliaSign::
	msgbox Laricia_Text_GalloroGenzaliaSign, MSGBOX_SIGN
	end

Laricia_Text_SagraBlocker:
	.string "Di qui non si passa ancora.\p"
	.string "Stiamo preparando la Sagra\n"
	.string "della Porchetta.$"

Laricia_Text_GalloroGenzaliaSign:
	.string "GALLORO / GENZALIA {RIGHT_ARROW}$"
