cmake_minimum_required( VERSION 2.8 )
project( Chipulator )

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/;${CMAKE_MODULE_PATH}")

find_package(SFML 2 REQUIRED COMPONENTS graphics window system)
if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
else()
	set(SFML_ROOT "" CACHE PATH "SFML top-level directory")
	message("\n-> SFML directory not found. Set SFML_ROOT to SFML's top-level path (containing \"include\" and \"lib\" directories).")
endif()

set(SRC_DIR "src")

set(
	SOURCES
	${SRC_DIR}/main.cpp
	${SRC_DIR}/Chipulator/Chipulator.cpp
)

include_directories( ${SRC_DIR} )

add_executable( ${CMAKE_PROJECT_NAME} ${LIB_TYPE} ${SOURCES} )
target_link_libraries(${CMAKE_PROJECT_NAME} ${SFML_LIBRARIES})
install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/run)
