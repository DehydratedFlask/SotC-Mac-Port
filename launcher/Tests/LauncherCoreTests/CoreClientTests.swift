import Foundation
import Testing
@testable import LauncherCore

/// Scripted test double. It answers exactly what the script says, so tests can state what the
/// launcher does with each possible core answer. It is test-only on purpose: it is not part of
/// LauncherCore and must never be wired into the app (a fake core could fake "running").
actor FakeCoreClient: CoreClient {
    enum StartScript: Sendable {
        case engineNotImplemented
        case succeedAndRun   // a FUTURE core; used to prove the mapping is not hard-wired to failure
    }

    nonisolated let abiVersion = CoreABIVersion(major: 1, minor: 0)
    private var script: StartScript
    private var state: CoreState = .idle
    private var lastError: CoreErrorCode = .ok
    private var validated = false
    private var ready = false
    private(set) var calls: [String] = []

    init(start script: StartScript = .engineNotImplemented) { self.script = script }

    func validate(assetsDirectory: URL) async throws -> CoreValidation {
        calls.append("validate")
        validated = true
        ready = assetsDirectory.lastPathComponent == "good"
        state = ready ? .ready : .idle
        return CoreValidation(
            verdict: ready ? .ok : .assetsNotReady, assetsReady: ready, engineReady: script == .succeedAndRun,
            startWouldSucceed: ready && script == .succeedAndRun, problemCount: ready ? 0 : 6,
            engineError: script == .succeedAndRun ? .ok : .engineNotImplemented, archives: [], detail: ""
        )
    }

    func start() async throws {
        calls.append("start")
        guard ready else {
            lastError = validated ? .assetsNotReady : .badState
            throw CoreFailure(code: lastError, message: "not ready")
        }
        switch script {
        case .engineNotImplemented:
            state = .failed
            lastError = .engineNotImplemented
            throw CoreFailure(code: .engineNotImplemented, message: "engine not implemented")
        case .succeedAndRun:
            state = .running
        }
    }

    func stop() async throws {
        calls.append("stop")
        if state == .running { state = .ready }
    }

    func status() async throws -> CoreStatus {
        CoreStatus(
            state: state, lastError: lastError, assetsValidated: validated, assetsReady: ready,
            engineImplemented: script == .succeedAndRun, ctor: .notLinked
        )
    }

    func coverage() async throws -> CoreCoverageReport {
        CoreCoverageReport(
            abiVersion: abiVersion, engineImplemented: script == .succeedAndRun, ctor: .notLinked,
            subsystems: [CoreSubsystem(name: "engine_boot", status: script == .succeedAndRun ? .implemented : .notImplemented, detail: "")]
        )
    }
}

struct CoreClientTests {
    private let good = URL(filePath: "/tmp/good", directoryHint: .isDirectory)
    private let bad = URL(filePath: "/tmp/bad", directoryHint: .isDirectory)

    @Test func abiVersionPacksAndOrders() {
        let version = CoreABIVersion(major: 1, minor: 2)
        #expect(version.packed == 0x0001_0002)
        #expect(CoreABIVersion(packed: 0x0001_0002) == version)
        #expect(version.description == "1.2")
        #expect(CoreABIVersion(major: 1, minor: 9) < CoreABIVersion(major: 2, minor: 0))
    }

    @Test func abiVersionMaximumFieldsPackAndRoundTrip() {
        let maximum = CoreABIVersion(major: .max, minor: .max)

        #expect(maximum.packed == UInt32.max)
        #expect(CoreABIVersion(packed: UInt32.max) == maximum)
    }

    @Test func numericCodesMapToTypedValues() {
        #expect(CoreState(code: 0) == .idle)
        #expect(CoreState(code: 4) == .running)
        #expect(CoreState(code: 6) == .failed)
        #expect(CoreState(code: 7) == nil)
        #expect(CoreErrorCode(code: 4) == .engineNotImplemented)
        #expect(CoreErrorCode(code: 3) == .assetsNotReady)
        #expect(CoreErrorCode(code: 99) == .unknown(99))
        #expect(CoreArchiveCheck.Status(code: 3) == .badMagic)
        #expect(CoreArchiveCheck.Status(code: 9) == .unknown(9))
        #expect(CoreSubsystem.Status(code: 3) == .notLinked)
        #expect(CoreSubsystem.Status(code: 2) == .implemented)
        #expect(CoreSubsystem.Status(code: 99) == .unknown(99))
    }

    @Test func nativeCoreRowIsNotBuiltWhileEngineIsNotReady() async throws {
        let core = FakeCoreClient()
        let validation = try await core.validate(assetsDirectory: good)
        let coverage = try await core.coverage()
        #expect(validation.assetsReady)  // assets ready must not make the engine row ready
        guard case .notBuilt(let message) = CoreClientMapping.nativeCoreStatus(validation: validation, coverage: coverage) else {
            Issue.record("expected .notBuilt")
            return
        }
        #expect(message.hasPrefix("NOT BUILT:"))
        #expect(message.contains("Play disabled"))
        #expect(message.contains("1.0"))
    }

    @Test func unqueriedCoreIsNotBuiltNotReady() {
        guard case .notBuilt(let message) = CoreClientMapping.nativeCoreStatus(validation: nil, coverage: nil) else {
            Issue.record("expected .notBuilt")
            return
        }
        #expect(message.contains("Play disabled"))
    }

