# Needed for testing non-public components
include_directories(../libsrc)

enable_testing()

find_package(Qt5Test REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(GTest REQUIRED)
find_package(GMock REQUIRED)

list(APPEND TEST_SOURCES
	TestConfigFile.cpp
	TestRgbImage.cpp
	TestBlackBorderDetector.cpp
	TestBlackBorderProcessor.cpp
	TestQtScreenshot.cpp
	TestQRegExp.cpp
	TestImage2LedsMap.cpp
	TestRunner.cpp
	TestQtScreenshot.cpp
)
list(APPEND TEST_LIBRARIES
	blackborder
	leddevice
	jsonserver
	hyperion-utils
	hyperion
	effectengine
	Qt5::Widgets
)

if(ENABLE_SPIDEV)
	list(APPEND TEST_SOURCES
		TestSpi.cpp
		spidev_test.c
		switchPinCtrl.c
	)

	list(APPEND TEST_LIBRARIES
		leddevice
		hyperion-utils
		hyperion
	)
endif(ENABLE_SPIDEV)

if(ENABLE_X11)
	find_package(X11 REQUIRED)

	list(APPEND TEST_SOURCES
		TestX11Performance.cpp
	)

	list(APPEND TEST_LIBRARIES
		 ${X11_LIBRARIES}
	)
endif(ENABLE_X11)

 if (ENABLE_DISPMANX)
#	add_subdirectory(dispmanx2png)
 endif (ENABLE_DISPMANX)

include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${GMOCK_INCLUDE_DIRS})
add_executable(hyperion-test ${TEST_SOURCES})

target_link_libraries(hyperion-test
	${TEST_LIBRARIES}
	${GTEST_BOTH_LIBRARIES}
	${GMOCK_LIBRARY}
)

add_test(NAME    test
	 COMMAND hyperion-test
)
