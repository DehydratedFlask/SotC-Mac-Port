import Foundation
import Testing
@testable import LauncherCore

/// Fully scripted `CoreClient` (a class, not an actor, so it can record which thread served a call).
/// Test-only: it can be told to lie ("engineReady", "running"), which is exactly why the app never uses it.
final class ScriptedCoreClient: CoreClient, @unchecked Sendable {
    enum StartResult: Sendable { case refuse(CoreErrorCode), accept }

    private let lock = NSLock()
    private var _calls: [String] = []
    private var _mainThreadCalls = 0
    private var _status: CoreStatus
    private var _stopError: CoreFailure?
    private var _statusError: CoreFailure?
    private var _validateError: CoreFailure?
    private var _coverageError: CoreFailure?
    let abiVersion = CoreABIVersion(major: 1, minor: 0)
    let engineReady: Bool
    let startResult: StartResult
    /// Status the core reports right after an accepted `start()`.
    let statusAfterAccept: CoreStatus

    static func status(
        _ state: CoreState, engine: Bool = false, error: CoreErrorCode = .ok, assetsReady: Bool = true
    ) -> CoreStatus {
        CoreStatus(
            state: state, lastError: error, assetsValidated: true, assetsReady: assetsReady,
            engineImplemented: engine, ctor: .notLinked
        )
    }

    init(engineReady: Bool = false, start: StartResult = .refuse(.engineNotImplemented), afterAccept: CoreStatus? = nil) {
        self.engineReady = engineReady
        self.startResult = start
        self.statusAfterAccept = afterAccept ?? Self.status(.running, engine: true)
        _status = Self.status(.idle, assetsReady: false)
    }

    var calls: [String] { lock.withLock { _calls } }
    var mainThreadCalls: Int { lock.withLock { _mainThreadCalls } }
    func count(_ name: String) -> Int { calls.filter { $0 == name }.count }
    func setStatus(_ value: CoreStatus) { lock.withLock { _status = value } }
    func failStatus(_ value: CoreFailure?) { lock.withLock { _statusError = value } }
    func failStop(_ value: CoreFailure?) { lock.withLock { _stopError = value } }
    func failValidate(_ value: CoreFailure?) { lock.withLock { _validateError = value } }
    func failCoverage(_ value: CoreFailure?) { lock.withLock { _coverageError = value } }

    private func note(_ name: String) {
        lock.withLock {
            _calls.append(name)
            if Thread.isMainThread { _mainThreadCalls += 1 }
        }
    }

    func validate(assetsDirectory: URL) async throws -> CoreValidation {
        note("validate")
        if let error = lock.withLock({ _validateError }) { throw error }
        let ready = assetsDirectory.lastPathComponent == "good"
        lock.withLock { _status = Self.status(ready ? .ready : .idle, engine: engineReady, assetsReady: ready) }
        return CoreValidation(
            verdict: ready ? .ok : .assetsNotReady, assetsReady: ready, engineReady: engineReady,
            startWouldSucceed: ready && engineReady, problemCount: ready ? 0 : 1,
            engineError: engineReady ? .ok : .engineNotImplemented, archives: [],
            detail: ready ? "" : "pak0.psarc not found"
        )
    }

    func start() async throws {
        note("start")
        switch startResult {
        case .refuse(let code):
            lock.withLock { _status = Self.status(.failed, error: code, assetsReady: true) }
            throw CoreFailure(code: code, message: "core refused start")
        case .accept:
            lock.withLock { _status = statusAfterAccept }
        }
    }

    func stop() async throws {
        note("stop")
        if let error = lock.withLock({ _stopError }) { throw error }
        lock.withLock { _status = Self.status(.ready, engine: engineReady) }
    }

    func status() async throws -> CoreStatus {
        note("status")
        if let error = lock.withLock({ _statusError }) { throw error }
        return lock.withLock { _status }
    }

