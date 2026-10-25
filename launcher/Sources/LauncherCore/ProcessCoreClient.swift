import CoreFoundation
import Foundation

/// `CoreClient` over the child-process protocol spoken by `SoCGameCore --serve` (protocol v1).
///
/// Wire format: one request per stdin line, `<id> <command> [argument]` with a monotonically
/// increasing unsigned `id`; exactly one single-line JSON object per request on stdout, with the
/// matching `"id"` and the command's `schema`. Requests are serialized (one in flight at a time),
/// so an answer can never be attributed to the wrong request.
///
/// Honest failure modes, all thrown as `CoreFailure`:
/// - the executable cannot be launched → `.io` from `init`;
/// - the child exits, closes stdout, answers late (default 5 s), answers with an unexpected id or
///   schema, or sends unparseable JSON → `.io`/`.internalError`, and the connection is torn down;
/// - unknown state/subsystem names → `.internalError` (never guessed);
/// - a core that reports ABI major ≠ 1 → `.abiMismatch`;
/// - `start` refused with `SOTC_ERR_ENGINE_NOT_IMPLEMENTED` → `.engineNotImplemented`.
///
/// This type never invents `.running`: `status()` returns exactly the state string the core sent, and
/// `start()` returning normally means only that the core accepted the request. The launcher still asks
/// `status()` and `CoreClientMapping` rejects running without an engine. `validate()` reports a failed
/// asset check (including an unusable directory) as a normal `CoreValidation`; only hard call failures
/// throw.
///
/// Thread-safe via one serial queue that owns all mutable state (`@unchecked Sendable`); reading runs
/// on a detached background task, not a busy-wait loop. `shutdown()` (also called from `deinit`)
/// sends `quit`, closes stdin, waits briefly for exit, terminates if needed and reaps the child once.
public final class ProcessCoreClient: CoreClient, @unchecked Sendable {
    public static let defaultResponseTimeout: TimeInterval = 5

    static let statusSchema = "sotc.core.status.v1"
    static let coverageSchema = "sotc.core.coverage.v1"
    static let validateSchema = "sotc.core.validate.v1"
    static let startSchema = "sotc.core.start.v1"

    private let responseTimeout: TimeInterval
    private let process = Process()
    private let stdinPipe = Pipe()
    private let stdoutPipe = Pipe()
    private let queue = DispatchQueue(label: "org.sotc.launcher.process-core-client")
    private let queueKey = DispatchSpecificKey<UInt8>()
    private let exitSemaphore = DispatchSemaphore(value: 0)
    private let readerDone = DispatchSemaphore(value: 0)
    private let abiLock = NSLock()

    private var readerTask: Task<Void, Never>?
    private var _abiVersion = CoreABIVersion(major: 1, minor: 0)

    // All properties below are owned by `queue`.
    private var nextID: UInt64 = 0
    private var inFlight: Request?
    private var queued: [Request] = []
    private var isShutDown = false
    private var transportFailure: CoreFailure?
    private var exitStatus: Int32?
    private var inputClosed = false
    private var outputClosed = false
    private var didTerminate = false
    private var didWaitForExit = false

    private struct Request {
        let id: UInt64
        let line: String
        let expectedSchema: String?
        let continuation: CheckedContinuation<Data, any Error>
    }

