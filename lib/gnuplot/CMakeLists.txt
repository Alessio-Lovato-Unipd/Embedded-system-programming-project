cmake_minimum_required( VERSION 3.25.0 )

project( Assegnamento_PSE_2022_2023_Lovato_Alessio
	 VERSION 0.1
	 LANGUAGES CXX
	 DESCRIPTION "Algoritmo concorrenziale per la gestione della pianificzione del moto di più robot olonomi contemporaneamente"
	)

##########################################################################
##########################################################################

add_library(gnuplot STATIC src/gnuplot.cpp)

target_include_directories(gnuplot PUBLIC include)

add_library(lib::gnuplot ALIAS gnuplot)

