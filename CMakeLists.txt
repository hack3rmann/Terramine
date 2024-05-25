cmake_minimum_required(VERSION 3.29.0)

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
    src/Voxels/Voxel.cpp
)

set(DEPS_DIR ${CMAKE_SOURCE_DIR}/deps)

target_include_directories(terramine PRIVATE ${DEPS_DIR}/glew/include)
target_link_directories(terramine PRIVATE ${DEPS_DIR}/glew/out/lib)
target_link_libraries(terramine glew opengl32)
target_compile_definitions(terramine PRIVATE GLEW_STATIC)

target_include_directories(terramine PRIVATE ${DEPS_DIR}/glfw/include)
target_link_directories(terramine PRIVATE ${DEPS_DIR}/glfw/build/src)
target_link_libraries(terramine glfw3)

target_include_directories(terramine PRIVATE ${DEPS_DIR}/glm)
target_link_directories(terramine PRIVATE ${DEPS_DIR}/glm/build/glm)
target_link_libraries(terramine glm)

target_include_directories(terramine PRIVATE ${DEPS_DIR}/rapidjson/include)

target_include_directories(terramine PRIVATE ${DEPS_DIR}/spng/spng)
target_link_directories(terramine PRIVATE ${DEPS_DIR}/spng/build)
target_link_libraries(terramine spng_static)
target_compile_definitions(terramine PRIVATE SPNG_STATIC)

target_include_directories(terramine PRIVATE ${DEPS_DIR}/zlib)
target_link_directories(terramine PRIVATE ${DEPS_DIR}/zlib)
target_link_libraries(terramine z)