import Foundation

/// One independently reported readiness fact. Never collapses distinct causes into a single "ok".
public struct ReadinessItem: Identifiable, Equatable, Sendable {
    public enum Kind: String, CaseIterable, Sendable {
        case assets = "Game assets"
        case nativeCore = "Native game core"
        case metal = "Metal"
        case audio = "Audio output"
    }

    public enum Status: Equatable, Sendable {
        case ready
        case missing(String)
        case notBuilt(String)
    }

    public let kind: Kind
    public let status: Status

    public var id: Kind { kind }

    public init(kind: Kind, status: Status) {
        self.kind = kind
        self.status = status
    }

    public var isReady: Bool { status == .ready }
}
