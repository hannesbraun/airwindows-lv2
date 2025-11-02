const std = @import("std");

pub fn main() !void {
    var debug_allocator = std.heap.DebugAllocator(.{}).init;
    defer _ = debug_allocator.deinit();
    const allocator = debug_allocator.allocator();

    const input_path = std.os.argv[1];
    var buffer = try std.fs.cwd().readFileAlloc(allocator, std.mem.span(input_path), 100000000);

    for (0..((std.os.argv.len - 2) / 2)) |i| {
        const a = 2 + 2 * i;
        const b = 2 + 2 * i + 1;
        errdefer allocator.free(buffer);
        const tmp = try std.mem.replaceOwned(u8, allocator, buffer, std.mem.span(std.os.argv[a]), std.mem.span(std.os.argv[b]));
        allocator.free(buffer);
        buffer = tmp;
    }
    defer allocator.free(buffer);

    var stdout_writer = std.fs.File.stdout().writer(&.{});
    const stdout = &stdout_writer.interface;

    try stdout.writeAll(buffer);
    try stdout.flush();
}
