const std = @import("std");

pub fn build(b: *std.Build) !void {
    const glfw = b.addStaticLibrary(.{
        .name = "glfw",
        .optimize = std.builtin.OptimizeMode.Debug,
        .target = .{ .os_tag = .windows, .cpu_arch = .x86_64 },
    });

    const glfw_path = "deps/glfw-3.4/src/";

    glfw.defineCMacro("__cplusplus", null);
    glfw.defineCMacro("_WIN32", null);
    glfw.linkLibCpp();

    glfw.addCSourceFiles(&.{
        glfw_path ++ "internal.h",
        glfw_path ++ "platform.h",
        glfw_path ++ "mappings.h",
        glfw_path ++ "context.c",
        glfw_path ++ "init.c",
        glfw_path ++ "input.c",
        glfw_path ++ "monitor.c",
        glfw_path ++ "platform.c",
        glfw_path ++ "vulkan.c",
        glfw_path ++ "window.c",
        glfw_path ++ "egl_context.c",
        glfw_path ++ "osmesa_context.c",
        glfw_path ++ "null_platform.h",
        glfw_path ++ "null_joystick.h",
        glfw_path ++ "null_init.c",
        glfw_path ++ "null_monitor.c",
        glfw_path ++ "null_window.c",
        glfw_path ++ "null_joystick.c",
    }, &.{});

    glfw.addIncludePath(std.Build.LazyPath.relative(
        "deps/cygwin64/usr/x86_64-w64-mingw32/sys-root/mingw/include"
    ));

    const exe = b.addExecutable(.{
        .name = "terramine",
        .root_source_file = .{ .path = "src/main.cpp" },
    });

    exe.addIncludePath(std.Build.LazyPath.relative("deps/glm"));
    exe.addIncludePath(std.Build.LazyPath.relative("deps/glew-2.1.0/include"));
    exe.addLibraryPath(std.Build.LazyPath.relative("deps/glew-2.1.0/lib/Release/x64/glew32s.lib"));
    exe.addIncludePath(std.Build.LazyPath.relative("deps/glfw-3.4/include"));
    exe.linkLibrary(glfw);
    exe.addIncludePath(std.Build.LazyPath.relative("deps/rapidjson/include"));
    exe.addIncludePath(std.Build.LazyPath.relative("deps/SDL_image/include"));
    exe.addIncludePath(std.Build.LazyPath.relative("deps/SDL/include"));
    exe.addIncludePath(std.Build.LazyPath.relative("deps/zlib/zlib"));
    exe.addCSourceFiles(&.{
        "src/Camera.cpp",
        "src/defines.cpp",
        "src/EventHandler.cpp",
        "src/indexBuffer.cpp",
        "src/Mesh.cpp",
        "src/Player.cpp",
        "src/vertexBuffer.cpp",
        "src/Window.cpp",
        "src/Graphics/FrameBuffer.cpp",
        "src/Graphics/LineBatch.cpp",
        "src/Graphics/MasterHandler.cpp",
        "src/Graphics/SceneHandler.cpp",
        "src/Graphics/Shader.cpp",
        "src/Graphics/Skybox.cpp",
        "src/Graphics/Texture.cpp",
        "src/Graphics/VoxelRenderer.cpp",
        "src/GUI/Button.cpp",
        "src/GUI/DynamicText.cpp",
        "src/GUI/GUI.cpp",
        "src/GUI/GUIHandler.cpp",
        "src/GUI/GUIObject.cpp",
        "src/GUI/Sprite.cpp",
        "src/GUI/Text.cpp",
        "src/Loaders/pngLoader.cpp",
        "src/Loaders/spng.c",
        "src/Voxels/BlockStore.cpp",
        "src/Voxels/Chunk.cpp",
        "src/Voxels/Chunks.cpp",
        "src/Voxels/Terrarian.cpp",
        "src/Voxels/Voxel.cpp",
    }, &.{ "-Wall" });

    exe.linkLibCpp();
    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());
    const run_step = b.step("run", "Run the game");
    run_step.dependOn(&run_cmd.step);
}
