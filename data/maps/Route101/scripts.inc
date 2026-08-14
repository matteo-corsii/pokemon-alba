Route101_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route101_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route101_OnFrame
	.byte 0

Route101_OnTransition:
	call ProfBirch_EventScript_UpdateLocation
	call_if_eq VAR_ALBERA_OPENING_STATE, 5, Route101_EventScript_SetAlberaCheckpoint
	call_if_eq VAR_ALBERA_VIA_VERDI_STATE, 2, Route101_EventScript_SetLiaAtPokemonBehavior
	call_if_eq VAR_ALBERA_VIA_VERDI_STATE, 3, Route101_EventScript_SetLiaAtAncientCanal
	call_if_ge VAR_ALBERA_VIA_VERDI_STATE, 4, Route101_EventScript_EndViaVerdiInvestigation
	end

Route101_EventScript_SetAlberaCheckpoint::
	setvar VAR_ALBERA_OPENING_STATE, 6
	return

Route101_EventScript_SetLiaAtPokemonBehavior::
	setobjectxyperm LOCALID_ROUTE101_LIA, 21, 9
	call Route101_EventScript_RestoreLiaForInvestigation
	setflag FLAG_HIDE_ROUTE101_NICO
	removeobject LOCALID_ROUTE101_NICO
	return

Route101_EventScript_SetLiaAtAncientCanal::
	setobjectxyperm LOCALID_ROUTE101_LIA, 10, 3
	call Route101_EventScript_RestoreLiaForInvestigation
	setflag FLAG_HIDE_ROUTE101_NICO
	removeobject LOCALID_ROUTE101_NICO
	return

Route101_EventScript_RestoreLiaForInvestigation::
	goto_if_unset FLAG_HIDE_ROUTE101_RESEARCH_PARTY, Route101_EventScript_RestoreLiaForInvestigation_Return
	clearflag FLAG_HIDE_ROUTE101_RESEARCH_PARTY
	addobject LOCALID_ROUTE101_LIA
Route101_EventScript_RestoreLiaForInvestigation_Return::
	return

Route101_EventScript_EndViaVerdiInvestigation::
	setflag FLAG_HIDE_ROUTE101_RESEARCH_PARTY
	setflag FLAG_HIDE_ROUTE101_NICO
	removeobject LOCALID_ROUTE101_LIA
	removeobject LOCALID_ROUTE101_NICO
	return

Route101_OnFrame:
	map_script_2 VAR_ROUTE101_STATE, 0, Route101_EventScript_HideMapNamePopup
	.2byte 0

Route101_EventScript_HideMapNamePopup::
	setflag FLAG_HIDE_MAP_NAME_POPUP
	setvar VAR_ROUTE101_STATE, 1
	end

Route101_EventScript_StartBirchRescue::
	lockall
	playbgm MUS_HELP, TRUE
	msgbox Route101_Text_HelpMe, MSGBOX_DEFAULT
	closemessage
	setobjectxy LOCALID_ROUTE101_BIRCH, 0, 15
	setobjectxy LOCALID_ROUTE101_ZIGZAGOON, 0, 16
	applymovement LOCALID_PLAYER, Route101_Movement_EnterScene
	applymovement LOCALID_ROUTE101_BIRCH, Route101_Movement_BirchRunAway1
	applymovement LOCALID_ROUTE101_ZIGZAGOON, Route101_Movement_ZigzagoonChase1
	waitmovement 0
	applymovement LOCALID_ROUTE101_ZIGZAGOON, Route101_Movement_ZigzagoonChaseInCircles
	applymovement LOCALID_ROUTE101_BIRCH, Route101_Movement_BirchRunInCircles
	waitmovement 0
	applymovement LOCALID_ROUTE101_BIRCH, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	applymovement LOCALID_ROUTE101_ZIGZAGOON, Route101_Movement_ZigzagoonFaceBirch
	applymovement LOCALID_ROUTE101_BIRCH, Route101_Movement_BirchFaceZigzagoon
	waitmovement 0
	msgbox Route101_Text_PleaseHelp, MSGBOX_DEFAULT
	closemessage
	setvar VAR_ROUTE101_STATE, 2
	releaseall
	end

Route101_EventScript_PreventExitSouth::
	lockall
	msgbox Route101_Text_DontLeaveMe, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, Route101_Movement_PreventExitSouth
	waitmovement 0
	releaseall
	end

Route101_EventScript_PreventExitWest::
	lockall
	msgbox Route101_Text_DontLeaveMe, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, Route101_Movement_PreventExitWest
	waitmovement 0
	releaseall
	end

Route101_EventScript_PreventExitNorth::
	lockall
	msgbox Route101_Text_DontLeaveMe, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, Route101_Movement_PreventExitNorth
	waitmovement 0
	releaseall
	end

Route101_Movement_PreventExitSouth:
	walk_up
	step_end

Route101_Movement_PreventExitWest:
	walk_right
	step_end