    /// - Parameters:
    ///   - coreURL: the executable to run with `--serve`. The locator owns the Mach-O policy; this
    ///     initializer only requires something `Process` can launch (tests use a shell script).
    ///   - responseTimeout: how long one request may wait for its answer. Timeout tears the child down.
    public init(coreURL: URL, responseTimeout: TimeInterval = ProcessCoreClient.defaultResponseTimeout) throws {
        self.responseTimeout = max(0.05, responseTimeout)
        process.executableURL = coreURL
        process.arguments = ["--serve"]
        process.standardInput = stdinPipe
        process.standardOutput = stdoutPipe
        process.standardError = FileHandle.nullDevice
        process.terminationHandler = { [weak self] finished in
            guard let self else { return }
            let status = finished.terminationStatus
            self.exitSemaphore.signal()
            self.queue.async { self.handleExit(status: status) }
        }
        do {
            try process.run()
        } catch {
            throw CoreFailure(
                code: .io,
                message: "Could not launch \(coreURL.lastPathComponent): \(error.localizedDescription)"
            )
        }
        // Writing to a pipe whose reader is gone must fail the request, not raise SIGPIPE.
        Self.suppressSIGPIPE(stdinPipe.fileHandleForWriting.fileDescriptor)
        // `Pipe` owns both ends; the parent must close the ends that belong to the child or EOF is
        // never observed (the reader would wait forever for a writer that only the child holds).
        try? stdoutPipe.fileHandleForWriting.close()
        try? stdinPipe.fileHandleForReading.close()
        queue.setSpecific(key: queueKey, value: 1)
        readerTask = Task.detached(priority: .utility) { [weak self] in
            await self?.readLoop()
        }
    }

    deinit { shutdown() }

    public var abiVersion: CoreABIVersion {
        abiLock.lock()
        defer { abiLock.unlock() }
        return _abiVersion
    }

    // MARK: - CoreClient

    public func status() async throws -> CoreStatus {
        let response = try await perform("status", schema: Self.statusSchema)
        do {
            let abi = try Self.abi(from: response)
            try record(abi)
            return try Self.status(from: response)
        } catch {
            throw Self.failure(from: error)
        }
    }

    public func coverage() async throws -> CoreCoverageReport {
        let response = try await perform("coverage", schema: Self.coverageSchema)
        do {
            let abi = try Self.abi(from: response)
            try record(abi)
            return try Self.coverage(from: response, abi: abi)
        } catch {
            throw Self.failure(from: error)
        }
    }

    public func validate(assetsDirectory: URL) async throws -> CoreValidation {
        let path = assetsDirectory.path(percentEncoded: false)
        guard !path.isEmpty else {
            throw CoreFailure(code: .invalidArgument, message: "The assets directory path is empty.")
        }
        guard !path.contains("\n"), !path.contains("\r") else {
            throw CoreFailure(
                code: .invalidArgument,
                message: "The assets directory path contains a line break, which the core protocol cannot carry."
            )
        }
        let response = try await perform("validate", argument: path, schema: Self.validateSchema)
        do {
            return try Self.validation(from: response)
        } catch {
            throw Self.failure(from: error)
        }
    }

    public func start() async throws {
        let response = try await perform("start", schema: Self.startSchema)
        do {
            guard try response.bool("ok") else {
                throw Self.refusal(from: response, fallback: "The native game core refused to start.")
            }
        } catch {
            throw Self.failure(from: error)
        }
    }

    public func stop() async throws {
        let response = try await perform("stop", schema: nil)
        do {
            guard try response.bool("ok") else {
                throw Self.refusal(from: response, fallback: "The native game core refused to stop.")
            }
        } catch {
            throw Self.failure(from: error)
        }
    }

    // MARK: - Shutdown

    /// Sends `quit` (best effort), closes stdin, and gives the child one second to exit. A child that
    /// ignores the request gets SIGTERM and a second one-second grace, then SIGKILL; the process is
    /// reaped exactly once and the read end is closed after the reader finished. Idempotent; also
    /// called from `deinit`.
    public func shutdown() {
        // `deinit` can run while the serial queue is draining one of our own blocks (the last
        // reference may be released by a queued closure). `queue.sync` there would trap, so run
        // inline when we are already on that queue.
        if DispatchQueue.getSpecific(key: queueKey) != nil {
            shutdownLocked()
        } else {
            queue.sync { self.shutdownLocked() }
        }
    }

    private func shutdownLocked() {
        guard !isShutDown else { return }
        isShutDown = true
        failPending(with: CoreFailure(code: .badState, message: "The native game core client has been shut down."))

        if process.isRunning {
            nextID &+= 1
            let quit = Data("\(nextID) quit\n".utf8)
            try? stdinPipe.fileHandleForWriting.write(contentsOf: quit)
            closeInput()
            if !waitForExit(seconds: 1) {
                terminateChild()
                if !waitForExit(seconds: 1) {
                    kill(process.processIdentifier, SIGKILL)
                    waitForExitOnce()
                }
            }
        } else {
            closeInput()
        }
        // Let the reader leave its `bytes.lines` loop before the read end is closed.
        let readerFinished = readerDone.wait(timeout: .now() + 1) == .success
        readerTask?.cancel()
        readerTask = nil
        if readerFinished { closeOutput() }
    }

