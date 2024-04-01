enable_testing(true)

include_directories(../src)

set(CMAKE_AUTOMOC ON)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Test)

# Tests for data library
add_executable(test_machine test_machine.cpp)
add_test(NAME test_machine COMMAND test_machine)
target_compile_definitions(
  test_machine
  PRIVATE
    TEST_ICON="${PROJECT_SOURCE_DIR}/src/icons/86BoxLauncher/machine/32/pc.svg")
target_link_libraries(test_machine PRIVATE data Qt${QT_VERSION_MAJOR}::Test)

# Tests for utils library
add_executable(test_formatter test_formatter.cpp)
add_test(NAME test_formatter COMMAND test_formatter)
target_link_libraries(test_formatter PRIVATE utils Qt${QT_VERSION_MAJOR}::Test)
