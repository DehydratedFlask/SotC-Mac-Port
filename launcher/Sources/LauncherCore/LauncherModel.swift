import Foundation
import Observation

/// Host probes that are not the game core's business. Injectable so tests need no GPU or audio device.
public struct HostChecks: Sendable {
    public var metal: @Sendable () -> ReadinessItem.Status
    public var audio: @Sendable () -> ReadinessItem.Status

    public init(metal: @escaping @Sendable () -> ReadinessItem.Status, audio: @escaping @Sendable () -> ReadinessItem.Status) {
        self.metal = metal
        self.audio = audio
    }

    public static let live = HostChecks(metal: { SystemChecks.metal() }, audio: { SystemChecks.audioOutput() })
}

/// Owns launcher state and routes every game-core question through a `CoreClient`.
///
/// Honesty rules, each covered by `LauncherModelTests`:
/// - `.running` is reachable only from a `CoreStatus` whose `state == .running` and
///   `engineImplemented == true` (`CoreClientMapping.launcherState`). A successful `start()` return
///   is never enough; the model asks the core for its status afterwards.
/// - A refused start (including `ENGINE_NOT_IMPLEMENTED`) becomes `.failed` and is presented as
///   unavailable (`LauncherPresentation`). It is never `.starting`/`.running` afterwards.
/// - Every core call and the status polling run off the main actor; the model only applies results.
@MainActor
@Observable
public final class LauncherModel {
    public private(set) var state: LauncherState = .idle
    public private(set) var report: ReadinessReport?
    public private(set) var coverage: CoreCoverageReport?
    public private(set) var coreStatus: CoreStatus?
    /// Core code behind the current `.failed` state, if the core supplied one.
    public private(set) var failureCode: CoreErrorCode?
    public var assetsDirectory: URL?

    @ObservationIgnored private let client: any CoreClient
    @ObservationIgnored private let hostChecks: HostChecks
    @ObservationIgnored private let pollInterval: Duration
    @ObservationIgnored private var pollTask: Task<Void, Never>?
    private var lastFailure: CoreFailure?
    private var queryFailure: CoreFailure?

    public init(client: any CoreClient, hostChecks: HostChecks = .live, pollInterval: Duration = .milliseconds(500)) {
        self.client = client
        self.hostChecks = hostChecks
        self.pollInterval = pollInterval
    }

    // MARK: - Derived

    public var canStart: Bool {
        LauncherLifecycle.canStart(readinessAllowsStart: report?.canStart == true, from: state)
    }

    public var canStop: Bool { state == .running || state == .starting }

    public var isFailed: Bool {
        if case .failed = state { return true }
        return false
    }

    public var presentation: LauncherPresentation {
        LauncherPresentation.make(state: state, blockedReason: report?.playBlockedSummary, failureCode: failureCode)
    }

    /// Copy-pasteable report: readiness lines, then the core's coverage report and last answers.
    public var diagnosticsText: String {
        CoreDiagnostics.text(
            report: report, coverage: coverage, status: coreStatus, lastFailure: lastFailure, queryFailure: queryFailure
        )
    }

    // MARK: - Validate

    private struct Snapshot: Sendable {
        var validation: CoreValidation?
        var validationFailure: CoreFailure?
        var coverage: CoreCoverageReport?
        var coverageFailure: CoreFailure?
        var status: CoreStatus?
        var statusFailure: CoreFailure?
        var metal: ReadinessItem.Status
        var audio: ReadinessItem.Status
    }

    private nonisolated static func failure(_ error: any Error) -> CoreFailure {
        (error as? CoreFailure) ?? CoreFailure(code: .internalError, message: error.localizedDescription)
    }

    private nonisolated static func gather(client: any CoreClient, directory: URL?, hostChecks: HostChecks) async -> Snapshot {
        var snapshot = Snapshot(metal: hostChecks.metal(), audio: hostChecks.audio())
        if let directory {
            do { snapshot.validation = try await client.validate(assetsDirectory: directory) } catch {
                snapshot.validationFailure = failure(error)
            }
        }
        do { snapshot.coverage = try await client.coverage() } catch { snapshot.coverageFailure = failure(error) }
        do { snapshot.status = try await client.status() } catch { snapshot.statusFailure = failure(error) }
        return snapshot
    }

    public func validate() async {
        guard state != .running, state != .starting, state != .stopping else { return }
        state = .validating
        failureCode = nil
        lastFailure = nil
        let client = client, directory = assetsDirectory, hostChecks = hostChecks
        let snapshot = await Task.detached(priority: .userInitiated) {
            await Self.gather(client: client, directory: directory, hostChecks: hostChecks)
        }.value

        coverage = snapshot.coverage
        coreStatus = snapshot.status
        queryFailure = snapshot.validationFailure ?? snapshot.coverageFailure ?? snapshot.statusFailure
        let result = CoreReadiness.report(
            assets: CoreReadiness.assetsStatus(directory: directory, validation: snapshot.validation, failure: snapshot.validationFailure),
            nativeCore: CoreReadiness.nativeCoreStatus(
                validation: snapshot.validation, coverage: snapshot.coverage,
                queryFailure: snapshot.validationFailure ?? snapshot.coverageFailure
            ),
            metal: snapshot.metal,
            audio: snapshot.audio
        )
        report = result
        state = result.canStart ? .ready : .idle
    }

