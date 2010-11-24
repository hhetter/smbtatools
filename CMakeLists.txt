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
INCLUDE(${CMAKE_SOURCE_DIR}/FindSqlite3.cmake)

# Build debugging enabled binaries
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_C_FLAGS_DEBUG "-g -Wall")
#SET(CMAKE_VERBOSE_MAKEFILE ON)

# Installation prefix /usr
SET(CMAKE_INSTALL_PREFIX /usr)

#
# check for sqlite3
#
include(FindSqlite3.cmake)
FIND_PACKAGE(Sqlite3)
if (NOT SQLITE3_FOUND)
	MESSAGE( FATAL_ERROR "ERROR: you'll need sqlite3 development files installed.")
ENDIF()

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
FIND_PACKAGE(Curses)
if (NOT CURSES_HAVE_CURSES_H)
	MESSAGE( FATAL_ERROR "ERROR: you will need curses development files installed.")
ENDIF()

#
# check for pthreads
#
FIND_PACKAGE(Threads)
if (NOT CMAKE_USE_PTHREADS_INIT)
	MESSAGE( FATAL_ERROR "ERROR: you will need pthread development files installed.")
ENDIF()

INCLUDE_DIRECTORIES( ${SQLITE3_INCLUDEDIR} ${TALLOC_INCLUDE_DIR} ${LIBSMBCLIENT_INCLUDE_DIRS} )

SET(CurrentExe "smbtaquery")
ADD_EXECUTABLE(${CurrentExe} 
	src/query/main.c
	src/common/common.c
	src/common/aes.c
	src/common/rijndael-alg-fst.c
	src/query/configuration.c
	src/query/interpreter.c
	src/query/network.c
	)

TARGET_LINK_LIBRARIES(${CurrentExe} ${INIPARSER_LIBRARIES} ${SQLITE3_LIBRARIES} ${TALLOC_LIBRARIES} iniparser)

SET(CurrentExe "smbtamonitor")
ADD_EXECUTABLE(${CurrentExe}
	src/monitor/main.c
	src/common/common.c
	src/common/aes.c
	src/common/rijndael-alg-fst.c
	src/monitor/configuration.c
	src/monitor/network.c
	src/monitor/monitor-list.c
	src/monitor/visual.c
	src/monitor/backlog.c
	)
TARGET_LINK_LIBRARIES(${CurrentExe} ${CURSES_LIBRARIES} ${INIPARSER_LIBRARIES} ${TALLOC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} iniparser)

SET(CurrentExe "smbtatorture")
ADD_EXECUTABLE(${CurrentExe}
	src/torture/smbtatorture.c
	)
TARGET_LINK_LIBRARIES(${CurrentExe} ${LIBSMBCLIENT_LIBRARIES})

SET(CurrentExe "rrddriver")
ADD_EXECUTABLE(${CurrentExe}
	src/webmon/rrddriver/main.c
        src/common/common.c
        src/common/aes.c
        src/common/rijndael-alg-fst.c
	src/webmon/rrddriver/configuration.c
	src/webmon/rrddriver/network.c
	src/webmon/rrddriver/monitor-list.c
	)
TARGET_LINK_LIBRARIES(${CurrentExe} ${INIPARSER_LIBRARIES} ${TALLOC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} iniparser)


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

