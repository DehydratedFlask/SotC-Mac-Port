import Foundation

/// What the status area shows and speaks for the launcher's current state.
///
/// A start refused with `ENGINE_NOT_IMPLEMENTED` is rendered as "Unavailable", distinct from a
/// crash-style failure and never anything resembling running. Pure mapping, unit-tested.
public struct LauncherPresentation: Equatable, Sendable {
    public enum Tone: Equatable, Sendable { case neutral, progress, success, unavailable, failure }

    public let status: String
    public let hint: String?
    public let tone: Tone
    /// Spoken label of the status element.
    public let accessibilityLabel: String
    /// Spoken value: always names the state in words, so colour or icon is never the only cue.
    public let accessibilityValue: String

    public static let accessibilityStatusLabel = "Game core status"
    public static let engineUnavailableStatus =
        "Unavailable: the native game engine is not implemented yet, so the game cannot start."

    public init(status: String, hint: String?, tone: Tone, accessibilityValue: String) {
        self.status = status
        self.hint = hint
        self.tone = tone
        self.accessibilityLabel = Self.accessibilityStatusLabel
        self.accessibilityValue = accessibilityValue
    }

    /// - Parameters:
    ///   - blockedReason: `ReadinessReport.playBlockedSummary`, shown while idle.
    ///   - failureCode: the core's code for the last refused request, if any.
    public static func make(state: LauncherState, blockedReason: String?, failureCode: CoreErrorCode?) -> LauncherPresentation {
        switch state {
        case .idle:
            let text = blockedReason ?? LauncherStatusCopy.for(state).status
            return LauncherPresentation(status: text, hint: nil, tone: .neutral, accessibilityValue: "Not ready. \(text)")
        case .validating:
            return plain(state, .progress, "Checking")
        case .ready:
            return plain(state, .success, "Ready")
        case .starting:
            return plain(state, .progress, "Starting")
        case .running:
            return plain(state, .success, "Running")
        case .stopping:
            return plain(state, .progress, "Stopping")
        case .failed(let message):
            if failureCode == .engineNotImplemented {
                return LauncherPresentation(
                    status: engineUnavailableStatus,
                    hint: "Nothing was started. Open Diagnostics to see which parts of the engine exist.",
                    tone: .unavailable,
                    accessibilityValue: "Unavailable. Not running. \(engineUnavailableStatus)"
                )
            }
            let copy = LauncherStatusCopy.for(state)
            return LauncherPresentation(
                status: copy.status, hint: copy.recoveryHint, tone: .failure,
                accessibilityValue: "Failed. Not running. \(message)"
            )
        }
    }

    private static func plain(_ state: LauncherState, _ tone: Tone, _ word: String) -> LauncherPresentation {
        let copy = LauncherStatusCopy.for(state)
        return LauncherPresentation(status: copy.status, hint: copy.recoveryHint, tone: tone, accessibilityValue: "\(word). \(copy.status)")
    }
}
