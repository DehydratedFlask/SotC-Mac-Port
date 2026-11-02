import Foundation
import Testing
@testable import LauncherCore

/// APP-01-VOICEOVER-READINESS-01: what the launcher's initial readiness screen exposes to
/// assistive technology, and whether the spoken copy still says the engine is not implemented.
///
/// Pure strings and report values only: no VoiceOver session, no window, no core process. Spoken
/// output and focus traversal stay NOT VERIFIED until an authorized screen-reader run.
struct InitialReadinessAccessibilityTests {
    /// The startup row a present core produces today: not linked engine, partial startup recovery.
    private static let startupDetail =
        "311 of 573 original constructor slots recovered; first unrecovered index 298"
    private static let coverage = CoreCoverageReport(
        abiVersion: CoreABIVersion(major: 1, minor: 0),
        engineImplemented: false,
        ctor: .notLinked,
        subsystems: [CoreSubsystem(name: "startup_constructors", status: .partial, detail: startupDetail)]
    )

    /// The four rows `LauncherModel.validate()` builds before a folder has been chosen.
    private static func initialReport() -> ReadinessReport {
        CoreReadiness.report(
            assets: CoreReadiness.assetsStatus(directory: nil, validation: nil, failure: nil),
            nativeCore: CoreClientMapping.nativeCoreStatus(validation: nil, coverage: coverage),
            metal: .ready,
            audio: .ready
        )
    }

    /// Every state the launcher can show, so the hint table cannot silently miss a case.
    private static var allPresentations: [LauncherPresentation] {
        var result = [
            LauncherState.validating, .ready, .starting, .running, .stopping,
        ].map { LauncherPresentation.make(state: $0, blockedReason: nil, failureCode: nil) }
        result.append(LauncherPresentation.make(state: .idle, blockedReason: nil, failureCode: nil))
        result.append(LauncherPresentation.make(state: .idle, blockedReason: "NOT BUILT: x", failureCode: nil))
        result.append(LauncherPresentation.make(state: .failed("boom"), blockedReason: nil, failureCode: .internalError))
        result.append(LauncherPresentation.make(state: .failed("boom"), blockedReason: nil, failureCode: .engineNotImplemented))
        return result
    }

    // MARK: - Status element

    @Test func spokenHintIsExactlyTheVisibleHint() {
        for presentation in Self.allPresentations {
            #expect(presentation.accessibilityHint == (presentation.hint ?? ""))
        }
        // Both directions: every visible hint is spoken, and silence means no hint.
        #expect(Self.allPresentations.contains { !$0.accessibilityHint.isEmpty })
        #expect(Self.allPresentations.contains { $0.accessibilityHint.isEmpty })
    }

    @Test func engineNotImplementedHintIsSpokenAndNamesTheDiagnosticsAction() {
        let presentation = LauncherPresentation.make(
            state: .failed("boom"), blockedReason: nil, failureCode: .engineNotImplemented)

        #expect(presentation.accessibilityLabel == "Game core status")
        #expect(presentation.accessibilityValue.hasPrefix("Unavailable. Not running."))
        #expect(presentation.accessibilityValue.contains("the native game engine is not implemented yet"))
        #expect(presentation.accessibilityHint.contains("Diagnostics"))
        #expect(presentation.tone == .unavailable)
        // The hint may describe the gate; it may never describe a state that does not exist.
        #expect(!presentation.accessibilityHint.localizedCaseInsensitiveContains("running"))
        #expect(!presentation.accessibilityHint.contains("Ready to play"))
    }

    @Test func idleBlockedStatusCarriesTheSameHonestReasonPlusAnAction() {
        let message = "NOT BUILT: native game engine not implemented. Play disabled until the engine is implemented"
        let presentation = LauncherPresentation.make(state: .idle, blockedReason: message, failureCode: nil)

        #expect(presentation.status == message) // verbatim, never softened
        #expect(presentation.accessibilityValue == "Not ready. \(message)")
        #expect(presentation.accessibilityHint == LauncherPresentation.blockedActionHint)
        #expect(presentation.accessibilityHint.contains("Play stays disabled"))
        #expect(presentation.accessibilityHint.contains("Diagnostics"))
        #expect(presentation.tone == .neutral)
        #expect(presentation.tone != .success)
        #expect(!presentation.accessibilityValue.localizedCaseInsensitiveContains("ready to play"))
        #expect(!presentation.accessibilityHint.localizedCaseInsensitiveContains("running"))
    }

    @Test func idleBeforeTheFirstReportShowsNoGuidance() {
        let presentation = LauncherPresentation.make(state: .idle, blockedReason: nil, failureCode: nil)

        #expect(presentation.status == "Not ready to play.")
        #expect(presentation.hint == nil)
        #expect(presentation.accessibilityHint.isEmpty) // nothing announced before data exists
    }

    // MARK: - Readiness rows

    @Test func initialRowsNeverAnnounceAReadyNativeCore() throws {
        let report = Self.initialReport()
        let rows = report.items.map { (label: $0.accessibilityLabel, value: $0.accessibilityValue) }

        #expect(rows.map(\.label) == ["Game assets", "Native game core", "Metal", "Audio output"])
        #expect(rows[0].value == "Missing: \(CoreReadiness.chooseFolderMessage)")

        let core = try #require(report.items.first { $0.kind == .nativeCore })
        #expect(core.accessibilityValue.hasPrefix("NOT BUILT: native game engine not implemented (core ABI 1.0)"))
        #expect(core.accessibilityValue.contains("Play disabled until the engine is implemented"))
        #expect(core.accessibilityValue.contains("311 of 573 original constructor slots recovered"))
        #expect(!core.accessibilityValue.contains("Ready"))
        // The absences-only wording belongs to the no-transport case, not to a core that answered.
        #expect(!core.accessibilityValue.contains("until a native SoCGameCore exists"))

        // Host capabilities are ready; that says nothing about the engine and has its own label.
        #expect(rows[2].value == "Ready")
        #expect(rows[3].value == "Ready")
        #expect(!report.canStart)
    }

    @Test func blockedPlayReasonIsTheEngineGapNotTheHostChecks() {
        let report = Self.initialReport()

        #expect(report.playBlockedSummary?.hasPrefix("NOT BUILT: native game engine not implemented") == true)
        #expect(report.playBlockedSummary?.contains("Play disabled") == true)
        #expect(report.playBlockedSummary?.hasPrefix("Game assets:") != true)
        #expect(report.playBlockedSummary?.hasPrefix("Metal:") != true)
        #expect(!report.canStart)
    }
}
