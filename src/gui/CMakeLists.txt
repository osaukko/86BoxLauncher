set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_library(
    gui STATIC mainwindow.cpp mainwindow.h  preferencesdialog.cpp
             preferencesdialog.h preferencesdialog.ui)

target_link_libraries(gui PUBLIC Qt${QT_VERSION_MAJOR}::Widgets)
