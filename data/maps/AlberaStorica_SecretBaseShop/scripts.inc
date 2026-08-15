AlberaStorica_SecretBaseShop_MapScripts::
	.byte 0

AlberaStorica_SecretBaseShop_EventScript_Shopkeeper::
	lock
	faceplayer
	msgbox AlberaStorica_SecretBaseShop_Text_Intro, MSGBOX_DEFAULT
	message gText_HowMayIServeYou
	waitmessage
	pokemartdecoration AlberaStorica_SecretBaseShop_PokemartDecor
	msgbox AlberaStorica_SecretBaseShop_Text_Repeat, MSGBOX_DEFAULT
	release
	end

	.align 2
AlberaStorica_SecretBaseShop_PokemartDecor:
	.2byte DECOR_RAGGED_CHAIR
	.2byte DECOR_RAGGED_DESK
	.2byte DECOR_RED_PLANT
	.2byte DECOR_TROPICAL_PLANT
	.2byte DECOR_PRETTY_FLOWERS
	.2byte DECOR_YELLOW_BRICK
	.2byte DECOR_RED_BRICK
	pokemartlistend

AlberaStorica_SecretBaseShop_EventScript_CustomerOne::
	msgbox AlberaStorica_SecretBaseShop_Text_CustomerOne, MSGBOX_NPC
	end

AlberaStorica_SecretBaseShop_EventScript_CustomerTwo::
	msgbox AlberaStorica_SecretBaseShop_Text_CustomerTwo, MSGBOX_NPC
	end

AlberaStorica_SecretBaseShop_Text_Intro:
	.string "Cerchi qualcosa per la tua base?\p"
	.string "Qui non trovi roba elegante, ma quello\n"
	.string "che vendo dura. Legno, piante e muratura...\p"
	.string "Per cominciare basta e avanza.$"

AlberaStorica_SecretBaseShop_Text_Repeat:
	.string "Una base cambia parecchio con poche cose\n"
	.string "messe al posto giusto.\p"
	.string "Il resto viene col tempo.$"

AlberaStorica_SecretBaseShop_Text_CustomerOne:
	.string "Continuo a spostare lo stesso tavolo\n"
	.string "da una parte all'altra.\p"
	.string "Arredare la base è più difficile\n"
	.string "che trovarla.$"

AlberaStorica_SecretBaseShop_Text_CustomerTwo:
	.string "Mi piacciono le piante di qui.\n"
	.string "Rendono anche una base scavata nella roccia\l"
	.string "un po' meno... scavata nella roccia.$"
