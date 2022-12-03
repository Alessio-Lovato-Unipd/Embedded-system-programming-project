cmake_minimum_required( VERSION 3.16.3 )

project( unity_lib
	VERSION 0.1
	LANGUAGES C
	DESCRIPTION "Unity come libreria"
	)

add_library( Unity STATIC src/unity.c )

target_include_directories( Unity
	PUBLIC src
	)

add_library( TTS::Unity ALIAS Unity )