Route101_Movement_PreventExitNorth:
	walk_down
	step_end

Route101_Movement_ZigzagoonChaseInCircles:
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_down
	walk_fast_down
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_down
	walk_fast_down
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_down
	walk_fast_down
	walk_fast_left
	walk_fast_left
	step_end

Route101_Movement_ZigzagoonChase1:
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_up
	step_end

@ Leftover data? This command is unused.
	step_end

Route101_Movement_ZigzagoonFaceBirch:
	walk_in_place_fast_left
	walk_in_place_fast_left
	walk_in_place_fast_left
	walk_in_place_fast_left
	step_end

Route101_Movement_EnterScene:
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_in_place_faster_left
	step_end

Route101_Movement_BirchRunInCircles:
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_down
	walk_fast_down
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_down
	walk_fast_down
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_down
	walk_fast_down
	walk_fast_left
	walk_fast_left
	walk_fast_left
	step_end

Route101_Movement_BirchRunAway1:
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_up
	walk_fast_up
	step_end

@ Leftover data? This command is unused.
	step_end

Route101_Movement_BirchFaceZigzagoon:
	walk_in_place_fast_right
	walk_in_place_fast_right
	walk_in_place_fast_right
	walk_in_place_fast_right
	step_end

Route101_Movement_Unused1:
	walk_up
	walk_up
	step_end

Route101_Movement_Unused2:
	walk_up
	walk_left
	walk_up
	step_end

Route101_EventScript_Youngster::
	msgbox Route101_Text_TakeTiredPokemonToPokeCenter, MSGBOX_NPC
	end

Route101_EventScript_AmbientHiker::
	msgbox Route101_Text_AmbientHiker, MSGBOX_NPC
	end

Route101_EventScript_Boy::
	msgbox Route101_Text_WildPokemonInTallGrass, MSGBOX_NPC
	end

Route101_EventScript_Lia::
	msgbox Route101_Text_LiaResearchReminder, MSGBOX_NPC
	end

Route101_EventScript_Nico::
	msgbox Route101_Text_NicoResearchReminder, MSGBOX_NPC
	end

Route101_EventScript_InvestigateSource::
	lockall
	applymovement LOCALID_ROUTE101_LIA, Common_Movement_FaceRight
	applymovement LOCALID_ROUTE101_NICO, Common_Movement_FaceRight
	waitmovement 0
	msgbox Route101_Text_LiaAlmostDrySource, MSGBOX_DEFAULT
	msgbox Route101_Text_NicoAtSource, MSGBOX_DEFAULT
	setvar VAR_ALBERA_VIA_VERDI_STATE, 2
	setobjectxy LOCALID_ROUTE101_LIA, 21, 9
	setobjectxyperm LOCALID_ROUTE101_LIA, 21, 9
	setflag FLAG_HIDE_ROUTE101_NICO
	removeobject LOCALID_ROUTE101_NICO
	releaseall
	end

Route101_EventScript_InvestigatePokemonBehavior::
	lockall
	applymovement LOCALID_ROUTE101_LIA, Common_Movement_FaceDown
	waitmovement 0
	msgbox Route101_Text_LiaPokemonBehavior, MSGBOX_DEFAULT
	setvar VAR_ALBERA_VIA_VERDI_STATE, 3
	setobjectxy LOCALID_ROUTE101_LIA, 10, 3
	setobjectxyperm LOCALID_ROUTE101_LIA, 10, 3
	releaseall
	end

Route101_EventScript_InvestigateAncientCanal::
	lockall
	applymovement LOCALID_ROUTE101_LIA, Common_Movement_FaceDown
	waitmovement 0
	msgbox Route101_Text_LiaAncientCanal, MSGBOX_DEFAULT
	playse SE_M_UPROAR
	setvar VAR_0x8006, 4
	setvar VAR_0x8007, 2
	special ShakeScreen
	delay 16
	msgbox Route101_Text_WaterFlowStops, MSGBOX_DEFAULT
	delay 12
	playse SE_M_WATERFALL
	msgbox Route101_Text_WaterFlowReturns, MSGBOX_DEFAULT
	setvar VAR_ALBERA_VIA_VERDI_STATE, 4
	setflag FLAG_HIDE_ROUTE101_RESEARCH_PARTY
	setflag FLAG_HIDE_ROUTE101_NICO
	removeobject LOCALID_ROUTE101_LIA
	releaseall
	end

Route101_EventScript_RouteSign::
	msgbox Route101_Text_RouteSign, MSGBOX_SIGN
	end

Route101_EventScript_AnticaVillaSign::
	msgbox Route101_Text_AnticaVillaSign, MSGBOX_SIGN
	end

