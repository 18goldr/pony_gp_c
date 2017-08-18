file(GLOB CSV_DIR "data/*.csv" )

set(DIR_STRING "${CSV_DIR}")
configure_file("include/params.h.in" "include/params.h")

include_directories(${CMAKE_CURRENT_BINARY_DIR})