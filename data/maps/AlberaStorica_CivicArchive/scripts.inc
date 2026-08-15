AlberaStorica_CivicArchive_MapScripts::
	.byte 0

AlberaStorica_CivicArchive_EventScript_Lia::
	lock
	faceplayer
	msgbox AlberaStorica_CivicArchive_Text_Lia, MSGBOX_DEFAULT
	release
	end

AlberaStorica_CivicArchive_EventScript_Archivist::
	msgbox AlberaStorica_CivicArchive_Text_Archivist, MSGBOX_NPC
	end

AlberaStorica_CivicArchive_EventScript_Scholar::
	msgbox AlberaStorica_CivicArchive_Text_Scholar, MSGBOX_NPC
	end

AlberaStorica_CivicArchive_EventScript_Resident::
	msgbox AlberaStorica_CivicArchive_Text_Resident, MSGBOX_NPC
	end

AlberaStorica_CivicArchive_EventScript_AmphitheatreRegister::
	msgbox AlberaStorica_CivicArchive_Text_AmphitheatreRegister, MSGBOX_SIGN
	end

AlberaStorica_CivicArchive_EventScript_TerritoryMap::
	msgbox AlberaStorica_CivicArchive_Text_TerritoryMap, MSGBOX_SIGN
	end

AlberaStorica_CivicArchive_EventScript_WaterRegister::
	msgbox AlberaStorica_CivicArchive_Text_WaterRegister, MSGBOX_SIGN
	end

AlberaStorica_CivicArchive_Text_Lia:
	.string "Eccoti.\p"
	.string "Ho dato un'occhiata ai registri dell'Archivio.\n"
	.string "Ci sono informazioni sulle acque e sulle\l"
	.string "vecchie opere costruite intorno ad Albèra.\p"
	.string "Alcuni dati recenti non coincidono affatto\n"
	.string "con quelli del passato.\p"
	.string "Credo di aver capito dove conviene fare\n"
	.string "i prossimi rilevamenti.\p"
	.string "Però prima pensa alla Palestra. Dopo Lirio,\n"
	.string "ci vediamo sulla strada che prosegue\l"
	.string "da Albèra. Ti spiego tutto lì.$"

AlberaStorica_CivicArchive_Text_Archivist:
	.string "Benvenuto nell'Archivio Civico di Albèra.\p"
	.string "Conserviamo registri, mappe e documenti\n"
	.string "sulla città e sul territorio circostante.\p"
	.string "Ultimamente arrivano molte richieste sui\n"
	.string "vecchi registri delle acque. Pare che qualcuno\l"
	.string "voglia capire perché certi valori sono cambiati.$"

AlberaStorica_CivicArchive_Text_Scholar:
	.string "Sto confrontando vecchie carte con quelle\n"
	.string "moderne.\p"
	.string "Strade, nomi e confini cambiano col tempo.\n"
	.string "Due documenti possono parlare dello stesso\l"
	.string "luogo e sembrare completamente diversi.$"

AlberaStorica_CivicArchive_Text_Resident:
	.string "Sono venuto a cercare documenti sulla casa\n"
	.string "dei miei nonni.\p"
	.string "Pensavo di metterci cinque minuti.\n"
	.string "Sono qui da tutta la mattina.$"

AlberaStorica_CivicArchive_Text_AmphitheatreRegister:
	.string "Un registro storico dell'Anfiteatro di Albèra.\p"
	.string "L'edificio è stato restaurato più volte\n"
	.string "nel corso degli anni e continua ancora oggi\l"
	.string "a essere uno dei luoghi principali della città.$"

AlberaStorica_CivicArchive_Text_TerritoryMap:
	.string "Una vecchia carta del territorio intorno\n"
	.string "ad Albèra.\p"
	.string "Sono segnati percorsi, rilievi e numerosi\n"
	.string "punti legati alla raccolta e al passaggio\l"
	.string "dell'acqua.$"

AlberaStorica_CivicArchive_Text_WaterRegister:
	.string "Un registro di misurazioni raccolte nel corso\n"
	.string "degli anni.\p"
	.string "Le annotazioni più recenti mostrano valori\n"
	.string "molto più irregolari rispetto a prima.\p"
	.string "Qualcuno ha lasciato diversi segni a matita\n"
	.string "accanto alle ultime pagine.$"
