import Darwin
import Foundation
import Testing
@testable import LauncherCore

/// APP-01-LAUNCHER-PROCESS-CLIENT-01: locator precedence and honest rejections.
struct CoreLocatorTests {
    private static func header(cpu: UInt32 = 0x0100000C, fileType: UInt32 = 2) -> [UInt8] {
        [UInt32(0xFEEDFACF), cpu, 0, fileType, 0, 0, 0, 0].flatMap { word in
            withUnsafeBytes(of: word.littleEndian) { Array($0) }
        }
    }

    /// A temp tree with one candidate per search location.
    private final class Sandbox {
        let root: URL
        let bundle: URL
        let envCore: URL
        let macCore: URL
        let resourcesCore: URL
        let toolDirectory: URL
        let siblingCore: URL
        let runningExecutable: URL

        init() throws {
            root = FileManager.default.temporaryDirectory
                .appending(path: "CoreLocatorTests-\(UUID().uuidString)", directoryHint: .isDirectory)
            bundle = root.appending(path: "SoCLauncher.app", directoryHint: .isDirectory)
            envCore = root.appending(path: "env/SoCGameCore", directoryHint: .notDirectory)
            macCore = bundle.appending(path: "Contents/MacOS/SoCGameCore", directoryHint: .notDirectory)
            resourcesCore = bundle.appending(path: "Contents/Resources/SoCGameCore", directoryHint: .notDirectory)
            toolDirectory = root.appending(path: "tools", directoryHint: .isDirectory)
            siblingCore = toolDirectory.appending(path: "SoCGameCore", directoryHint: .notDirectory)
            runningExecutable = toolDirectory.appending(path: "SoCLauncher", directoryHint: .notDirectory)
            try FileManager.default.createDirectory(at: root, withIntermediateDirectories: true)
        }

        func cleanup() { try? FileManager.default.removeItem(at: root) }

        func writeCore(_ url: URL, header: [UInt8] = CoreLocatorTests.header(), executable: Bool = true) throws {
            try FileManager.default.createDirectory(
                at: url.deletingLastPathComponent(), withIntermediateDirectories: true
            )
            try Data(header).write(to: url)
            try FileManager.default.setAttributes(
                [.posixPermissions: executable ? 0o755 : 0o644], ofItemAtPath: url.path(percentEncoded: false)
            )
        }

        func remove(_ url: URL) { try? FileManager.default.removeItem(at: url) }

        func writeScript(_ url: URL) throws {
            try FileManager.default.createDirectory(
                at: url.deletingLastPathComponent(), withIntermediateDirectories: true
            )
            try Data("#!/bin/sh\nexit 0\n".utf8).write(to: url)
            try FileManager.default.setAttributes(
                [.posixPermissions: 0o755], ofItemAtPath: url.path(percentEncoded: false)
            )
        }
    }

    private func notBuiltMessage(_ status: ReadinessItem.Status) -> String? {
        if case .notBuilt(let message) = status { return message }
        return nil
    }

    private func locate(_ env: String?, _ sandbox: Sandbox) -> CoreLocator.Location {
        var environment: [String: String] = [:]
        if let env { environment[CoreLocator.environmentVariable] = env }
        return CoreLocator().locate(
            environment: environment, bundleURL: sandbox.bundle, executableURL: sandbox.runningExecutable
        )
    }