Route101_EventScript_BirchsBag::
	lock
	faceplayer
	setflag FLAG_SYS_POKEMON_GET
	setflag FLAG_RESCUED_BIRCH
	fadescreen FADE_TO_BLACK
	removeobject LOCALID_ROUTE101_ZIGZAGOON
	setobjectxy LOCALID_PLAYER, 6, 13
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	special ChooseStarter
	applymovement LOCALID_ROUTE101_BIRCH, Route101_Movement_BirchApproachPlayer
	waitmovement 0
	msgbox Route101_Text_YouSavedMe, MSGBOX_DEFAULT
	special HealPlayerParty
	setflag FLAG_HIDE_ROUTE_101_BIRCH_ZIGZAGOON_BATTLE
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_BIRCH
	setflag FLAG_HIDE_ROUTE_101_BIRCH_STARTERS_BAG
	setvar VAR_BIRCH_LAB_STATE, 2
	setvar VAR_ROUTE101_STATE, 3
	clearflag FLAG_HIDE_MAP_NAME_POPUP
	checkplayergender
	call_if_eq VAR_RESULT, MALE, Route101_EventScript_HideMayInBedroom
	call_if_eq VAR_RESULT, FEMALE, Route101_EventScript_HideBrendanInBedroom
	warp MAP_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB, 6, 5
	waitstate
	release
	end

Route101_EventScript_HideMayInBedroom::
	setflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_RIVAL_BEDROOM
	return

Route101_EventScript_HideBrendanInBedroom::
	setflag FLAG_HIDE_LITTLEROOT_TOWN_BRENDANS_HOUSE_RIVAL_BEDROOM
	return

Route101_Movement_BirchApproachPlayer:
	walk_right
	step_end

Route101_Text_HelpMe:
	.string "A-aiuto!$"

Route101_Text_PleaseHelp:
	.string "Ehi! Tu, laggiù!\n"
	.string "Presto! Aiutami!\p"
	.string "Nella mia BORSA!\n"
	.string "C'è una POKé BALL!$"

Route101_Text_DontLeaveMe:
	.string "D-Dove stai andando?!\n"
	.string "Non lasciarmi qui così!$"

Route101_Text_YouSavedMe:
	.string "PROF. LAURO: Fiuu…\p"
	.string "Studiavo i POKéMON selvatici nell'erba\n"
	.string "alta, quando mi hanno assalito.\p"
	.string "Mi hai salvato.\n"
	.string "Grazie davvero!\p"
	.string "Oh?\p"
	.string "Ma tu sei {PLAYER}{KUN}!\p"
	.string "Non è il posto adatto per parlare. Vieni\n"
	.string "più tardi al mio LABORATORIO, d'accordo?$"

Route101_Text_TakeTiredPokemonToPokeCenter:
	.string "Proseguendo arriverai a\n"
	.string "PORTA PRETORIA,\l"
	.string "l'ingresso di ALBÈRA STORICA.$"

Route101_Text_AmbientHiker:
	.string "Da queste parti il terreno resta umido\n"
	.string "anche quando il sole picchia.$"

Route101_Text_WildPokemonInTallGrass:
	.string "Nell'erba alta, i POKéMON selvatici\n"
	.string "possono saltarti addosso.\p"
	.string "Se vuoi catturarli, devi entrare\n"
	.string "nell'erba alta e cercarli.$"

Route101_Text_RouteSign:
	.string "VIA VERDI\n"
	.string "{UP_ARROW} PORTA PRETORIA$"

Route101_Text_AnticaVillaSign:
	.string "RESTI ROMANI\n"
	.string "Antica Villa dei Cavallacci$"

Route101_Text_LiaResearchReminder:
	.string "LIA: Seguiamo il percorso e annotiamo\n"
	.string "solo cio' che osserviamo.$"

Route101_Text_NicoResearchReminder:
	.string "NICO: Va bene, niente sfide.\n"
	.string "Tengo gli occhi aperti.$"

Route101_Text_LiaAlmostDrySource:
	.string "LIA: Il canale e' quasi asciutto,\n"
	.string "ma il terreno e' ancora umido.\p"
	.string "Non sembra una semplice\n"
	.string "mancanza d'acqua in superficie.$"

Route101_Text_NicoAtSource:
	.string "NICO: Niente sfida, eh?\n"
	.string "Va bene: guardiamo cosa succede.$"

Route101_Text_LiaPokemonBehavior:
	.string "LIA: Qui i POKéMON passano oltre\n"
	.string "o evitano il tratto.\p"
	.string "E' un'osservazione, non una\n"
	.string "spiegazione. Segniamola.$"

Route101_Text_LiaAncientCanal:
	.string "LIA: Queste pietre seguono una linea.\n"
	.string "Sembrano parte di una vecchia\n"
	.string "canalizzazione.\p"
	.string "E' diretta verso\n"
	.string "PORTA PRETORIA.$"

Route101_Text_WaterFlowStops:
	.string "Un suono profondo attraversa\n"
	.string "il canale.\p"
	.string "Per un attimo, il filo d'acqua\n"
	.string "si ferma.$"

Route101_Text_WaterFlowReturns:
	.string "Poi l'acqua riprende\n"
	.string "a scorrere.$"
