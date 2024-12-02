cmake_minimum_required(VERSION 3.28.2)

include(FetchContent)

project(terramine)

set(DEPS_DIR ${CMAKE_SOURCE_DIR}/deps)
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
execute_process(COMMAND ln -sfT ${BUILD_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json)

option(SANITIZE "Enable sanitizers" OFF)
option(THREAD_SANITIZE "Enable thread sanitizer" OFF)
option(OPTIMIZE "Enable compiler optimizarions" OFF)

if(${SANITIZE})
    message(STATUS "Build with sanitizers")
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -g -fsanitize=address -fsanitize=undefined -fsanitize=leak")
endif()

if(${OPTIMIZE})
    message(STATUS "Optimized build")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

if(${THREAD_SANITIZE})
    message(STATUS "Build with thread sanitizer")
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -g -fsanitize=thread -fsanitize=undefined")
endif()

file(GLOB TERRAMINE_SOURCE_FILES src/**/*.cpp)

add_executable(terramine
    src/main.cpp
    ${TERRAMINE_SOURCE_FILES})

add_executable(test
    tests/main.cpp
    tests/parse/fnt.cpp
    tests/vec.cpp
    ${TERRAMINE_SOURCE_FILES})

target_include_directories(test PRIVATE src)


option(BUILD_EXAMPLES "" OFF)


find_package(OpenMP)
if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


include(FetchContent)
set(FETCHCONTENT_QUIET NO)


include(cmake/CPM.cmake)

CPMAddPackage("gh:fmtlib/fmt#11.0.2")
CPMAddPackage("gh:hack3rmann/comb#0.3.1")
CPMAddPackage(
    NAME libspng
    VERSION 0.7.4
    GITHUB_REPOSITORY randy408/libspng
    OPTIONS "BUILD_EXAMPLES OFF")

set(LIBS glad glfw3 fmt::fmt comb spng glm)

target_link_libraries(terramine ${LIBS})
target_link_libraries(test ${LIBS})

add_subdirectory(${DEPS_DIR}/glad ${BUILD_DIR}/deps/glad)

add_subdirectory(${DEPS_DIR}/glfw ${BUILD_DIR}/deps/glfw)
target_include_directories(terramine PRIVATE ${DEPS_DIR}/glfw/include)
target_link_directories(terramine PRIVATE ${BUILD_DIR}/deps/glfw/src)
target_include_directories(test PRIVATE ${DEPS_DIR}/glfw/include)
target_link_directories(test PRIVATE ${BUILD_DIR}/deps/glfw/src)

target_compile_definitions(terramine PRIVATE SPNG_STATIC)
target_compile_definitions(test PRIVATE SPNG_STATIC)


add_subdirectory(${DEPS_DIR}/glm ${BUILD_DIR}/deps/glm)
target_include_directories(terramine PRIVATE ${DEPS_DIR}/glm)
target_link_directories(terramine PRIVATE ${BUILD_DIR}/deps/glm/glm)
target_include_directories(test PRIVATE ${DEPS_DIR}/glm)
target_link_directories(test PRIVATE ${BUILD_DIR}/deps/glm/glm)


target_include_directories(terramine PRIVATE ${DEPS_DIR}/rapidjson/include)
target_include_directories(test PRIVATE ${DEPS_DIR}/rapidjson/include)


