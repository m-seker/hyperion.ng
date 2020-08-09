# Define the current source locations
SET(CURRENT_HEADER_DIR ${CMAKE_SOURCE_DIR}/include/grabber)
SET(CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/libsrc/grabber/obs)

find_package(OBS REQUIRED)

include_directories(
        ${OBS_INCLUDE_DIR}
)

FILE (GLOB OBS_GRAB_SOURCES
	"${CURRENT_HEADER_DIR}/Obs*.h"
	"${CURRENT_SOURCE_DIR}/*.h"
	"${CURRENT_SOURCE_DIR}/*.c"
	"${CURRENT_SOURCE_DIR}/*.cpp")

add_library(obs-grabber ${OBS_GRAB_SOURCES})

target_link_libraries(obs-grabber
	hyperion
	${OBS_LIBRARY}
	${QT_LIBRARIES})

