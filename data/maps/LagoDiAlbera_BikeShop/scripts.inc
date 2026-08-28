LagoDiAlbera_BikeShop_MapScripts::
	.byte 0

LagoDiAlbera_BikeShop_EventScript_Owner::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_LAGO_BIKE, LagoDiAlbera_BikeShop_EventScript_Switch
	msgbox LagoDiAlbera_BikeShop_Text_Owner, MSGBOX_DEFAULT
	multichoice 0, 0, MULTI_BIKE, FALSE
	switch VAR_RESULT
	case 0, LagoDiAlbera_BikeShop_EventScript_Mach
	case 1, LagoDiAlbera_BikeShop_EventScript_Acro
	release
	end
LagoDiAlbera_BikeShop_EventScript_Mach::
	giveitem ITEM_MACH_BIKE
	goto_if_eq VAR_RESULT, FALSE, LagoDiAlbera_BikeShop_EventScript_Full
	setflag FLAG_RECEIVED_LAGO_BIKE
	release
	end
LagoDiAlbera_BikeShop_EventScript_Acro::
	giveitem ITEM_ACRO_BIKE
	goto_if_eq VAR_RESULT, FALSE, LagoDiAlbera_BikeShop_EventScript_Full
	setflag FLAG_RECEIVED_LAGO_BIKE
	release
	end
LagoDiAlbera_BikeShop_EventScript_Full::
	msgbox gText_BagFull, MSGBOX_DEFAULT
	release
	end
LagoDiAlbera_BikeShop_EventScript_Switch::
	msgbox LagoDiAlbera_BikeShop_Text_Switch, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, LagoDiAlbera_BikeShop_EventScript_Keep
	special SwapRegisteredBike
	msgbox LagoDiAlbera_BikeShop_Text_Switched, MSGBOX_DEFAULT
LagoDiAlbera_BikeShop_EventScript_Keep::
	release
	end
LagoDiAlbera_BikeShop_Text_Owner:
	.string "Una buona bici rende piu semplici\ngli spostamenti e tiene in allenamento.\pSto preparando una promozione speciale\nper chi visita il Lago.$"
LagoDiAlbera_BikeShop_Text_Switch:
	.string "Hai gia una bici registrata.\nVuoi cambiare modello gratuitamente?$"
LagoDiAlbera_BikeShop_Text_Switched:
	.string "Ecco fatto! La tua bici registrata\ne stata sostituita.$"
