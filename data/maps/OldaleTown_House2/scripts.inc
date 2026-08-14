OldaleTown_House2_MapScripts::
	.byte 0

OldaleTown_House2_EventScript_Woman::
	msgbox OldaleTown_House2_Text_PokemonLevelUp, MSGBOX_NPC
	end

OldaleTown_House2_EventScript_Man::
	msgbox OldaleTown_House2_Text_YoullGoFurtherWithStrongPokemon, MSGBOX_NPC
	end

OldaleTown_House2_Text_PokemonLevelUp:
	.string "Le pietre di Porta Pretoria conservano\n"
	.string "ancora tracce delle antiche opere\n"
	.string "romane.$"

OldaleTown_House2_Text_YoullGoFurtherWithStrongPokemon:
	.string "Da qui si passa verso Albèra Storica.\p"
	.string "Là sembra che ogni strada abbia una\n"
	.string "storia da raccontare.$"
