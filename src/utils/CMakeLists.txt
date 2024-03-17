set(CMAKE_AUTOMOC ON)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Test)

add_library(utils STATIC formatter.cpp formatter.h)

target_link_libraries(utils PUBLIC Qt${QT_VERSION_MAJOR}::Core
                                   Qt${QT_VERSION_MAJOR}::Gui)

add_executable(test_formatter test_formatter.cpp )
add_test(NAME test_formatter COMMAND test_formatter)

target_link_libraries(test_formatter PRIVATE utils Qt${QT_VERSION_MAJOR}::Test)
