import Foundation

/// Deterministic, copy-pasteable diagnostics text for a readiness report.
///
/// One line per subsystem in a fixed order, then one overall Play line. Pure: no clock, no
/// environment, no filesystem, no locale — the same report always renders the same text.
///
/// Stable wording (see launcher/README.md):
/// - `Game assets: READY`
/// - `Game assets: MISSING — <detail>`
/// - `Native game core: NOT BUILT — <detail>` (detail is the verbatim NOT BUILT message)
/// - `<subsystem>: NOT REPORTED — no result for this subsystem` when the report omits the kind
/// - overall: `Play is ready: every subsystem reported READY.` only when all four kinds are
///   present and ready; otherwise `Play is disabled: <text>`.
public enum DiagnosticsSummary {
    /// Fixed subsystem order. Assets and the native core first because they carry the blockers
    /// users can act on; Metal and audio are host checks.
    public static let subsystemOrder: [ReadinessItem.Kind] = [.assets, .nativeCore, .metal, .audio]

    /// A report is startable only when every subsystem reported READY. Stricter than
    /// `ReadinessReport.canStart` on purpose: an empty (or partial) report must never be
    /// described as ready, because nothing was verified.
    public static func isStartable(_ report: ReadinessReport) -> Bool {
        guard report.canStart else { return false }
        return subsystemOrder.allSatisfy { kind in
            report.items.contains { $0.kind == kind }
        }
    }

    /// Ordered report lines; one per subsystem (first occurrence wins for a duplicated kind),
    /// followed by exactly one overall Play line.
    public static func lines(for report: ReadinessReport) -> [String] {
        var lines: [String] = []
        for kind in subsystemOrder {
            if let item = report.items.first(where: { $0.kind == kind }) {
                lines.append(subsystemLine(kind: kind, status: item.status))
            } else {
                lines.append("\(kind.accessibilityLabel): NOT REPORTED — no result for this subsystem")
            }
        }
        lines.append(overallLine(for: report))
        return lines
    }

    /// Full multi-line diagnostics text (no trailing newline).
    public static func text(for report: ReadinessReport) -> String {
        lines(for: report).joined(separator: "\n")
    }

    private static func subsystemLine(kind: ReadinessItem.Kind, status: ReadinessItem.Status) -> String {
        switch status {
        case .ready:
            "\(kind.accessibilityLabel): READY"
        case .missing(let detail):
            "\(kind.accessibilityLabel): MISSING — \(detail)"
        case .notBuilt(let detail):
            "\(kind.accessibilityLabel): NOT BUILT — \(detail)"
        }
    }

    private static func overallLine(for report: ReadinessReport) -> String {
        // `ReadinessReport.canStart` is vacuously true for an empty report, so handle it first.
        guard !report.items.isEmpty else {
            return "Play is disabled: no subsystem reported ready."
        }
        if isStartable(report) {
            return "Play is ready: every subsystem reported READY."
        }
        if let summary = report.playBlockedSummary {
            return "Play is disabled: \(summary)"
        }
        let absent = subsystemOrder
            .filter { kind in !report.items.contains { $0.kind == kind } }
            .map(\.accessibilityLabel)
            .joined(separator: ", ")
        return "Play is disabled: no result reported for \(absent)."
    }
}
