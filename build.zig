const std = @import("std");

// This build file has some code duplication. Sorry.
// But it's good enough for now.
// I might or might not refactor that at some later point.

const targets = [_]std.Target.Query{
    .{ .cpu_arch = .aarch64, .os_tag = .macos, .os_version_min = .{ .semver = .{ .major = 11, .minor = 0, .patch = 0 } } },
    .{ .cpu_arch = .aarch64, .os_tag = .windows },
    .{ .cpu_arch = .x86, .os_tag = .windows },
    .{ .cpu_arch = .x86_64, .os_tag = .macos, .os_version_min = .{ .semver = .{ .major = 10, .minor = 7, .patch = 0 } } },
    .{ .cpu_arch = .x86_64, .os_tag = .windows },
};

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const dest_dir = switch (target.result.os.tag) {
        .macos => "Library/Audio/Plug-Ins/LV2",
        .windows => "Program Files/Common Files/LV2",
        else => "lib/lv2",
    };

    const all = b.step("all", "builds the plugins for multiple os/cpu combinations");
    const dist = b.step("dist", "builds the plugins ready for distribution");

    // Substitution tool required during build
    const subst_mod = b.createModule(.{
        .root_source_file = b.path("substitute.zig"),
        .target = b.resolveTargetQuery(.{}),
    });
    const subst_exe = b.addExecutable(.{
        .name = "subst",
        .root_module = subst_mod,
    });

    // Extract version number
    var version = try std.ArrayList(u8).initCapacity(b.allocator, 5);
    try extractVersion(b, &version);
    try all.addWatchInput(b.path("meson.build"));

    var zip_found = true;
    _ = b.findProgram(&.{"zip"}, &.{}) catch {
        zip_found = false;
        std.debug.print("Warning: zip not found\n", .{});
    };
    var tar_found = true;
    _ = b.findProgram(&.{"tar"}, &.{}) catch {
        tar_found = false;
        std.debug.print("Warning: tar not found\n", .{});
    };

    var src_dir = try b.build_root.handle.openDir("src", .{ .iterate = true });
    defer src_dir.close();
    var it = src_dir.iterate();
    while (try it.next()) |file| {
        if (file.kind != .directory) {
            continue;
        }

        var dest_sub_path_lib = b.pathJoin(&.{
            "Airwindows.lv2",
            try std.mem.join(b.allocator, "", &.{
                file.name,
                target.result.dynamicLibSuffix(),
            }),
        });
        const dest_sub_path_ttl = b.pathJoin(&.{
            "Airwindows.lv2",
            try std.mem.join(b.allocator, "", &.{
                file.name,
                ".ttl",
            }),
        });

        // Default
        b.getInstallStep().dependOn(&b.addInstallArtifact(try buildShared(b, target, optimize, file.name), .{
            .dest_dir = .{ .override = .{ .custom = dest_dir } },
            .dest_sub_path = dest_sub_path_lib,
            .pdb_dir = .disabled,
            .implib_dir = .disabled,
        }).step);
        b.getInstallStep().dependOn(&b.addInstallFileWithDir(b.path(b.pathJoin(&.{
            "src",
            file.name,
            try std.mem.join(b.allocator, "", &.{ file.name, ".ttl" }),
        })), .{ .custom = dest_dir }, dest_sub_path_ttl).step);

        // Cross build
        for (targets) |cross_target| {
            const resolved_cross_target = b.resolveTargetQuery(cross_target);
            const cross_dest_dir = b.pathJoin(&.{ "cross", try crossName(b, version.items, resolved_cross_target) });
            dest_sub_path_lib = b.pathJoin(&.{
                "Airwindows.lv2",
                try std.mem.join(b.allocator, "", &.{
                    file.name,
                    resolved_cross_target.result.dynamicLibSuffix(),
                }),
            });

            all.dependOn(&b.addInstallArtifact(try buildShared(b, resolved_cross_target, optimize, file.name), .{
                .dest_dir = .{ .override = .{ .custom = cross_dest_dir } },
                .dest_sub_path = dest_sub_path_lib,
                .pdb_dir = .disabled,
                .implib_dir = .disabled,
            }).step);
            all.dependOn(&b.addInstallFileWithDir(b.path(b.pathJoin(&.{
                "src",
                file.name,
                try std.mem.join(b.allocator, "", &.{ file.name, ".ttl" }),
            })), .{ .custom = cross_dest_dir }, dest_sub_path_ttl).step);
        }
    }

    // Default
    const configure_manifest = b.addRunArtifact(subst_exe);
    configure_manifest.addFileArg(b.path("src/manifest.ttl.in"));
    configure_manifest.addArgs(&.{ "@CMAKE_SHARED_LIBRARY_SUFFIX@", target.result.dynamicLibSuffix() });
    b.getInstallStep().dependOn(&b.addInstallFileWithDir(configure_manifest.captureStdOut(), .{ .custom = dest_dir }, "Airwindows.lv2/manifest.ttl").step);

    // Cross build
    for (targets) |cross_target| {
        const resolved_cross_target = b.resolveTargetQuery(cross_target);
        const cross_name = try crossName(b, version.items, resolved_cross_target);
        const cross_dest_dir = b.pathJoin(&.{ "cross", cross_name });
        const configure_manifest_cross = b.addRunArtifact(subst_exe);
        configure_manifest_cross.addFileArg(b.path("src/manifest.ttl.in"));
        configure_manifest_cross.addArgs(&.{ "@CMAKE_SHARED_LIBRARY_SUFFIX@", resolved_cross_target.result.dynamicLibSuffix() });
        all.dependOn(&b.addInstallFileWithDir(configure_manifest_cross.captureStdOut(), .{ .custom = cross_dest_dir }, "Airwindows.lv2/manifest.ttl").step);

        const install_license = b.addInstallFile(b.path("LICENSE"), b.pathJoin(&.{cross_dest_dir, "LICENSE"}));

        switch (resolved_cross_target.result.os.tag) {
            .macos => {
                if (tar_found) {
                    const tar = b.addSystemCommand(&.{"tar"});
                    tar.setCwd(.{ .cwd_relative = b.pathJoin(&.{ b.install_path, "cross" }) });
                    tar.addArgs(&.{"-czf"});
                    tar.addArg(try std.mem.join(b.allocator, "", &.{ cross_name, ".tar.gz" }));
                    tar.addArg(cross_name);
                    tar.has_side_effects = true;
                    tar.step.dependOn(all);
                    tar.step.dependOn(&install_license.step);
                    dist.dependOn(&tar.step);
                }
            },
            .windows => {
                if (zip_found) {
                    const zip = b.addSystemCommand(&.{"zip"});
                    zip.setCwd(.{ .cwd_relative = b.pathJoin(&.{ b.install_path, "cross" }) });
                    zip.addArgs(&.{ "-9", "-r" });
                    zip.addArg(try std.mem.join(b.allocator, "", &.{ cross_name, ".zip" }));
                    zip.addArg(cross_name);
                    zip.has_side_effects = true;
                    zip.step.dependOn(all);
                    zip.step.dependOn(&install_license.step);
                    dist.dependOn(&zip.step);
                }
            },
            else => unreachable,
        }
    }
}

