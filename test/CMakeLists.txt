cmake_minimum_required( VERSION 3.25.0 )

project( Assegnamento_PSE_2022_2023_Lovato_Alessio
	 VERSION 0.1
	 LANGUAGES CXX
	 DESCRIPTION "Algoritmo concorrenziale per la gestione della pianificzione del moto di più robot olonomi contemporaneamente"
	)

##########################################################################
##########################################################################

#Aggiunta sottocartelle dei test

add_executable(creazione test_creazione_mappa.cpp)
target_link_libraries(creazione PRIVATE mappa)





