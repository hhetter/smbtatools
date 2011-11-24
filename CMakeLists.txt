#
#
#
#
#

# Check the minimum required cmake version for this project
cmake_minimum_required(VERSION 2.6)
###
# Change to also build cpp files
# -> seems to not help, so commented out
PROJECT(smbtatools)
#PROJECT(smbtatools C)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")
set(APPLICATION_NAME "smbtatools")
set(APPLICATION_VERSION "0.0.1")
INCLUDE(${CMAKE_SOURCE_DIR}/FindSqlite.cmake)

# Build debugging enabled binaries
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_C_FLAGS_DEBUG "-g -Wall")
#SET(CMAKE_VERBOSE_MAKEFILE ON)

#
# check for QT(4)
#
FIND_PACKAGE(Qt4)
IF (NOT QT_INCLUDE_DIR) 
	MESSAGE( FATAL_ERROR "ERROR: you need the qt development files and library installed.")
ENDIF()
IF (QT_INCLUDE_DIR)
	INCLUDE_DIRECTORIES($QT_INCLUDE_DIR)
ENDIF()


#
# check for pango
#
FIND_PACKAGE(PANGO)
IF (NOT LIBPANGO_INCLUDE_DIRS)
	MESSAGE( FATAL_ERROR "ERROR: you need the pango library and headers installed.")
ENDIF()
IF (LIBPANGO_INCLUDE_DIRS)
	INCLUDE_DIRECTORIES($LIBPANGO_INCLUDE_DIRS)
ENDIF()

#
# check for cairo
#
FIND_PACKAGE(CAIRO)
if (NOT LIBCAIRO_INCLUDE_DIRS) 
        MESSAGE( FATAL_ERROR "ERROR: you need the cairo library and headers installed.")
ENDIF() 
IF (LIBCAIRO_INCLUDE_DIRS)
	INCLUDE_DIRECTORIES($LIBCAIRO_INCLUDE_DIRS)
ENDIF()


#
# check for libcurl
#
FIND_PACKAGE(CURL)
if (NOT CURL_INCLUDE_DIRS)
	                MESSAGE( FATAL_ERROR "ERROR: you will need curl development files installed.")
		ENDIF()
		if (CURL_INCLUDE_DIRS)
			        INCLUDE_DIRECTORIES($CURL_INCLUDE_DIRS)
			ENDIF()

# compile on SunOS
if (CMAKE_SYSTEM_NAME STREQUAL "SunOS" )
	ADD_DEFINITIONS(-DSOLARIS)
        SET(Libraries
            ${Libraries}
	    socket
	    nsl)
endif()


#
# check for libdbi
#
include(FindLibDBI.cmake)
FIND_PACKAGE(LibDBI)
if (NOT LIBDBI_INCLUDE_DIR)
	        MESSAGE( FATAL_ERROR "ERROR: you will need the libdbi development files installed.")
	ENDIF()
	if (LIBDBI_INCLUDE_DIR)
		        INCLUDE_DIRECTORIES(${LIBDBI_INCLUDE_DIR})
		ENDIF()

#
# check for sqlite3
#
include(FindSqlite.cmake)
FIND_PACKAGE(Sqlite)
if (NOT SQLITE_FOUND OR CMAKE_FORCE_EMBEDDED_SQLITE)
        MESSAGE( STATUS "----> FYI: Seems like there're no sqlite3 development files")
        MESSAGE( STATUS "           installed.")
        MESSAGE( STATUS "           That's ok, we will build the ")
        MESSAGE( STATUS "           sqlite amalgamation code then.")
        SET(SQLITE_LIBRARIES "")
ELSEIF (SQLITE_FOUND)
        MESSAGE( STATUS "----> FYI: Nice, your system provides sqlite >= 3.7.0, we")
        MESSAGE( STATUS "           will make the build linking shared to this library.")
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D SQLITE_FOUND")
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

INCLUDE_DIRECTORIES( ${SQLITE3_INCLUDE_DIRS} ${TALLOC_INCLUDE_DIR} ${LIBSMBCLIENT_INCLUDE_DIRS} )

SET(CurrentExe "smbtaquery")
ADD_EXECUTABLE(${CurrentExe} 
	src/query/sqlite/sqlite3.c
	src/query/main.c
	src/common/common.c
	src/common/aes.c
	src/common/rijndael-alg-fst.c
	src/query/configuration.c
	src/query/interpreter.c
	src/query/network.c
	src/query/convert.c
	src/query/graphics.c
	)

TARGET_LINK_LIBRARIES(${CurrentExe} ${LIBCAIRO_LIBRARIES} ${CURL_LIBRARIES} ${LIBDBI_LIBRARIES} ${INIPARSER_LIBRARIES} ${SQLITE_LIBRARIES} ${TALLOC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} iniparser dl)

SET(CurrentExe "smbtamonitor-gen")
ADD_EXECUTABLE(${CurrentExe}
	src/monitor/main.c
	src/common/common.c
	src/common/aes.c
	src/common/rijndael-alg-fst.c
	src/monitor/configuration.c
	src/monitor/network.c
	src/monitor/monitor-list.c
	)
TARGET_LINK_LIBRARIES(${CurrentExe} ${LIBDBI_LIBRARIES} ${CURSES_LIBRARIES} ${INIPARSER_LIBRARIES} ${TALLOC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} iniparser)

SET(CurrentExe "smbtatorture")
ADD_EXECUTABLE(${CurrentExe}
	src/torture/smbtatorture.c
	)
TARGET_LINK_LIBRARIES(${CurrentExe} ${LIBSMBCLIENT_LIBRARIES})

SET(CurrentExe "smbtatorturesrv")
ADD_EXECUTABLE(${CurrentExe}
	src/torture/srv/smbtatorturesrv.c
	)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

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
TARGET_LINK_LIBRARIES(${CurrentExe} ${LIBDBI_LIBRARIES} ${INIPARSER_LIBRARIES} ${TALLOC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} iniparser)

# Adding the subdirectory for monitor-qt

add_subdirectory( src/monitor-qt )

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
#install xslt tranformers
INSTALL(FILES src/query/xslt/xslt-ascii.xml
	DESTINATION "share/smbtatools"
	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ RENAME xslt-ascii.xml)
INSTALL(FILES src/query/xslt/xslt-html.xml
	DESTINATION "share/smbtatools"
	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ RENAME xslt-html.xml)
INSTALL(FILES src/torture/srv/filenames.txt
	DESTINATION "share/smbtatools"
	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ RENAME filenames.txt)
INSTALL(FILES src/torture/srv/directories.txt
	DESTINATION "share/smbtatools"
	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ RENAME directories.txt)

INSTALL(TARGETS smbtaquery smbtamonitor-gen  smbtatorture rrddriver smbtatorturesrv DESTINATION bin)

