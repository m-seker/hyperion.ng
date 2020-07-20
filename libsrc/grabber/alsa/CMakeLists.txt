# Define the current source locations
SET(CURRENT_HEADER_DIR ${CMAKE_SOURCE_DIR}/include/grabber)
SET(CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/libsrc/grabber/alsa)

find_package(ALSA REQUIRED)

include_directories(${ALSA_INCLUDES})

FILE(GLOB ALSA_SOURCES "${CURRENT_HEADER_DIR}/Alsa*.h"  "${CURRENT_SOURCE_DIR}/*.h"  "${CURRENT_SOURCE_DIR}/*.cpp")

add_library(alsa-grabber ${ALSA_SOURCES})

target_link_libraries(alsa-grabber
	hyperion
	${ALSA_LIBRARIES}
)
