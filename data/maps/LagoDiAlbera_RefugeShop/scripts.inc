LagoDiAlbera_RefugeShop_MapScripts::
	.byte 0

LagoDiAlbera_RefugeShop_EventScript_Shopkeeper::
	lock
	faceplayer
	msgbox LagoDiAlbera_RefugeShop_Text_Shopkeeper, MSGBOX_DEFAULT
	message gText_HowMayIServeYou
	waitmessage
	pokemartdecoration LagoDiAlbera_RefugeShop_PokemartDecor
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end
LagoDiAlbera_RefugeShop_EventScript_Customer::
	lock
	faceplayer
	msgbox LagoDiAlbera_RefugeShop_Text_Customer, MSGBOX_NPC
	release
	end
LagoDiAlbera_RefugeShop_Text_Shopkeeper:
	.string "Qui raccogliamo arredi robusti e leggeri,\nadatti ai rifugi intorno al Lago.$"
LagoDiAlbera_RefugeShop_Text_Customer:
	.string "Cerco qualcosa che renda accogliente\nanche un rifugio scavato nella pietra.$"
LagoDiAlbera_RefugeShop_PokemartDecor:
	.2byte DECOR_CAMP_CHAIR
	.2byte DECOR_CAMP_DESK
	.2byte DECOR_HARD_CHAIR
	.2byte DECOR_COMFORT_CHAIR
	.2byte DECOR_GLITTER_MAT
	.2byte DECOR_SPIN_MAT
	.2byte DECOR_PRETTY_FLOWERS
	.2byte DECOR_RED_PLANT
	pokemartlistend
