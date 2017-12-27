file(GLOB CSV_DIR "data/*.csv" )
file(GLOB INI_DIR "data/*.ini")

set(DIR_STRING "${CSV_DIR}")
set(INI_DIR "${INI_DIR}")
configure_file("include/params.h.in" "include/params.h")

include_directories(${CMAKE_CURRENT_BINARY_DIR})