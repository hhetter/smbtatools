# - Try to find Cairographics
# Once done, this will define
#

find_package(PkgConfig)
pkg_check_modules(PC_LIBCAIRO QUIET cairo)
set(LIBCAIRO_DEFINITIONS ${PC_LIBCAIRO_CFLAGS_OTHER})
find_path(LIBCAIRO_INCLUDE_DIR cairo.h
	HINTS ${PC_LIBCAIRO_INCLUDE_DIR} ${PC_LIBCAIRO_INCLUDE_DIRS}
		        PATH_SUFFIXES cairo )
		find_library(LIBCAIRO_LIBRARY NAMES cairo libcairo libcairo-1.0 cairo-1.0 
			HINTS ${PC_LIBCAIRO_LIBDIR} ${PC_LIBCAIRO_LIBRARY_DIRS})
		set(LIBCAIRO_LIBRARIES ${LIBCAIRO_LIBRARY} )
		set(LIBCAIRO_INCLUDE_DIRS ${LIBCAIRO_INCLUDE_DIR})
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRES arguments and set LIBDBI_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(libcairo DEFAULT_MSG
	LIBCAIRO_LIBRARY LIBCAIRO_INCLUDE_DIR)
mark_as_advanced(LIBCAIRO_INCLUDE_DIR LIBCAIRO_LIBRARY)

