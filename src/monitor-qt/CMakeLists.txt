project( smbtamonitor-qt )
cmake_minimum_required(VERSION 2.6)

find_package(Qt4 REQUIRED)
include_directories( ${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${TALLOC_INCLUDE_DIR})

SET( smbtamonitor-qt_SRC  main.cpp
			  ../common/aes.c
			  ../common/rijndael-alg-fst.c
			  ../common/common.c
                          frontend.cpp
                          configuration.cpp
                          smbtamonitor-run.cpp
                          visual.cpp
                          timeclass.cpp
                          processrunner.cpp
			  queryshares.cpp)

qt4_automoc( ${smbtamonitor-qt_SRC} )

ADD_EXECUTABLE( smbtamonitor-qt ${smbtamonitor-qt_SRC})
TARGET_LINK_LIBRARIES( smbtamonitor-qt ${LIBDBI_LIBRARIES} ${TALLOC_LIBRARIES} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )

INSTALL(TARGETS smbtamonitor-qt DESTINATION bin)
