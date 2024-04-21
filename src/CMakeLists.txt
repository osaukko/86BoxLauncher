add_executable(86BoxLauncher main.cpp)

target_link_libraries(86BoxLauncher gui data)

include_directories(.)

add_subdirectory(data)
add_subdirectory(gui)
add_subdirectory(mvc)
add_subdirectory(utils)

# Windows-specific instructions
if(WIN32)
    # Application icon
    target_sources(86BoxLauncher PRIVATE 86boxlauncher.rc)

    # Do not open the console when the application is launched
    set_property(TARGET 86BoxLauncher PROPERTY WIN32_EXECUTABLE true)
endif()

# Making icons available for development builds
if(COPY_ICONS_TO_BUILD_DIR)
  if(WIN32)
    # Symbolic links require administrator privileges or developer mode on
    # Windows and are a bit problematic because of that. Therefore, we copy
    # files instead of symbolic links for Windows releases.
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
    file(COPY icons/86BoxLauncher DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/icons)
  endif()
endif()

# Files to install when the install target is run
if(WIN32)
  install(TARGETS 86BoxLauncher DESTINATION .)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/icons DESTINATION .)
else()
  install(TARGETS 86BoxLauncher)
  install(DIRECTORY icons/86BoxLauncher DESTINATION share/86BoxLauncher/icons)
endif()
