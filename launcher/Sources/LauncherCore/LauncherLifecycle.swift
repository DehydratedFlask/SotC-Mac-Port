import Foundation

/// Outcome of a native child process ending.
public enum LauncherProcessOutcome: Equatable, Sendable {
    case requestedStop
    case cleanExit
    case unexpectedExit(status: Int32)
}

/// Pure launcher state transitions that can be verified without launching a child process.
public enum LauncherLifecycle {
    public static func classifyTermination(
        stopWasRequested: Bool,
        reason: Process.TerminationReason,
        status: Int32
    ) -> LauncherProcessOutcome {
        if stopWasRequested { return .requestedStop }
        if reason == .exit && status == 0 { return .cleanExit }
        return .unexpectedExit(status: status)
    }

    public static func canStart(
        readinessAllowsStart: Bool,
        from state: LauncherState
    ) -> Bool {
        guard readinessAllowsStart else { return false }
        switch state {
        case .ready, .failed:
            return true
        case .idle, .validating, .starting, .running, .stopping:
            return false
        }
    }

    public static func state(
        after outcome: LauncherProcessOutcome,
        readinessAllowsStart: Bool
    ) -> LauncherState {
        switch outcome {
        case .requestedStop, .cleanExit:
            readinessAllowsStart ? .ready : .idle
        case .unexpectedExit(let status):
            .failed("Game core exited unexpectedly (status \(status))")
        }
    }
}

/// Converts folder-import errors into alert text while keeping user cancellation silent.
public enum FolderImportErrorClassifier {
    public static func alertMessage(for error: any Error) -> String? {
        guard !isCancellation(error) else { return nil }
        return error.localizedDescription
    }

    private static func isCancellation(_ error: any Error) -> Bool {
        if error is CancellationError { return true }

        let cocoaError = error as NSError
        return cocoaError.domain == NSCocoaErrorDomain && cocoaError.code == NSUserCancelledError
    }
}
