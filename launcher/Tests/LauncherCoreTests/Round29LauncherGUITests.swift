import Foundation
import Testing
@testable import LauncherCore

/// Round 29 lane B: defects found by driving the packaged .app with the real core present.
struct Round29LauncherGUITests {
    private static let startup = CoreSubsystem(
        name: "startup_constructors", status: .partial,
        detail: "311 of 573 original constructor slots recovered; first unrecovered index 298; 23 registered slots ran, a callback failed at original RVA 0xe71990"
    )

    private static func coverage(subsystems: [CoreSubsystem], ctor: CoreCtorPrefix = .notLinked) -> CoreCoverageReport {
        CoreCoverageReport(abiVersion: CoreABIVersion(major: 1, minor: 0), engineImplemented: false, ctor: ctor, subsystems: subsystems)
    }

    @Test func presentCoreWithoutEngineNeverClaimsTheExecutableIsMissing() {
        let status = CoreClientMapping.nativeCoreStatus(validation: nil, coverage: Self.coverage(subsystems: [Self.startup]))
        guard case .notBuilt(let message) = status else { Issue.record("expected .notBuilt"); return }
        #expect(!message.contains("until a native SoCGameCore exists"))
        #expect(message.hasSuffix("Play disabled until the engine is implemented"))
        #expect(message.hasPrefix("NOT BUILT: native game engine not implemented (core ABI 1.0)"))
    }

    @Test func nativeCoreRowCarriesTheCoresTypedStartupReadinessVerbatim() {
        let status = CoreClientMapping.nativeCoreStatus(validation: nil, coverage: Self.coverage(subsystems: [Self.startup]))
        guard case .notBuilt(let message) = status else { Issue.record("expected .notBuilt"); return }
        #expect(message.contains("311 of 573 original constructor slots recovered"))
        #expect(message.contains("first unrecovered index 298"))
        #expect(message.contains("startup constructors PARTIAL"))
    }

    @Test func rowWithoutAStartupSubsystemAddsNothingInvented() {
        let status = CoreClientMapping.nativeCoreStatus(validation: nil, coverage: Self.coverage(subsystems: []))
        #expect(status == .notBuilt("NOT BUILT: native game engine not implemented (core ABI 1.0). Play disabled until the engine is implemented"))
    }

    @Test func diagnosticsUsesStatusCtorFactsWhenCoverageCarriesNone() {
        let ctor = CoreCtorPrefix(
            linked: true, slotsRegistered: 311, slotsWithCallback: 311, completed: 23,
            runOK: false, callbackFailed: true, blockedOnRVA: 0xe71990,
            originalSlots: 573, firstUnrecoveredIndex: 298
        )
        let status = CoreStatus(state: .idle, lastError: .ok, assetsValidated: false, assetsReady: false, engineImplemented: false, ctor: ctor)
        let lines = CoreDiagnostics.coreSection(
            coverage: Self.coverage(subsystems: []), status: status, lastFailure: nil, queryFailure: nil)
        let line = lines.first { $0.hasPrefix("  Constructor prefix:") }
        #expect(line == "  Constructor prefix: 23 of 311 callbacks completed (311 slots registered), run did not complete, blocked on RVA 0xe71990; 311 of 573 original slots recovered, first unrecovered index 298")
        #expect(!lines.contains { $0.contains("NOT LINKED") })
    }

    @Test func diagnosticsStillSaysNotLinkedWhenNeitherSourceIsLinked() {
        let status = CoreStatus(state: .idle, lastError: .ok, assetsValidated: false, assetsReady: false, engineImplemented: false, ctor: .notLinked)
        let lines = CoreDiagnostics.coreSection(coverage: Self.coverage(subsystems: []), status: status, lastFailure: nil, queryFailure: nil)
        #expect(lines.contains("  Constructor prefix: NOT LINKED in this build"))
    }

    @Test func diagnosticsBlockSpeaksTheReportNotJustItsName() {
        #expect(DiagnosticsAccessibility.label == "Diagnostics report")
        #expect(DiagnosticsAccessibility.value(for: "Core ABI: 1.0\nEngine implemented: NO") == "Core ABI: 1.0\nEngine implemented: NO")
        #expect(DiagnosticsAccessibility.value(for: "") == "Empty")
    }
}

/// Transport honesty: an absent or unlaunchable `SoCGameCore` must be named, not hidden by the fallback.
struct Round29CoreTransportTests {
    private func sandboxRoot() throws -> URL {
        let root = FileManager.default.temporaryDirectory
            .appending(path: "Round29Transport-\(UUID().uuidString)", directoryHint: .isDirectory)
        try FileManager.default.createDirectory(at: root, withIntermediateDirectories: true)
        return root
    }

    private let fallback: @Sendable () -> any CoreClient = {
        UnavailableCoreClient(failure: CoreFailure(code: .io, message: "no core"))
    }

