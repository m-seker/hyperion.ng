find_path (
	GMOCK_INCLUDE_DIR
	NAMES gmock.h
	DOC   "Include directory for Google Mock."
)

find_library (
	GMOCK_LIBRARY
	NAMES gmock
	DOC   "Link library for Google Mock (gmock)."
   )

message(STATUS "MURSE ${GMOCK_INCLUDE_DIR}")

mark_as_advanced (GMOCK_INCLUDE_DIR)
mark_as_advanced (GMOCK_LIBRARY)

# ----------------------------------------------------------------------------
# prerequisite libraries
set (GMOCK_INCLUDES  "${GMOCK_INCLUDE_DIR}")
set (GMOCK_LIBRARIES "${GMOCK_LIBRARY}")

# ----------------------------------------------------------------------------
# handle the QUIETLY and REQUIRED arguments and set *_FOUND to TRUE
# if all listed variables are found or TRUE
include (FindPackageHandleStandardArgs)

find_package_handle_standard_args (
	GMOCK
	REQUIRED_VARS
	GMOCK_INCLUDE_DIR
	GMOCK_LIBRARY
)

set (GMOCK_FOUND "${GMOCK_FOUND}")