    func coverage() async throws -> CoreCoverageReport {
        note("coverage")
        if let error = lock.withLock({ _coverageError }) { throw error }
        return CoreCoverageReport(
            abiVersion: abiVersion, engineImplemented: engineReady, ctor: .notLinked,
            subsystems: [
                CoreSubsystem(name: "engine_boot", status: engineReady ? .implemented : .notImplemented, detail: "no engine"),
                CoreSubsystem(name: "audio", status: .notImplemented, detail: ""),
            ]
        )
    }
}

@MainActor
private func makeModel(
    _ client: any CoreClient, directory: String? = "good",
    metal: ReadinessItem.Status = .ready, audio: ReadinessItem.Status = .ready
) -> LauncherModel {
    let model = LauncherModel(
        client: client, hostChecks: HostChecks(metal: { metal }, audio: { audio }), pollInterval: .milliseconds(5)
    )
    model.assetsDirectory = directory.map { URL(filePath: "/tmp/\($0)", directoryHint: .isDirectory) }
    return model
}

@MainActor
private func waitUntil(_ condition: @MainActor () -> Bool) async -> Bool {
    for _ in 0..<600 {
        if condition() { return true }
        try? await Task.sleep(for: .milliseconds(5))
    }
    return condition()
}

@MainActor
struct LauncherModelValidationTests {
    @Test func realisticCoreWithoutEngineIsIdleWithNativeCoreNotBuiltAndPlayDisabled() async throws {
        let client = ScriptedCoreClient()
        let model = makeModel(client)
        await model.validate()

        #expect(model.state == .idle)
        #expect(!model.canStart)
        let items = try #require(model.report).items
        #expect(items.first { $0.kind == .assets }?.status == .ready)  // assets ready must not hide the engine
        guard case .notBuilt(let message) = items.first(where: { $0.kind == .nativeCore })?.status else {
            Issue.record("native core row must be NOT BUILT")
            return
        }
        #expect(message.contains("Play disabled"))
        #expect(model.presentation.status.contains("NOT BUILT"))
        #expect(client.count("validate") == 1)
    }

    @Test func coverageReportIsSurfacedInDiagnostics() async {
        let model = makeModel(ScriptedCoreClient())
        await model.validate()
        let text = model.diagnosticsText
        #expect(text.contains("Game assets: READY"))
        #expect(text.contains("Native game core: NOT BUILT"))
        #expect(text.contains("Core ABI: 1.0"))
        #expect(text.contains("Engine implemented: NO"))
        #expect(text.contains("Constructor prefix: NOT LINKED in this build"))
        #expect(text.contains("Subsystem engine_boot: NOT IMPLEMENTED — no engine"))
        #expect(text.contains("Subsystem audio: NOT IMPLEMENTED"))
        #expect(text.contains("Core state: ready; last error: OK"))
        #expect(text.contains("Play is disabled:"))
    }

    @Test func noFolderChosenNeverAsksTheCoreToValidate() async throws {
        let client = ScriptedCoreClient()
        let model = makeModel(client, directory: nil)
        await model.validate()
        #expect(client.count("validate") == 0)
        #expect(client.count("coverage") == 1)
        #expect(client.count("status") == 1)
        let assets = try #require(model.report?.items.first { $0.kind == .assets })
        #expect(assets.status == .missing(CoreReadiness.chooseFolderMessage))
        #expect(model.state == .idle)
    }

    @Test func badAssetsShowTheCoresFirstProblem() async throws {
        let model = makeModel(ScriptedCoreClient(), directory: "bad")
        await model.validate()
        let assets = try #require(model.report?.items.first { $0.kind == .assets })
        #expect(assets.status == .missing("pak0.psarc not found"))
        #expect(model.state == .idle)
    }

