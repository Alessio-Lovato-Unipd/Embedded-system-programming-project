cmake_minimum_required( VERSION 3.25.0 )

project( Assegnamento_PSE_2022_2023_Lovato_Alessio
	 VERSION 0.1
	 LANGUAGES CXX
	 DESCRIPTION "Algoritmo concorrenziale per la gestione della pianificzione del moto di più robot olonomi contemporaneamente"
	)

##########################################################################
##########################################################################

find_package(Gnuplot 5.2.8 REQUIRED)

#Aggiunta sottocartelle dei test
set(NOME_PROGRAMMA "robot_simultanei")
add_executable(${NOME_PROGRAMMA} robot_simultanei.cpp)
target_link_libraries(${NOME_PROGRAMMA} PRIVATE 
										mappa
										robot
										gnuplot
										monitor)
										
add_executable(lettura_satelliti lettura_satelliti.cpp)
target_link_libraries(lettura_satelliti PRIVATE 
										mappa
										robot
										gnuplot
										monitor)

#porto il file di test anche nella cartella dove genero l'eseguibile
add_custom_command(TARGET ${NOME_PROGRAMMA} POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
		"${CMAKE_CURRENT_SOURCE_DIR}/obbiettivi_1.txt"   # <--this is in-file
        $<TARGET_FILE_DIR:${NOME_PROGRAMMA}>)                 # <--this is out-file path

#porto il file di test anche nella cartella dove genero l'eseguibile
add_custom_command(TARGET ${NOME_PROGRAMMA} POST_BUILD        # Adds a post-build event to MyTest
COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
	"${CMAKE_CURRENT_SOURCE_DIR}/obbiettivi_2.txt"   # <--this is in-file
	$<TARGET_FILE_DIR:${NOME_PROGRAMMA}>)                 # <--this is out-file path


#porto il file di test anche nella cartella dove genero l'eseguibile
add_custom_command(TARGET ${NOME_PROGRAMMA} POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
		"${CARTELLA_TEST}/ostacoli.csv"   # <--this is in-file
        $<TARGET_FILE_DIR:${NOME_PROGRAMMA}>)                 # <--this is out-file path

