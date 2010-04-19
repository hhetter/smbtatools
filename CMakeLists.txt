#
#
#
#


# Check the minimum required cmake version for this project
cmake_minimum_required(VERSION 2.6)
PROJECT(smbtatools C)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")
set(APPLICATION_NAME "smbtatools")
set(APPLICATION_VERSION "0.0.1")

# Version setting for the cpack package creator
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_PACKAGE_FILE_NAME 
 "${APPLICATION_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_SOURCE_IGNORE_FILES ".git/;CMakeFiles/")
include(CPack)
#include(CPackConfig.cmake)

# Build debugging enabled binaries
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_C_FLAGS_DEBUG "-g -Wall")
#SET(CMAKE_VERBOSE_MAKEFILE ON)

# Installation prefix /usr
SET(CMAKE_INSTALL_PREFIX /usr)
SET(Libraries
iniparser
ncurses
talloc
# fixme! this needs a check
smbclient
)
SET(PLATFORM_LINKFLAGS "-pthread")

#
# check for libsmbclient
#
include(FindLibsmbclient.cmake)
FIND_PACKAGE(Libsmbclient)
if (NOT LIBSMBCLIENT_INCLUDE_DIRS)
        MESSAGE( FATAL_ERROR "ERROR: you will need libsmbclient
                 development files installed.")
ENDIF()

#
# check for talloc
#
include(FindTalloc.cmake)
FIND_PACKAGE(Talloc)
if (NOT TALLOC_INCLUDE_DIR)
	MESSAGE( FATAL_ERROR "ERROR: you will need talloc development
		 files installed.")
ENDIF()

#
# include iniparser on demand
#
include(FindIniparser.cmake)
FIND_PACKAGE(Iniparser)
if (NOT INIPARSER_INCLUDE_DIR)
	MESSAGE( STATUS "----> FYI: Unable to find iniparser libraries on your system")
	MESSAGE( STATUS "---->      compiling my own version and link it statically.")
	add_custom_target(
        	buildiniparser ALL
        	COMMAND "make"
        	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/iniparser3.0b"
        	COMMENT "Building iniparser."
        	VERBATIM )
	ADD_LIBRARY(iniparser STATIC IMPORTED)
	set_target_properties(iniparser PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/iniparser3.0b/libiniparser.a)
ENDIF()

#
# check for curses
#
SET(CURSES_NEED_NCURSES TRUE)
FIND_PACKAGE(Curses)
if (NOT CURSES_HAVE_NCURSES_H)
	MESSAGE( FATAL_ERROR "ERROR: you will need curses development files installed.")
ENDIF()

SET(CurrentExe "smbtaquery")
ADD_EXECUTABLE(${CurrentExe} 
	src/query/main.c
	src/common/common.c
	src/common/aes.c
	src/common/rijndael-alg-fst.c
	src/query/configuration.c
	src/query/interpreter.c
	)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries} ${INIPARSER_LIBRARIES} ${TALLOC_LIBRARIES})

SET(CurrentExe "smbtamonitor")
ADD_EXECUTABLE(${CurrentExe}
	src/monitor/main.c
	src/common/common.c
	src/common/aes.c
	src/common/rijndael-alg-fst.c
	)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries} ${CURSES_LIBRARIES} ${INIPARSER_LIBRARIES} ${TALLOC_LIBRARIES})

SET(CurrentExe "smbtatorture")
ADD_EXECUTABLE(${CurrentExe}
	src/torture/smbtatorture.c
	)

TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})
# install documentation
INSTALL(FILES doc/smbtaquery.1
        DESTINATION "share/man/man1"
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ RENAME smbtaquery.1)
INSTALL(FILES doc/smbtamonitor.1
        DESTINATION "share/man/man1"
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ RENAME smbtamonitor.1)
INSTALL(FILES doc/smbtatorture.1
	DESTINATION "share/man/man1"
	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ RENAME smbtatorture.1)

INSTALL(TARGETS smbtaquery smbtamonitor smbtatorture DESTINATION bin)

