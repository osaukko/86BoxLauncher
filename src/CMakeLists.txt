add_executable(86BoxLauncher main.cpp)

target_link_libraries(86BoxLauncher gui data)

# Copy icons to the current binary folder
file(COPY ../icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(gui)
add_subdirectory(data)
