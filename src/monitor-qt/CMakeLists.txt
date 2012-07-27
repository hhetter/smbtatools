project( smbtamonitor-qt )
cmake_minimum_required(VERSION 2.6)

find_package(Qt4 REQUIRED)
include_directories( ${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${TALLOC_INCLUDE_DIR})

SET( smbtamonitor-qt_SRC  main.cpp
                          frontend.cpp
			  configform.cpp
			  monitorform.cpp
                          timeclass.cpp
                          processrunner.cpp
			  dpoint.cpp
			  graph.cpp)




qt4_wrap_ui( smbtamonitor-qt_ui configform.ui frontend.ui monitorform.ui )
qt4_automoc( ${smbtamonitor-qt_SRC} )
ADD_EXECUTABLE( smbtamonitor-qt ${smbtamonitor-qt_SRC} ${smbtamonitor-qt_ui})
TARGET_LINK_LIBRARIES( smbtamonitor-qt ${LIBDBI_LIBRARIES} ${TALLOC_LIBRARIES} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY} )

INSTALL(TARGETS smbtamonitor-qt DESTINATION bin)
