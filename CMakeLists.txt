cmake_minimum_required(VERSION 3.28.2)

include(FetchContent)

set(DEPS_DIR ${CMAKE_SOURCE_DIR}/deps)
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
execute_process(COMMAND ln -sfT ${BUILD_DIR}/compile_commands.json
                        ${CMAKE_SOURCE_DIR}/compile_commands.json)

option(SANITIZE "Enable sanitizers" OFF)

if(${SANITIZE})
    message(STATUS "Build with sanitizers")
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -g -fsanitize=address -fsanitize=undefined -fsanitize=leak"
  )
endif()

project(terramine)

add_executable(
  terramine
  ${DEPS_DIR}/glad/src/gl.c
  src/main.cpp
  src/Camera.cpp
  src/EventHandler.cpp
  src/Player.cpp
  src/Window.cpp
  src/graphics/FrameBuffer.cpp
  src/graphics/LineBatch.cpp
  src/graphics/MasterHandler.cpp
  src/graphics/SceneHandler.cpp
  src/graphics/shader.cpp
  src/graphics/mesh.cpp
  src/graphics/Skybox.cpp
  src/graphics/texture.cpp
  src/graphics/VoxelRenderer.cpp
  src/GUI/Button.cpp
  src/GUI/GUI.cpp
  src/GUI/GUIHandler.cpp
  src/GUI/GUIObject.cpp
  src/GUI/Sprite.cpp
  src/GUI/Text.cpp
  src/loaders/png.cpp
  src/loaders/shader.cpp
  src/loaders/string.cpp
  src/data/image.cpp
  src/Voxels/BlockStore.cpp
  src/Voxels/Chunk.cpp
  src/Voxels/Chunks.cpp
  src/Voxels/Terrarian.cpp
  src/Voxels/Voxel.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/include)

add_subdirectory(${DEPS_DIR}/glfw ${BUILD_DIR}/deps/glfw)
target_include_directories(terramine PRIVATE ${DEPS_DIR}/glfw/include)
target_link_directories(terramine PRIVATE ${BUILD_DIR}/deps/glfw/src)
target_link_libraries(terramine glfw3)

add_subdirectory(${DEPS_DIR}/glm ${BUILD_DIR}/deps/glm)
target_include_directories(terramine PRIVATE ${DEPS_DIR}/glm)
target_link_directories(terramine PRIVATE ${BUILD_DIR}/deps/glm/glm)
target_link_libraries(terramine glm)

target_include_directories(terramine PRIVATE ${DEPS_DIR}/rapidjson/include)

set(BUILD_EXAMPLES OFF)
add_subdirectory(${DEPS_DIR}/spng ${BUILD_DIR}/deps/spng)
target_include_directories(terramine PRIVATE ${DEPS_DIR}/spng/spng)
target_link_directories(terramine PRIVATE ${BUILD_DIR}/deps/spng)
target_link_libraries(terramine spng_static)
target_compile_definitions(terramine PRIVATE SPNG_STATIC)
