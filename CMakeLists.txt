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

if(${SANITIZE})
    message(STATUS "Build with sanitizers")
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -g -fsanitize={address,undefined,leak}"
    )
endif()

if(${THREAD_SANITIZE})
    message(STATUS "Build with thread sanitizer")
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -g -fsanitize={thread,undefined}"
    )
endif()

set(TERRAMINE_SOURCE_FILES
    ${DEPS_DIR}/glad/src/gl.c
    src/Player.cpp
    src/graphics/FrameBuffer.cpp
    src/graphics/MasterHandler.cpp
    src/graphics/SceneHandler.cpp
    src/graphics/shader.cpp
    src/graphics/texture.cpp
    src/gui/sprite.cpp
    src/gui/text.cpp
    src/gui/button.cpp
    src/gui/gui.cpp
    src/controls/camera.cpp
    src/events/io.cpp
    src/loaders/font.cpp
    src/loaders/png.cpp
    src/loaders/shader.cpp
    src/loaders/string.cpp
    src/loaders/game_objects.cpp
    src/objects/skybox.cpp
    src/objects/line_batch.cpp
    src/objects/terrain.cpp
    src/parser/fnt.cpp
    src/terrain/chunk.cpp
    src/terrain/chunk_array.cpp
    src/terrain/renderer.cpp
    src/window/window.cpp)

add_executable(terramine
    src/main.cpp
    ${TERRAMINE_SOURCE_FILES})

add_executable(test
    tests/main.cpp
    tests/parse/fnt.cpp
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


set(COMB_VERSION 0.1.0)
FetchContent_Declare(
    comb
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/hack3rmann/comb/archive/refs/tags/${COMB_VERSION}.tar.gz)

FetchContent_MakeAvailable(comb)
target_link_libraries(terramine comb)
target_link_libraries(test comb)


set(FMT_VERSION 11.0.2)
find_package(fmt ${FMT_VERSION} QUIET)

if (NOT fmt_FOUND)
    FetchContent_Declare(
        fmt
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/fmtlib/fmt/archive/refs/tags/${FMT_VERSION}.tar.gz)

    FetchContent_MakeAvailable(fmt)

    target_link_libraries(terramine fmt)
    target_link_libraries(test fmt)
endif()


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/include)


add_subdirectory(${DEPS_DIR}/glfw ${BUILD_DIR}/deps/glfw)
target_include_directories(terramine PRIVATE ${DEPS_DIR}/glfw/include)
target_link_directories(terramine PRIVATE ${BUILD_DIR}/deps/glfw/src)
target_link_libraries(terramine glfw3)
target_include_directories(test PRIVATE ${DEPS_DIR}/glfw/include)
target_link_directories(test PRIVATE ${BUILD_DIR}/deps/glfw/src)
target_link_libraries(test glfw3)


add_subdirectory(${DEPS_DIR}/glm ${BUILD_DIR}/deps/glm)
target_include_directories(terramine PRIVATE ${DEPS_DIR}/glm)
target_link_directories(terramine PRIVATE ${BUILD_DIR}/deps/glm/glm)
target_link_libraries(terramine glm)
target_include_directories(test PRIVATE ${DEPS_DIR}/glm)
target_link_directories(test PRIVATE ${BUILD_DIR}/deps/glm/glm)
target_link_libraries(test glm)

target_include_directories(terramine PRIVATE ${DEPS_DIR}/rapidjson/include)
target_include_directories(test PRIVATE ${DEPS_DIR}/rapidjson/include)

set(BUILD_EXAMPLES OFF)
add_subdirectory(${DEPS_DIR}/spng ${BUILD_DIR}/deps/spng)
target_include_directories(terramine PRIVATE ${DEPS_DIR}/spng/spng)
target_link_directories(terramine PRIVATE ${BUILD_DIR}/deps/spng)
target_link_libraries(terramine spng_static)
target_compile_definitions(terramine PRIVATE SPNG_STATIC)
target_include_directories(test PRIVATE ${DEPS_DIR}/spng/spng)
target_link_directories(test PRIVATE ${BUILD_DIR}/deps/spng)
target_link_libraries(test spng_static)
target_compile_definitions(test PRIVATE SPNG_STATIC)