    @Test func selectReportsAnAbsentCoreAsNotBuiltFallback() throws {
        let root = try sandboxRoot()
        defer { try? FileManager.default.removeItem(at: root) }
        let selection = CoreClientFactory.select(
            environment: [:], bundleURL: root.appending(path: "X.app", directoryHint: .isDirectory),
            executableURL: root.appending(path: "bin/X", directoryHint: .notDirectory), fallback: fallback)
        #expect(selection.client is UnavailableCoreClient)
        guard case .inProcessFallback(let status) = selection.transport,
              case .notBuilt(let message) = status else {
            Issue.record("expected an in-process fallback with a NOT BUILT reason")
            return
        }
        #expect(message.hasPrefix("NOT BUILT: no SoCGameCore path"))
        #expect(selection.transport.nativeCoreRowOverride == status)
        #expect(selection.transport.diagnosticsLine.hasPrefix("  Core transport: in-process ABI fallback (no engine) — NOT BUILT:"))
    }

    @Test func selectReportsAFoundCoreThatCannotBeStartedWithoutPretendingItWorks() throws {
        let root = try sandboxRoot()
        defer { try? FileManager.default.removeItem(at: root) }
        // A thin ARM64 MH_EXECUTE header passes architecture checks, but has no load commands.
        let core = root.appending(path: "SoCGameCore", directoryHint: .notDirectory)
        try Data([0xCF, 0xFA, 0xED, 0xFE, 0x0C, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 0]
            + [UInt8](repeating: 0, count: 16)).write(to: core)
        try FileManager.default.setAttributes([.posixPermissions: 0o755], ofItemAtPath: core.path(percentEncoded: false))
        let selection = CoreClientFactory.select(
            environment: [CoreLocator.environmentVariable: core.path(percentEncoded: false)],
            bundleURL: nil, executableURL: nil, fallback: fallback)
        if case .childProcess = selection.transport {
            Issue.record("an unrunnable core must not be reported as the active child process")
            return
        }
        guard case .inProcessFallback(.notBuilt(let message)) = selection.transport else {
            Issue.record("expected a NOT BUILT fallback")
            return
        }
        #expect(message.contains("SoCGameCore was found but could not be started"))
        #expect(message.contains("Play disabled"))
    }

    @Test func selectReportsTheChildProcessWhenACoreStarts() throws {
        let root = try sandboxRoot()
        defer { try? FileManager.default.removeItem(at: root) }
        let core = root.appending(path: "SoCGameCore", directoryHint: .notDirectory)
        try NativeExecutableFixture.write(to: core)
        let selection = CoreClientFactory.select(
            environment: [CoreLocator.environmentVariable: core.path(percentEncoded: false)],
            bundleURL: nil, executableURL: nil, fallback: fallback)
        #expect(selection.transport == .childProcess(core))
        #expect(selection.transport.nativeCoreRowOverride == nil)
        #expect(selection.transport.diagnosticsLine == "  Core transport: child process \(core.path(percentEncoded: false))")
        (selection.client as? ProcessCoreClient)?.shutdown()
    }

    @MainActor
    @Test func modelWithAbsentCoreShowsTheNamedRowDisabledPlayAndTransportDiagnostics() async throws {
        let status = ReadinessItem.Status.notBuilt(
            "NOT BUILT: no SoCGameCore path (auxiliary executable absent). Play disabled until a native SoCGameCore exists")
        let model = LauncherModel(
            client: ScriptedCoreClient(), transport: .inProcessFallback(status),
            hostChecks: HostChecks(metal: { .ready }, audio: { .ready }), pollInterval: .milliseconds(5))
        model.assetsDirectory = URL(filePath: "/tmp/good", directoryHint: .isDirectory)
        await model.validate()
        let row = try #require(model.report?.items.first { $0.kind == .nativeCore })
        #expect(row.status == status)
        #expect(!model.canStart)
        #expect(model.diagnosticsText.contains("Core transport: in-process ABI fallback (no engine) — NOT BUILT: no SoCGameCore path"))
        #expect(model.diagnosticsText.contains("Play is disabled:"))
    }

    @MainActor
    @Test func modelWithChildTransportKeepsTheComputedRowAndShowsThePath() async throws {
        let path = URL(filePath: "/Applications/X.app/Contents/MacOS/SoCGameCore")
        let model = LauncherModel(
            client: ScriptedCoreClient(), transport: .childProcess(path),
            hostChecks: HostChecks(metal: { .ready }, audio: { .ready }), pollInterval: .milliseconds(5))
        model.assetsDirectory = URL(filePath: "/tmp/good", directoryHint: .isDirectory)
        await model.validate()
        let row = try #require(model.report?.items.first { $0.kind == .nativeCore })
        guard case .notBuilt(let message) = row.status else { Issue.record("expected NOT BUILT"); return }
        #expect(message.contains("native game engine not implemented"))
        #expect(!model.canStart)
        #expect(model.diagnosticsText.contains("Core transport: child process /Applications/X.app/Contents/MacOS/SoCGameCore"))
    }
}
