import Foundation
import LauncherCore
import Observation

/// Owns launcher state. The game core runs as a native child process (ownership decision: process, not embedded view).
@MainActor
@Observable
final class LauncherModel {
    private(set) var state: LauncherState = .idle
    private(set) var report: ReadinessReport?
    var assetsDirectory: URL?
    var coreExecutable: URL? = LauncherModel.bundledCoreURL()

    private var process: Process?

    var canStart: Bool {
        LauncherLifecycle.canStart(readinessAllowsStart: report?.canStart == true, from: state)
    }

    var isFailed: Bool {
        if case .failed = state { return true }
        return false
    }

    var canStop: Bool { state == .running || state == .starting }

    func validate() async {
        guard state != .running, state != .starting, state != .stopping else { return }
        state = .validating
        let result = await ReadinessReport.evaluate(assets: assetsDirectory, coreExecutable: coreExecutable)
        report = result
        state = result.canStart ? .ready : .idle
    }

    func start() {
        // Defense in depth: never launch when the honest probe says NOT BUILT / incomplete.
        guard canStart else { return }
        guard let core = coreExecutable, let assets = assetsDirectory else { return }
        if case .notBuilt(let message) = SystemChecks.nativeCore(executable: core) {
            state = .failed(message)
            return
        }
        if case .missing(let message) = SystemChecks.nativeCore(executable: core) {
            state = .failed(message)
            return
        }
        state = .starting
        let child = Process()
        child.executableURL = core
        child.arguments = ["--assets", assets.path(percentEncoded: false)]
        child.terminationHandler = { [weak self] finished in
            let status = finished.terminationStatus
            let reason = finished.terminationReason
            Task { @MainActor in self?.processEnded(status: status, reason: reason) }
        }
        do {
            try child.run()
            process = child
            state = .running
        } catch {
            state = .failed("Could not start the game core: \(error.localizedDescription)")
        }
    }

    func stop() {
        guard let process, process.isRunning else { return }
        state = .stopping
        process.terminate()
    }

    private func processEnded(status: Int32, reason: Process.TerminationReason) {
        let wasStopping = state == .stopping
        process = nil
        let outcome: LauncherProcessOutcome
        if wasStopping {
            outcome = .requestedStop
        } else if reason == .exit && status == 0 {
            outcome = .cleanExit
        } else {
            outcome = .unexpectedExit(status: status)
        }
        state = LauncherLifecycle.state(
            after: outcome,
            readinessAllowsStart: report?.canStart == true
        )
    }

    private static func bundledCoreURL() -> URL? {
        Bundle.main.url(forAuxiliaryExecutable: "SoCGameCore")
    }
}
