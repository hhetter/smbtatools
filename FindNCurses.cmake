# - Try to find Curses
#
# Search/Replace-Work fron "FindTalloc.cmake"
# Once done this will define
#
#  CURSES_FOUND - system has Curses
#  CURSES_INCLUDE_DIRS - the Curses include directory
#  CURSES_LIBRARIES - Link these to use Curses
#  CURSES_DEFINITIONS - Compiler switches required for using Curses
#
#  Copyright (c) 2010 Holger Hetterich <hhetter@novell.com>
#  Copyright (c) 2007 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (CURSES_LIBRARIES AND CURSES_INCLUDE_DIRS)
  # in cache already
  set(CURSES_FOUND TRUE)
else (CURSES_LIBRARIES AND CURSES_INCLUDE_DIRS)
  find_path(CURSES_INCLUDE_DIR
    NAMES
      curses.h
    PATHS
      /usr/local/include
      /usr/local/include/ncurses
      /usr/include
  )

  find_library(CURSES_LIBRARY
    NAMES
      ncurses
    PATHS
      /usr/local/lib
      /usr/lib
  )

  if (CURSES_LIBRARY)
    set(CURSES_FOUND TRUE)
  endif (CURSES_LIBRARY)

  set(CURSES_INCLUDE_DIRS
    ${INIPARSER_INCLUDE_DIR}
  )

  if (CURSES_FOUND)
    set(CURSES_LIBRARIES
      ${CURSES_LIBRARIES}
      ${CURSES_LIBRARY}
    )
  endif (CURSES_FOUND)

  if (CURSES_INCLUDE_DIRS AND CURSES_LIBRARIES)
     set(CURSES_FOUND TRUE)
  endif (CURSES_INCLUDE_DIRS AND CURSES_LIBRARIES)

  if (CURSES_FOUND)
    if (NOT Curses_FIND_QUIETLY)
      message(STATUS "Found Curses: ${CURSES_LIBRARIES}")
    endif (NOT Curses_FIND_QUIETLY)
  else (CURSES_FOUND)
    if (Curses_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find Curses")
    endif (Curses_FIND_REQUIRED)
  endif (CURSES_FOUND)

  # show the CURSES_INCLUDE_DIRS and CURSES_LIBRARIES variables only in the advanced view
  mark_as_advanced(CURSES_INCLUDE_DIRS CURSES_LIBRARIES)

endif (CURSES_LIBRARIES AND CURSES_INCLUDE_DIRS)
