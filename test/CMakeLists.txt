cmake_minimum_required( VERSION 3.25.0 )

project( Assegnamento_PSE_2022_2023_Lovato_Alessio
	 VERSION 0.1
	 LANGUAGES CXX
	 DESCRIPTION "Algoritmo concorrenziale per la gestione della pianificzione del moto di più robot olonomi contemporaneamente"
	)

##########################################################################
##########################################################################

#Aggiunta sottocartelle dei test

set(CARTELLA_TEST ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(generazione_mappa)
add_subdirectory(test_robot)
add_subdirectory(movimento_robot_simultaneo)




