const std = @import("std");

pub fn build(b: *std.Build) void {
    const exe = b.addExecutable(.{
        .name = "terramine",
        .root_source_file = .{ .path = "src/main.cpp" },
    });

    exe.linkLibCpp();
    b.installArtifact(exe);
}
