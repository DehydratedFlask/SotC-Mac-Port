import Foundation
import Testing
@testable import LauncherCore
@testable import LauncherNativeCore

/// Integration tests over the real C ABI (native/api/sotc_core.cpp compiled into this package).
struct NativeCoreClientTests {
    private func makeDirectory(_ files: [String: Data]) throws -> URL {
        let dir = FileManager.default.temporaryDirectory.appending(path: UUID().uuidString, directoryHint: .isDirectory)
        try FileManager.default.createDirectory(at: dir, withIntermediateDirectories: true)
        for (name, data) in files {
            try data.write(to: dir.appending(path: name, directoryHint: .notDirectory))
        }
        return dir
    }

    private func completeSet() -> [String: Data] {
        Dictionary(uniqueKeysWithValues: AssetValidator.requiredArchives.map { ($0, Data("PSAR....".utf8)) })
    }

    @Test func speaksAbiOneAndReportsItself() async throws {
        let core = try NativeCoreClient()
        #expect(core.abiVersion.major == 1)
        let coverage = try await core.coverage()
        #expect(coverage.abiVersion == core.abiVersion)
    }

    @Test func startNeverRunsAndReturnsEngineNotImplemented() async throws {
        let dir = try makeDirectory(completeSet())
        defer { try? FileManager.default.removeItem(at: dir) }
        let core = try NativeCoreClient()

        let validation = try await core.validate(assetsDirectory: dir)
        #expect(validation.assetsReady)
        #expect(validation.engineReady == false)
        #expect(validation.startWouldSucceed == false)
        #expect(validation.engineError == .engineNotImplemented)
        #expect(try await core.status().state == .ready)

        do {
            try await core.start()
            Issue.record("start must throw while the core has no engine")
        } catch let failure as CoreFailure {
            #expect(failure.code == .engineNotImplemented)
            #expect(failure.message.contains("not implemented"))
        }
        let status = try await core.status()
        #expect(status.state == .failed)
        #expect(status.lastError == .engineNotImplemented)
        #expect(status.engineImplemented == false)
        #expect(CoreClientMapping.launcherState(for: status) != .running)

        for _ in 0..<20 {
            await #expect(throws: CoreFailure.self) { try await core.start() }
            #expect(try await core.status().state != .running)
        }
        try await core.stop()  // idempotent no-op
        #expect(try await core.status().state == .failed)
    }

    @Test func startBeforeValidationIsBadStateNotReady() async throws {
        let core = try NativeCoreClient()
        do {
            try await core.start()
            Issue.record("expected failure")
        } catch let failure as CoreFailure {
            #expect(failure.code == .badState)
        }
    }

    @Test func validationMatchesTheSwiftAssetValidator() async throws {
        var files = completeSet()
        files["pak1.psarc"] = Data("XXXX....".utf8)
        files["pakN.psarc"] = nil
        files["pak3.psarc"] = nil
        let dir = try makeDirectory(files)
        defer { try? FileManager.default.removeItem(at: dir) }

        let swiftReport = AssetValidator().report(directory: dir)
        let core = try NativeCoreClient()
        let validation = try await core.validate(assetsDirectory: dir)

        #expect(validation.assetsReady == false)
        #expect(validation.verdict == .assetsNotReady)
        #expect(validation.archives.map(\.name) == AssetValidator.requiredArchives)
        #expect(validation.problemCount == swiftReport.problems.count)
        #expect(validation.archives.filter { $0.status == .missing }.map(\.name) == swiftReport.missingArchives)
        #expect(validation.detail == swiftReport.problems.first)
        #expect(validation.archives.first { $0.name == "pak1.psarc" }?.status == .badMagic)
        #expect(try await core.status().state == .idle)

        do {
            try await core.start()
            Issue.record("expected failure")
        } catch let failure as CoreFailure {
            #expect(failure.code == .assetsNotReady)
        }
    }

