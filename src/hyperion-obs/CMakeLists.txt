cmake_minimum_required(VERSION 3.0.0)
project(hyperion-obs)

find_package(OBS REQUIRED)
find_package(Qt5Widgets REQUIRED)

include_directories(
	${OBS_INCLUDE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../libsrc/flatbufserver
	${FLATBUFFERS_INCLUDE_DIRS}
)

qt5_wrap_ui(UI_HEADERS HyperionProperties.ui)
message(STATUS "MURSE : ${UI_HEADERS}")

set(Hyperion-obs_HEADERS
	ObsWrapper.h
	HyperionProperties.h
	${UI_HEADERS})

set(Hyperion-obs_SOURCES
	ObsWrapper.cpp
	HyperionProperties.cpp
	hyperion-obs.cpp
	hyperion-obs-module.cpp)


# generate windows .rc file for this binary
if (WIN32)
	include(${CMAKE_SOURCE_DIR}/cmake/win/win_rc.cmake)
	generate_win_rc_file(${PROJECT_NAME})

	set(hyperion-obs_PLATFORM_DEPS
		ws2_32
		w32-pthreads
		winmm
		Iphlpapi)

	set(MODULE_DESCRIPTION "Hyperion OBS output module")
	configure_file(${CMAKE_SOURCE_DIR}/cmake/winrc/obs-module.rc.in hyperion-obs.rc)
	list(APPEND hyperion-obs_SOURCES
		hyperion-obs.rc)
endif()

add_library(${PROJECT_NAME} MODULE
	${Hyperion-obs_HEADERS}
	${Hyperion-obs_SOURCES}
	${${PROJECT_NAME}_WIN_RC_PATH}
)

if(APPLE)
	find_library(FOUNDATION_FRAMEWORK Foundation)
	find_library(SECURITY_FRAMEWORK Security)

	set(hyperion-obs_PLATFORM_DEPS
		${hyperion-obs_PLATFORM_DEPS}
		${FOUNDATION_FRAMEWORK}
		${SECURITY_FRAMEWORK})
endif()

target_link_libraries(${PROJECT_NAME}
	commandline
	obs-grabber
	flatbufserver
	flatbuffers
	ssdp
	Qt5::Core
	Qt5::Network
	Qt5::Widgets
	${OBS_LIBRARY}
	${hyperion-obs_PLATFORM_DEPS}
)

if(NOT WIN32)
	install ( TARGETS ${PROJECT_NAME} DESTINATION "share/hyperion/bin/plugins" COMPONENT "hyperion_obs" )
else()
	install ( TARGETS ${PROJECT_NAME} DESTINATION "bin" COMPONENT "hyperion_obs" )
endif()

if(CMAKE_HOST_UNIX)
	install(CODE "EXECUTE_PROCESS(COMMAND ln -sf \"../share/hyperion/bin/plugins/${PROJECT_NAME}\" \"${CMAKE_BINARY_DIR}/plugins/symlink_${PROJECT_NAME}\" )" COMPONENT  "hyperion_obs" )
	install(FILES "${CMAKE_BINARY_DIR}/plugins/symlink_${PROJECT_NAME}" DESTINATION "bin/plugins/" RENAME "${PROJECT_NAME}" COMPONENT "hyperion_obs" )
	install(CODE "FILE (REMOVE ${CMAKE_BINARY_DIR}/plugins/symlink_${PROJECT_NAME} )" COMPONENT  "hyperion_obs" )
endif(CMAKE_HOST_UNIX)

