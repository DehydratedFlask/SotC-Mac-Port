import Foundation
import Testing
@testable import LauncherCore

/// APP-01-DIAGNOSTICS-SUMMARY-01: stable ordered multi-line report from a `ReadinessReport`.
struct DiagnosticsSummaryTests {
    private static let notBuilt = "NOT BUILT: no SoCGameCore path (auxiliary executable absent). Play disabled until a native SoCGameCore exists"

    private func report(_ pairs: [(ReadinessItem.Kind, ReadinessItem.Status)]) -> ReadinessReport {
        ReadinessReport(items: pairs.map { ReadinessItem(kind: $0.0, status: $0.1) })
    }

    @Test func mixedReportKeepsFixedOrderAndExplicitWords() {
        let mixed = report([
            (.audio, .ready),
            (.metal, .missing("No Metal device available")),
            (.nativeCore, .notBuilt(Self.notBuilt)),
            (.assets, .ready),
        ])

        let lines = DiagnosticsSummary.lines(for: mixed)
        #expect(lines == [
            "Game assets: READY",
            "Native game core: NOT BUILT — \(Self.notBuilt)",
            "Metal: MISSING — No Metal device available",
            "Audio output: READY",
            "Play is disabled: \(Self.notBuilt)",
        ])
        #expect(!DiagnosticsSummary.isStartable(mixed))
    }

    @Test func allReadyReportSaysPlayIsReady() {
        let ready = report(DiagnosticsSummary.subsystemOrder.map { ($0, ReadinessItem.Status.ready) })

        #expect(DiagnosticsSummary.lines(for: ready) == [
            "Game assets: READY",
            "Native game core: READY",
            "Metal: READY",
            "Audio output: READY",
            "Play is ready: every subsystem reported READY.",
        ])
        #expect(DiagnosticsSummary.isStartable(ready))
    }

    @Test func emptyReportIsHonestAboutNothingBeingChecked() {
        let empty = ReadinessReport(items: [])

        // `ReadinessReport.canStart` is vacuously true here; the summary must not inherit that.
        #expect(empty.canStart)
        #expect(!DiagnosticsSummary.isStartable(empty))
        #expect(DiagnosticsSummary.lines(for: empty) == [
            "Game assets: NOT REPORTED — no result for this subsystem",
            "Native game core: NOT REPORTED — no result for this subsystem",
            "Metal: NOT REPORTED — no result for this subsystem",
            "Audio output: NOT REPORTED — no result for this subsystem",
            "Play is disabled: no subsystem reported ready.",
        ])
    }

    @Test func partialReportNeverClaimsPlayIsReady() {
        let partial = report([
            (.assets, .ready),
            (.nativeCore, .ready),
            (.metal, .ready),
        ])

        #expect(partial.canStart) // vacuously true over the three present items
        #expect(!DiagnosticsSummary.isStartable(partial))
        let lines = DiagnosticsSummary.lines(for: partial)
        #expect(lines.last == "Play is disabled: no result reported for Audio output.")
        #expect(lines.contains("Audio output: NOT REPORTED — no result for this subsystem"))
        #expect(!lines.contains { $0.hasPrefix("Play is ready") })
    }

    @Test func noDuplicateOrOmittedSubsystemLines() {
        let mixed = report([
            (.assets, .missing("pakN.psarc not found")),
            (.nativeCore, .notBuilt(Self.notBuilt)),
            (.metal, .ready),
            // Duplicated kind: first occurrence wins, no second line.
            (.assets, .ready),
        ])

        let lines = DiagnosticsSummary.lines(for: mixed)
        #expect(lines.count == 5) // 4 subsystems + 1 overall line
        #expect(lines.filter { $0.hasPrefix("Game assets:") } == ["Game assets: MISSING — pakN.psarc not found"])

        for kind in DiagnosticsSummary.subsystemOrder {
            #expect(lines.filter { $0.hasPrefix("\(kind.accessibilityLabel):") }.count == 1)
        }
        #expect(lines.filter { $0.hasPrefix("Play ") }.count == 1)
        #expect(lines.last?.hasPrefix("Play is disabled: ") == true)
    }

    @Test func laterConflictingDuplicateCannotMakeReadyReportStartable() {
        let conflicting = report([
            (.assets, .ready),
            (.nativeCore, .ready),
            (.metal, .ready),
            (.audio, .ready),
            (.assets, .missing("bad magic")),
        ])

        #expect(!DiagnosticsSummary.isStartable(conflicting))
        #expect(DiagnosticsSummary.lines(for: conflicting).last?.hasPrefix("Play is disabled: ") == true)
    }

    @Test func textJoinsLinesWithSingleNewlines() {
        let mixed = report([
            (.assets, .ready),
            (.nativeCore, .notBuilt(Self.notBuilt)),
            (.metal, .ready),
            (.audio, .missing("No default audio output device")),
        ])

        let text = DiagnosticsSummary.text(for: mixed)
        #expect(text == DiagnosticsSummary.lines(for: mixed).joined(separator: "\n"))
        #expect(text.split(separator: "\n").count == 5)
        #expect(!text.hasSuffix("\n"))
        // Deterministic: rendering twice produces identical bytes.
        #expect(text == DiagnosticsSummary.text(for: mixed))
    }
}
