cmake_minimum_required(VERSION 3.0)
project (pony_gp)

file(GLOB_RECURSE pony_gp_SOURCES "src/*.c")
file(GLOB_RECURSE pony_gp_HEADERS "include/*.h")

foreach (_headerFile ${pony_gp_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND pony_gp_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES pony_gp_INCLUDE_DIRS)

include_directories(${pony_gp_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/userdata")
add_executable (pony_gpExe ${pony_gp_SOURCES})

target_link_libraries(pony_gpExe m)