    private func waitForExit(seconds: TimeInterval) -> Bool {
        exitSemaphore.wait(timeout: .now() + seconds) == .success
    }

    // MARK: - Reader

    private func readLoop() async {
        defer { readerDone.signal() }
        do {
            for try await line in stdoutPipe.fileHandleForReading.bytes.lines {
                queue.async { self.receive(line: line) }
            }
        } catch {
            // EOF, cancelled reader, or a closed handle: handled below.
        }
        queue.async { self.handleEndOfOutput() }
    }

    private func receive(line: String) {
        guard !isShutDown else { return }
        guard let request = inFlight else {
            abort(with: CoreFailure(
                code: .internalError,
                message: "The native game core sent an answer nobody asked for."
            ))
            return
        }
        inFlight = nil
        do {
            try Self.check(data: Data(line.utf8), id: request.id, schema: request.expectedSchema)
            request.continuation.resume(returning: Data(line.utf8))
            pump()
        } catch {
            let failure = Self.failure(from: error)
            request.continuation.resume(throwing: failure)
            abort(with: failure)
        }
    }

    private func handleEndOfOutput() {
        guard !isShutDown else { return }
        if process.isRunning { terminateChild() }
        let message = exitStatus.map { "The native game core exited unexpectedly (status \($0))." }
            ?? "The native game core closed its output before answering."
        abort(with: CoreFailure(code: .io, message: message))
    }

    /// Records the exit but does not fail pending requests: stdout may still hold the final answer,
    /// which the reader delivers before EOF. Waiting for EOF keeps a complete answer from being
    /// replaced by an exit error because the two notifications can be scheduled in either order.
    private func handleExit(status: Int32) {
        guard !isShutDown else { return }
        exitStatus = status
    }

    // MARK: - Request pipeline (queue-owned)

    private func perform(_ command: String, argument: String? = nil, schema: String?) async throws -> JSONResponse {
        let data = try await withCheckedThrowingContinuation { (continuation: CheckedContinuation<Data, any Error>) in
            queue.async {
                self.enqueue(command: command, argument: argument, schema: schema, continuation: continuation)
            }
        }
        do {
            return try JSONResponse(data: data)
        } catch {
            throw Self.failure(from: error)
        }
    }

    private func enqueue(
        command: String, argument: String?, schema: String?,
        continuation: CheckedContinuation<Data, any Error>
    ) {
        if isShutDown {
            continuation.resume(throwing: CoreFailure(
                code: .badState, message: "The native game core client has been shut down."
            ))
            return
        }
        if let transportFailure {
            continuation.resume(throwing: transportFailure)
            return
        }
        nextID &+= 1
        var line = "\(nextID) \(command)"
        if let argument { line += " \(argument)" }
        queued.append(Request(id: nextID, line: line, expectedSchema: schema, continuation: continuation))
        pump()
    }

    private func pump() {
        guard !isShutDown, transportFailure == nil, inFlight == nil, !queued.isEmpty else { return }
        let request = queued.removeFirst()
        inFlight = request
        do {
            try stdinPipe.fileHandleForWriting.write(contentsOf: Data((request.line + "\n").utf8))
        } catch {
            inFlight = nil
            let failure = CoreFailure(
                code: .io,
                message: "Could not send a request to the native game core: \(error.localizedDescription)"
            )
            request.continuation.resume(throwing: failure)
            abort(with: failure)
            return
        }
        let id = request.id
        queue.asyncAfter(deadline: .now() + responseTimeout) { [weak self] in
            self?.timedOut(id: id)
        }
    }

