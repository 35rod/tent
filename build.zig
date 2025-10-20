const std = @import("std");

const include_path: []const u8 = "include";
const lib_path: []const u8 = "lib";

const tent_exe_sources = [_][]const u8 {
    "src/main.cpp",
    "src/types.cpp",
    "src/native.cpp",
    "src/ast.cpp",
    "src/args.cpp",
    "src/lexer.cpp",
    "src/evaluator.cpp",
    "src/compiler.cpp",
    "src/vm.cpp",
    "src/parser.cpp",
    "src/errors.cpp",
    "src/token.cpp",
    "src/esc_codes.cpp",
};

const cxx_compiler_flags = [_][]const u8 {
    "-Wall",
    "-Wextra",
    "-Wpedantic",
};

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    var lib_dir = std.fs.cwd().openDir(lib_path, .{.access_sub_paths = true, .iterate = true}) catch {
        std.log.err("'" ++ lib_path ++ "' directory could not be opened. make sure it exists.", .{});
        return;
    };
    defer lib_dir.close();
    var it = lib_dir.iterate();
    while (it.next() catch {
        std.log.err("failed to open a file in the '" ++ lib_path ++ "' directory\n", .{});
        return;
    }) |entry| {
        if (entry.kind != .file or entry.name.len < 5 or !std.mem.eql(u8, entry.name[entry.name.len-4..], ".cpp")) continue;
        const full_path = b.fmt("lib/{s}", .{entry.name});
        
        var libentry_mod = b.createModule(.{
            .root_source_file = null,
            .target = target,
            .optimize = optimize,
            .link_libcpp = true,
        });
        const libentry = b.addLibrary(.{
            .name = entry.name[0..entry.name.len-4],
            .linkage = .dynamic,
            .root_module = libentry_mod,
        });
        libentry_mod.addIncludePath(b.path(include_path));
        libentry_mod.addCSourceFile(.{
            .file = b.path(full_path),
            .flags = &cxx_compiler_flags,
            .language = .cpp,
        });
        installArtifactOptions(b, libentry, .{
            .dest_dir = .{ .override = .{ .custom = "lib/tent" } },
        });
    }

    // EXECUTABLE
    var tent_exe_mod = b.createModule(.{
        .root_source_file = null, // C++ project
        .target = target,
        .optimize = optimize,
        .link_libcpp = true,
    });
    const tent_exe = b.addExecutable(.{
        .name = "tent",
        .root_module = tent_exe_mod,
    });
    tent_exe_mod.addIncludePath(b.path(include_path));
    tent_exe_mod.addCSourceFiles(.{
        .files = &tent_exe_sources,
        .flags = &cxx_compiler_flags,
        .language = .cpp, // dunno if this is necessary but i'll be specific
    });

    b.installArtifact(tent_exe);
}

fn installArtifactOptions(b: *std.Build, artifact: *std.Build.Step.Compile, options: std.Build.Step.InstallArtifact.Options) void {
    b.getInstallStep().dependOn(&b.addInstallArtifact(artifact, options).step);
}