    @Test func aThrowingValidateIsAMissingAssetsRowNotACrash() async throws {
        let client = ScriptedCoreClient()
        client.failValidate(CoreFailure(code: .io, message: "folder unreadable"))
        client.failCoverage(CoreFailure(code: .io, message: "folder unreadable"))
        let model = makeModel(client)
        await model.validate()
        let items = try #require(model.report).items
        #expect(items.first { $0.kind == .assets }?.status == .missing("Could not check the game files: folder unreadable"))
        guard case .notBuilt(let message) = items.first(where: { $0.kind == .nativeCore })?.status else {
            Issue.record("native core row must be NOT BUILT when the core cannot be queried")
            return
        }
        #expect(message.contains("could not be queried"))
        #expect(model.state == .idle)
        #expect(model.diagnosticsText.contains("Core query failed: folder unreadable (IO)"))
    }

    @Test func unavailableClientNeverBecomesReady() async {
        let failure = CoreFailure(code: .abiMismatch, message: "ABI handshake failed")
        let model = makeModel(UnavailableCoreClient(failure: failure))
        await model.validate()
        #expect(model.report?.canStart == false)
        #expect(model.state == .idle)
        await model.startCore()
        #expect(model.state == .idle)
        #expect(model.diagnosticsText.contains("ABI_MISMATCH"))
    }

    @Test func hostChecksStillGatePlay() async throws {
        let model = makeModel(ScriptedCoreClient(engineReady: true), metal: .missing("No Metal device available"))
        await model.validate()
        #expect(model.state == .idle)
        #expect(model.presentation.status.contains("Metal"))
        let items = try #require(model.report).items
        #expect(items.first { $0.kind == .nativeCore }?.status == .ready)
    }

    @Test func allReadyCoreMakesTheModelReady() async {
        let model = makeModel(ScriptedCoreClient(engineReady: true))
        await model.validate()
        #expect(model.state == .ready)
        #expect(model.canStart)
        #expect(model.presentation.tone == .success)
    }
}

@MainActor
struct LauncherModelStartStopTests {
    @Test func gatedStartNeverReachesTheCoreWhenReadinessSaysNo() async {
        let client = ScriptedCoreClient()  // engine not ready, so Play is disabled
        let model = makeModel(client)
        await model.validate()
        await model.startCore()
        #expect(client.count("start") == 0)
        #expect(model.state == .idle)
    }

    @Test func engineNotImplementedIsFailedAndUnavailableNeverRunning() async {
        // The core claims ready in validate but refuses start: the refusal must win.
        let client = ScriptedCoreClient(engineReady: true, start: .refuse(.engineNotImplemented))
        let model = makeModel(client)
        await model.validate()
        #expect(model.canStart)
        var seen: [LauncherState] = []
        await model.startCore()
        seen.append(model.state)

        guard case .failed = model.state else {
            Issue.record("expected .failed, got \(model.state)")
            return
        }
        #expect(!seen.contains(.running))
        #expect(model.failureCode == .engineNotImplemented)
        let presentation = model.presentation
        #expect(presentation.tone == .unavailable)
        #expect(presentation.status == LauncherPresentation.engineUnavailableStatus)
        #expect(presentation.status.contains("not implemented"))
        #expect(presentation.accessibilityLabel == "Game core status")
        #expect(presentation.accessibilityValue.hasPrefix("Unavailable. Not running."))
        #expect(!presentation.accessibilityValue.localizedCaseInsensitiveContains("running.") || presentation.accessibilityValue.contains("Not running"))
        #expect(model.diagnosticsText.contains("Last core answer to a request: ENGINE_NOT_IMPLEMENTED"))
        #expect(!model.canStop)

        // No polling after a refused start, and no later transition to running.
        let statusCalls = client.count("status")
        try? await Task.sleep(for: .milliseconds(60))
        #expect(client.count("status") == statusCalls)
        #expect(model.state != .running)
    }

