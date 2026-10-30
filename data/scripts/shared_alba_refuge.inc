AlbaRefuge_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, SecretBase_OnWarp
	map_script MAP_SCRIPT_ON_TRANSITION, AlbaRefuge_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, AlbaRefuge_OnLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, SecretBase_OnFrame
	map_script MAP_SCRIPT_ON_RESUME, AlbaRefuge_OnResume
	.byte 0

AlbaRefuge_OnTransition:
	call SecretBase_EventScript_SetDecorationFlags
	special SetSecretBaseOwnerGfxId
	special InitSecretBaseVars
	goto_if_ne VAR_CURRENT_SECRET_BASE, 0, AlbaRefuge_EventScript_HideFrascotto
	goto_if_set FLAG_UCCELLIERA_CHEF_DEFEATED, AlbaRefuge_EventScript_ShowFrascotto

AlbaRefuge_EventScript_HideFrascotto:
	setflag FLAG_TEMP_5
	end

AlbaRefuge_EventScript_ShowFrascotto:
	clearflag FLAG_TEMP_5
	end

AlbaRefuge_OnLoad:
AlbaRefuge_EventScript_UpdateBallPress:
	goto_if_set FLAG_ALBA_BALL_PRESS_UNLOCKED, AlbaRefuge_EventScript_BallPressVisible
	setmetatileinrange 13, 1, 14, 2, METATILE_SecretBase_Ground, FALSE, 3
	return

AlbaRefuge_EventScript_BallPressVisible:
	return

AlbaRefuge_OnResume:
	setstepcallback STEP_CB_SECRET_BASE
	call AlbaRefuge_EventScript_UpdateBallPress
	end

AlbaRefuge_EventScript_Frascotto::
	lock
	faceplayer
	msgbox AlbaRefuge_Text_FrascottoIntro, MSGBOX_NPC
	multichoice 0, 0, MULTI_ALBA_FRASCOTTO, FALSE
	switch VAR_RESULT
	case 0, AlbaRefuge_EventScript_FrascottoRecipePanino
	case 1, AlbaRefuge_EventScript_FrascottoRecipeCarbonara
	case 2, AlbaRefuge_EventScript_FrascottoRecipeCacio
	case 3, AlbaRefuge_EventScript_FrascottoRecipeAmatriciana
	case 4, AlbaRefuge_EventScript_FrascottoRecipeGricia
	case 5, AlbaRefuge_EventScript_FrascottoEnd
	case MULTI_B_PRESSED, AlbaRefuge_EventScript_FrascottoEnd
	end

AlbaRefuge_EventScript_FrascottoRecipePanino:
	setvar VAR_0x8004, ITEM_ORAN_BERRY
	setvar VAR_0x8005, ITEM_PANINO_CON_PORCHETTA
	goto AlbaRefuge_EventScript_FrascottoExchange

AlbaRefuge_EventScript_FrascottoRecipeCarbonara:
	setvar VAR_0x8004, ITEM_PECHA_BERRY
	setvar VAR_0x8005, ITEM_CARBONARA
	goto AlbaRefuge_EventScript_FrascottoExchange

AlbaRefuge_EventScript_FrascottoRecipeCacio:
	setvar VAR_0x8004, ITEM_CHERI_BERRY
	setvar VAR_0x8005, ITEM_CACIO_E_PEPE
	goto AlbaRefuge_EventScript_FrascottoExchange

AlbaRefuge_EventScript_FrascottoRecipeAmatriciana:
	setvar VAR_0x8004, ITEM_SITRUS_BERRY
	setvar VAR_0x8005, ITEM_AMATRICIANA
	goto AlbaRefuge_EventScript_FrascottoExchange

AlbaRefuge_EventScript_FrascottoRecipeGricia:
	setvar VAR_0x8004, ITEM_LEPPA_BERRY
	setvar VAR_0x8005, ITEM_GRICIA

