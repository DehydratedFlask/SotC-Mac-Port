import Foundation
import Testing
@testable import LauncherCore

/// APP-01-LIFECYCLE-UX-01: honest user-facing lifecycle copy.
struct LauncherStatusCopyTests {
    private static let outcomes: [LauncherProcessOutcome] = [
        .requestedStop,
        .cleanExit,
        .unexpectedExit(status: 9),
    ]

    private static let failedMessage = "Game core exited unexpectedly (status 17)"
    private static let states: [LauncherState] = [
        .idle, .validating, .ready, .starting, .running, .stopping, .failed(failedMessage),
    ]

    @Test func everyOutcomeHasNonEmptyDistinctStatus() {
        let copies = Self.outcomes.map { LauncherStatusCopy.for($0) }

        for copy in copies {
            #expect(!copy.status.isEmpty)
        }
        #expect(Set(copies.map(\.status)).count == Self.outcomes.count)
    }

    @Test func requestedStopIsNeverDescribedAsACrashAndHasNoHint() {
        let copy = LauncherStatusCopy.for(LauncherProcessOutcome.requestedStop)

        #expect(copy.status == "Stopped at your request.")
        #expect(copy.recoveryHint == nil)
        let lowered = copy.status.lowercased()
        #expect(!lowered.contains("crash"))
        #expect(!lowered.contains("failed"))
        #expect(!lowered.contains("unexpected"))
        #expect(copy.status.contains("request"))
    }

    @Test func cleanExitIsNotDescribedAsAFailureAndHasNoHint() {
        let copy = LauncherStatusCopy.for(LauncherProcessOutcome.cleanExit)

        #expect(copy.status == "Game core exited normally (status 0).")
        #expect(copy.recoveryHint == nil)
        #expect(!copy.status.lowercased().contains("unexpected"))
        #expect(!copy.status.lowercased().contains("crash"))
    }

    @Test func unexpectedExitIsNotDescribedAsSuccessAndCarriesAHint() {
        let copy = LauncherStatusCopy.for(LauncherProcessOutcome.unexpectedExit(status: 9))

        #expect(copy.status.contains("unexpectedly"))
        #expect(copy.status.contains("9"))
        let lowered = copy.status.lowercased()
        #expect(!lowered.contains("success"))
        #expect(!lowered.contains("completed"))
        #expect(!lowered.contains("normally"))
        #expect(copy.recoveryHint?.isEmpty == false)
    }

    @Test func everyStateHasNonEmptyDistinctStatus() {
        let copies = Self.states.map { LauncherStatusCopy.for($0) }

        for copy in copies {
            #expect(!copy.status.isEmpty)
        }
        #expect(Set(copies.map(\.status)).count == Self.states.count)
    }

    @Test func onlyFailedStateCarriesARecoveryHint() {
        for state in Self.states {
            let copy = LauncherStatusCopy.for(state)
            switch state {
            case .failed:
                #expect(copy.recoveryHint?.isEmpty == false)
            case .idle, .validating, .ready, .starting, .running, .stopping:
                #expect(copy.recoveryHint == nil)
            }
        }
    }

    @Test func failedStateKeepsTheUnderlyingMessage() {
        let copy = LauncherStatusCopy.for(LauncherState.failed(Self.failedMessage))

        #expect(copy.status.contains(Self.failedMessage))
        #expect(copy.recoveryHint?.isEmpty == false)
        #expect(copy.recoveryHint != copy.status)
    }
}
