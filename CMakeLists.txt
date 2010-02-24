cmake_minimum_required(VERSION 2.6)
PROJECT(stad C)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")
set(APPLICATION_NAME "smbtatools")
set(APPLICATION_VERSION "0.0.1")

set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_PACKAGE_FILE_NAME 
 "${APPLICATION_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_SOURCE_IGNORE_FILES ".git/;CMakeFiles/")
include(CPack)
#include(CPackConfig.cmake)
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_C_FLAGS_DEBUG "-g")
#SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_INSTALL_PREFIX /usr)
SET(Libraries
iniparser
)
SET(PLATFORM_LINKFLAGS "-pthread")

add_custom_target(
	buildiniparser ALL
	COMMAND "make"
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/iniparser3.0b"
	COMMENT "Building iniparser."
	VERBATIM )
ADD_LIBRARY(iniparser STATIC IMPORTED)
set_target_properties(iniparser PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/iniparser3.0b/libiniparser.a)

SET(CurrentExe "smbtaquery")
ADD_EXECUTABLE(${CurrentExe} 
	src/query/main.c
	)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CMAKE_C_FLAGS "-lncurses")
SET(CurrentExe "smbtamonitor")
ADD_EXECUTABLE(${CurrentExe}
	src/monitor/main.c
	)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

# install documentation
INSTALL(FILES doc/smbtaquery.1
        DESTINATION "share/man/man1"
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ RENAME stad.1)
INSTALL(FILES doc/smbtamonitor.1
        DESTINATION "share/man/man1"
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ RENAME stad.conf.1)


INSTALL(TARGETS smbtaquery smbtamonitor DESTINATION bin)

