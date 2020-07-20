# Define the current source locations
SET(CURRENT_HEADER_DIR ${CMAKE_SOURCE_DIR}/include/grabber)
SET(CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/libsrc/grabber/pulse)

find_package(Pulse REQUIRED)

include_directories(${PULSE_INCLUDE_DIR})

FILE(GLOB PULSE_SOURCES "${CURRENT_HEADER_DIR}/Pulse*.h"  "${CURRENT_SOURCE_DIR}/*.h"  "${CURRENT_SOURCE_DIR}/*.cpp")

add_library(pulse-grabber ${PULSE_SOURCES})

target_link_libraries(pulse-grabber
	hyperion
	${PULSE_LIBRARY}
)
