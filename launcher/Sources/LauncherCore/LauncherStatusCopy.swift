import Foundation

/// User-facing lifecycle wording: a short status line plus an optional recovery hint.
///
/// Pure mapping, no actor, no I/O. The hint is `nil` whenever nothing is wrong, so routine states
/// (and a requested Stop) never display recovery advice.
///
/// Honesty rules (see launcher/README.md):
/// - A Stop the user asked for is never described as a crash or a failure.
/// - An unexpected end is never described as success. `LauncherProcessOutcome` cannot tell a
///   signal from a nonzero exit code (`LauncherLifecycle.classifyTermination` folds both into
///   `.unexpectedExit(status:)`), so this copy claims neither cause — only "ended unexpectedly"
///   with the raw status.
public struct LauncherStatusCopy: Equatable, Sendable {
    public let status: String
    public let recoveryHint: String?

    public init(status: String, recoveryHint: String?) {
        self.status = status
        self.recoveryHint = recoveryHint
    }

    /// Copy for a child-process outcome. Requested Stop carries no hint.
    public static func `for`(_ outcome: LauncherProcessOutcome) -> LauncherStatusCopy {
        switch outcome {
        case .requestedStop:
            LauncherStatusCopy(
                status: "Stopped at your request.",
                recoveryHint: nil
            )
        case .cleanExit:
            LauncherStatusCopy(
                status: "Game core exited normally (status 0).",
                recoveryHint: nil
            )
        case .unexpectedExit(let status):
            LauncherStatusCopy(
                status: "Game core ended unexpectedly (status \(status)).",
                recoveryHint: "Press Play to retry. If it ends the same way, open Diagnostics and include the report with a bug report."
            )
        }
    }

    /// Copy for the launcher's own state. Only `.failed` carries a hint.
    public static func `for`(_ state: LauncherState) -> LauncherStatusCopy {
        switch state {
        case .idle:
            LauncherStatusCopy(status: "Not ready to play.", recoveryHint: nil)
        case .validating:
            LauncherStatusCopy(status: "Checking files and hardware…", recoveryHint: nil)
        case .ready:
            LauncherStatusCopy(status: "Ready to play.", recoveryHint: nil)
        case .starting:
            LauncherStatusCopy(status: "Starting the game core…", recoveryHint: nil)
        case .running:
            LauncherStatusCopy(status: "Game core running.", recoveryHint: nil)
        case .stopping:
            LauncherStatusCopy(status: "Stopping the game core…", recoveryHint: nil)
        case .failed(let message):
            LauncherStatusCopy(
                status: "Failed: \(message)",
                recoveryHint: "Check the readiness rows, then try again. If the failure repeats, capture Diagnostics text for the bug report."
            )
        }
    }
}
