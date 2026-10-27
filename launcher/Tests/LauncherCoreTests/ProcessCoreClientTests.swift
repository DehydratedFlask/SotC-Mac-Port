import Foundation
import Testing
@testable import LauncherCore

/// APP-01-LAUNCHER-PROCESS-CLIENT-01: protocol v1 over a real child process.
///
/// These tests use a temporary shell-script fake core that speaks the protocol on stdin/stdout. The
/// locator is deliberately bypassed: it rejects scripts, and these tests must not need the real
/// `SoCGameCore` (built by the native worker) to exist.
struct ProcessCoreClientTests {
    private enum Fixtures {
        static func status(
            state: String = "idle", abi: Int = 65536, schema: String = "sotc.core.status.v1"
        ) -> String {
            """
            {"id":__ID__,"schema":"\(schema)","abi":\(abi),"state":"\(state)","last_error":0,"last_error_name":"SOTC_OK","assets_validated":false,"assets_ready":false,"engine_implemented":false,"ctor":{"linked":true,"original_slots":573,"slots_registered":308,"slots_with_callback":308,"completed":308,"run_ok":true,"callback_failed":false,"blocked_on_rva":0,"first_unrecovered_index":296}}
            """
        }

        static let coverage = """
        {"id":__ID__,"schema":"sotc.core.coverage.v1","abi":65536,"engine_implemented":false,"subsystems":[{"name":"startup_constructors","status":"partial","detail":"308 of 573 slots"},{"name":"audio","status":"not_implemented","detail":""}]}
        """

        static let validation = """
        {"id":__ID__,"schema":"sotc.core.validate.v1","ok":true,"error":0,"error_name":"SOTC_OK","assets_ready":false,"engine_ready":false,"start_would_succeed":false,"archives":[{"name":"cachedFiles.psarc","status":"ok","size_bytes":1024},{"name":"pak0.psarc","status":"missing","size_bytes":0}],"detail":"pak0.psarc not found"}
        """

        static let validationAssetsNotReady = """
        {"id":__ID__,"schema":"sotc.core.validate.v1","ok":false,"error":3,"error_name":"SOTC_ERR_ASSETS_NOT_READY","assets_ready":false,"engine_ready":false,"start_would_succeed":false,"archives":[{"name":"cachedFiles.psarc","status":"ok","size_bytes":1024},{"name":"pak0.psarc","status":"missing","size_bytes":0}],"detail":"pak0.psarc not found"}
        """

        static let validationIOVerdict = """
        {"id":__ID__,"schema":"sotc.core.validate.v1","ok":false,"error":5,"error_name":"SOTC_ERR_IO","assets_ready":false,"engine_ready":false,"start_would_succeed":false,"archives":[{"name":"cachedFiles.psarc","status":"missing","size_bytes":0}],"detail":"assets path is not a directory: /tmp/nope"}
        """

        static let validationHardFailure = """
        {"id":__ID__,"schema":"sotc.core.validate.v1","ok":false,"error":2,"error_name":"SOTC_ERR_BAD_STATE","assets_ready":false,"engine_ready":false,"start_would_succeed":false,"archives":[],"detail":"validate() is not valid while the core is running"}
        """

        static let startRefused = """
        {"id":__ID__,"schema":"sotc.core.start.v1","ok":false,"error":4,"error_name":"SOTC_ERR_ENGINE_NOT_IMPLEMENTED"}
        """

        static let startAccepted = """
        {"id":__ID__,"schema":"sotc.core.start.v1","ok":true}
        """

        static let stopOK = """
        {"id":__ID__,"schema":"sotc.core.stop.v1","ok":true}
        """
    }

    private struct FakeCore {
        let directory: URL
        let executable: URL
        let log: URL

        func remove() {
            try? FileManager.default.removeItem(at: directory)
        }
    }

    private func newFake() throws -> FakeCore {
        let directory = FileManager.default.temporaryDirectory
            .appending(path: "ProcessCoreClientTests-\(UUID().uuidString)", directoryHint: .isDirectory)
        try FileManager.default.createDirectory(at: directory, withIntermediateDirectories: true)
        return FakeCore(
            directory: directory,
            executable: directory.appending(path: "fake-core", directoryHint: .notDirectory),
            log: directory.appending(path: "commands.log", directoryHint: .notDirectory)
        )
    }

