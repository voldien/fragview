CMAKE_MINIMUM_REQUIRED( VERSION 3.0 )
PROJECT(fragview)
OPTION(FRAGVIEW_BUILD_WITH_SANITY_CHECK "" ON)

#OPTION(BUILD_WITH_SDL_WINDOW "Use SDL for window mangement." ON)

# # Find packages.
# FIND_PACKAGE(PkgConfig)
# FIND_PROGRAM(OPENSSL openssl)
# IF(PKG_CONFIG_FOUND)
# 	IF(BUILD_WITH_SDL_WINDOW)
# 		PKG_CHECK_MODULES(SDL2 REQUIRED sdl2)
# 	ENDIF()
# ELSE()
# 	MESSAGE(WARNING "Could not find pgk-config, attempting default values.")
# 	#TODO default values for SDL2
# ENDIF()

# IF(SDL2_FOUND)
# 	MESSAGE(STATUS "SDL2 found: ${SDL2_LIBRARIES} -- ${SDL2_VERSION}")
# ELSE()

# ENDIF()

#TODO add for fragengine and fragcore!
#PKG_CHECK_MODULES(SDL2 REQUIRED sdl2)

#
FILE(GLOB SAMPLE_GLSL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/samples/glsl/* )
FILE(GLOB SAMPLE_HLSL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/samples/hlsl/* )
FILE(GLOB SAMPLE_SPIRV_FILES ${CMAKE_CURRENT_SOURCE_DIR}/samples/spirv/* )
FILE(GLOB SAMPLE_CLC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/samples/clc/* )


# Source files.
FILE(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
FILE(GLOB HEADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

# Create default shader zip file.
SET( SHADERZIP "fragview-internal-asset" )
SET( SHADERZIPFILE "${SHADERZIP}.zip" )


#
ADD_EXECUTABLE(fragview ${SOURCE_FILES} ${HEADER_FILES})
TARGET_LINK_LIBRARIES(fragview fragcore fragengine)
TARGET_INCLUDE_DIRECTORIES(fragview PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/ )
TARGET_COMPILE_DEFINITIONS(fragview PRIVATE FRAGVIEW_INTERNAL_ASSET_FILENAME="${SHADERZIPFILE}")
ADD_DEPENDENCIES(fragview fragcore fragengine)

FILE(GLOB SAMPLE_SHADER_GLSL_FILES ${PROJECT_SOURCE_DIR}/shaders/glsl/* )
FILE(GLOB SAMPLE_SHADER_SPIRV_FILES ${PROJECT_SOURCE_DIR}/shaders/spriv/* )


# Install binary program
IF(BUILD_WITH_INSTALL)
	INSTALL(TARGETS fragview DESTINATION bin COMPONENT ${BINFRAGVIEW_COMPONENT})

	IF(UNIX)
		Find_PACKAGE(UnixCommands REQUIRED)

		# Generate compressed man page.
		ADD_CUSTOM_TARGET(mangzip
				COMMAND ${GZIP} -k -c ${CMAKE_CURRENT_SOURCE_DIR}/fragview.1 > ${CMAKE_CURRENT_SOURCE_DIR}/fragview.1.gz
				COMMENT "Compressing man page.")
		ADD_DEPENDENCIES(fragview mangzip)
		INSTALL( FILES ${CMAKE_CURRENT_SOURCE_DIR}/fragview.1.gz DESTINATION /usr/share/man/man1/)

		#TOOD add spriv generating file.

		IF(ZIP)
			# TODO add addition for adding support for spirv and combine multiple files
			#get_filename_component(RELATIVE_SHADER_SOURCE "${PROJECT_SOURCE_DIR}/shaders" DIRECTORY BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
			#FIXME add for relative path.
			SET(RELATIVE_SHADER_SOURCE "shaders")
			ADD_CUSTOM_TARGET(	internal-shader-generation
					COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/shaders
					COMMAND ${CP} -r ${PROJECT_SOURCE_DIR}/shaders/* ${CMAKE_CURRENT_BINARY_DIR}/shaders/
					COMMAND zip -r ${SHADERZIPFILE} ${RELATIVE_SHADER_SOURCE}/
					COMMAND ${RM} -r ${CMAKE_CURRENT_BINARY_DIR}/shaders
					COMMENT "")
			ADD_DEPENDENCIES(fragview internal-shader-generation)
			INSTALL( FILES ${SHADERZIPFILE} DESTINATION /usr/share/fragview )
		ELSE()

		ENDIF()

		# Install scripts
		INSTALL( FILES ${CMAKE_CURRENT_SOURCE_DIR}/scripts/newshad.sh DESTINATION /usr/share/fragview/ )
		ADD_DEFINITIONS(-DICON_LOCATION="/usr/share/fragview/")

		INSTALL( FILES ${CMAKE_CURRENT_SOURCE_DIR}/fragview.png DESTINATION /usr/share/fragview/ )
		INSTALL( FILES ${CMAKE_CURRENT_SOURCE_DIR}/fragview.desktop DESTINATION /usr/share/fragview/ )

	ENDIF()

	# Install samples.
	#TODO resolve install path
	# INSTALL( FILES ${SAMPLE_GLSL_FILES} DESTINATION /usr/share/fragview/samples/glsl COMPONENT ${BINFRAGVIEW_COMPONENT})
	# INSTALL( FILES ${SAMPLE_HLSL_FILES} DESTINATION /usr/share/fragview/samples/hlsl COMPONENT ${BINFRAGVIEW_COMPONENT})
	# INSTALL( FILES ${SAMPLE_SPIRV_FILES} DESTINATION /usr/share/fragview/samples/spriv COMPONENT ${BINFRAGVIEW_COMPONENT})
	# INSTALL( FILES ${SAMPLE_CLC_FILES} DESTINATION /usr/share/fragview/samples/clc COMPONENT ${BINFRAGVIEW_COMPONENT})
ENDIF()


#SET(FRAGVIEW_BIN ${CMAKE_CURRENT_SOURCE_DIR}/../client/fragview)

IF(BUILD_WITH_TEST)
	ENABLE_TESTING()
	IF (UNIX)
		#TODO improve with arguments for more flexiable testing.
		ADD_TEST(NAME assert-samples
				COMMAND ${BASH} sample-testing.sh ${CMAKE_CURRENT_SOURCE_DIR}/samples/ $<TARGET_FILE:fragview>
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	ENDIF ()

	FIND_PROGRAM(MEMORYCHECK_COMMAND valgrind)
	IF (MEMORYCHECK_COMMAND)
		MESSAGE(STATUS "valgrind found: ${MEMORYCHECK_COMMAND}")
		SET(MEMORYCHECK_COMMAND_OPTIONS "--tool=memcheck --xml=yes --xml-file=/tmp/valgrind --gen-suppressions=all --leak-check=full --leak-resolution=med --track-origins=yes --vgdb=no")
		SET(MEMORYCHECK_SUPPRESSIONS_FILE "${PROJECT_SOURCE_DIR}/valgrind_suppress.txt")
		SET(MEMORYCHECK_FULL_COMMAND "${MEMORYCHECK_COMMAND} ${MEMORYCHECK_COMMAND_OPTIONS}")

		ADD_TEST(NAME memory-leak-default
				COMMAND ${MEMORYCHECK_FULL_COMMAND} ${FRAGVIEW_BIN} --version)
	ENDIF ()
ENDIF(BUILD_WITH_TEST)

IF(OPENSSL AND FRAGVIEW_BUILD_WITH_SANITY_CHECK)
	# Generate verification finger print.
	MESSAGE(STATUS "openssl found: ${OPENSSL}")
	# ADD_CUSTOM_TARGET(hash-resources
	# 		COMMENT ".")
	# FOREACH(glslFile ${SAMPLE_SHADER_GLSL_FILES})
	# 	ADD_CUSTOM_COMMAND(TARGET hash-resources
	# 			COMMAND ${OPENSSL} dgst -md5 ${glslFile})
	# ENDFOREACH()
	# ADD_DEPENDENCIES(fragview hash-resources)
ENDIF()