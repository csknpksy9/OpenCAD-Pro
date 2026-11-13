cmake_minimum_required(VERSION 3.16)
project(OpenCADPro VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(opencadpro_geometry src/geometry.cpp)
target_include_directories(opencadpro_geometry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(opencadpro_geometry PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

add_executable(opencadpro src/main.cpp)
target_link_libraries(opencadpro PRIVATE opencadpro_geometry)

include(CTest)
if(BUILD_TESTING)
    add_executable(opencadpro_tests tests/geometry_tests.cpp)
    target_link_libraries(opencadpro_tests PRIVATE opencadpro_geometry)
    add_test(NAME geometry_tests COMMAND opencadpro_tests)
endif()

install(TARGETS opencadpro RUNTIME DESTINATION bin)
