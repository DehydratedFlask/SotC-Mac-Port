import Foundation
import Testing
@testable import LauncherCore

struct SystemChecksArchitectureTests {
    @Test func rejectsExecutable32BitI386MachOHeader() throws {
        let root = FileManager.default.temporaryDirectory.appending(path: UUID().uuidString, directoryHint: .isDirectory)
        try FileManager.default.createDirectory(at: root, withIntermediateDirectories: true)
        defer { try? FileManager.default.removeItem(at: root) }

        let executable = root.appending(path: "SoCGameCore", directoryHint: .notDirectory)
        let i386ExecutableHeader = Data([
            0xCE, 0xFA, 0xED, 0xFE, // MH_MAGIC (32-bit, little-endian)
            0x07, 0x00, 0x00, 0x00, // CPU_TYPE_I386
            0x03, 0x00, 0x00, 0x00, // CPU_SUBTYPE_I386_ALL
            0x02, 0x00, 0x00, 0x00, // MH_EXECUTE
            0x00, 0x00, 0x00, 0x00, // ncmds
            0x00, 0x00, 0x00, 0x00, // sizeofcmds
            0x00, 0x00, 0x00, 0x00, // flags
        ])
        try i386ExecutableHeader.write(to: executable)
        try FileManager.default.setAttributes(
            [.posixPermissions: 0o755],
            ofItemAtPath: executable.path(percentEncoded: false)
        )

        guard case .notBuilt = SystemChecks.nativeCore(executable: executable) else {
            Issue.record("expected a 32-bit i386 Mach-O to remain not built")
            return
        }
    }
}
