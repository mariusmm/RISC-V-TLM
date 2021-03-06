## This project requires systemC to be compiled with cmake using C++17:
## cd <systemc directory>
## mkdir build
## cd build
## cmake ../ -DCMAKE_CXX_STANDARD=17
## make

cmake_minimum_required(VERSION 3.1)
project(RISCV_TLM)


# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

#add_compile_options(-Wall -I. -O3 -std=c++17 -g -Wextra -Wunused-function -Wpedantic)
add_compile_options(-O3 -g -Wall -Wextra -Wunused-function -Wpedantic)

include_directories(./inc/)
file(GLOB SRC "./src/*.cpp")

find_package(SystemCLanguage CONFIG REQUIRED)
set (CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD})


add_executable(RISCV_TLM ${SRC} )
target_link_libraries(RISCV_TLM SystemC::systemc)

option(BUILD_DOC "Build documentation" ON)
find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_SOURCE_DIR}/doc)
    message("Doxygen build started")
    add_custom_target( doc
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
            WORKING_DIRECTORY ${DOXYGEN_OUT}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)



