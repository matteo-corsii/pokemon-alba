Laricia_Mart_MapScripts::
	.byte 0

Laricia_Mart_EventScript_Clerk::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart Laricia_Mart_Pokemart
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
Laricia_Mart_Pokemart:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	pokemartlistend

Laricia_Mart_EventScript_Customer::
	msgbox Laricia_Mart_Text_Customer, MSGBOX_NPC
	end

Laricia_Mart_Text_Customer:
	.string "Prima di attraversare la Valle,\n"
	.string "faccio sempre scorta qui.$"