    private func writeScript(_ body: String, to fake: FakeCore) throws {
        try Data(body.utf8).write(to: fake.executable)
        try FileManager.default.setAttributes(
            [.posixPermissions: 0o755], ofItemAtPath: fake.executable.path(percentEncoded: false)
        )
    }

    /// Replaces the placeholder with printf's `%s` and escapes single quotes for the shell script.
    private func printfLiteral(_ json: String) -> String {
        json
            .replacingOccurrences(of: "__ID__", with: "%s")
            .replacingOccurrences(of: "'", with: "'\\''")
    }

    private func makeProtocolFake(
        status: String = Fixtures.status(),
        coverage: String = Fixtures.coverage,
        validate: String = Fixtures.validation,
        start: String = Fixtures.startRefused,
        stop: String = Fixtures.stopOK,
        slowResponses: Bool = false
    ) throws -> FakeCore {
        let fake = try newFake()
        let log = fake.log.path(percentEncoded: false).replacingOccurrences(of: "'", with: "'\\''")
        let responseDelay = slowResponses ? "sleep 0.05" : ":"
        let script = """
        #!/bin/sh
        LOG='\(log)'
        while IFS= read -r line; do
          printf '%s\\n' "$line" >> "$LOG"
          id=${line%% *}
          rest=${line#* }
          cmd=${rest%% *}
          \(responseDelay)
          case "$cmd" in
          status) printf '\(printfLiteral(status))\\n' "$id" ;;
          coverage) printf '\(printfLiteral(coverage))\\n' "$id" ;;
          validate) printf '\(printfLiteral(validate))\\n' "$id" ;;
          start) printf '\(printfLiteral(start))\\n' "$id" ;;
          stop) printf '\(printfLiteral(stop))\\n' "$id" ;;
          quit) printf '{"id":%s,"ok":true,"bye":true}\\n' "$id"; exit 0 ;;
          esac
        done
        exit 0
        """
        try writeScript(script, to: fake)
        return fake
    }

    private func makeRawFake(_ body: String) throws -> FakeCore {
        let fake = try newFake()
        try writeScript(body, to: fake)
        return fake
    }

    private func logLines(_ fake: FakeCore) -> [String] {
        guard let text = try? String(contentsOf: fake.log, encoding: .utf8) else { return [] }
        return text.split(whereSeparator: \.isNewline).map(String.init)
    }

    private func expectFailure(
        _ operation: () async throws -> Void, sourceLocation: SourceLocation = #_sourceLocation
    ) async -> CoreFailure? {
        do {
            try await operation()
            Issue.record("expected a CoreFailure", sourceLocation: sourceLocation)
            return nil
        } catch let failure as CoreFailure {
            return failure
        } catch {
            Issue.record("expected CoreFailure, got \(error)", sourceLocation: sourceLocation)
            return nil
        }
    }

    // MARK: - Protocol mapping

    @Test func statusMapsEveryReportedField() async throws {
        let fake = try makeProtocolFake()
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable)
        defer { client.shutdown() }

        let status = try await client.status()