AlbaRefuge_EventScript_FrascottoExchange:
	checkitem VAR_0x8004
	goto_if_eq VAR_RESULT, FALSE, AlbaRefuge_EventScript_FrascottoNoBerry
	checkitemspace VAR_0x8005
	goto_if_eq VAR_RESULT, FALSE, AlbaRefuge_EventScript_FrascottoBagFull
	msgbox AlbaRefuge_Text_FrascottoConfirm, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, AlbaRefuge_EventScript_FrascottoEnd
	removeitem VAR_0x8004
	goto_if_eq VAR_RESULT, FALSE, AlbaRefuge_EventScript_FrascottoExchangeFailed
	additem VAR_0x8005
	goto_if_eq VAR_RESULT, TRUE, AlbaRefuge_EventScript_FrascottoSuccess
	additem VAR_0x8004
	goto AlbaRefuge_EventScript_FrascottoExchangeFailed

AlbaRefuge_EventScript_FrascottoSuccess:
	msgbox AlbaRefuge_Text_FrascottoSuccess, MSGBOX_DEFAULT
	goto AlbaRefuge_EventScript_FrascottoEnd

AlbaRefuge_EventScript_FrascottoExchangeFailed:
	msgbox AlbaRefuge_Text_FrascottoExchangeFailed, MSGBOX_DEFAULT
	goto AlbaRefuge_EventScript_FrascottoEnd

AlbaRefuge_EventScript_FrascottoNoBerry:
	msgbox AlbaRefuge_Text_FrascottoNoBerry, MSGBOX_DEFAULT
	goto AlbaRefuge_EventScript_FrascottoEnd

AlbaRefuge_EventScript_FrascottoBagFull:
	msgbox AlbaRefuge_Text_FrascottoBagFull, MSGBOX_DEFAULT

AlbaRefuge_EventScript_FrascottoEnd:
	release
	end

AlbaRefuge_EventScript_BallPress::
	lock
	faceplayer
	goto_if_unset FLAG_ALBA_BALL_PRESS_UNLOCKED, AlbaRefuge_EventScript_BallPressLocked
	msgbox AlbaRefuge_Text_BallPressIntro, MSGBOX_DEFAULT
	multichoice 0, 0, MULTI_ALBA_BALL_PRESS, FALSE
	switch VAR_RESULT
	case 0, AlbaRefuge_EventScript_BallPressPokeball
	case 1, AlbaRefuge_EventScript_BallPressGreatball
	case 2, AlbaRefuge_EventScript_BallPressEnd
	case MULTI_B_PRESSED, AlbaRefuge_EventScript_BallPressEnd
	end

AlbaRefuge_EventScript_BallPressPokeball:
	setvar VAR_0x8004, ITEM_CHERI_BERRY
	setvar VAR_0x8005, ITEM_POKE_BALL
	goto AlbaRefuge_EventScript_BallPressChooseQuantity

AlbaRefuge_EventScript_BallPressGreatball:
	setvar VAR_0x8004, ITEM_RAWST_BERRY
	setvar VAR_0x8005, ITEM_GREAT_BALL
	goto AlbaRefuge_EventScript_BallPressChooseQuantity

@ Kept dormant until a future Galloro event exposes this recipe.
AlbaRefuge_EventScript_BallPressUltraRecipe:
	setvar VAR_0x8004, ITEM_SITRUS_BERRY
	setvar VAR_0x8005, ITEM_ULTRA_BALL

