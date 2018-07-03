file(GLOB CD "data/*.csv")
file(GLOB CFD "data/*.ini")

set(CSV_DIR "${CD}")
set(CONFIG_DIR "${CFD}")

configure_file("include/data.h.in" "include/data.h")

include_directories("${CMAKE_CURRENT_BINARY_DIR}")