    @Test func otherRefusalsAreFailedButNotTheUnavailableTone() async {
        let client = ScriptedCoreClient(engineReady: true, start: .refuse(.badState))
        let model = makeModel(client)
        await model.validate()
        await model.startCore()
        #expect(model.state == .failed(CoreClientMapping.failureText(for: .badState)))
        #expect(model.presentation.tone == .failure)
        #expect(model.presentation.accessibilityValue.hasPrefix("Failed. Not running."))
    }

    @Test func runningRequiresTheCoreToReportRunningWithAnEngine() async {
        let client = ScriptedCoreClient(engineReady: true, start: .accept)  // after accept: running + engine
        let model = makeModel(client)
        await model.validate()
        await model.startCore()
        #expect(model.state == .running)
        #expect(model.canStop)
        #expect(model.presentation.accessibilityValue == "Running. Game core running.")
        await model.stopCore()
        #expect(model.state == .ready)
    }

    @Test func acceptedStartWithNonRunningStatusIsNotRunning() async {
        for reported in [CoreState.ready, .idle, .validating] {
            let client = ScriptedCoreClient(
                engineReady: true, start: .accept, afterAccept: ScriptedCoreClient.status(reported, engine: true)
            )
            let model = makeModel(client)
            await model.validate()
            await model.startCore()
            guard case .failed(let message) = model.state else {
                Issue.record("start returned OK but core said \(reported): expected .failed, got \(model.state)")
                continue
            }
            #expect(message.contains("did not report that it is running"))
            #expect(model.presentation.tone == .failure)
        }
    }

    @Test func runningWithoutEngineImplementedIsNeverRunning() async {
        let client = ScriptedCoreClient(
            engineReady: true, start: .accept, afterAccept: ScriptedCoreClient.status(.running, engine: false)
        )
        let model = makeModel(client)
        await model.validate()
        await model.startCore()
        guard case .failed = model.state else {
            Issue.record("expected .failed, got \(model.state)")
            return
        }
        #expect(model.state != .running)
    }

    @Test func startingStatePollsUntilTheCoreSaysRunning() async {
        let client = ScriptedCoreClient(
            engineReady: true, start: .accept, afterAccept: ScriptedCoreClient.status(.starting, engine: true)
        )
        let model = makeModel(client)
        await model.validate()
        await model.startCore()
        #expect(model.state == .starting)
        #expect(model.canStop)
        client.setStatus(ScriptedCoreClient.status(.running, engine: true))
        #expect(await waitUntil { model.state == .running })
    }

    @Test func pollingNeverTouchesTheMainThread() async {
        let client = ScriptedCoreClient(
            engineReady: true, start: .accept, afterAccept: ScriptedCoreClient.status(.starting, engine: true)
        )
        let model = makeModel(client)
        await model.validate()
        await model.startCore()
        let before = client.count("status")
        #expect(await waitUntil { client.count("status") >= before + 5 })
        client.setStatus(ScriptedCoreClient.status(.running, engine: true))
        #expect(await waitUntil { model.state == .running })
        await model.stopCore()
        #expect(client.mainThreadCalls == 0)
        #expect(client.count("validate") == 1 && client.count("start") == 1 && client.count("stop") == 1)
    }

    @Test func coreFailingWhileRunningBecomesFailed() async {
        let client = ScriptedCoreClient(engineReady: true, start: .accept)
        let model = makeModel(client)
        await model.validate()
        await model.startCore()
        #expect(model.state == .running)
        client.setStatus(ScriptedCoreClient.status(.failed, engine: true, error: .internalError))
        #expect(await waitUntil { model.isFailed })
        #expect(model.state != .running)
        #expect(model.failureCode == .internalError)
    }

    @Test func coreStoppingByItselfIsNotShownAsRunningOrAsAUserStop() async {
        let client = ScriptedCoreClient(engineReady: true, start: .accept)
        let model = makeModel(client)
        await model.validate()
        await model.startCore()
        client.setStatus(ScriptedCoreClient.status(.ready, engine: true))
        #expect(await waitUntil { model.isFailed })
        #expect(model.state == .failed("The game core stopped without being asked to."))
    }

