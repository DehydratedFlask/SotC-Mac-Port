import Foundation
@testable import LauncherCore

/// A runnable native slice of the host's harmless `true` command. Never launch its x86 slice.
enum NativeExecutableFixture {
    static func write(to destination: URL) throws {
        let lipo = Process()
        lipo.executableURL = URL(filePath: "/usr/bin/lipo")
        lipo.arguments = ["/usr/bin/true", "-thin", "arm64e", "-output", destination.path(percentEncoded: false)]
        lipo.standardOutput = FileHandle.nullDevice
        lipo.standardError = FileHandle.nullDevice
        try lipo.run()
        lipo.waitUntilExit()
        guard lipo.terminationStatus == 0 else {
            throw CoreFailure(code: .io, message: "Could not extract the host's native true fixture")
        }
        try FileManager.default.setAttributes([.posixPermissions: 0o755], ofItemAtPath: destination.path(percentEncoded: false))
    }
}