    private func timedOut(id: UInt64) {
        guard !isShutDown, let request = inFlight, request.id == id else { return }
        inFlight = nil
        let failure = CoreFailure(
            code: .io,
            message: "The native game core did not answer within \(Self.seconds(responseTimeout))."
        )
        request.continuation.resume(throwing: failure)
        abort(with: failure)
    }

    /// Sticky transport failure: every pending and future request sees the same `CoreFailure`.
    private func abort(with failure: CoreFailure) {
        if transportFailure == nil { transportFailure = failure }
        failPending(with: transportFailure ?? failure)
        terminateChild()
    }

    private func failPending(with failure: CoreFailure) {
        if let pending = inFlight {
            inFlight = nil
            pending.continuation.resume(throwing: failure)
        }
        let pendingQueue = queued
        queued.removeAll()
        for request in pendingQueue {
            request.continuation.resume(throwing: failure)
        }
    }

    private func terminateChild() {
        guard !didTerminate else { return }
        didTerminate = true
        if process.isRunning { process.terminate() }
    }

    private func waitForExitOnce() {
        guard !didWaitForExit else { return }
        didWaitForExit = true
        process.waitUntilExit()
    }

    private func closeInput() {
        guard !inputClosed else { return }
        inputClosed = true
        try? stdinPipe.fileHandleForWriting.close()
    }

    private func closeOutput() {
        guard !outputClosed else { return }
        outputClosed = true
        try? stdoutPipe.fileHandleForReading.close()
    }

    private func record(_ version: CoreABIVersion) throws {
        guard version.major == 1 else {
            throw CoreFailure(
                code: .abiMismatch,
                message: "The native game core speaks ABI \(version); this launcher speaks 1.x."
            )
        }
        abiLock.lock()
        defer { abiLock.unlock() }
        _abiVersion = version
    }

    // MARK: - Answer parsing

    private static func abi(from response: JSONResponse) throws -> CoreABIVersion {
        CoreABIVersion(packed: try response.uint32("abi"))
    }

    private static func status(from response: JSONResponse) throws -> CoreStatus {
        let stateName = try response.string("state")
        guard let state = state(named: stateName) else {
            throw ResponseDecodingError("unknown core state \"\(stateName)\"")
        }
        return try CoreStatus(
            state: state,
            lastError: CoreErrorCode(code: response.uint32("last_error")),
            assetsValidated: response.bool("assets_validated"),
            assetsReady: response.bool("assets_ready"),
            engineImplemented: response.bool("engine_implemented"),
            ctor: ctor(from: response.object("ctor"))
        )
    }

    private static func coverage(from response: JSONResponse, abi: CoreABIVersion) throws -> CoreCoverageReport {
        let subsystems = try response.array("subsystems").map { item -> CoreSubsystem in
            let statusName = try item.string("status")
            guard let status = subsystemStatus(named: statusName) else {
                throw ResponseDecodingError("unknown subsystem status \"\(statusName)\"")
            }
            return try CoreSubsystem(
                name: item.string("name"), status: status, detail: item.string("detail")
            )
        }
        // Protocol v1 coverage does not carry the ctor prefix; status does. Only a core that adds
        // the optional object gets ctor facts here, otherwise the row stays NOT LINKED (never guessed).
        let ctor = try response.objectIfPresent("ctor").map { try Self.ctor(from: $0) } ?? .notLinked
        return try CoreCoverageReport(
            abiVersion: abi,
            engineImplemented: response.bool("engine_implemented"),
            ctor: ctor,
            subsystems: subsystems
        )
    }