    @Test func losingTheCoreWhilePollingIsAFailureNotAFrozenRunning() async {
        let client = ScriptedCoreClient(engineReady: true, start: .accept)
        let model = makeModel(client)
        await model.validate()
        await model.startCore()
        client.failStatus(CoreFailure(code: .internalError, message: "core vanished"))
        #expect(await waitUntil { model.isFailed })
        #expect(model.state == .failed("Lost contact with the game core: core vanished"))
    }

    @Test func stopFailureIsReportedAndStopsPolling() async {
        let client = ScriptedCoreClient(engineReady: true, start: .accept)
        let model = makeModel(client)
        await model.validate()
        await model.startCore()
        client.failStop(CoreFailure(code: .badState, message: "cannot stop"))
        await model.stopCore()
        #expect(model.state == .failed("Could not stop the game core: cannot stop"))
        let calls = client.count("status")
        try? await Task.sleep(for: .milliseconds(60))
        #expect(client.count("status") == calls)
    }

    @Test func requestedStopIsNotAFailure() async {
        let client = ScriptedCoreClient(engineReady: true, start: .accept)
        let model = makeModel(client)
        await model.validate()
        await model.startCore()
        await model.stopCore()
        #expect(model.state == .ready)
        #expect(model.failureCode == nil)
        #expect(model.presentation.tone == .success)
    }

    @Test func stopWhenNotRunningDoesNothing() async {
        let client = ScriptedCoreClient(engineReady: true)
        let model = makeModel(client)
        await model.validate()
        await model.stopCore()
        #expect(client.count("stop") == 0)
        #expect(model.state == .ready)
    }

    @Test func revalidatingWhileRunningIsIgnored() async {
        let client = ScriptedCoreClient(engineReady: true, start: .accept)
        let model = makeModel(client)
        await model.validate()
        await model.startCore()
        await model.validate()
        #expect(model.state == .running)
        #expect(client.count("validate") == 1)
    }
}

struct LauncherPresentationTests {
    @Test func everyStateHasHonestSpokenCopy() {
        let cases: [(LauncherState, String, LauncherPresentation.Tone)] = [
            (.validating, "Checking. Checking files and hardware…", .progress),
            (.ready, "Ready. Ready to play.", .success),
            (.starting, "Starting. Starting the game core…", .progress),
            (.running, "Running. Game core running.", .success),
            (.stopping, "Stopping. Stopping the game core…", .progress),
        ]
        for (state, value, tone) in cases {
            let presentation = LauncherPresentation.make(state: state, blockedReason: nil, failureCode: nil)
            #expect(presentation.accessibilityValue == value)
            #expect(presentation.tone == tone)
            #expect(presentation.accessibilityLabel == "Game core status")
        }
    }

    @Test func idleShowsTheBlockerWordedAsNotReady() {
        let presentation = LauncherPresentation.make(state: .idle, blockedReason: "NOT BUILT: x", failureCode: nil)
        #expect(presentation.status == "NOT BUILT: x")
        #expect(presentation.accessibilityValue == "Not ready. NOT BUILT: x")
        #expect(presentation.tone == .neutral)
        #expect(LauncherPresentation.make(state: .idle, blockedReason: nil, failureCode: nil).status == "Not ready to play.")
    }

