set(CMAKE_AUTOMOC ON)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui)

add_library(data STATIC machine.cpp machine.h settings.cpp settings.h)

target_link_libraries(data PUBLIC Qt${QT_VERSION_MAJOR}::Core
                                  Qt${QT_VERSION_MAJOR}::Gui)
