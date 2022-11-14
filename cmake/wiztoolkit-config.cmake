find_path(WIZTOOLKIT_INCLUDE_DIR NAMES wtk/IRTree.h)
find_library(WIZTOOLKIT_LIBRARY NAMES wiztoolkit PATH /usr/local/lib/)
find_library(STEALTH_LOGGING NAMES stealth_logging PATH /usr/local/lib/)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(wiztoolkit DEFAULT_MSG WIZTOOLKIT_INCLUDE_DIR WIZTOOLKIT_LIBRARY STEALTH_LOGGING)

if(WIZTOOLKIT_FOUND)
	set(WIZTOOLKIT_LIBRARIES ${WIZTOOLKIT_LIBRARY} ${STEALTH_LOGGING})
	set(WIZTOOLKIT_INCLUDE_DIRS ${WIZTOOLKIT_INCLUDE_DIR})
endif()