cmake_minimum_required(VERSION 2.8)
project(girdap)

#set( CMAKE_C_COMPILER emcc )
set( CMAKE_CXX_COMPILER em++ ) 

#if (CMAKE_VERSION VERSION_LESS "3.1")
#  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
set (CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
set (CMAKE_EXE_LINKER_FLAGS "-o bin/girdap.html ${CMAKE_EXE_LINKER_FLAGS}")
#  endif ()
#else ()
#  set (CMAKE_CXX_STANDARD 11)
#endif ()

include_directories(include)

add_subdirectory(src)
#add_library(girdap STATIC ${SOURCES})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
add_executable(girdap src/main.cpp)
target_link_libraries(girdap girdapcore)
install(TARGETS girdap DESTINATION bin)
#install(TARGETS girdap DESTINATION girdap)
#install(DIRECTORY include/ DESTINATION include/girdap PATTERN "*cpp" EXCLUDE)

set(EXMPS div lsp)
foreach(EX ${EXMPS})
  add_executable(${EX} EXCLUDE_FROM_ALL example/${EX}/main.cpp)
  target_link_libraries(${EX} girdapcore)
endforeach(EX)


