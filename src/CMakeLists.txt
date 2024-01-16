add_executable(86BoxLauncher main.cpp)

target_link_libraries(86BoxLauncher gui)

add_subdirectory(gui)

# Copy icons to the current binary folder
file(COPY ../icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