fn buildShared(b: *std.Build, target: std.Build.ResolvedTarget, optimize: std.builtin.OptimizeMode, name: []const u8) !*std.Build.Step.Compile {
    const mod = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libc = true,
        .strip = true,
    });
    mod.addCSourceFile(.{ .file = b.path(b.pathJoin(&.{
        "src",
        name,
        try std.mem.join(b.allocator, "", &.{ name, ".c" }),
    })) });
    mod.addIncludePath(.{ .cwd_relative = "/usr/include" });
    mod.linkSystemLibrary("m", .{});
    const lib = b.addLibrary(.{
        .name = name,
        .linkage = .dynamic,
        .root_module = mod,
    });
    return lib;
}

fn crossName(b: *std.Build, version: []const u8, target: std.Build.ResolvedTarget) ![]const u8 {
    return try std.mem.join(b.allocator, "", &.{
        "airwindows-lv2-", version, "-",
        switch (target.result.cpu.arch) {
            .aarch64 => "aarch64",
            .x86 => "i686",
            .x86_64 => "amd64",
            else => unreachable,
        },
        "-",
        switch (target.result.os.tag) {
            .macos => "macos",
            .windows => "windows",
            else => unreachable,
        },
    });
}

fn extractVersion(b: *std.Build, output: *std.ArrayList(u8)) !void {
    var buffer = try std.fs.cwd().readFileAlloc(b.allocator, "meson.build", 100000000);
    defer b.allocator.free(buffer);

    const idx = std.mem.indexOf(u8, buffer, "version : '").?;
    const idx_end = std.mem.indexOfPos(u8, buffer, idx + 11, "'").?;
    try output.appendSlice(b.allocator, buffer[idx + 11 .. idx_end]);
}
