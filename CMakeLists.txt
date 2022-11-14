cmake_minimum_required (VERSION 3.0)
project (emp-ir)
set(NAME "emp-ir")

set(CMAKE_PREFIX_PATH cmake/)
find_path(CMAKE_FOLDER NAMES cmake/emp-tool-config.cmake)
include(${CMAKE_FOLDER}/cmake/emp-base.cmake)

find_package(emp-zk REQUIRED)
include_directories(${EMP-ZK_INCLUDE_DIRS})

find_package(wiztoolkit REQUIRED)
include_directories(${WIZTOOLKIT_INCLUDE_DIRS})

add_executable(emp-compile "emp-compile.cpp")
target_link_libraries(emp-compile ${WIZTOOLKIT_LIBRARIES})

add_executable(emp-interpret "emp-interpret.cpp")
target_link_libraries(emp-interpret ${WIZTOOLKIT_LIBRARIES} ${EMP-ZK_LIBRARIES})

install(DIRECTORY emp-ir DESTINATION include/)
install(TARGETS emp-compile  DESTINATION bin/)
install(TARGETS emp-interpret  DESTINATION bin/)
install(FILES emp-IR120.sh DESTINATION bin/ PERMISSIONS WORLD_READ WORLD_EXECUTE)