    private static func validation(from response: JSONResponse) throws -> CoreValidation {
        let ok = try response.bool("ok")
        let verdict = CoreErrorCode(code: try response.uint32("error"))
        let detail = try response.stringIfPresent("detail") ?? ""
        if ok {
            guard verdict == .ok else {
                throw ResponseDecodingError("validate answered ok with error \(verdict)")
            }
        } else {
            // `assets_not_ready` and `io` are validation verdicts with the full archive payload
            // (the C ABI returns SOTC_OK for both). Anything else is a hard call failure
            // (invalid argument, bad state, internal) and must be thrown, not reported as a check.
            switch verdict {
            case .assetsNotReady, .io: break
            default:
                throw refusal(from: response, fallback: "The native game core refused to validate the files.")
            }
        }
        let archives = try response.array("archives").map { item -> CoreArchiveCheck in
            let statusName = try item.string("status")
            guard let status = archiveStatus(named: statusName) else {
                throw ResponseDecodingError("unknown archive status \"\(statusName)\"")
            }
            return try CoreArchiveCheck(
                name: item.string("name"), status: status, sizeBytes: item.uint64("size_bytes")
            )
        }
        let engineReady = try response.bool("engine_ready")
        return try CoreValidation(
            verdict: verdict,
            assetsReady: response.bool("assets_ready"),
            engineReady: engineReady,
            startWouldSucceed: response.bool("start_would_succeed"),
            problemCount: archives.filter { $0.status != .ok }.count,
            // The wire has one verdict and an independent engine flag. Map the flag to the ABI's
            // separate engine_error field exactly as `sotc_core.cpp` does; never claim engine success.
            engineError: engineReady ? .ok : .engineNotImplemented,
            archives: archives,
            detail: detail
        )
    }

    private static func ctor(from response: JSONResponse) throws -> CoreCtorPrefix {
        try CoreCtorPrefix(
            linked: response.bool("linked"),
            slotsRegistered: response.int("slots_registered"),
            slotsWithCallback: response.int("slots_with_callback"),
            completed: response.int("completed"),
            runOK: response.bool("run_ok"),
            callbackFailed: response.bool("callback_failed"),
            blockedOnRVA: response.uint32("blocked_on_rva"),
            originalSlots: response.intIfPresent("original_slots") ?? 0,
            firstUnrecoveredIndex: response.intIfPresent("first_unrecovered_index")
        )
    }

    private static func refusal(from response: JSONResponse, fallback: String) -> CoreFailure {
        let code = (try? response.uint32("error")).map { CoreErrorCode(code: $0) } ?? .internalError
        let detail: String? = try? response.stringIfPresent("detail")
        let name: String? = try? response.stringIfPresent("error_name")
        let message: String
        if let detail, !detail.isEmpty {
            message = detail
        } else if case .unknown = code, let name, !name.isEmpty {
            message = name
        } else {
            message = fallback
        }
        return CoreFailure(code: code == .ok ? .internalError : code, message: message)
    }

    private static func state(named name: String) -> CoreState? {
        switch name {
        case "idle": .idle
        case "validating": .validating
        case "ready": .ready
        case "starting": .starting
        case "running": .running
        case "stopping": .stopping
        case "failed": .failed
        default: nil
        }
    }

    private static func subsystemStatus(named name: String) -> CoreSubsystem.Status? {
        switch name {
        case "not_implemented": .notImplemented
        case "partial": .partial
        case "implemented": .implemented
        case "not_linked": .notLinked
        default: nil
        }
    }

    private static func archiveStatus(named name: String) -> CoreArchiveCheck.Status? {
        switch name {
        case "ok": .ok
        case "missing": .missing
        case "unreadable": .unreadable
        case "bad_magic": .badMagic
        default: nil
        }
    }

    private static func check(data: Data, id: UInt64, schema: String?) throws {
        let response = try JSONResponse(data: data)
        let answerID = try response.uint64("id")
        guard answerID == id else {
            throw ResponseDecodingError("answer id \(answerID) does not match request id \(id)")
        }
        if let schema {
            let answerSchema = try response.string("schema")
            guard answerSchema == schema else {
                throw ResponseDecodingError("answer schema \"\(answerSchema)\" is not \"\(schema)\"")
            }
        }
    }

    private static func failure(from error: any Error) -> CoreFailure {
        if let failure = error as? CoreFailure { return failure }
        if let decoding = error as? ResponseDecodingError {
            return CoreFailure(
                code: .internalError,
                message: "The native game core sent a malformed answer: \(decoding.description)"
            )
        }
        return CoreFailure(
            code: .internalError,
            message: "The native game core sent a malformed answer: \(error.localizedDescription)"
        )
    }

    private static func seconds(_ value: TimeInterval) -> String {
        String(format: "%.1f s", value)
    }

