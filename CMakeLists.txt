cmake_minimum_required(VERSION 3.4.3)
project (pony_gp)
set(DIR ${pony_gp_SOURCE_DIR})

file(GLOB_RECURSE pony_gp_SOURCES "${DIR}/src/*.c")
file(GLOB_RECURSE pony_gp_HEADERS "${DIR}/include/*.h")

foreach (_headerFile ${pony_gp_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND pony_gp_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES pony_gp_INCLUDE_DIRS)

include("${DIR}/include/CMakeLists.txt")
include_directories(${pony_gp_INCLUDE_DIRS})
add_executable (pony_gpExe ${pony_gp_SOURCES})

if (CMAKE_COMPILER_IS_GNUCC)
	target_link_libraries(pony_gpExe m)
endif()