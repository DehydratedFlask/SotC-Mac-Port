import Foundation

/// How the launcher reached (or failed to reach) its game core, for the readiness row and diagnostics.
///
/// Without this the launcher answers identically whether a child `SoCGameCore` is running or the
/// bundled executable is missing and the in-process C ABI stands in; the second case must say so.
public enum CoreTransport: Equatable, Sendable {
    /// A valid `SoCGameCore` was located and its `--serve` process is the client.
    case childProcess(URL)
    /// No usable child core; the in-process ABI (which has no engine) answers. The payload is always a
    /// `.notBuilt` status explaining why.
    case inProcessFallback(ReadinessItem.Status)

    /// A row that replaces the computed "Native game core" row, or nil when the child core answers.
    public var nativeCoreRowOverride: ReadinessItem.Status? {
        if case .inProcessFallback(let status) = self { return status }
        return nil
    }

    public var diagnosticsLine: String {
        switch self {
        case .childProcess(let url):
            return "  Core transport: child process \(url.path(percentEncoded: false))"
        case .inProcessFallback(let status):
            let reason: String
            switch status {
            case .notBuilt(let detail), .missing(let detail): reason = detail
            case .ready: reason = "no child core"
            }
            return "  Core transport: in-process ABI fallback (no engine) — \(reason)"
        }
    }
}
