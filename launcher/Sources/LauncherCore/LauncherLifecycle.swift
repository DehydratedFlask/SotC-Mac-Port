/// Outcome of a native child process ending.
public enum LauncherProcessOutcome: Equatable, Sendable {
    case requestedStop
    case cleanExit
    case unexpectedExit(status: Int32)
}

/// Pure launcher state transitions that can be verified without launching a child process.
public enum LauncherLifecycle {
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