    @Test func rejectsNonNativeOrNonExecutableMachOHeaders() throws {
        let sandbox = try Sandbox()
        defer { sandbox.cleanup() }
        let invalid: [[UInt8]] = [
            Self.header(cpu: 0x01000007), // x86_64 MH_EXECUTE must never reach Process/Rosetta.
            Self.header(fileType: 6), // ARM64 MH_DYLIB is not a child executable.
            [0xCA, 0xFE, 0xBA, 0xBE] + [UInt8](repeating: 0, count: 28), // FAT_MAGIC
            [0xBF, 0xBA, 0xFE, 0xCA] + [UInt8](repeating: 0, count: 28), // FAT_CIGAM_64
            Array(Self.header().prefix(8)), // truncated thin ARM64
            [0xFE, 0xED, 0xFA, 0xCF] + [UInt8](repeating: 0, count: 28), // wrong byte order / CPU
        ]
        for header in invalid {
            try sandbox.writeCore(sandbox.envCore, header: header)
            #expect(locate(sandbox.envCore.path(percentEncoded: false), sandbox).url == nil)
            #expect(locate(sandbox.envCore.path(percentEncoded: false), sandbox).rejection
                == SystemChecks.nativeCore(executable: sandbox.envCore))
        }
    }

    @Test func acceptsAThinARM64ExecutableHeader() throws {
        let sandbox = try Sandbox()
        defer { sandbox.cleanup() }
        try sandbox.writeCore(sandbox.envCore)
        #expect(locate(sandbox.envCore.path(percentEncoded: false), sandbox).url == sandbox.envCore)
    }

    @Test func rejectsADirectoryWithTheSystemChecksWording() throws {
        let sandbox = try Sandbox()
        defer { sandbox.cleanup() }
        try FileManager.default.createDirectory(
            at: sandbox.envCore.deletingLastPathComponent(), withIntermediateDirectories: true
        )
        try FileManager.default.createDirectory(at: sandbox.envCore, withIntermediateDirectories: true)

        guard let actual = notBuiltMessage(locate(sandbox.envCore.path(percentEncoded: false), sandbox).rejection ?? .ready),
              let expected = notBuiltMessage(SystemChecks.nativeCore(executable: sandbox.envCore)) else {
            Issue.record("expected NOT BUILT from both the locator and SystemChecks")
            return
        }
        #expect(actual == expected)
        #expect(actual.contains("is a directory, not an executable"))
        #expect(actual.hasPrefix("NOT BUILT:"))
        #expect(actual.contains("Play disabled until a native SoCGameCore exists"))
    }

    @Test func rejectsAScriptWithTheSystemChecksWording() throws {
        let sandbox = try Sandbox()
        defer { sandbox.cleanup() }
        try sandbox.writeScript(sandbox.envCore)

        guard let actual = notBuiltMessage(locate(sandbox.envCore.path(percentEncoded: false), sandbox).rejection ?? .ready),
              let expected = notBuiltMessage(SystemChecks.nativeCore(executable: sandbox.envCore)) else {
            Issue.record("expected NOT BUILT from both the locator and SystemChecks")
            return
        }
        #expect(actual == expected)
        #expect(actual.contains("is not a Mach-O executable (refusing scripts/stubs)"))
    }

    @Test func acceptsASymlinkToAValidCore() throws {
        let sandbox = try Sandbox()
        defer { sandbox.cleanup() }
        let target = sandbox.root.appending(path: "real/SoCGameCore", directoryHint: .notDirectory)
        try sandbox.writeCore(target)
        try FileManager.default.createDirectory(
            at: sandbox.envCore.deletingLastPathComponent(), withIntermediateDirectories: true
        )
        try FileManager.default.createSymbolicLink(at: sandbox.envCore, withDestinationURL: target)

        #expect(locate(sandbox.envCore.path(percentEncoded: false), sandbox).url == sandbox.envCore)
    }

    @Test func rejectsANonRegularFileWithoutBlockingOnIt() throws {
        let sandbox = try Sandbox()
        defer { sandbox.cleanup() }
        try FileManager.default.createDirectory(
            at: sandbox.envCore.deletingLastPathComponent(), withIntermediateDirectories: true
        )
        let path = sandbox.envCore.path(percentEncoded: false)
        guard mkfifo(path, 0o755) == 0 else {
            Issue.record("could not create a FIFO for the test")
            return
        }

        guard case .notFound(let status) = locate(path, sandbox),
              let message = notBuiltMessage(status) else {
            Issue.record("expected notFound for a FIFO")
            return
        }
        #expect(message.contains("is not a regular file, not an executable"))
    }

    @Test func rejectsANonExecutableMachOWithTheSystemChecksWording() throws {
        let sandbox = try Sandbox()
        defer { sandbox.cleanup() }
        try sandbox.writeCore(sandbox.envCore, executable: false)

        guard let actual = notBuiltMessage(locate(sandbox.envCore.path(percentEncoded: false), sandbox).rejection ?? .ready),
              let expected = notBuiltMessage(SystemChecks.nativeCore(executable: sandbox.envCore)) else {
            Issue.record("expected NOT BUILT from both the locator and SystemChecks")
            return
        }
        #expect(actual == expected)
        #expect(actual.contains("is not executable"))
    }

    @Test func aMissingCoreMatchesTheSystemChecksAbsentCoreWording() throws {
        let sandbox = try Sandbox()
        defer { sandbox.cleanup() }

        guard let actual = notBuiltMessage(locate(sandbox.envCore.path(percentEncoded: false), sandbox).rejection ?? .ready),
              let expected = notBuiltMessage(SystemChecks.nativeCore(executable: nil)) else {
            Issue.record("expected NOT BUILT from both the locator and SystemChecks")
            return
        }
        #expect(actual == expected)
        #expect(actual.contains("auxiliary executable absent"))
    }

    @Test func searchOrderIsEnvironmentThenBundleMacOSThenResourcesThenExecutableDirectory() throws {
        let sandbox = try Sandbox()
        defer { sandbox.cleanup() }
        for core in [sandbox.envCore, sandbox.macCore, sandbox.resourcesCore, sandbox.siblingCore] {
            try sandbox.writeCore(core)
        }

        #expect(locate(sandbox.envCore.path(percentEncoded: false), sandbox) == .found(sandbox.envCore))
        #expect(locate(nil, sandbox) == .found(sandbox.macCore))

        sandbox.remove(sandbox.macCore)
        #expect(locate(nil, sandbox) == .found(sandbox.resourcesCore))

        sandbox.remove(sandbox.resourcesCore)
        #expect(locate(nil, sandbox) == .found(sandbox.siblingCore))
    }

    @Test func anInvalidEnvironmentCandidateDoesNotHideAValidBundleCore() throws {
        let sandbox = try Sandbox()
        defer { sandbox.cleanup() }
        try FileManager.default.createDirectory(
            at: sandbox.envCore.deletingLastPathComponent(), withIntermediateDirectories: true
        )
        try FileManager.default.createDirectory(at: sandbox.envCore, withIntermediateDirectories: true)
        try sandbox.writeCore(sandbox.macCore)

        #expect(locate(sandbox.envCore.path(percentEncoded: false), sandbox) == .found(sandbox.macCore))
    }

    @Test func anEmptyEnvironmentVariableIsIgnored() throws {
        let sandbox = try Sandbox()
        defer { sandbox.cleanup() }
        try sandbox.writeCore(sandbox.macCore)

        #expect(locate("", sandbox) == .found(sandbox.macCore))
    }

    @Test func aBrokenCandidateIsReportedWhenNothingElseIsUsable() throws {
        let sandbox = try Sandbox()
        defer { sandbox.cleanup() }
        try sandbox.writeScript(sandbox.envCore)

        guard case .notFound(let status) = locate(sandbox.envCore.path(percentEncoded: false), sandbox),
              let message = notBuiltMessage(status) else {
            Issue.record("expected notFound")
            return
        }
        #expect(message.contains("refusing scripts/stubs"))
    }

    @Test func factoryFallsBackWhenNoCoreIsFound() throws {
        let sandbox = try Sandbox()
        defer { sandbox.cleanup() }
        let client = CoreClientFactory.make(
            environment: [:],
            bundleURL: sandbox.bundle,
            executableURL: sandbox.runningExecutable,
            fallback: { UnavailableCoreClient(failure: CoreFailure(code: .io, message: "no core")) }
        )
        #expect(client is UnavailableCoreClient)
    }

    @Test func factoryPrefersTheProcessClientWhenTheLocatorFindsAValidCore() throws {
        let sandbox = try Sandbox()
        defer { sandbox.cleanup() }
        // A native slice of `true` proves transport selection without invoking Rosetta. It speaks no
        // protocol; protocol agreement is covered by the opt-in real core test.
        try FileManager.default.createDirectory(
            at: sandbox.envCore.deletingLastPathComponent(), withIntermediateDirectories: true
        )
        try NativeExecutableFixture.write(to: sandbox.envCore)

        let client = CoreClientFactory.make(
            environment: [CoreLocator.environmentVariable: sandbox.envCore.path(percentEncoded: false)],
            bundleURL: sandbox.bundle,
            executableURL: sandbox.runningExecutable,
            fallback: { UnavailableCoreClient(failure: CoreFailure(code: .io, message: "no core")) }
        )
        #expect(client is ProcessCoreClient)
        (client as? ProcessCoreClient)?.shutdown()
    }
}
