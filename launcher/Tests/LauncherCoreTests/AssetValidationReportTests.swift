import Foundation
import Testing
@testable import LauncherCore

/// APP-01-ASSET-VALIDATOR-REPORT-01: ordered, complete asset problem reporting.
struct AssetValidationReportTests {
    private static let psarcMagic = Data("PSAR....".utf8)

    private func makeDirectory() throws -> URL {
        let dir = FileManager.default.temporaryDirectory.appending(path: UUID().uuidString, directoryHint: .isDirectory)
        try FileManager.default.createDirectory(at: dir, withIntermediateDirectories: true)
        return dir
    }

    private func write(_ archives: [String], into dir: URL, magic: Data = psarcMagic) throws {
        for name in archives {
            try magic.write(to: dir.appending(path: name, directoryHint: .notDirectory))
        }
    }

    @Test func emptyDirectoryReportsSixProblemsInRequiredOrder() throws {
        let dir = try makeDirectory()
        defer { try? FileManager.default.removeItem(at: dir) }

        let report = AssetValidator().report(directory: dir)

        #expect(report.problems == AssetValidator.requiredArchives.map { "\($0) not found" })
        #expect(report.problems.count == 6)
        #expect(report.missingArchives == AssetValidator.requiredArchives)
        #expect(!report.isReady)
        #expect(report.status == .missing(report.problems.joined(separator: "; ")))
        // The public contract is unchanged.
        #expect(AssetValidator().validate(directory: dir) == report.status)
    }

    @Test func onePresentButNotPsarcIsTheOnlyProblem() throws {
        let dir = try makeDirectory()
        defer { try? FileManager.default.removeItem(at: dir) }
        try write(AssetValidator.requiredArchives, into: dir)
        try Data("NOPE....".utf8).write(to: dir.appending(path: "pak0.psarc", directoryHint: .notDirectory))

        let report = AssetValidator().report(directory: dir)

        #expect(report.problems == ["pak0.psarc is not a PSARC archive"])
        #expect(report.missingArchives.isEmpty) // it exists; it is just not an archive
        #expect(report.status == .missing("pak0.psarc is not a PSARC archive"))
    }

    @Test func shortMagicHeaderIsNotMissingButIsNotPsarc() throws {
        let dir = try makeDirectory()
        defer { try? FileManager.default.removeItem(at: dir) }
        try write(AssetValidator.requiredArchives, into: dir)
        try Data("PS".utf8).write(to: dir.appending(path: "pak0.psarc", directoryHint: .notDirectory))

        let report = AssetValidator().report(directory: dir)

        #expect(report.problems == ["pak0.psarc is not a PSARC archive"])
        #expect(report.missingArchives.isEmpty)
    }

    @Test func completeValidSetIsReadyWithNoProblems() throws {
        let dir = try makeDirectory()
        defer { try? FileManager.default.removeItem(at: dir) }
        try write(AssetValidator.requiredArchives, into: dir)

        let report = AssetValidator().report(directory: dir)

        #expect(report.problems.isEmpty)
        #expect(report.missingArchives.isEmpty)
        #expect(report.isReady)
        #expect(report.status == .ready)
        #expect(AssetValidator().validate(directory: dir) == .ready)
    }

    @Test func directoryNamedLikeArchiveIsNotAcceptedAsAFile() throws {
        let dir = try makeDirectory()
        defer { try? FileManager.default.removeItem(at: dir) }

        let others = AssetValidator.requiredArchives.filter { $0 != "cachedFiles.psarc" }
        try write(others, into: dir)

        // A directory wearing the archive's name, itself containing a real PSAR file.
        let impostor = dir.appending(path: "cachedFiles.psarc", directoryHint: .isDirectory)
        try FileManager.default.createDirectory(at: impostor, withIntermediateDirectories: false)
        try Self.psarcMagic.write(to: impostor.appending(path: "cachedFiles.psarc", directoryHint: .notDirectory))

        let report = AssetValidator().report(directory: dir)

        #expect(report.problems == ["cachedFiles.psarc not found"])
        #expect(report.missingArchives == ["cachedFiles.psarc"])
        #expect(AssetValidator().validate(directory: dir) == .missing("cachedFiles.psarc not found"))
    }

    @Test func unreadableArchiveIsReportedAsNotReadable() throws {
        let dir = try makeDirectory()
        defer { try? FileManager.default.removeItem(at: dir) }
        try write(AssetValidator.requiredArchives, into: dir)

        let target = dir.appending(path: "cachedFiles.psarc", directoryHint: .notDirectory)
        let path = target.path(percentEncoded: false)
        try FileManager.default.setAttributes([.posixPermissions: 0o000], ofItemAtPath: path)
        defer { try? FileManager.default.setAttributes([.posixPermissions: 0o644], ofItemAtPath: path) }

        // Root (or a filesystem ignoring mode bits) can still open the file, so the condition
        // cannot be simulated there. Skip honestly instead of asserting a fake result.
        guard (try? FileHandle(forReadingFrom: target)) == nil else { return }

        let report = AssetValidator().report(directory: dir)
        #expect(report.problems == ["cachedFiles.psarc is not readable"])
        #expect(report.missingArchives.isEmpty)
        #expect(report.status == .missing("cachedFiles.psarc is not readable"))
    }
}
