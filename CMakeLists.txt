cmake_minimum_required(VERSION 3.28.2)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(terramine)

add_executable(
  terramine
  src/main.cpp
  src/Camera.cpp
  src/defines.cpp
  src/EventHandler.cpp
  src/indexBuffer.cpp
  src/Mesh.cpp
  src/Player.cpp
  src/vertexBuffer.cpp
  src/Window.cpp
  src/Graphics/FrameBuffer.cpp
  src/Graphics/LineBatch.cpp
  src/Graphics/MasterHandler.cpp
  src/Graphics/SceneHandler.cpp
  src/Graphics/Shader.cpp
  src/Graphics/Skybox.cpp
  src/Graphics/Texture.cpp
  src/Graphics/VoxelRenderer.cpp
  src/GUI/Button.cpp
  src/GUI/DynamicText.cpp
  src/GUI/GUI.cpp
  src/GUI/GUIHandler.cpp
  src/GUI/GUIObject.cpp
  src/GUI/Sprite.cpp
  src/GUI/Text.cpp
  src/Loaders/pngLoader.cpp
  src/Voxels/BlockStore.cpp
  src/Voxels/Chunk.cpp
  src/Voxels/Chunks.cpp
  src/Voxels/Terrarian.cpp
  src/Voxels/Voxel.cpp)

set(DEPS_DIR ${CMAKE_SOURCE_DIR}/deps)
set(TARGET_DIR ${CMAKE_SOURCE_DIR}/target)

add_subdirectory(${DEPS_DIR}/glew ${TARGET_DIR}/deps/glew)
target_include_directories(terramine PRIVATE ${DEPS_DIR}/glew/include)
target_link_directories(terramine PRIVATE ${TARGET_DIR}/deps/glew/lib)
target_link_libraries(terramine glew opengl32)
target_compile_definitions(terramine PRIVATE GLEW_STATIC)

add_subdirectory(${DEPS_DIR}/glfw ${TARGET_DIR}/deps/glfw)
target_include_directories(terramine PRIVATE ${DEPS_DIR}/glfw/include)
target_link_directories(terramine PRIVATE ${TARGET_DIR}/deps/glfw/src)
target_link_libraries(terramine glfw3)

add_subdirectory(${DEPS_DIR}/glm ${TARGET_DIR}/deps/glm)
target_include_directories(terramine PRIVATE ${DEPS_DIR}/glm)
target_link_directories(terramine PRIVATE ${TARGET_DIR}/deps/glm/glm)
target_link_libraries(terramine glm)

target_include_directories(terramine PRIVATE ${DEPS_DIR}/rapidjson/include)

set(ZLIB_ENABLE_TESTS OFF)
set(ZLIB_COMPAT ON)
add_subdirectory(${DEPS_DIR}/zlib ${TARGET_DIR}/deps/zlib)
target_include_directories(terramine PRIVATE ${DEPS_DIR}/zlib)
target_link_directories(terramine PRIVATE ${TARGET_DIR}/deps/zlib)
target_link_libraries(terramine z)

set(ZLIB_LIBRARY ${TARGET_DIR}/deps/zlib/libz.a)
set(ZLIB_INCLUDE_DIR ${DEPS_DIR}/zlib)
set(BUILD_EXAMPLES OFF)
add_subdirectory(${DEPS_DIR}/spng ${TARGET_DIR}/deps/spng)
target_include_directories(terramine PRIVATE ${DEPS_DIR}/spng/spng)
target_link_directories(terramine PRIVATE ${TARGET_DIR}/deps/spng)
target_link_libraries(terramine spng_static)
target_compile_definitions(terramine PRIVATE SPNG_STATIC)
