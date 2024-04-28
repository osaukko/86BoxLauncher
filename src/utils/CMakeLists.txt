set(CMAKE_AUTOMOC ON)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Test)

add_library(utils STATIC formatter.cpp formatter.h utilities.cpp utilities.h)

target_link_libraries(utils PUBLIC Qt${QT_VERSION_MAJOR}::Core
                                   Qt${QT_VERSION_MAJOR}::Widgets)