AlbaRefuge_EventScript_BallPressChooseQuantity:
	special AlbaRefuge_ChooseBallQuantity
	waitstate
	copyvar VAR_0x8006, VAR_RESULT
	goto_if_eq VAR_0x8006, 0, AlbaRefuge_EventScript_BallPressCancelled
	bufferitemnameplural STR_VAR_1, VAR_0x8005, VAR_0x8006
	bufferitemnameplural STR_VAR_2, VAR_0x8004, VAR_0x8006
	msgbox AlbaRefuge_Text_BallPressConfirm, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, AlbaRefuge_EventScript_BallPressCancelled
	checkitem VAR_0x8004, VAR_0x8006
	goto_if_eq VAR_RESULT, FALSE, AlbaRefuge_EventScript_BallPressNoBerry
	checkitemspace VAR_0x8005, VAR_0x8006
	goto_if_eq VAR_RESULT, FALSE, AlbaRefuge_EventScript_BallPressBagFull
	removeitem VAR_0x8004, VAR_0x8006
	goto_if_eq VAR_RESULT, FALSE, AlbaRefuge_EventScript_BallPressFailed
	additem VAR_0x8005, VAR_0x8006
	goto_if_eq VAR_RESULT, TRUE, AlbaRefuge_EventScript_BallPressSuccess
	additem VAR_0x8004, VAR_0x8006

AlbaRefuge_EventScript_BallPressFailed:
	msgbox AlbaRefuge_Text_BallPressFailed, MSGBOX_DEFAULT
	goto AlbaRefuge_EventScript_BallPressEnd

AlbaRefuge_EventScript_BallPressSuccess:
	playse SE_USE_ITEM
	msgbox AlbaRefuge_Text_BallPressSuccess, MSGBOX_DEFAULT
	goto AlbaRefuge_EventScript_BallPressEnd

AlbaRefuge_EventScript_BallPressNoBerry:
	msgbox AlbaRefuge_Text_BallPressNoBerry, MSGBOX_DEFAULT
	goto AlbaRefuge_EventScript_BallPressEnd

AlbaRefuge_EventScript_BallPressBagFull:
	msgbox AlbaRefuge_Text_BallPressBagFull, MSGBOX_DEFAULT
	goto AlbaRefuge_EventScript_BallPressEnd

AlbaRefuge_EventScript_BallPressCancelled:
	goto AlbaRefuge_EventScript_BallPressEnd

AlbaRefuge_EventScript_BallPressLocked:
	msgbox AlbaRefuge_Text_BallPressLocked, MSGBOX_DEFAULT

AlbaRefuge_EventScript_BallPressEnd:
	release
	end

AlbaRefuge_Text_FrascottoIntro:
	.string "Ah, eccoti! Hai portato qualche Bacca?\n"
	.string "Dai un'occhiata alle mie specialita!$"

AlbaRefuge_Text_FrascottoConfirm:
	.string "Confermare lo scambio?$"

AlbaRefuge_Text_FrascottoSuccess:
	.string "Ecco qui! Una specialita della casa.$"

AlbaRefuge_Text_FrascottoNoBerry:
	.string "Mi serve una Baccarancia per iniziare.$"

AlbaRefuge_Text_FrascottoBagFull:
	.string "Prima fai spazio nello zaino.$"

AlbaRefuge_Text_FrascottoExchangeFailed:
	.string "Lo scambio non e riuscito. La Bacca e salva.$"

AlbaRefuge_Text_BallPressIntro:
	.string "La Pressa Ball e pronta.\n"
	.string "Quale Ball vuoi produrre?$"

AlbaRefuge_Text_BallPressLocked:
	.string "La Pressa Ball non e ancora attiva.$"

AlbaRefuge_Text_BallPressConfirm:
	.string "Produrre {STR_VAR_1}\n"
	.string "usando {STR_VAR_2}?$"

AlbaRefuge_Text_BallPressNoBerry:
	.string "Non hai abbastanza Bacche per questa produzione.$"

AlbaRefuge_Text_BallPressBagFull:
	.string "Non c'e spazio sufficiente nello zaino.$"

AlbaRefuge_Text_BallPressFailed:
	.string "La produzione non e riuscita. La Bacca e salva.$"

AlbaRefuge_Text_BallPressSuccess:
	.string "Produzione completata!$"
