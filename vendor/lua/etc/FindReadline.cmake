# FindReadline.cmake
#
# Defines:
#   READLINE_FOUND: true if readline has been found, false otherwise;
#   READLINE_INCLUDE_DIR: location of readline/readline.h;
#   READLINE_LIBRARY: path of library to link against;

FIND_PATH(READLINE_INCLUDE_DIR readline/readline.h)
FIND_LIBRARY(READLINE_LIBRARY NAMES readline)
IF ( READLINE_INCLUDE_DIR AND READLINE_LIBRARY )
  SET(READLINE_FOUND TRUE)
  MESSAGE(STATUS "Found Readline library at ${READLINE_LIBRARY}")
  MESSAGE(STATUS "Readline headers found at at ${READLINE_INCLUDE_DIR}")
  INCLUDE_DIRECTORIES(${READLINE_INCLUDE_DIR})
ELSE()
  SET(READLINE_FOUND FALSE)
  MESSAGE(FATAL_ERROR "Readline library not found!")
ENDIF()