    @Test func failedCoreQueryWithoutValidationOrCoverageIsNotBuilt() {
        let failure = CoreFailure(code: .io, message: "status request failed")
        guard case .notBuilt(let message) = CoreReadiness.nativeCoreStatus(
            validation: nil, coverage: nil, queryFailure: failure
        ) else {
            Issue.record("expected .notBuilt")
            return
        }

        #expect(message == "NOT BUILT: the native core could not be queried (status request failed). Play disabled until a native SoCGameCore exists")
    }

    @Test func coverageWithoutValidationIncludesCoreABIVersion() {
        let coverage = CoreCoverageReport(
            abiVersion: CoreABIVersion(major: 1, minor: 2), engineImplemented: false,
            ctor: .notLinked, subsystems: []
        )
        guard case .notBuilt(let message) = CoreClientMapping.nativeCoreStatus(validation: nil, coverage: coverage) else {
            Issue.record("expected .notBuilt")
            return
        }
        #expect(message.contains("(core ABI 1.2)"))
    }

    @Test func startOnEngineLessCoreThrowsDistinctErrorAndNeverRuns() async throws {
        let core = FakeCoreClient()
        _ = try await core.validate(assetsDirectory: good)
        do {
            try await core.start()
            Issue.record("start must not succeed without an engine")
        } catch let failure as CoreFailure {
            #expect(failure.code == .engineNotImplemented)
        }
        let status = try await core.status()
        #expect(status.state == .failed)
        #expect(status.engineImplemented == false)
        let state = CoreClientMapping.launcherState(for: status)
        #expect(state != .running)
        guard case .failed(let text) = state else {
            Issue.record("expected .failed")
            return
        }
        #expect(text.contains("not implemented"))
        // Play stays disabled for the launcher as a whole.
        let validation = try await core.validate(assetsDirectory: good)
        let report = ReadinessReport(items: [
            ReadinessItem(kind: .assets, status: .ready),
            ReadinessItem(kind: .nativeCore, status: CoreClientMapping.nativeCoreStatus(validation: validation, coverage: nil)),
            ReadinessItem(kind: .metal, status: .ready),
            ReadinessItem(kind: .audio, status: .ready),
        ])
        #expect(!report.canStart)
    }

    @Test func startBeforeAssetsAreReadyReportsAssetsOrBadState() async throws {
        let core = FakeCoreClient()
        await #expect(throws: CoreFailure.self) { try await core.start() }
        _ = try await core.validate(assetsDirectory: bad)
        do {
            try await core.start()
            Issue.record("expected failure")
        } catch let failure as CoreFailure {
            #expect(failure.code == .assetsNotReady)
        }
    }

    @Test func everyCoreStateHasADefinedLauncherState() {
        func status(_ state: CoreState, engine: Bool = false, error: CoreErrorCode = .ok) -> CoreStatus {
            CoreStatus(state: state, lastError: error, assetsValidated: true, assetsReady: true, engineImplemented: engine, ctor: .notLinked)
        }
        #expect(CoreClientMapping.launcherState(for: status(.idle)) == .idle)
        #expect(CoreClientMapping.launcherState(for: status(.validating)) == .validating)
        #expect(CoreClientMapping.launcherState(for: status(.ready)) == .ready)
        #expect(CoreClientMapping.launcherState(for: status(.starting)) == .starting)
        #expect(CoreClientMapping.launcherState(for: status(.stopping)) == .stopping)
        #expect(CoreClientMapping.launcherState(for: status(.running, engine: true)) == .running)
        guard case .failed = CoreClientMapping.launcherState(for: status(.failed, error: .engineNotImplemented)) else {
            Issue.record("expected .failed")
            return
        }
        #expect(CoreClientMapping.launcherState(for: status(.failed, error: .engineNotImplemented), failureMessage: "synthetic status detail") == .failed("synthetic status detail"))
    }

    @Test func runningWithoutAnEngineIsRejectedAsFailure() {
        let impossible = CoreStatus(
            state: .running, lastError: .ok, assetsValidated: true, assetsReady: true,
            engineImplemented: false, ctor: .notLinked
        )
        guard case .failed = CoreClientMapping.launcherState(for: impossible) else {
            Issue.record("a core claiming running without an engine must not reach the UI as running")
            return
        }
    }

    @Test func aFutureEngineWouldBeReportedRunningOnlyBecauseTheCoreSaidSo() async throws {
        let core = FakeCoreClient(start: .succeedAndRun)
        _ = try await core.validate(assetsDirectory: good)
        try await core.start()
        let status = try await core.status()
        #expect(CoreClientMapping.launcherState(for: status) == .running)
        try await core.stop()
        #expect(try await core.status().state == .ready)
    }

    @Test func failureTextCoversTheErrorsTheLauncherCanSee() {
        #expect(CoreClientMapping.failureText(for: .engineNotImplemented).contains("not implemented"))
        #expect(CoreClientMapping.failureText(for: .assetsNotReady).contains("validation"))
        #expect(CoreClientMapping.failureText(for: .unknown(5)) == CoreClientMapping.failureText(for: .internalError))
    }
}
