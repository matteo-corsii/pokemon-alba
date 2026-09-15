LagoDiAlbera_Mart_MapScripts::
	.byte 0

LagoDiAlbera_Mart_EventScript_Clerk::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart LagoDiAlbera_Mart_Pokemart
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LagoDiAlbera_Mart_Pokemart:
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_ESCAPE_ROPE
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_X_SP_ATK
	.2byte ITEM_X_SPEED
	.2byte ITEM_X_ATTACK
	.2byte ITEM_X_DEFENSE
	.2byte ITEM_DIRE_HIT
	.2byte ITEM_GUARD_SPEC
	pokemartlistend

LagoDiAlbera_Mart_EventScript_Fisherman::
	msgbox LagoDiAlbera_Mart_Text_Fisherman, MSGBOX_NPC
	end

LagoDiAlbera_Mart_EventScript_Hiker::
	msgbox LagoDiAlbera_Mart_Text_Hiker, MSGBOX_NPC
	end

LagoDiAlbera_Mart_Text_Fisherman:
	.string "Sul lago parto sempre preparato.\n"
	.string "Una giornata tranquilla può diventare\n"
	.string "lunga senza accorgertene.$"

LagoDiAlbera_Mart_Text_Hiker:
	.string "Tra pescatori ed escursionisti questo\n"
	.string "negozio non resta mai vuoto.$"
