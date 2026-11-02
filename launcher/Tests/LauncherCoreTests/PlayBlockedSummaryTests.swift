import Foundation
import Testing
@testable import LauncherCore

/// APP-01-PLAY-BLOCKED-SUMMARY-NATIVE-CORE-PRECEDENCE-FIXTURE-01
///
/// Pins honest Play-disabled copy: a native-core `.notBuilt` row wins over concurrent
/// missing assets/Metal/audio so the summary stays NOT BUILT / Play disabled, not a
/// softer assets-only message. Synthetic report values only; no UI/runtime claim.
struct PlayBlockedSummaryTests {
    private static let notBuilt =
        "NOT BUILT: native game engine not implemented. Play disabled until a native SoCGameCore exists"
    private static let missingAssets = "Choose the folder with the extracted game archives"
    private static let missingMetal = "No Metal device available"
    private static let missingAudio = "No audio output device"

    private func report(
        assets: ReadinessItem.Status,
        nativeCore: ReadinessItem.Status,
        metal: ReadinessItem.Status = .ready,
        audio: ReadinessItem.Status = .ready
    ) -> ReadinessReport {
        ReadinessReport(items: [
            ReadinessItem(kind: .assets, status: assets),
            ReadinessItem(kind: .nativeCore, status: nativeCore),
            ReadinessItem(kind: .metal, status: metal),
            ReadinessItem(kind: .audio, status: audio),
        ])
    }

    @Test func notBuiltNativeCoreWinsOverMissingAssetsMetalAndAudio() {
        let blocked = report(
            assets: .missing(Self.missingAssets),
            nativeCore: .notBuilt(Self.notBuilt),
            metal: .missing(Self.missingMetal),
            audio: .missing(Self.missingAudio)
        )

        #expect(!blocked.canStart)
        #expect(blocked.playBlockedSummary == Self.notBuilt)
        #expect(blocked.playBlockedSummary?.contains("NOT BUILT") == true)
        #expect(blocked.playBlockedSummary?.contains("Play disabled") == true)
        // Must not demote the engine gap to an assets-prefixed or host-check message.
        #expect(blocked.playBlockedSummary?.hasPrefix("Game assets:") != true)
        #expect(blocked.playBlockedSummary?.hasPrefix("Metal:") != true)
        #expect(blocked.playBlockedSummary?.hasPrefix("Audio output:") != true)
    }

    @Test func readyNativeCoreFallsThroughToMissingAssetsCopy() {
        let assetsOnly = report(
            assets: .missing(Self.missingAssets),
            nativeCore: .ready
        )

        #expect(!assetsOnly.canStart)
        #expect(assetsOnly.playBlockedSummary == "Game assets: \(Self.missingAssets)")
        #expect(assetsOnly.playBlockedSummary?.contains("NOT BUILT") != true)
    }

    @Test func allReadyReportHasNoPlayBlockedSummary() {
        let ready = report(
            assets: .ready,
            nativeCore: .ready,
            metal: .ready,
            audio: .ready
        )

        #expect(ready.canStart)
        #expect(ready.playBlockedSummary == nil)
    }

    @Test func idlePresentationKeepsVerbatimNotBuiltBlockedReason() {
        let presentation = LauncherPresentation.make(
            state: .idle,
            blockedReason: Self.notBuilt,
            failureCode: nil
        )

        #expect(presentation.status == Self.notBuilt)
        #expect(presentation.status.contains("NOT BUILT"))
        #expect(presentation.status.contains("Play disabled"))
        #expect(presentation.accessibilityValue == "Not ready. \(Self.notBuilt)")
        #expect(presentation.tone == .neutral)
        // APP-01-VOICEOVER-READINESS-01: the idle status now also carries the guidance it shows. It
        // must stay advice about the gate, never a readiness or running claim.
        #expect(presentation.hint == LauncherPresentation.blockedActionHint)
        #expect(presentation.accessibilityHint == LauncherPresentation.blockedActionHint)
        // Never softens into unavailable/running language while idle with NOT BUILT.
        #expect(!presentation.status.lowercased().contains("running"))
        #expect(presentation.tone != .success)
        #expect(presentation.tone != .unavailable)
    }
}
