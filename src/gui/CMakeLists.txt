set(CMAKE_AUTOMOC ON)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_library(gui STATIC mainwindow.h mainwindow.cpp)

target_link_libraries(gui PUBLIC Qt${QT_VERSION_MAJOR}::Widgets)
