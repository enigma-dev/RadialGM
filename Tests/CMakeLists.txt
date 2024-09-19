set(RGM_TESTS "RGM-Tests" CACHE STRING "RGM Tests")

project(${RGM_TESTS} LANGUAGES CXX)

file(GLOB RGM_TESTS_SRC_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Editors/VisualShaderEditorTests.cpp
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

enable_testing()

add_executable(${RGM_TESTS} ${RGM_TESTS_SRC_FILES})

find_package(Qt5Test REQUIRED)
target_link_libraries(${RGM_TESTS} PRIVATE Qt5::Test)

find_package(Qt5 COMPONENTS Core Widgets Gui PrintSupport Multimedia REQUIRED)
target_link_libraries(${RGM_TESTS} PRIVATE Qt5::Core Qt5::Widgets Qt5::Gui Qt5::PrintSupport Qt5::Multimedia)

target_link_libraries(${RGM_TESTS} PRIVATE ${LIB_PROTO} ${SHARED_LIB})

add_test(NAME ${RGM_TESTS} COMMAND ${RGM_TESTS})
