import Foundation

/// The launcher's only view of the native game core. Transport-agnostic: the in-process C ABI
/// (`LauncherNativeCore`), a future child-process bridge and test doubles all conform.
///
/// Contract the launcher relies on (and `CoreClientContract` documents in tests):
/// - `start()` throws `CoreFailure` until the core has an engine; today the code is
///   `.engineNotImplemented`. Returning normally means the core itself reports `.running`.
/// - Nothing here may invent `.running`; the UI derives it only from `status()`.
public protocol CoreClient: Sendable {
    /// ABI version the connected core speaks.
    var abiVersion: CoreABIVersion { get }

    /// Checks the asset directory. Throws only for hard errors (`.invalidArgument`, `.badState`,
    /// `.internalError`); "assets not ready" is a normal `CoreValidation` with `assetsReady == false`.
    func validate(assetsDirectory: URL) async throws -> CoreValidation
    func start() async throws
    func stop() async throws
    func status() async throws -> CoreStatus
    func coverage() async throws -> CoreCoverageReport
}

/// Pure mapping from core facts to launcher vocabulary. No I/O.
public enum CoreClientMapping {
    /// The "Native game core" readiness row. `.ready` requires the core to say the engine is ready;
    /// asset readiness is a different row and never leaks into this one.
    public static func nativeCoreStatus(
        validation: CoreValidation?, coverage: CoreCoverageReport?
    ) -> ReadinessItem.Status {
        if let validation, validation.engineReady, validation.engineError == .ok { return .ready }
        if validation == nil, coverage == nil {
            return .notBuilt("NOT BUILT: native core has not been queried. Play disabled until a native SoCGameCore exists")
        }
        let abi = coverage.map { " (core ABI \($0.abiVersion))" } ?? ""
        return .notBuilt("NOT BUILT: native game engine not implemented\(abi). Play disabled until a native SoCGameCore exists")
    }

    /// Launcher state for a core status. An unrecognised or impossible combination is reported as
    /// `.failed`, never as `.running`: only `status.state == .running` from the core yields `.running`.
    public static func launcherState(for status: CoreStatus, failureMessage: String? = nil) -> LauncherState {
        switch status.state {
        case .idle: return .idle
        case .validating: return .validating
        case .ready: return .ready
        case .starting: return .starting
        case .running: return status.engineImplemented ? .running : .failed("Core reported running without an engine")
        case .stopping: return .stopping
        case .failed: return .failed(failureMessage ?? failureText(for: status.lastError))
        }
    }

    public static func failureText(for code: CoreErrorCode) -> String {
        switch code {
        case .engineNotImplemented: return "The native game engine is not implemented yet, so the game cannot start."
        case .assetsNotReady: return "The game files did not pass validation."
        case .io: return "The chosen assets folder cannot be used."
        case .badState: return "The game core is not in a state that allows that action."
        case .abiMismatch: return "The game core was built for a different interface version."
        default: return "The game core reported an error."
        }
    }
}
