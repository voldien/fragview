CMAKE_MINIMUM_REQUIRED( VERSION 3.11.0 FATAL_ERROR)
PROJECT(fragview VERSION 0.1.0 LANGUAGES C CXX)

#
OPTION(FRAGVIEW_BUILD_WITH_SANITY_CHECK "" ON)
OPTION(BUILD_WITH_COVERAGE "Enable profiling and coverage report analysis." ON)
OPTION(BUILD_WITH_PROFILER "Enable profiling." ON)
OPTION(BUILD_WITH_EXTERNAL_PROJECT "Enable external project build." ON)
OPTION(BUILD_WITH_INSTALL "." ON )
OPTION(BUILD_WITH_TEST "" ON)

# Set version of the project.
SET(VERSION_MAJOR 0)
SET(VERSION_MINOR 1)
SET(VERSION_REVISION 0)
SET(VERSION_STATE a)
SET(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}${VERSION_STATE}${VERSION_REVISION} )
SET(PROJECT_VERSION "${VERSION}")
ADD_DEFINITIONS(-DFV_VERSION="${VERSION}")

# Find packages.
FIND_PACKAGE(Git QUIET)
SET(FRAGENGINE_SUBMODULE_PATH ${PROJECT_SOURCE_DIR}/extern/repo/fragengine)
IF(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
	# Update submodules as needed
	OPTION(GIT_SUBMODULE "Check submodules during build" ON)
	IF(GIT_SUBMODULE)
		FILE(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/extern/repo/fragengine)
		MESSAGE(STATUS "Submodule update  --recursive")
		EXECUTE_PROCESS(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
						WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
						RESULT_VARIABLE GIT_SUBMOD_RESULT)
		IF(NOT GIT_SUBMOD_RESULT EQUAL "0")
			MESSAGE(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
		ENDIF()
	ENDIF()
	
	#TODO determine if can be done auto.
	#SET(FRAG_CORE_INCLUDE "${PROJECT_SOURCE_DIR}/extern/repo/fragengine/frag-core/include")
	#SET(FRAG_ENGINE_INCLUDE "${PROJECT_SOURCE_DIR}/extern/repo/fragengine/frag-engine/include")
	ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/extern/repo/fragengine EXCLUDE_FROM_ALL)
ELSE()

ENDIF()

IF(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/repo/fragengine")
	MESSAGE(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
ENDIF()

ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/client)

#
FILE(GLOB SAMPLE_GLSL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/samples/glsl/* )
FILE(GLOB SAMPLE_HLSL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/samples/hlsl/* )
FILE(GLOB SAMPLE_SPIRV_FILES ${CMAKE_CURRENT_SOURCE_DIR}/samples/spirv/* )
FILE(GLOB SAMPLE_CLC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/samples/clc/* )

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

		#TODO add spriv generating file.

		IF(ZIP)
			# TODO add addition for adding support for spirv and combine multiple files
			#get_filename_component(RELATIVE_SHADER_SOURCE "${PROJECT_SOURCE_DIR}/shaders" DIRECTORY BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
			#FIXME add for relative path.
			SET(RELATIVE_SHADER_SOURCE "shaders")
			ADD_CUSTOM_TARGET(	internal-shader-generation
					COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/shaders
					COMMAND ${CP} -r ${PROJECT_SOURCE_DIR}/shaders/* ${CMAKE_CURRENT_BINARY_DIR}/shaders/
					COMMAND zip -r ${SHADERZIPFILE} ${RELATIVE_SHADER_SOURCE}
					COMMAND ${RM} -r ${CMAKE_CURRENT_BINARY_DIR}/shaders
					COMMENT "Created Asset file")
			#ADD_DEPENDENCIES(fragview internal-shader-generation)
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

IF(BUILD_WITH_TEST)
	ENABLE_TESTING()
	ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/test)
ENDIF(BUILD_WITH_TEST)