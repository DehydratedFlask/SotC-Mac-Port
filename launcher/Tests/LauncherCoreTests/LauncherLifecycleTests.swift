import Foundation
import Testing
@testable import LauncherCore

struct LauncherLifecycleTests {
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
