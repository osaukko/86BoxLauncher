# The minimum version is based on the regex CMAKE_MATCH_n support, which was
# introduced in CMake version 3.9
cmake_minimum_required(VERSION 3.9)

# Detect Qt Version
#
# This function tries to query the Qt version from the QMake program. You can
# set the QT_QMAKE_EXECUTABLE variable to point to the location of the QMake
# program. Note that the Qt Creator does this automatically for you.
#
# If the QT_QMAKE_EXECUTABLE variable is missing, this function tries to find
# the QMake itself and sets the variable to point it.
#
# If the version query is successful, the following variables are set:
#
# * QT_VERSION_MAJOR
# * QT_VERSION_MINOR
# * QT_VERSION_PATCH
# * QT_VERSION_STR
#
function(detect_qt_version)
  # Try to find qmake if QT_QMAKE_EXECUTABLE is not set
  if(NOT QT_QMAKE_EXECUTABLE)
    find_program(QT_QMAKE_EXECUTABLE NAMES qmake qmake5 qmake-qt5 qmake6
                                           qmake-qt6 REQUIRED)
  endif()

  # Request qmake to print version information
  execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -v
                  OUTPUT_VARIABLE QMAKE_RAW_OUTPUT)

  # Parse version information from the output
  if(QMAKE_RAW_OUTPUT MATCHES "Using Qt version ([0-9]+)\.([0-9]+)\.([0-9]+)")
    set(QT_VERSION_MAJOR ${CMAKE_MATCH_1} PARENT_SCOPE)
    set(QT_VERSION_MINOR ${CMAKE_MATCH_2} PARENT_SCOPE)
    set(QT_VERSION_PATCH ${CMAKE_MATCH_3} PARENT_SCOPE)
    set(QT_VERSION_STR
        "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}"
        PARENT_SCOPE)
  else()
    message(FATAL_ERROR "Could not parse version info from the qmake output.")
  endif()
endfunction()
