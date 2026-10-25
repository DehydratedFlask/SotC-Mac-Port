import Foundation
import Testing
@testable import LauncherCore

/// End-to-end protocol check against the **real** native `SoCGameCore` executable.
///
/// The test is enabled only when `SOTC_CORE_PATH` points at the built core, so the normal hermetic
/// suite never depends on a native build. Run it manually after building `soc_game_core`:
///
///     SOTC_CORE_PATH="$PWD/local/build/orch-r22/SoCGameCore" \
///       swift test --package-path launcher --scratch-path local/build/launcher \
///       --filter RealCoreIntegrationTests
///
/// It proves the Swift client and the C++ executable agree on protocol v1. It proves nothing about
/// an engine: the core must still report `engineImplemented == false` and refuse `start`.
@Suite("Real SoCGameCore protocol")
struct RealCoreIntegrationTests {
    private static var corePath: String? {
        guard let path = ProcessInfo.processInfo.environment["SOTC_CORE_PATH"], !path.isEmpty else {
            return nil
        }
        return path
    }

    @Test(.enabled(if: corePath != nil))
    func realCoreAnswersTheProtocolAndRefusesStart() async throws {
        let path = try #require(Self.corePath)
        let client = try ProcessCoreClient(coreURL: URL(filePath: path))
        defer { client.shutdown() }

        let status = try await client.status()
        #expect(status.engineImplemented == false)
        #expect(status.ctor.linked)
        #expect(status.ctor.originalSlots == 573)
        #expect(status.ctor.slotsRegistered > 0)
        #expect(status.ctor.slotsRegistered == status.ctor.slotsWithCallback)
        // The index moves as the recovery batches land; the protocol contract is that it is
        // reported, not a particular value.
        #expect(status.ctor.firstUnrecoveredIndex != nil)
        #expect(status.ctor.runOK == (status.ctor.blockedOnRVA == 0 && !status.ctor.callbackFailed))

        let coverage = try await client.coverage()
        #expect(coverage.abiVersion.major == 1)
        #expect(coverage.engineImplemented == false)
        #expect(coverage.subsystems.map(\.name) == [
            "startup_constructors", "engine_boot", "metal_renderer", "audio", "save_load",
        ])
        #expect(coverage.subsystems.allSatisfy { $0.status != .implemented })
        #expect(coverage.subsystems.first { $0.name == "startup_constructors" }?.status == .partial)
        #expect(coverage.subsystems.first { $0.name == "save_load" }?.status == .partial)

        // The launcher's start path validates first. Synthetic PSAR headers are enough for the
        // container checks; they are not game data and are removed immediately.
        let assets = FileManager.default.temporaryDirectory
            .appending(path: "RealCoreIntegrationTests-\(UUID().uuidString)", directoryHint: .isDirectory)
        try FileManager.default.createDirectory(at: assets, withIntermediateDirectories: true)
        defer { try? FileManager.default.removeItem(at: assets) }
        for name in AssetValidator.requiredArchives {
            try Data("PSARxxxx".utf8).write(to: assets.appending(path: name))
        }
        let validation = try await client.validate(assetsDirectory: assets)
        #expect(validation.verdict == .ok)
        #expect(validation.assetsReady)
        #expect(validation.engineReady == false)
        #expect(validation.startWouldSucceed == false)

        do {
            try await client.start()
            Issue.record("the real core accepted start without an engine")
        } catch let failure as CoreFailure {
            #expect(failure.code == .engineNotImplemented)
        }

        let after = try await client.status()
        #expect(after.state == .failed)
        #expect(after.lastError == .engineNotImplemented)
        #expect(after.engineImplemented == false)

        try await client.stop()
    }
}
