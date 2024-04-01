set(CMAKE_AUTOMOC ON)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_library(mvc STATIC machinelistmodel.cpp machinelistmodel.h)

target_link_libraries(mvc PUBLIC Qt${QT_VERSION_MAJOR}::Widgets)
