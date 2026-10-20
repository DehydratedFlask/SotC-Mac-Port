import Foundation
import Testing
@testable import LauncherCore

struct AssetValidatorTests {
    private func makeDirectory(_ files: [String: Data]) throws -> URL {
        let dir = FileManager.default.temporaryDirectory.appending(path: UUID().uuidString, directoryHint: .isDirectory)
        try FileManager.default.createDirectory(at: dir, withIntermediateDirectories: true)
        for (name, data) in files {
            try data.write(to: dir.appending(path: name, directoryHint: .notDirectory))
        }
        return dir
    }

    @Test func acceptsCompleteSet() throws {
        let files = Dictionary(uniqueKeysWithValues: AssetValidator.requiredArchives.map { ($0, Data("PSAR....".utf8)) })
        let dir = try makeDirectory(files)
        defer { try? FileManager.default.removeItem(at: dir) }
        #expect(AssetValidator().validate(directory: dir) == .ready)
    }

    @Test func reportsMissingArchive() throws {
        var files = Dictionary(uniqueKeysWithValues: AssetValidator.requiredArchives.map { ($0, Data("PSAR....".utf8)) })
        files["pakN.psarc"] = nil
        let dir = try makeDirectory(files)
        defer { try? FileManager.default.removeItem(at: dir) }
        guard case .missing(let message) = AssetValidator().validate(directory: dir) else {
            Issue.record("expected .missing")
            return
        }
        #expect(message.contains("pakN.psarc not found"))
    }

    @Test func rejectsWrongMagic() throws {
        var files = Dictionary(uniqueKeysWithValues: AssetValidator.requiredArchives.map { ($0, Data("PSAR....".utf8)) })
        files["pak0.psarc"] = Data("NOPE....".utf8)
        let dir = try makeDirectory(files)
        defer { try? FileManager.default.removeItem(at: dir) }
        guard case .missing(let message) = AssetValidator().validate(directory: dir) else {
            Issue.record("expected .missing")
            return
        }
        #expect(message.contains("pak0.psarc is not a PSARC archive"))
    }

    @Test func rejectsDirectoryPresentedAsArchive() throws {
        var files = Dictionary(uniqueKeysWithValues: AssetValidator.requiredArchives.map { ($0, Data("PSAR....".utf8)) })
        files["pak1.psarc"] = nil
        let dir = try makeDirectory(files)
        defer { try? FileManager.default.removeItem(at: dir) }
        try FileManager.default.createDirectory(
            at: dir.appending(path: "pak1.psarc", directoryHint: .isDirectory), withIntermediateDirectories: false)
        guard case .missing(let message) = AssetValidator().validate(directory: dir) else {
            Issue.record("expected .missing")
            return
        }
        #expect(message.contains("pak1.psarc not found"))
    }

    @Test func nativeCoreIsNeverFaked() {
        #expect(SystemChecks.nativeCore(executable: nil) != .ready)
        #expect(SystemChecks.nativeCore(executable: URL(filePath: "/nonexistent/core")) != .ready)
        guard case .notBuilt(let message) = SystemChecks.nativeCore(executable: nil) else {
            Issue.record("expected .notBuilt for nil core")
            return
        }
        #expect(message.contains("NOT BUILT"))
        #expect(message.contains("Play disabled"))
    }

    @Test func nativeCoreRejectsScriptStub() throws {
        let dir = FileManager.default.temporaryDirectory.appending(path: UUID().uuidString, directoryHint: .isDirectory)
        try FileManager.default.createDirectory(at: dir, withIntermediateDirectories: true)
        defer { try? FileManager.default.removeItem(at: dir) }
        let stub = dir.appending(path: "SoCGameCore", directoryHint: .notDirectory)
        try Data("#!/bin/sh\necho fake\n".utf8).write(to: stub)
        try FileManager.default.setAttributes([.posixPermissions: 0o755], ofItemAtPath: stub.path(percentEncoded: false))
        guard case .notBuilt(let message) = SystemChecks.nativeCore(executable: stub) else {
            Issue.record("expected .notBuilt for shell stub")
            return
        }
        #expect(message.contains("NOT BUILT"))
        #expect(message.contains("Mach-O") || message.contains("Play disabled"))
    }

    @Test func reportCannotStartWithoutAssetsOrCore() async {
        let report = await ReadinessReport.evaluate(assets: nil, coreExecutable: nil)
        #expect(report.items.map(\.kind) == ReadinessItem.Kind.allCases)
        #expect(!report.canStart)
        #expect(report.playBlockedSummary?.contains("NOT BUILT") == true)
    }

    @Test func reportCannotStartWithAssetsButNoCore() async throws {
        let files = Dictionary(uniqueKeysWithValues: AssetValidator.requiredArchives.map { ($0, Data("PSAR....".utf8)) })
        let dir = try makeDirectory(files)
        defer { try? FileManager.default.removeItem(at: dir) }
        let report = await ReadinessReport.evaluate(assets: dir, coreExecutable: nil)
        #expect(report.items.first { $0.kind == .assets }?.isReady == true)
        #expect(report.items.first { $0.kind == .nativeCore }?.isReady == false)
        #expect(!report.canStart)
        #expect(report.playBlockedSummary?.contains("NOT BUILT") == true)
    }
}