    @Test func engineNotImplementedIsUnavailableOnlyWithThatCode() {
        let unavailable = LauncherPresentation.make(state: .failed("x"), blockedReason: nil, failureCode: .engineNotImplemented)
        #expect(unavailable.tone == .unavailable)
        #expect(unavailable.hint != nil)
        #expect(unavailable.accessibilityValue == "Unavailable. Not running. Unavailable: the native game engine is not implemented yet, so the game cannot start.")
        let other = LauncherPresentation.make(state: .failed("boom"), blockedReason: nil, failureCode: .internalError)
        #expect(other.tone == .failure)
        #expect(other.status == "Failed: boom")
        #expect(other.accessibilityValue == "Failed. Not running. boom")
        // The code alone never changes a non-failed state.
        let running = LauncherPresentation.make(state: .running, blockedReason: nil, failureCode: .engineNotImplemented)
        #expect(running.tone == .success)
    }
}

struct CoreReadinessTests {
    @Test func assetsStatusMapsEveryCase() {
        let dir = URL(filePath: "/tmp/x", directoryHint: .isDirectory)
        func validation(ready: Bool, verdict: CoreErrorCode, problems: Int, detail: String) -> CoreValidation {
            CoreValidation(
                verdict: verdict, assetsReady: ready, engineReady: false, startWouldSucceed: false,
                problemCount: problems, engineError: .engineNotImplemented, archives: [], detail: detail
            )
        }
        #expect(CoreReadiness.assetsStatus(directory: nil, validation: nil, failure: nil) == .missing(CoreReadiness.chooseFolderMessage))
        #expect(CoreReadiness.assetsStatus(directory: dir, validation: validation(ready: true, verdict: .ok, problems: 0, detail: ""), failure: nil) == .ready)
        #expect(CoreReadiness.assetsStatus(directory: dir, validation: validation(ready: false, verdict: .assetsNotReady, problems: 2, detail: "a"), failure: nil) == .missing("a"))
        #expect(CoreReadiness.assetsStatus(directory: dir, validation: validation(ready: false, verdict: .assetsNotReady, problems: 2, detail: ""), failure: nil) == .missing("2 required archive problem(s)"))
        // A core that says ready but also reports problems is not ready.
        #expect(CoreReadiness.assetsStatus(directory: dir, validation: validation(ready: true, verdict: .ok, problems: 1, detail: ""), failure: nil) != .ready)
        #expect(CoreReadiness.assetsStatus(directory: dir, validation: nil, failure: nil) == .missing("The game files were not checked"))
    }

    @Test func coverageTextIsDeterministicAndShowsLinkedCtorFacts() {
        let ctor = CoreCtorPrefix(
            linked: true, slotsRegistered: 573, slotsWithCallback: 40, completed: 12,
            runOK: false, callbackFailed: false, blockedOnRVA: 0xf06410
        )
        let coverage = CoreCoverageReport(
            abiVersion: CoreABIVersion(major: 1, minor: 0), engineImplemented: false, ctor: ctor,
            subsystems: [
                CoreSubsystem(name: "save", status: .partial, detail: "fixture"),
                CoreSubsystem(name: "future", status: .unknown(99), detail: "future code"),
            ]
        )
        let lines = CoreDiagnostics.coreSection(coverage: coverage, status: nil, lastFailure: nil, queryFailure: nil)
        #expect(lines == [
            "Core report:",
            "  Core ABI: 1.0",
            "  Engine implemented: NO",
            "  Constructor prefix: 12 of 40 callbacks completed (573 slots registered), run did not complete, blocked on RVA 0xf06410",
            "  Subsystem save: PARTIAL — fixture",
            "  Subsystem future: UNKNOWN(99) — future code",
        ])
        #expect(CoreDiagnostics.coreSection(coverage: nil, status: nil, lastFailure: nil, queryFailure: nil)
            == ["Core report:", "  Coverage: NOT REPORTED"])
        #expect(CoreDiagnostics.text(report: nil, coverage: nil, status: nil, lastFailure: nil, queryFailure: nil)
            .hasPrefix("No readiness check has run yet."))
    }

    @Test func unknownCoreErrorCodeKeepsItsRawValueInDiagnostics() {
        #expect(CoreDiagnostics.code(.unknown(99)) == "UNKNOWN(99)")
    }
}
