cmake_minimum_required( VERSION 2.8 )
project( Chipulator )

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/;${CMAKE_MODULE_PATH}")

set(SRC_DIR "src")

set(
	SOURCES
	${SRC_DIR}/main.cpp
	${SRC_DIR}/Chipulator/Chipulator.cpp
)

include_directories( ${SRC_DIR} )

add_executable( ${CMAKE_PROJECT_NAME} ${LIB_TYPE} ${SOURCES} )
install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/run)
