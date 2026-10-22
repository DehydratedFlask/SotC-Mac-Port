import Foundation
import Testing
@testable import LauncherCore

struct SystemChecksArchitectureTests {
    private static func le(_ words: [UInt32]) -> Data {
        var data = Data()
        for w in words { withUnsafeBytes(of: w.littleEndian) { data.append(contentsOf: $0) } }
        return data
    }

    private static func probe(_ header: Data) throws -> ReadinessItem.Status {
        let root = FileManager.default.temporaryDirectory.appending(path: UUID().uuidString, directoryHint: .isDirectory)
        try FileManager.default.createDirectory(at: root, withIntermediateDirectories: true)
        defer { try? FileManager.default.removeItem(at: root) }
        let executable = root.appending(path: "SoCGameCore", directoryHint: .notDirectory)
        try header.write(to: executable)
        try FileManager.default.setAttributes([.posixPermissions: 0o755], ofItemAtPath: executable.path(percentEncoded: false))
        return SystemChecks.nativeCore(executable: executable)
    }

    @Test func rejectsExecutable32BitI386MachOHeader() throws {
        let status = try Self.probe(Self.le([0xFEEDFACE, 7, 3, 2, 0, 0, 0]))
        guard case .notBuilt = status else { Issue.record("expected i386 Mach-O to remain not built"); return }
    }

    @Test func acceptsThin64BitARM64Executable() throws {
        let status = try Self.probe(Self.le([0xFEEDFACF, 0x0100000C, 0, 2, 0, 0, 0, 0]))
        guard case .ready = status else { Issue.record("expected thin ARM64 MH_EXECUTE to be accepted, got \(status)"); return }
    }

    @Test func rejectsX8664ThinExecutable() throws {
        let status = try Self.probe(Self.le([0xFEEDFACF, 0x01000007, 3, 2, 0, 0, 0, 0]))
        guard case .notBuilt = status else { Issue.record("expected x86_64 Mach-O to remain not built"); return }
    }

    @Test func rejectsARM64NonExecutableFileType() throws {
        let status = try Self.probe(Self.le([0xFEEDFACF, 0x0100000C, 0, 6, 0, 0, 0, 0])) // MH_DYLIB
        guard case .notBuilt = status else { Issue.record("expected ARM64 dylib to remain not built"); return }
    }

    @Test func rejectsFatUniversalFile() throws {
        // FAT_MAGIC is big-endian 0xCAFEBABE on disk.
        let fat = Data([0xCA, 0xFE, 0xBA, 0xBE, 0, 0, 0, 1, 0x01, 0, 0, 0x0C, 0, 0, 0, 0])
        let status = try Self.probe(fat)
        guard case .notBuilt = status else { Issue.record("expected fat file to remain not built"); return }
    }

    @Test func rejectsTruncatedHeader() throws {
        let status = try Self.probe(Self.le([0xFEEDFACF, 0x0100000C]))
        guard case .notBuilt = status else { Issue.record("expected truncated header to remain not built"); return }
    }
}
