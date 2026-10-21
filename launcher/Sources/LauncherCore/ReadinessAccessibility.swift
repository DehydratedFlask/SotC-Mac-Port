import Foundation

/// Spoken (VoiceOver) description of one readiness fact: a label plus a value.
///
/// Deterministic and pure — no locale lookup, no pluralization, no filesystem access. The value is
/// the honest status wording, so a screen reader can never report a softer state than the visible
/// row. `.notBuilt` deliberately passes the underlying message through unmodified: those messages
/// already begin with "NOT BUILT" and end with the Play-disabled sentence.
///
/// Documented, stable strings (see launcher/README.md):
/// - every `.ready`: `Ready`
/// - every `.missing(detail)`: `Missing: <detail>`
/// - every `.notBuilt(detail)`: `<detail>` verbatim
public struct ReadinessAccessibility: Equatable, Sendable {
    public let label: String
    public let value: String

    public init(kind: ReadinessItem.Kind, status: ReadinessItem.Status) {
        self.label = kind.accessibilityLabel
        self.value = status.accessibilityValue
    }

    public init(item: ReadinessItem) {
        self.init(kind: item.kind, status: item.status)
    }
}

extension ReadinessItem.Kind {
    /// Spoken label for a readiness row. Exhaustive on purpose: a new subsystem must decide its
    /// own label instead of inheriting a catch-all.
    public var accessibilityLabel: String {
        switch self {
        case .assets: "Game assets"
        case .nativeCore: "Native game core"
        case .metal: "Metal"
        case .audio: "Audio output"
        }
    }
}

extension ReadinessItem.Status {
    /// Honest status wording. Exhaustive: no catch-all can hide a future status case.
    public var accessibilityValue: String {
        switch self {
        case .ready:
            "Ready"
        case .missing(let detail):
            "Missing: \(detail)"
        case .notBuilt(let detail):
            // Verbatim, never softened to "unavailable"/"not ready"/"ok".
            detail
        }
    }
}

extension ReadinessItem {
    /// Spoken label (identical to the visible row label).
    public var accessibilityLabel: String { kind.accessibilityLabel }

    /// Spoken value (honest status wording).
    public var accessibilityValue: String { status.accessibilityValue }

    public var accessibilityDescription: ReadinessAccessibility {
        ReadinessAccessibility(item: self)
    }
}