        #expect(status.state == .idle)
        #expect(status.lastError == .ok)
        #expect(!status.assetsValidated)
        #expect(!status.assetsReady)
        #expect(!status.engineImplemented)
        #expect(status.ctor.linked)
        #expect(status.ctor.originalSlots == 573)
        #expect(status.ctor.slotsRegistered == 308)
        #expect(status.ctor.slotsWithCallback == 308)
        #expect(status.ctor.completed == 308)
        #expect(status.ctor.runOK)
        #expect(!status.ctor.callbackFailed)
        #expect(status.ctor.blockedOnRVA == 0)
        #expect(status.ctor.firstUnrecoveredIndex == 296)
        #expect(client.abiVersion == CoreABIVersion(major: 1, minor: 0))
        #expect(CoreClientMapping.launcherState(for: status) == .idle)
    }

    @Test func statusAcceptsANullFirstUnrecoveredIndex() async throws {
        // The native formatter emits null when it has no index; that must not break the parse.
        let nullIndex = Fixtures.status()
            .replacingOccurrences(of: "\"first_unrecovered_index\":296", with: "\"first_unrecovered_index\":null")
        let fake = try makeProtocolFake(status: nullIndex)
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable)
        defer { client.shutdown() }

        let status = try await client.status()
        #expect(status.ctor.firstUnrecoveredIndex == nil)
        #expect(status.ctor.originalSlots == 573)
    }

    @Test func coverageMapsABIAndSubsystems() async throws {
        let fake = try makeProtocolFake()
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable)
        defer { client.shutdown() }

        let coverage = try await client.coverage()

        #expect(coverage.abiVersion == CoreABIVersion(major: 1, minor: 0))
        #expect(!coverage.engineImplemented)
        #expect(coverage.ctor == .notLinked) // protocol v1 coverage carries no ctor object
        #expect(coverage.subsystems.count == 2)
        #expect(coverage.subsystems[0].name == "startup_constructors")
        #expect(coverage.subsystems[0].status == .partial)
        #expect(coverage.subsystems[0].detail == "308 of 573 slots")
        #expect(coverage.subsystems[1].status == .notImplemented)
    }

    @Test func concurrentStatusAndCoverageRequestsKeepResponsesPaired() async throws {
        let fake = try makeProtocolFake(slowResponses: true)
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable)
        defer { client.shutdown() }

        async let statusTask = client.status()
        async let coverageTask = client.coverage()
        let (status, coverage) = try await (statusTask, coverageTask)

        #expect(status.state == .idle)
        #expect(status.lastError == .ok)
        #expect(!status.engineImplemented)
        #expect(coverage.abiVersion == CoreABIVersion(major: 1, minor: 0))
        #expect(coverage.subsystems.map(\.name) == ["startup_constructors", "audio"])
        #expect(coverage.subsystems[0].status == .partial)
        #expect(coverage.subsystems[1].status == .notImplemented)

        let requests = logLines(fake).compactMap { line -> (id: String, command: String)? in
            let fields = line.split(separator: " ")
            guard fields.count == 2 else { return nil }
            return (String(fields[0]), String(fields[1]))
        }
        #expect(requests.count == 2)
        #expect(Set(requests.map { $0.command }) == Set(["status", "coverage"]))
        #expect(Set(requests.map { $0.id }) == Set(["1", "2"]))
    }

    @Test func validateMapsArchivesAndEngineFacts() async throws {
        let fake = try makeProtocolFake()
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable)
        defer { client.shutdown() }

        let validation = try await client.validate(assetsDirectory: URL(filePath: "/tmp/good-assets", directoryHint: .isDirectory))

        #expect(validation.verdict == .ok)
        #expect(!validation.assetsReady)
        #expect(!validation.engineReady)
        #expect(!validation.startWouldSucceed)
        #expect(validation.engineError == .engineNotImplemented)
        #expect(validation.problemCount == 1)
        #expect(validation.detail == "pak0.psarc not found")
        #expect(validation.archives == [
            CoreArchiveCheck(name: "cachedFiles.psarc", status: .ok, sizeBytes: 1024),
            CoreArchiveCheck(name: "pak0.psarc", status: .missing, sizeBytes: 0),
        ])
        #expect(CoreClientMapping.nativeCoreStatus(validation: validation, coverage: nil) != .ready)
    }

    @Test func assetsNotReadyIsANormalValidationNotAnError() async throws {
        let fake = try makeProtocolFake(validate: Fixtures.validationAssetsNotReady)
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable)
        defer { client.shutdown() }
        let directory = URL(filePath: "/tmp/unready-assets", directoryHint: .isDirectory)

        // The native --serve implementation reports ok:false/error:3 for a failed asset check; that is
        // a CoreValidation verdict, not a transport error, so the assets row can show the core's detail.
        let validation = try await client.validate(assetsDirectory: directory)

        #expect(validation.verdict == .assetsNotReady)
        #expect(!validation.assetsReady)
        #expect(!validation.engineReady)
        #expect(validation.problemCount == 1)
        #expect(validation.detail == "pak0.psarc not found")
        #expect(CoreReadiness.assetsStatus(directory: directory, validation: validation, failure: nil)
            == .missing("pak0.psarc not found"))
    }

    @Test func anIOVerdictIsANormalValidationLikeTheInProcessClient() async throws {
        let fake = try makeProtocolFake(validate: Fixtures.validationIOVerdict)
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable)
        defer { client.shutdown() }
        let directory = URL(filePath: "/tmp/nope", directoryHint: .isDirectory)

        let validation = try await client.validate(assetsDirectory: directory)

        #expect(validation.verdict == .io)
        #expect(!validation.assetsReady)
        #expect(validation.problemCount == 1)
        #expect(validation.detail == "assets path is not a directory: /tmp/nope")
        #expect(CoreReadiness.assetsStatus(directory: directory, validation: validation, failure: nil)
            == .missing("assets path is not a directory: /tmp/nope"))
    }

    @Test func validateHardFailureThrowsTheCoreCode() async throws {
        let fake = try makeProtocolFake(validate: Fixtures.validationHardFailure)
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable)
        defer { client.shutdown() }

        let failure = try #require(await expectFailure {
            _ = try await client.validate(assetsDirectory: URL(filePath: "/tmp/nope", directoryHint: .isDirectory))
        })
        #expect(failure.code == .badState)
        #expect(failure.message == "validate() is not valid while the core is running")
    }

    @Test func startRefusalThrowsEngineNotImplementedAndNeverRuns() async throws {
        let fake = try makeProtocolFake()
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable)
        defer { client.shutdown() }

        let failure = try #require(await self.expectFailure { try await client.start() })
        #expect(failure.code == .engineNotImplemented)

        // A refusal must not be turned into running: the core still reports idle, no engine.
        let status = try await client.status()
        #expect(!status.engineImplemented)
        #expect(CoreClientMapping.launcherState(for: status) != .running)
        #expect(logLines(fake).contains { $0.hasSuffix(" start") })
    }

    @Test func aFutureAcceptedStartDoesNotInventRunning() async throws {
        let fake = try makeProtocolFake(start: Fixtures.startAccepted)
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable)
        defer { client.shutdown() }

        try await client.start()

        // The client only reports what status() says; the fake core never says running.
        let status = try await client.status()
        #expect(status.state == .idle)
        #expect(CoreClientMapping.launcherState(for: status) == .idle)
    }

    @Test func stopSendsTheStopCommand() async throws {
        let fake = try makeProtocolFake()
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable)
        defer { client.shutdown() }

        try await client.stop()

        #expect(logLines(fake).contains { $0.hasSuffix(" stop") })
    }

    // MARK: - Lifecycle

    @Test func shutdownSendsQuitAndIsIdempotent() async throws {
        let fake = try makeProtocolFake()
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable)

        _ = try await client.status()
        client.shutdown()
        client.shutdown()

        #expect(logLines(fake).contains { $0.hasSuffix(" quit") })
    }

    @Test func shutdownFailsPendingStatusBeforeSendingQuit() async throws {
        let fake = try newFake()
        defer { fake.remove() }
        let log = fake.log.path(percentEncoded: false).replacingOccurrences(of: "'", with: "'\\''")
        try writeScript("""
        #!/bin/sh
        LOG='\(log)'
        while IFS= read -r line; do
          printf '%s\\n' "$line" >> "$LOG"
          command=${line#* }
          case "$command" in
          status) : ;; # Keep status pending until shutdown sends quit.
          quit) exit 0 ;;
          esac
        done
        exit 0
        """, to: fake)
        let client = try ProcessCoreClient(coreURL: fake.executable)
        defer { client.shutdown() }

        let statusTask = Task { try await client.status() }
        var statusWasLogged = false
        for _ in 0..<100 {
            if logLines(fake).contains(where: { $0.hasSuffix(" status") }) {
                statusWasLogged = true
                break
            }
            try await Task.sleep(for: .milliseconds(10))
        }
        #expect(statusWasLogged)

        client.shutdown()

        let failure = try #require(await expectFailure { _ = try await statusTask.value })
        #expect(failure.code == .badState)
        let commands = logLines(fake).compactMap { $0.split(separator: " ").last.map(String.init) }
        #expect(commands == ["status", "quit"])
    }

    @Test func requestsAfterShutdownFailWithBadState() async throws {
        let fake = try makeProtocolFake()
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable)
        client.shutdown()

        let failure = try #require(await self.expectFailure { _ = try await client.status() })
        #expect(failure.code == .badState)
    }

    // MARK: - Misbehaving core

    @Test func timeoutFailsWithIOAndTearsTheChildDown() async throws {
        let fake = try makeRawFake("""
        #!/bin/sh
        read -r line
        exec /bin/sleep 30
        """)
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable, responseTimeout: 0.2)
        defer { client.shutdown() }

        let failure = try #require(await self.expectFailure { _ = try await client.status() })
        #expect(failure.code == .io)
        #expect(failure.message.contains("did not answer"))
    }

    @Test func earlyExitFailsWithIO() async throws {
        let fake = try makeRawFake("""
        #!/bin/sh
        exit 0
        """)
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable, responseTimeout: 2)
        defer { client.shutdown() }

        let failure = try #require(await self.expectFailure { _ = try await client.status() })
        #expect(failure.code == .io)
        #expect(failure.message.contains("native game core"))
    }

    @Test func exitAfterReceivingRequestButBeforeReplyFailsWithIO() async throws {
        let fake = try newFake()
        defer { fake.remove() }
        let log = fake.log.path(percentEncoded: false).replacingOccurrences(of: "'", with: "'\\''")
        try writeScript("""
        #!/bin/sh
        LOG='\(log)'
        IFS= read -r line || exit 3
        printf '%s\\n' "$line" >> "$LOG"
        exit 17
        """, to: fake)
        let client = try ProcessCoreClient(coreURL: fake.executable, responseTimeout: 2)
        defer { client.shutdown() }

        let failure = try #require(await self.expectFailure { _ = try await client.status() })

        #expect(failure.code == .io)
        #expect(!failure.message.contains("did not answer"))
        let requests = logLines(fake)
        #expect(requests.count == 1)
        #expect(requests.first?.hasSuffix(" status") == true)
    }

    @Test func exitAfterOneAnswerFailsTheNextRequest() async throws {
        let fake = try makeRawFake("""
        #!/bin/sh
        read -r line
        id=${line%% *}
        printf '{"id":%s,"schema":"sotc.core.status.v1","abi":65536,"state":"idle","last_error":0,"last_error_name":"SOTC_OK","assets_validated":false,"assets_ready":false,"engine_implemented":false,"ctor":{"linked":false,"slots_registered":0,"slots_with_callback":0,"completed":0,"run_ok":false,"callback_failed":false,"blocked_on_rva":0}}\\n' "$id"
        exit 0
        """)
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable, responseTimeout: 2)
        defer { client.shutdown() }

        _ = try await client.status()

        let failure = try #require(await self.expectFailure { _ = try await client.coverage() })
        #expect(failure.code == .io)
    }

    @Test func malformedJSONFailsWithInternalErrorAndIsSticky() async throws {
        let fake = try makeRawFake("""
        #!/bin/sh
        read -r line
        printf 'this is not json\\n'
        exec /bin/sleep 10
        """)
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable, responseTimeout: 2)
        defer { client.shutdown() }

        let failure = try #require(await self.expectFailure { _ = try await client.status() })
        #expect(failure.code == .internalError)
        #expect(failure.message.contains("malformed answer"))

        let second = try #require(await self.expectFailure { _ = try await client.status() })
        #expect(second.code == .internalError)
    }

    @Test func aWrongIDFailsWithInternalError() async throws {
        let fake = try makeProtocolFake(status: Fixtures.status().replacingOccurrences(of: "\"id\":__ID__", with: "\"id\":999"))
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable, responseTimeout: 2)
        defer { client.shutdown() }

        let failure = try #require(await self.expectFailure { _ = try await client.status() })
        #expect(failure.code == .internalError)
        #expect(failure.message.contains("does not match request id"))
    }

    @Test func unknownStateFailsWithInternalErrorInsteadOfGuessing() async throws {
        let fake = try makeProtocolFake(status: Fixtures.status(state: "teleporting"))
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable, responseTimeout: 2)
        defer { client.shutdown() }

        let failure = try #require(await self.expectFailure { _ = try await client.status() })
        #expect(failure.code == .internalError)
        #expect(failure.message.contains("unknown core state"))
    }

    @Test func anotherABIMajorIsRejected() async throws {
        let fake = try makeProtocolFake(status: Fixtures.status(abi: 131_072))
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable, responseTimeout: 2)
        defer { client.shutdown() }

        let failure = try #require(await self.expectFailure { _ = try await client.status() })
        #expect(failure.code == .abiMismatch)
    }

    @Test func aMismatchedSchemaIsRejected() async throws {
        let fake = try makeProtocolFake(status: Fixtures.status(schema: "sotc.core.status.v2"))
        defer { fake.remove() }
        let client = try ProcessCoreClient(coreURL: fake.executable, responseTimeout: 2)
        defer { client.shutdown() }

        let failure = try #require(await self.expectFailure { _ = try await client.status() })
        #expect(failure.code == .internalError)
        #expect(failure.message.contains("answer schema"))
    }
}
