import Foundation
import Testing
@testable import LauncherCore

struct LauncherLifecycleTests {
    @Test func requestedStopTakesPrecedenceOverTerminationReasonAndStatus() {
        let outcome = LauncherLifecycle.classifyTermination(
            stopWasRequested: true,
            reason: .uncaughtSignal,
            status: 9
        )

        #expect(outcome == .requestedStop)
    }

    @Test func normalZeroExitIsClean() {
        #expect(
            LauncherLifecycle.classifyTermination(
                stopWasRequested: false,
                reason: .exit,
                status: 0
            ) == .cleanExit
        )
    }

    @Test func nonzeroExitIsUnexpected() {
        #expect(
            LauncherLifecycle.classifyTermination(
                stopWasRequested: false,
                reason: .exit,
                status: 17
            ) == .unexpectedExit(status: 17)
        )
    }

    @Test func signalTerminationIsUnexpected() {
        #expect(
            LauncherLifecycle.classifyTermination(
                stopWasRequested: false,
                reason: .uncaughtSignal,
                status: 9
            ) == .unexpectedExit(status: 9)
        )
    }

    @Test func cleanExitRestoresReadyStateWhenReadinessStillAllowsStart() {
        let state = LauncherLifecycle.state(after: .cleanExit, readinessAllowsStart: true)
        #expect(state == .ready)
        #expect(LauncherLifecycle.canStart(readinessAllowsStart: true, from: state))
    }

    @Test func requestedStopRestoresReadyStateWhenReadinessStillAllowsStart() {
        let state = LauncherLifecycle.state(after: .requestedStop, readinessAllowsStart: true)
        #expect(state == .ready)
        #expect(LauncherLifecycle.canStart(readinessAllowsStart: true, from: state))
    }

    @Test func cleanExitAndRequestedStopRespectCurrentReadiness() {
        #expect(LauncherLifecycle.state(after: .cleanExit, readinessAllowsStart: false) == .idle)
        #expect(LauncherLifecycle.state(after: .requestedStop, readinessAllowsStart: false) == .idle)
    }

    @Test func unexpectedExitPreservesFailureStatus() {
        let state = LauncherLifecycle.state(after: .unexpectedExit(status: 17), readinessAllowsStart: true)
        #expect(state == .failed("Game core exited unexpectedly (status 17)"))
        #expect(LauncherLifecycle.canStart(readinessAllowsStart: true, from: state))
    }

    @Test func readinessIsRequiredEvenWhenStateWouldPermitStart() {
        #expect(!LauncherLifecycle.canStart(readinessAllowsStart: false, from: .ready))
        #expect(!LauncherLifecycle.canStart(readinessAllowsStart: false, from: .failed("retryable")))
        #expect(!LauncherLifecycle.canStart(readinessAllowsStart: true, from: .idle))
        #expect(!LauncherLifecycle.canStart(readinessAllowsStart: true, from: .running))
        #expect(!LauncherLifecycle.canStart(readinessAllowsStart: true, from: .validating))
        #expect(!LauncherLifecycle.canStart(readinessAllowsStart: true, from: .starting))
        #expect(!LauncherLifecycle.canStart(readinessAllowsStart: true, from: .stopping))
    }
}

struct FolderImportErrorClassifierTests {
    @Test func cancellationDoesNotProduceAlertText() {
        let cocoaCancellation = NSError(domain: NSCocoaErrorDomain, code: NSUserCancelledError)

        #expect(FolderImportErrorClassifier.alertMessage(for: cocoaCancellation) == nil)
        #expect(FolderImportErrorClassifier.alertMessage(for: CancellationError()) == nil)
    }

    @Test func ordinaryFailureUsesLocalizedDescription() {
        let failure = NSError(
            domain: "FolderImporterTests",
            code: 1,
            userInfo: [NSLocalizedDescriptionKey: "The selected folder could not be read."]
        )

        #expect(FolderImportErrorClassifier.alertMessage(for: failure) == "The selected folder could not be read.")
    }
}
