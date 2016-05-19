cmake_minimum_required (VERSION 2.8)
project (price-import)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)
set(sources main.cpp ./src/ConfigSingleton.cpp ./src/csvreader.cpp ./src/DataFileReader.cpp
    ./src/DBSingleton.cpp ./src/ExcelReader.cpp ./src/FieldCoordinates.cpp ./src/SaveCommand.cpp)
include_directories(
    ./include
    ./vendor/forge/include
    ./vendor/cxxtest/
    ./vendor/tclap-code/build/include/
    ./vendor/el/
    ./vendor/libxls/libxls/build/include
)
add_library(forge STATIC IMPORTED)
add_library(xls STATIC IMPORTED)
set_property(TARGET forge PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/vendor/forge/libforge.a)
set_property(TARGET xls PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/vendor/libxls/libxls/build/lib/libxlsreader.a)
add_executable(price-import ${sources})
target_link_libraries(price-import forge xls pqxx)
