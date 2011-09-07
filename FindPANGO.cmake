# - Try to find Pango
# Once done, this will define
#
#  Pango_FOUND - system has Pango
#  Pango_INCLUDE_DIRS - the Pango include directories
#  Pango_LIBRARIES - link these to use Pango

find_package(PkgConfig)
pkg_check_modules(PC_LIBPANGO QUIET pango)
set(LIBPANGO_DEFINITIONS ${PC_LIBPANGO_CFLAGS_OTHER})
find_path(LIBPANGO_INCLUDE_DIR pango.h
	HINTS ${PC_LIBPANGO_INCLUDE_DIR} ${PC_LIBPANGO_INCLUDE_DIRS}
		        PATH_SUFFIXES pango )
find_library(LIBPANGO_LIBRARY NAMES pango libpango libpango-1.0 pango-1.0 
	HINTS ${PC_LIBPANGO_LIBDIR} ${PC_LIBPANGO_LIBRARY_DIRS})
set(LIBPANGO_LIBRARIES ${LIBPANGO_LIBRARY} )
set(LIBPANGO_INCLUDE_DIRS ${LIBPANGO_INCLUDE_DIR})
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRES arguments and set LIBDBI_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(libpango DEFAULT_MSG
	LIBPANGO_LIBRARY LIBPANGO_INCLUDE_DIR)
mark_as_advanced(LIBPANGO_INCLUDE_DIR LIBPANGO_LIBRARY)

