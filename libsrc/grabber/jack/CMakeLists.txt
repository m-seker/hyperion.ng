# Define the current source locations
SET(CURRENT_HEADER_DIR ${CMAKE_SOURCE_DIR}/include/grabber)
SET(CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/libsrc/grabber/jack)

find_package(SoundIO REQUIRED)

include_directories(${SOUNDIO_INCLUDE_DIR})

FILE(GLOB JACK_SOURCES "${CURRENT_HEADER_DIR}/Jack*.h"  "${CURRENT_SOURCE_DIR}/*.h"  "${CURRENT_SOURCE_DIR}/*.cpp")

add_library(jack-grabber ${JACK_SOURCES})

target_link_libraries(jack-grabber
	hyperion
	${SOUNDIO_LIBRARY}
)