    private static func suppressSIGPIPE(_ descriptor: Int32) {
        let enabled: Int32 = 1
        _ = fcntl(descriptor, F_SETNOSIGPIPE, enabled)
    }
}

/// A validated single-line JSON object from the core.
private struct JSONResponse {
    let object: [String: Any]

    init(data: Data) throws {
        let parsed: Any
        do {
            parsed = try JSONSerialization.jsonObject(with: data, options: [.fragmentsAllowed])
        } catch {
            throw ResponseDecodingError("not JSON (\(error.localizedDescription))")
        }
        guard let object = parsed as? [String: Any] else {
            throw ResponseDecodingError("not a JSON object")
        }
        self.object = object
    }

    func string(_ key: String) throws -> String {
        guard let value = object[key] else { throw ResponseDecodingError("missing field \"\(key)\"") }
        guard let string = value as? String else {
            throw ResponseDecodingError("field \"\(key)\" is not a string")
        }
        return string
    }

    func stringIfPresent(_ key: String) throws -> String? {
        guard let value = object[key], !(value is NSNull) else { return nil }
        guard let string = value as? String else {
            throw ResponseDecodingError("field \"\(key)\" is not a string")
        }
        return string
    }

    func bool(_ key: String) throws -> Bool {
        guard let value = object[key] else { throw ResponseDecodingError("missing field \"\(key)\"") }
        guard let number = value as? NSNumber, CFGetTypeID(number) == CFBooleanGetTypeID() else {
            throw ResponseDecodingError("field \"\(key)\" is not a boolean")
        }
        return number.boolValue
    }

    func int(_ key: String) throws -> Int {
        guard let value = Int(try numberText(key)) else {
            throw ResponseDecodingError("field \"\(key)\" is not an integer")
        }
        return value
    }

    func intIfPresent(_ key: String) throws -> Int? {
        guard object[key] != nil, !(object[key] is NSNull) else { return nil }
        return try int(key)
    }

    func uint32(_ key: String) throws -> UInt32 {
        guard let value = UInt32(try numberText(key)) else {
            throw ResponseDecodingError("field \"\(key)\" is not an unsigned 32-bit integer")
        }
        return value
    }

    func uint64(_ key: String) throws -> UInt64 {
        guard let value = UInt64(try numberText(key)) else {
            throw ResponseDecodingError("field \"\(key)\" is not an unsigned 64-bit integer")
        }
        return value
    }

    func object(_ key: String) throws -> JSONResponse {
        guard let value = object[key] else { throw ResponseDecodingError("missing field \"\(key)\"") }
        guard let nested = value as? [String: Any] else {
            throw ResponseDecodingError("field \"\(key)\" is not an object")
        }
        return JSONResponse(object: nested)
    }

    func objectIfPresent(_ key: String) throws -> JSONResponse? {
        guard let value = object[key], !(value is NSNull) else { return nil }
        guard let nested = value as? [String: Any] else {
            throw ResponseDecodingError("field \"\(key)\" is not an object")
        }
        return JSONResponse(object: nested)
    }

    func array(_ key: String) throws -> [JSONResponse] {
        guard let value = object[key] else { throw ResponseDecodingError("missing field \"\(key)\"") }
        guard let items = value as? [Any] else {
            throw ResponseDecodingError("field \"\(key)\" is not an array")
        }
        return try items.map { item in
            guard let nested = item as? [String: Any] else {
                throw ResponseDecodingError("an item of \"\(key)\" is not an object")
            }
            return JSONResponse(object: nested)
        }
    }

    private init(object: [String: Any]) {
        self.object = object
    }

    private func numberText(_ key: String) throws -> String {
        guard let value = object[key] else { throw ResponseDecodingError("missing field \"\(key)\"") }
        guard let number = value as? NSNumber, CFGetTypeID(number) != CFBooleanGetTypeID() else {
            throw ResponseDecodingError("field \"\(key)\" is not a number")
        }
        return number.stringValue
    }
}

private struct ResponseDecodingError: Error, CustomStringConvertible {
    let description: String
    init(_ description: String) { self.description = description }
}
