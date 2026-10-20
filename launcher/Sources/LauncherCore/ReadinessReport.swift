import Foundation

/// Runs all checks off the main actor and returns the items in a fixed order.
public struct ReadinessReport: Equatable, Sendable {
    public let items: [ReadinessItem]

    public init(items: [ReadinessItem]) {
        self.items = items
    }

    public var canStart: Bool { items.allSatisfy(\.isReady) }

    /// First non-ready native-core status, if any (for honest Play-disabled copy).
    public var nativeCoreStatus: ReadinessItem.Status? {
        items.first(where: { $0.kind == .nativeCore })?.status
    }

    /// Human summary when Play must stay disabled; nil only when `canStart` is true.
    public var playBlockedSummary: String? {
        guard !canStart else { return nil }
        if let status = nativeCoreStatus {
            switch status {
            case .notBuilt(let message), .missing(let message):
                return message
            case .ready:
                break
            }
        }
        if let first = items.first(where: { !$0.isReady }) {
            switch first.status {
            case .missing(let message), .notBuilt(let message):
                return "\(first.kind.rawValue): \(message)"
            case .ready:
                break
            }
        }
        return "Not ready to play"
    }

    public static func evaluate(assets: URL?, coreExecutable: URL?) async -> ReadinessReport {
        await Task.detached(priority: .userInitiated) {
            let assetStatus: ReadinessItem.Status = assets.map { AssetValidator().validate(directory: $0) }
                ?? .missing("Choose the folder with the extracted game archives")
            return ReadinessReport(items: [
                ReadinessItem(kind: .assets, status: assetStatus),
                ReadinessItem(kind: .nativeCore, status: SystemChecks.nativeCore(executable: coreExecutable)),
                ReadinessItem(kind: .metal, status: SystemChecks.metal()),
                ReadinessItem(kind: .audio, status: SystemChecks.audioOutput()),
            ])
        }.value
    }
}
