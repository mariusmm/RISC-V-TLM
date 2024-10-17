## This project requires systemC to be compiled with cmake using C++17:
## cd <systemc directory>
## mkdir build
## cd build
## cmake ../ -DCMAKE_CXX_STANDARD=17
## make

cmake_minimum_required(VERSION 3.10)
project(RISCV_TLM LANGUAGES CXX C)

set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_PREFIX_PATH $ENV{BOOST_ROOT};$ENV{SYSTEMC_HOME};$ENV{SPDLOG_HOME})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-O3 -g -Wall -Wextra -Wunused-function -Wpedantic -Werror)

include_directories(./inc/)
file(GLOB SRC "./src/*.cpp")

find_package(SystemCLanguage CONFIG REQUIRED)
set (CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD})

find_package(spdlog CONFIG REQUIRED)

if(POLICY CMP0167)
cmake_policy(SET CMP0167 NEW)
endif()

add_executable(RISCV_TLM ${SRC} )
target_link_libraries(RISCV_TLM SystemC::systemc)
target_link_libraries(RISCV_TLM spdlog::spdlog)

# specify the C++ standard
target_compile_features(RISCV_TLM PUBLIC cxx_std_17)
target_compile_features(RISCV_TLM PUBLIC c_std_17)

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

