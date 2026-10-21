import Foundation
import Testing
@testable import LauncherCore

/// APP-01-A11Y-LABELS-01: spoken label/value for every Kind × Status combination.
struct ReadinessAccessibilityTests {
    /// One representative status per case, including synthetic details for the associated values.
    static let allStatuses: [ReadinessItem.Status] = [
        .ready,
        .missing("cachedFiles.psarc not found"),
        .notBuilt("NOT BUILT: no SoCGameCore path (auxiliary executable absent). Play disabled until a native SoCGameCore exists"),
    ]

    /// Independent expectation table — deliberately does not call the production helpers.
    private func expectedValue(for status: ReadinessItem.Status) -> String {
        switch status {
        case .ready:
            return "Ready"
        case .missing(let detail):
            return "Missing: \(detail)"
        case .notBuilt(let detail):
            return detail
        }
    }

    @Test func everyKindAndStatusCombinationHasLabelAndValue() {
        #expect(ReadinessItem.Kind.allCases.count == 4)
        var seen: Set<String> = []

        for kind in ReadinessItem.Kind.allCases {
            for status in Self.allStatuses {
                let copy = ReadinessAccessibility(kind: kind, status: status)

                #expect(copy.label == kind.accessibilityLabel)
                #expect(!copy.label.isEmpty)
                #expect(copy.value == expectedValue(for: status))
                #expect(!copy.value.isEmpty)

                seen.insert("\(copy.label)|\(copy.value)")
            }
        }

        // 4 kinds × 3 statuses = 12 distinct label/value pairs (no accidental collision).
        #expect(seen.count == 12)
    }

    @Test func labelsAreDistinctPerKind() {
        let labels = ReadinessItem.Kind.allCases.map(\.accessibilityLabel)

        #expect(Set(labels).count == ReadinessItem.Kind.allCases.count)
        #expect(labels == ["Game assets", "Native game core", "Metal", "Audio output"])
        // The visible row label and the spoken label must stay identical.
        #expect(labels == ReadinessItem.Kind.allCases.map(\.rawValue))
    }

    @Test func nativeCoreNotBuiltValueKeepsVerbatimWording() {
        let status = SystemChecks.nativeCore(executable: nil)
        let copy = ReadinessAccessibility(kind: .nativeCore, status: status)

        guard case .notBuilt(let detail) = status else {
            Issue.record("expected .notBuilt for nil core")
            return
        }

        #expect(copy.label == "Native game core")
        #expect(copy.value == detail) // verbatim, unmodified
        #expect(copy.value.contains("NOT BUILT"))
        #expect(copy.value.contains("Play disabled"))
        // Never softened.
        #expect(!copy.value.contains("unavailable"))
        #expect(!copy.value.lowercased().contains("not ready"))
    }

    @Test func missingValueCarriesProblemDetailAndReadySaysReady() {
        let missing = ReadinessAccessibility(
            kind: .assets,
            status: .missing("pakN.psarc not found")
        )
        #expect(missing.value == "Missing: pakN.psarc not found")
        #expect(missing.value.contains("pakN.psarc not found"))

        for kind in ReadinessItem.Kind.allCases {
            #expect(ReadinessAccessibility(kind: kind, status: .ready).value == "Ready")
        }
    }

    @Test func itemConvenienceMatchesExplicitConstruction() {
        let item = ReadinessItem(
            kind: .audio,
            status: .missing("No default audio output device")
        )

        #expect(item.accessibilityLabel == "Audio output")
        #expect(item.accessibilityValue == "Missing: No default audio output device")
        #expect(item.accessibilityDescription == ReadinessAccessibility(kind: .audio, status: item.status))
    }
}