    @Test func completeSetMatchesAndReportsSizes() async throws {
        let dir = try makeDirectory(completeSet())
        defer { try? FileManager.default.removeItem(at: dir) }
        let validation = try await NativeCoreClient().validate(assetsDirectory: dir)
        #expect(validation.problemCount == 0 && validation.verdict == .ok)
        #expect(validation.archives.allSatisfy { $0.status == .ok && $0.sizeBytes == 8 })
        #expect(AssetValidator().validate(directory: dir) == .ready)
    }

    @Test func archiveStatusNumericCodesMapToAbiCases() {
        #expect(CoreArchiveCheck.Status(code: 0) == .ok)
        #expect(CoreArchiveCheck.Status(code: 1) == .missing)
        #expect(CoreArchiveCheck.Status(code: 2) == .unreadable)
        #expect(CoreArchiveCheck.Status(code: 3) == .badMagic)
        #expect(CoreArchiveCheck.Status(code: 9) == .unknown(9))
        #expect(CoreArchiveCheck.Status(code: UInt32.max) == .unknown(UInt32.max))
    }

    @Test func nonDirectoryAssetsPathIsAnIoVerdict() async throws {
        let validation = try await NativeCoreClient().validate(assetsDirectory: URL(filePath: "/nonexistent/sotc-launcher-test"))
        #expect(validation.verdict == .io)
        #expect(validation.assetsReady == false)
        #expect(validation.problemCount == AssetValidator.requiredArchives.count)
    }

    @Test func coverageIsHonestAboutWhatIsMissing() async throws {
        let coverage = try await NativeCoreClient().coverage()
        #expect(coverage.engineImplemented == false)
        #expect(coverage.subsystems.map(\.name).contains("engine_boot"))
        #expect(coverage.subsystems.allSatisfy { $0.status != .implemented })
        #expect(coverage.subsystems.first { $0.name == "engine_boot" }?.status == .notImplemented)
        for name in ["metal_renderer", "audio"] {
            #expect(coverage.subsystems.first { $0.name == name }?.status == .notImplemented)
        }
        // The fixture-verified save transaction/metadata is real code, so the row is PARTIAL; it is
        // still not a platform save or a real PS4 acceptance.
        #expect(coverage.subsystems.first { $0.name == "save_load" }?.status == .partial)
        // This package links the ABI facade only; the constructor runner is reported as not linked.
        #expect(coverage.ctor == .notLinked)
        #expect(coverage.subsystems.first { $0.name == "startup_constructors" }?.status == .notLinked)
    }

    @Test func realCoreAnswersFeedTheLauncherRowAsNotBuilt() async throws {
        let dir = try makeDirectory(completeSet())
        defer { try? FileManager.default.removeItem(at: dir) }
        let core = try NativeCoreClient()
        let validation = try await core.validate(assetsDirectory: dir)
        let coverage = try await core.coverage()
        guard case .notBuilt(let message) = CoreClientMapping.nativeCoreStatus(validation: validation, coverage: coverage) else {
            Issue.record("expected .notBuilt")
            return
        }
        #expect(message.contains("Play disabled"))
    }

    @MainActor
    @Test func launcherModelOverTheRealCoreNeverPlaysAndShowsCoverage() async throws {
        let dir = try makeDirectory(completeSet())
        defer { try? FileManager.default.removeItem(at: dir) }
        let model = LauncherModel(
            client: try NativeCoreClient(),
            hostChecks: HostChecks(metal: { .ready }, audio: { .ready }),
            pollInterval: .milliseconds(5)
        )
        model.assetsDirectory = dir
        await model.validate()

        #expect(model.state == .idle)
        #expect(!model.canStart)
        #expect(model.report?.items.first { $0.kind == .assets }?.status == .ready)
        #expect(model.presentation.status.contains("NOT BUILT"))
        await model.startCore()  // gated: must not reach the core nor change state
        #expect(model.state == .idle)
        #expect(model.diagnosticsText.contains("Engine implemented: NO"))
        #expect(model.diagnosticsText.contains("Subsystem engine_boot: NOT IMPLEMENTED"))
        #expect(model.diagnosticsText.contains("Constructor prefix: NOT LINKED in this build"))
    }
}
