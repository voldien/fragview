
FIND_PACKAGE(GTest REQUIRED)
IF(GTEST_FOUND)
	MESSAGE(STATUS "Gtest found: ${GTEST_BOTH_LIBRARIES}")
ELSE()
	MESSAGE(ERROR "Gtest is required in order to compile and run the tests")
ENDIF()

# Source files.
FILE(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
FILE(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

get_target_property(FRAGVIEW_INCLUDES fragview INCLUDE_DIRECTORIES)

ADD_EXECUTABLE(fragview-assert ${SOURCE_FILES} ${HEADER_FILES})
TARGET_LINK_LIBRARIES(fragview-assert ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} fragcore fragengine pthread )
TARGET_INCLUDE_DIRECTORIES(fragview-assert PRIVATE ${GTEST_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include/ ${FRAG_CORE_INCLUDE} ${FRAGVIEW_INCLUDES})
ADD_DEPENDENCIES(fragview-assert fragview)

ADD_TEST(NAME assert-functionality
		COMMAND fragview-assert
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

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
