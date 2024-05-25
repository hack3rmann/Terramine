const std = @import("std");

pub fn build(b: *std.Build) !void {
    const exe = b.addExecutable(.{
        .name = "terramine",
        .root_source_file = .{ .path = "src/main.cpp" },
    });
    exe.addIncludePath(std.Build.LazyPath.relative("deps/glm"));
    exe.addIncludePath(std.Build.LazyPath.relative("deps/glew-2.1.0/include"));
    exe.addIncludePath(std.Build.LazyPath.relative("deps/glfw-3.4/include"));
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
