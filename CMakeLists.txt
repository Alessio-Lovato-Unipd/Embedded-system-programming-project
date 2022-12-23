cmake_minimum_required( VERSION 3.25.0 )

project( Assegnamento_PSE_2022_2023_Lovato_Alessio
	 VERSION 0.1
	 LANGUAGES CXX
	 DESCRIPTION "Algoritmo concorrenziale per la gestione della pianificzione del moto di più robot olonomi contemporaneamente"
	)

##########################################################################
##########################################################################

#impostazione dell tipo di compilazione da eseguire (se stable o test)
set(TARGET_GROUP stable CACHE STRING "Gruppo di sorgenti da compilare")

##########################################################################
##########################################################################


#ricerca pacchetti necessari
#nessun pacchetto necessario

#impostazione verione di c++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Aggiunta opzioni di compilazione comuni
if (MSVC)
	# warning level 4 and all warnings as errors
	add_compile_options(/W4 /WX)
else()
	# lots of warnings and all warnings as errors
	add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# Parametri specifici di compilazione
if(TARGET_GROUP STREQUAL stable)
	set(CMAKE_BUILD_TYPE Release)
	message(STATUS "Setting build type to '${CMAKE_BUILD_TYPE}'.")
	add_subdirectory(app)

elseif(TARGET_GROUP STREQUAL test)
	set(CMAKE_BUILD_TYPE Debug)
	message(STATUS "Setting build type to '${CMAKE_BUILD_TYPE}'.")
	include(CTest)
	add_subdirectory(test)
	add_subdirectory(external/Unity)  # <-- Unity non è inserito dal CMakeLists della sottocartella external

else()
	message(FATAL_ERROR "TARGET_GROUP sconosciuto")
endif() 

# Aggiunta sottocartelle delle librerie
add_subdirectory(lib)
add_subdirectory(external)

