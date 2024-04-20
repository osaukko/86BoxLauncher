add_executable(86BoxLauncher main.cpp)

target_link_libraries(86BoxLauncher gui data)

include_directories(.)

add_subdirectory(data)
add_subdirectory(gui)
add_subdirectory(mvc)
add_subdirectory(utils)

if(COPY_ICONS_TO_BUILD_DIR)
  if(WIN32)
    # Symbolic links require administrator privileges on Windows and are a bit
    # problematic because of that. Therefore, we copy files instead of symbolic
    # links for Windows releases.
    file(
      GLOB_RECURSE ICONFILES
      LIST_DIRECTORIES false
      RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
      FOLLOW_SYMLINKS icons/*)
    foreach(ICONFILE ${ICONFILES})
      cmake_path(GET ICONFILE PARENT_PATH ICONPATH)
      file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${ICONPATH})
      file(COPY_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${ICONFILE}
           ${CMAKE_CURRENT_BINARY_DIR}/${ICONFILE} ONLY_IF_DIFFERENT)
    endforeach()
  else()
    file(COPY icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  endif()
endif()

if(WIN32)
  install(TARGETS 86BoxLauncher DESTINATION .)
  install(DIRECTORY icons DESTINATION .)
else()
  install(TARGETS 86BoxLauncher)
  install(DIRECTORY icons DESTINATION share/86BoxLauncher)
endif()
