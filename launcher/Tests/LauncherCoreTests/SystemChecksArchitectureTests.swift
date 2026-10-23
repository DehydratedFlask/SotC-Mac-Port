import Foundation
import Testing
@testable import LauncherCore

/// APP-01-SYSTEM-CHECKS-ARCH-NOT-BUILT-COPY-FIXTURE-01
///
/// Pins honest Play-disabled copy for core Mach-O shape mismatches: fat, wrong CPU, wrong
/// file type, and 32-bit headers keep NOT BUILT / Play disabled wording and name the real
/// mismatch instead of claiming "not a Mach-O". Synthetic headers only; no UI/runtime claim.
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

    private static func expectNotBuilt(_ status: ReadinessItem.Status, exact: String, sourceLocation: SourceLocation = #_sourceLocation) {
        guard case .notBuilt(let message) = status else {
            Issue.record("expected .notBuilt, got \(status)", sourceLocation: sourceLocation)
            return
        }
        #expect(message == exact, sourceLocation: sourceLocation)
        #expect(message.contains("NOT BUILT"), sourceLocation: sourceLocation)
        #expect(message.contains("Play disabled"), sourceLocation: sourceLocation)
    }

    @Test func rejectsExecutable32BitI386MachOHeaderWithExactCopy() throws {
        let status = try Self.probe(Self.le([0xFEEDFACE, 7, 3, 2, 0, 0, 0]))
        Self.expectNotBuilt(
            status,
            exact: "NOT BUILT: SoCGameCore is a 32-bit Mach-O, not a thin 64-bit ARM64 executable. Play disabled until a native SoCGameCore exists"
        )
    }

    @Test func acceptsThin64BitARM64Executable() throws {
        let status = try Self.probe(Self.le([0xFEEDFACF, 0x0100000C, 0, 2, 0, 0, 0, 0]))
        guard case .ready = status else { Issue.record("expected thin ARM64 MH_EXECUTE to be accepted, got \(status)"); return }
    }

    @Test func rejectsX8664ThinExecutableWithExactCopy() throws {
        let status = try Self.probe(Self.le([0xFEEDFACF, 0x01000007, 3, 2, 0, 0, 0, 0]))
        Self.expectNotBuilt(
            status,
            exact: "NOT BUILT: SoCGameCore is not a thin ARM64 Mach-O executable (wrong CPU type). Play disabled until a native SoCGameCore exists"
        )
    }

    @Test func rejectsARM64NonExecutableFileTypeWithExactCopy() throws {
        let status = try Self.probe(Self.le([0xFEEDFACF, 0x0100000C, 0, 6, 0, 0, 0, 0])) // MH_DYLIB
        Self.expectNotBuilt(
            status,
            exact: "NOT BUILT: SoCGameCore is not a Mach-O executable file type (MH_EXECUTE required). Play disabled until a native SoCGameCore exists"
        )
    }

    @Test func rejectsFatUniversalFileWithExactCopy() throws {
        // FAT_MAGIC is big-endian 0xCAFEBABE on disk.
        let fat = Data([0xCA, 0xFE, 0xBA, 0xBE, 0, 0, 0, 1, 0x01, 0, 0, 0x0C, 0, 0, 0, 0])
        let status = try Self.probe(fat)
        Self.expectNotBuilt(
            status,
            exact: "NOT BUILT: SoCGameCore is a fat/universal Mach-O, not a thin ARM64 executable. Play disabled until a native SoCGameCore exists"
        )
    }

    @Test func rejectsTruncatedHeaderWithExactCopy() throws {
        let status = try Self.probe(Self.le([0xFEEDFACF, 0x0100000C]))
        Self.expectNotBuilt(
            status,
            exact: "NOT BUILT: SoCGameCore Mach-O header is truncated. Play disabled until a native SoCGameCore exists"
        )
    }

    @Test func wrongArchNativeCoreWinsPlayBlockedSummaryOverMissingAssets() throws {
        let status = try Self.probe(Self.le([0xFEEDFACF, 0x01000007, 3, 2, 0, 0, 0, 0]))
        guard case .notBuilt(let message) = status else {
            Issue.record("expected .notBuilt for x86_64 core")
            return
        }
        let report = ReadinessReport(items: [
            ReadinessItem(kind: .assets, status: .missing("Choose the folder with the extracted game archives")),
            ReadinessItem(kind: .nativeCore, status: status),
            ReadinessItem(kind: .metal, status: .ready),
            ReadinessItem(kind: .audio, status: .ready),
        ])
        #expect(!report.canStart)
        #expect(report.playBlockedSummary == message)
        #expect(report.playBlockedSummary?.contains("wrong CPU type") == true)
        #expect(report.playBlockedSummary?.contains("NOT BUILT") == true)
        #expect(report.playBlockedSummary?.contains("Play disabled") == true)
        #expect(report.playBlockedSummary?.hasPrefix("Game assets:") != true)
    }
}
