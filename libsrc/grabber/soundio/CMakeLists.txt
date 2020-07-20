# Define the current source locations
SET(CURRENT_HEADER_DIR ${CMAKE_SOURCE_DIR}/include/grabber)
SET(CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/libsrc/grabber/soundio)

find_package(SoundIO REQUIRED)

include_directories(${SOUNDIO_INCLUDE_DIR})

FILE(GLOB SOUNDIO_SOURCES "${CURRENT_HEADER_DIR}/SoundIO*.h"  "${CURRENT_SOURCE_DIR}/*.h"  "${CURRENT_SOURCE_DIR}/*.cpp")

add_library(soundio-grabber ${SOUNDIO_SOURCES})

target_link_libraries(soundio-grabber
	hyperion
	${SOUNDIO_LIBRARY}
)