    // MARK: - Start / stop

    public func start() { Task { await startCore() } }
    public func stop() { Task { await stopCore() } }

    private enum Context { case afterStart, afterStop, poll }

    public func startCore() async {
        // Defense in depth: the same gate the Play button uses.
        guard canStart else { return }
        lastFailure = nil
        failureCode = nil
        state = .starting
        let client = client
        do {
            try await Task.detached { try await client.start() }.value
        } catch {
            let refusal = Self.failure(error)
            record(refusal)
            state = .failed(Self.message(for: refusal))
            await refreshStatusForDiagnostics()
            return
        }
        await reconcileWithCore(.afterStart)
    }

    public func stopCore() async {
        guard canStop else { return }
        state = .stopping
        let client = client
        do {
            try await Task.detached { try await client.stop() }.value
        } catch {
            let refusal = Self.failure(error)
            record(refusal)
            stopPolling()
            state = .failed("Could not stop the game core: \(refusal.message)")
            return
        }
        await reconcileWithCore(.afterStop)
    }

    private static func message(for failure: CoreFailure) -> String {
        switch failure.code {
        case .engineNotImplemented, .assetsNotReady, .badState, .abiMismatch:
            CoreClientMapping.failureText(for: failure.code)
        default:
            failure.message.isEmpty ? CoreClientMapping.failureText(for: failure.code) : failure.message
        }
    }

    private func record(_ failure: CoreFailure) {
        lastFailure = failure
        failureCode = failure.code
    }

    private func refreshStatusForDiagnostics() async {
        let client = client
        if let status = try? await Task.detached(operation: { try await client.status() }).value {
            coreStatus = status
        }
    }

    // MARK: - Reconciling with the core

    private func reconcileWithCore(_ context: Context) async {
        let client = client
        do {
            let status = try await Task.detached { try await client.status() }.value
            apply(status, context: context)
        } catch {
            let refusal = Self.failure(error)
            record(refusal)
            stopPolling()
            state = .failed("Lost contact with the game core: \(refusal.message)")
        }
    }

    /// The only place a core status becomes a launcher state.
    private func apply(_ status: CoreStatus, context: Context) {
        // A late answer must not move a settled state: a poll that was in flight when the state left
        // starting/running/stopping (or a stop already resolved by the poller) is dropped.
        switch state {
        case .starting, .running, .stopping: break
        default:
            stopPolling()
            return
        }
        coreStatus = status
        let mapped = CoreClientMapping.launcherState(for: status)
        if case .failed = mapped, status.lastError != .ok {
            lastFailure = CoreFailure(code: status.lastError, message: CoreClientMapping.failureText(for: status.lastError))
            failureCode = status.lastError
        }
        let allowsStart = report?.canStart == true
        switch (state, mapped) {
        case (.stopping, .idle), (.stopping, .ready):
            state = LauncherLifecycle.state(after: .requestedStop, readinessAllowsStart: allowsStart)
        case (.stopping, .running), (.stopping, .starting), (.stopping, .stopping):
            state = .stopping
        case (_, .idle), (_, .ready), (_, .validating):
            switch context {
            case .afterStart:
                state = .failed("The game core accepted Start but did not report that it is running.")
            case .poll, .afterStop:
                state = .failed("The game core stopped without being asked to.")
            }
        default:
            state = mapped
        }
        switch state {
        case .starting, .running, .stopping: startPolling()
        default: stopPolling()
        }
    }

    // MARK: - Polling (off the main actor)

    private func startPolling() {
        guard pollTask == nil else { return }
        let client = client, interval = pollInterval
        pollTask = Task.detached(priority: .utility) { [weak self] in
            while !Task.isCancelled {
                let result: Result<CoreStatus, CoreFailure>
                do { result = .success(try await client.status()) } catch { result = .failure(Self.failure(error)) }
                guard let keepGoing = await self?.applyPoll(result), keepGoing else { return }
                try? await Task.sleep(for: interval)
            }
        }
    }

    private func stopPolling() {
        pollTask?.cancel()
        pollTask = nil
    }

    /// Returns whether polling should continue.
    private func applyPoll(_ result: Result<CoreStatus, CoreFailure>) -> Bool {
        switch result {
        case .success(let status):
            apply(status, context: .poll)
        case .failure(let refusal):
            record(refusal)
            stopPolling()
            state = .failed("Lost contact with the game core: \(refusal.message)")
        }
        return pollTask != nil
    }
}
