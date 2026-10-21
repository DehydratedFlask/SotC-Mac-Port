import Foundation

/// Pure construction of readiness rows and diagnostics text from `CoreClient` answers.
///
/// No I/O, no actor: the model gathers the facts (off the main actor) and this type decides what
/// they mean. Nothing here can produce `.ready` for the native core unless the core itself said the
/// engine is ready (`CoreClientMapping.nativeCoreStatus`).
public enum CoreReadiness {
    public static let chooseFolderMessage = "Choose the folder with the extracted game archives"

    /// "Game assets" row. The verdict comes from `CoreClient.validate`; the launcher adds no checks.
    public static func assetsStatus(
        directory: URL?, validation: CoreValidation?, failure: CoreFailure?
    ) -> ReadinessItem.Status {
        guard directory != nil else { return .missing(chooseFolderMessage) }
        if let failure {
            return .missing("Could not check the game files: \(failure.message)")
        }
        guard let validation else { return .missing("The game files were not checked") }
        if validation.assetsReady && validation.verdict == .ok && validation.problemCount == 0 { return .ready }
        if !validation.detail.isEmpty { return .missing(validation.detail) }
        return .missing(
            validation.problemCount > 0
                ? "\(validation.problemCount) required archive problem(s)"
                : CoreClientMapping.failureText(for: .assetsNotReady)
        )
    }

    /// "Native game core" row. A failed query is NOT BUILT, never ready.
    public static func nativeCoreStatus(
        validation: CoreValidation?, coverage: CoreCoverageReport?, queryFailure: CoreFailure?
    ) -> ReadinessItem.Status {
        if let queryFailure, validation == nil, coverage == nil {
            return .notBuilt("NOT BUILT: the native core could not be queried (\(queryFailure.message)). Play disabled until a native SoCGameCore exists")
        }
        return CoreClientMapping.nativeCoreStatus(validation: validation, coverage: coverage)
    }

    public static func report(
        assets: ReadinessItem.Status, nativeCore: ReadinessItem.Status,
        metal: ReadinessItem.Status, audio: ReadinessItem.Status
    ) -> ReadinessReport {
        ReadinessReport(items: [
            ReadinessItem(kind: .assets, status: assets),
            ReadinessItem(kind: .nativeCore, status: nativeCore),
            ReadinessItem(kind: .metal, status: metal),
            ReadinessItem(kind: .audio, status: audio),
        ])
    }
}

/// Deterministic text for the core coverage report and the core's last answers.
///
/// Appended under `DiagnosticsSummary` so a bug report carries the core's own claims. Pure; the
/// same inputs always give the same text.
public enum CoreDiagnostics {
    /// Full diagnostics: readiness lines, then the core section.
    public static func text(
        report: ReadinessReport?, coverage: CoreCoverageReport?, status: CoreStatus?,
        lastFailure: CoreFailure?, queryFailure: CoreFailure?
    ) -> String {
        var parts: [String] = []
        parts.append(report.map(DiagnosticsSummary.text(for:)) ?? "No readiness check has run yet.")
        parts.append(coreSection(coverage: coverage, status: status, lastFailure: lastFailure, queryFailure: queryFailure).joined(separator: "\n"))
        return parts.joined(separator: "\n\n")
    }

    public static func coreSection(
        coverage: CoreCoverageReport?, status: CoreStatus?, lastFailure: CoreFailure?, queryFailure: CoreFailure?
    ) -> [String] {
        var lines = ["Core report:"]
        if let queryFailure {
            lines.append("  Core query failed: \(queryFailure.message) (\(code(queryFailure.code)))")
        }
        if let coverage {
            lines.append("  Core ABI: \(coverage.abiVersion)")
            lines.append("  Engine implemented: \(coverage.engineImplemented ? "YES" : "NO")")
            lines.append("  Constructor prefix: \(ctor(coverage.ctor))")
            if coverage.subsystems.isEmpty {
                lines.append("  Subsystems: none reported")
            }
            for subsystem in coverage.subsystems {
                let detail = subsystem.detail.isEmpty ? "" : " — \(subsystem.detail)"
                lines.append("  Subsystem \(subsystem.name): \(subsystemStatus(subsystem.status))\(detail)")
            }
        } else {
            lines.append("  Coverage: NOT REPORTED")
        }
        if let status {
            lines.append("  Core state: \(stateName(status.state)); last error: \(code(status.lastError))")
        }
        if let lastFailure {
            lines.append("  Last core answer to a request: \(code(lastFailure.code)) — \(lastFailure.message)")
        }
        return lines
    }

    public static func code(_ error: CoreErrorCode) -> String {
        switch error {
        case .ok: "OK"
        case .invalidArgument: "INVALID_ARGUMENT"
        case .badState: "BAD_STATE"
        case .assetsNotReady: "ASSETS_NOT_READY"
        case .engineNotImplemented: "ENGINE_NOT_IMPLEMENTED"
        case .io: "IO"
        case .bufferTooSmall: "BUFFER_TOO_SMALL"
        case .abiMismatch: "ABI_MISMATCH"
        case .internalError: "INTERNAL_ERROR"
        case .unknown(let raw): "UNKNOWN(\(raw))"
        }
    }

    public static func stateName(_ state: CoreState) -> String {
        switch state {
        case .idle: "idle"
        case .validating: "validating"
        case .ready: "ready"
        case .starting: "starting"
        case .running: "running"
        case .stopping: "stopping"
        case .failed: "failed"
        }
    }

    static func subsystemStatus(_ status: CoreSubsystem.Status) -> String {
        switch status {
        case .notImplemented: "NOT IMPLEMENTED"
        case .partial: "PARTIAL"
        case .implemented: "IMPLEMENTED"
        case .notLinked: "NOT LINKED"
        case .unknown(let raw): "UNKNOWN(\(raw))"
        }
    }

    static func ctor(_ prefix: CoreCtorPrefix) -> String {
        guard prefix.linked else { return "NOT LINKED in this build" }
        let outcome = prefix.runOK ? "run OK" : "run did not complete"
        let blocked = prefix.blockedOnRVA == 0 ? "" : ", blocked on RVA 0x\(String(prefix.blockedOnRVA, radix: 16))"
        return "\(prefix.completed) of \(prefix.slotsWithCallback) callbacks completed (\(prefix.slotsRegistered) slots registered), \(outcome)\(blocked)"
    }
}
