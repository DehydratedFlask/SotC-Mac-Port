import CSotcCore
import Foundation
import Testing
@testable import LauncherCore
@testable import LauncherNativeCore

/// Pins the C header (`native/api/sotc_core.h`) against the Swift mirror in `CoreModels.swift` and
/// the in-process client in `NativeCoreClient.swift`.
///
/// The expected values come from the header's own constants, not from repeated magic numbers, so
/// renumbering a C error/state/archive/subsystem code without updating the Swift mirror fails here
/// instead of silently mislabelling a core answer.
struct AbiContractTests {
    // MARK: - Version handshake

    @Test func clientStatesTheVersionItCompiledAgainst() {
        // These two macros are plain literals and therefore visible to the Swift Clang importer;
        // `SOTC_CORE_ABI_VERSION` itself is an expression and is not (see the header comment).
        #expect(
            NativeCoreClient.compiledAgainstABIVersion
                == CoreABIVersion(major: UInt16(SOTC_CORE_ABI_MAJOR), minor: UInt16(SOTC_CORE_ABI_MINOR))
        )
        // This package compiles the C++ facade from the same header, so the compile-time and
        // run-time answers agree here. The handshake test below proves a disagreement is refused.
        #expect(NativeCoreClient.compiledAgainstABIVersion == CoreABIVersion(packed: sotc_core_abi_version()))
        #expect(NativeCoreClient.compiledAgainstABIVersion.major >= 1)
    }

    @Test func theHandshakeRefusesAnIncompatibleCaller() {
        let major = UInt32(SOTC_CORE_ABI_MAJOR)
        let minor = UInt32(SOTC_CORE_ABI_MINOR)
        // A newer major (breaks the ABI), a newer minor (appends), no version, and garbage.
        for expected in [(major + 1) << 16, (major << 16) | (minor + 1), 0, UInt32.max] {
            var handle: OpaquePointer?
            #expect(sotc_core_create(expected, &handle) == SOTC_ERR_ABI_MISMATCH, "expected \(expected) to be refused")
            #expect(handle == nil, "a refused create must not hand back a handle")
        }
        // The version the client actually passes is accepted and yields a live handle.
        var handle: OpaquePointer?
        #expect(sotc_core_create(NativeCoreClient.compiledAgainstABIVersion.packed, &handle) == SOTC_OK)
        #expect(handle != nil)
        sotc_core_destroy(handle)
    }

    @Test func theClientRefusesToConnectToAnIncompatibleLibrary() {
        // Proves `init` really hands its expectation to `sotc_core_create` instead of asking the
        // library for its own version (which would make the handshake vacuous).
        let newerMajor = CoreABIVersion(major: UInt16(SOTC_CORE_ABI_MAJOR) + 1, minor: UInt16(SOTC_CORE_ABI_MINOR))
        do {
            _ = try NativeCoreClient(expectedAbiVersion: newerMajor)
            Issue.record("connecting to a library that speaks ABI \(newerMajor) must be refused")
        } catch let failure as CoreFailure {
            #expect(failure.code == .abiMismatch)
            #expect(!failure.message.isEmpty)
        } catch {
            Issue.record("expected a CoreFailure, got \(error)")
        }
    }

    // MARK: - Numeric codes

    @Test func everyNativeErrorCodeHasItsOwnSwiftCase() {
        let codes: [(sotc_error, CoreErrorCode)] = [
            (SOTC_OK, .ok),
            (SOTC_ERR_INVALID_ARGUMENT, .invalidArgument),
            (SOTC_ERR_BAD_STATE, .badState),
            (SOTC_ERR_ASSETS_NOT_READY, .assetsNotReady),
            (SOTC_ERR_ENGINE_NOT_IMPLEMENTED, .engineNotImplemented),
            (SOTC_ERR_IO, .io),
            (SOTC_ERR_BUFFER_TOO_SMALL, .bufferTooSmall),
            (SOTC_ERR_ABI_MISMATCH, .abiMismatch),
            (SOTC_ERR_INTERNAL, .internalError),
        ]
        for (native, expected) in codes {
            #expect(CoreErrorCode(code: native.rawValue) == expected, "C code \(native.rawValue) mapped to the wrong case")
        }
        // Out-of-range codes stay numeric instead of being guessed.
        let firstUnknown = UInt32(SOTC_ERR_INTERNAL.rawValue) + 1
        #expect(CoreErrorCode(code: firstUnknown) == .unknown(firstUnknown))
        #expect(CoreErrorCode(code: UInt32.max) == .unknown(UInt32.max))
    }

    @Test func everyNativeStateCodeHasItsOwnSwiftCase() {
        let states: [(sotc_state, CoreState)] = [
            (SOTC_STATE_IDLE, .idle),
            (SOTC_STATE_VALIDATING, .validating),
            (SOTC_STATE_READY, .ready),
            (SOTC_STATE_STARTING, .starting),
            (SOTC_STATE_RUNNING, .running),
            (SOTC_STATE_STOPPING, .stopping),
            (SOTC_STATE_FAILED, .failed),
        ]
        for (native, expected) in states {
            #expect(CoreState(code: native.rawValue) == expected, "C state \(native.rawValue) mapped to the wrong case")
        }
        // An unknown state is rejected, never mapped onto a neighbouring one.
        let firstUnknown = UInt32(SOTC_STATE_FAILED.rawValue) + 1
        #expect(CoreState(code: firstUnknown) == nil)
        #expect(CoreState(code: UInt32.max) == nil)
    }

    @Test func everyNativeArchiveStatusHasItsOwnSwiftCase() {
        let statuses: [(sotc_archive_status, CoreArchiveCheck.Status)] = [
            (SOTC_ARCHIVE_OK, .ok),
            (SOTC_ARCHIVE_MISSING, .missing),
            (SOTC_ARCHIVE_UNREADABLE, .unreadable),
            (SOTC_ARCHIVE_BAD_MAGIC, .badMagic),
        ]
        for (native, expected) in statuses {
            #expect(
                CoreArchiveCheck.Status(code: native.rawValue) == expected,
                "C archive status \(native.rawValue) mapped to the wrong case"
            )
        }
        let firstUnknown = UInt32(SOTC_ARCHIVE_BAD_MAGIC.rawValue) + 1
        #expect(CoreArchiveCheck.Status(code: firstUnknown) == .unknown(firstUnknown))
    }

    @Test func everyNativeSubsystemStatusHasItsOwnSwiftCase() {
        let statuses: [(sotc_subsystem_status, CoreSubsystem.Status)] = [
            (SOTC_SUBSYSTEM_NOT_IMPLEMENTED, .notImplemented),
            (SOTC_SUBSYSTEM_PARTIAL, .partial),
            (SOTC_SUBSYSTEM_IMPLEMENTED, .implemented),
            (SOTC_SUBSYSTEM_NOT_LINKED, .notLinked),
        ]
        for (native, expected) in statuses {
            #expect(
                CoreSubsystem.Status(code: native.rawValue) == expected,
                "C subsystem status \(native.rawValue) mapped to the wrong case"
            )
        }
        let firstUnknown = UInt32(SOTC_SUBSYSTEM_NOT_LINKED.rawValue) + 1
        #expect(CoreSubsystem.Status(code: firstUnknown) == .unknown(firstUnknown))
    }

    // MARK: - Layout the client relies on

    @Test func fixedSizeArraysAreLaidOutTheWayTheClientDecodesThem() {
        // `NativeCoreClient` reads the archive/subsystem arrays by rebinding the tuple's bytes to the
        // element type. That is only correct while an element's size equals its stride and the tuple
        // is exactly count * stride, so a header change that adds tail padding fails here.
        #expect(MemoryLayout<sotc_archive_check>.size == MemoryLayout<sotc_archive_check>.stride)
        #expect(MemoryLayout<sotc_subsystem_report>.size == MemoryLayout<sotc_subsystem_report>.stride)

        let validation = sotc_validation_result()
        let archiveBytes = withUnsafeBytes(of: validation.archives) { $0.count }
        #expect(archiveBytes == Int(SOTC_ARCHIVE_COUNT) * MemoryLayout<sotc_archive_check>.stride)

        let coverage = sotc_coverage_report()
        let subsystemBytes = withUnsafeBytes(of: coverage.subsystems) { $0.count }
        #expect(subsystemBytes == Int(SOTC_SUBSYSTEM_COUNT) * MemoryLayout<sotc_subsystem_report>.stride)
    }

    @Test func headerCountsMatchTheLauncher() {
        #expect(Int(SOTC_ARCHIVE_COUNT) == AssetValidator.requiredArchives.count)
        // Every archive name the launcher checks must fit the ABI's fixed name buffer with its NUL.
        let longestName = AssetValidator.requiredArchives.map(\.count).max() ?? 0
        #expect(longestName + 1 <= Int(SOTC_NAME_CAPACITY))
    }

    // MARK: - Failure construction

    @Test func aFailureMessageBelongsToItsOwnCode() {
        // The handle's detail text describes its *most recent* failure. Attaching it to a different
        // code would report the engine gap for an argument error.
        let staleEngineText = "the native game engine is not implemented in this build"
        let mismatched = NativeCoreClient.failure(
            code: SOTC_ERR_INVALID_ARGUMENT.rawValue,
            handleLastError: SOTC_ERR_ENGINE_NOT_IMPLEMENTED.rawValue,
            handleMessage: staleEngineText
        )
        #expect(mismatched.code == .invalidArgument)
        #expect(mismatched.message != staleEngineText)
        #expect(mismatched.message.localizedCaseInsensitiveContains("invalid argument"))

        let matching = NativeCoreClient.failure(
            code: SOTC_ERR_BAD_STATE.rawValue,
            handleLastError: SOTC_ERR_BAD_STATE.rawValue,
            handleMessage: "start() is not valid while the core is idle; validate the assets first"
        )
        #expect(matching.code == .badState)
        #expect(matching.message == "start() is not valid while the core is idle; validate the assets first")

        // The handle could not be asked: fall back to the fixed ABI text, never to leftover detail.
        let noHandleAnswer = NativeCoreClient.failure(
            code: SOTC_ERR_ENGINE_NOT_IMPLEMENTED.rawValue, handleLastError: nil, handleMessage: "leftover text"
        )
        #expect(noHandleAnswer.code == .engineNotImplemented)
        #expect(noHandleAnswer.message != "leftover text")
        #expect(noHandleAnswer.message.localizedCaseInsensitiveContains("not implemented"))
    }

    @Test func aCreateAnswerNeverBecomesAnOkFailure() {
        // A successful create with no handle is the one case that must not be reported as success.
        #expect(NativeCoreClient.createFailure(SOTC_OK) == NativeCoreClient.missingHandleFailure)
        #expect(NativeCoreClient.createFailure(SOTC_OK).code == .internalError)
        #expect(NativeCoreClient.createFailure(SOTC_ERR_ABI_MISMATCH).code == .abiMismatch)
        #expect(NativeCoreClient.createFailure(SOTC_ERR_INTERNAL).code == .internalError)
        #expect(NativeCoreClient.missingHandleFailure.code == .internalError)
        #expect(!NativeCoreClient.missingHandleFailure.message.isEmpty)
    }

    @Test func aRefusedArgumentKeepsItsOwnMessageOnTheRealHandle() async throws {
        let directory = FileManager.default.temporaryDirectory.appending(
            path: UUID().uuidString, directoryHint: .isDirectory
        )
        try FileManager.default.createDirectory(at: directory, withIntermediateDirectories: true)
        defer { try? FileManager.default.removeItem(at: directory) }
        for name in AssetValidator.requiredArchives {
            try Data("PSAR....".utf8).write(to: directory.appending(path: name, directoryHint: .notDirectory))
        }

        let core = try NativeCoreClient()
        _ = try await core.validate(assetsDirectory: directory)
        // Leaves ENGINE_NOT_IMPLEMENTED as the handle's most recent failure.
        await #expect(throws: CoreFailure.self) { try await core.start() }

        // An empty path is refused by the ABI before the state machine, so it sets no handle error.
        let emptyPath = try #require(URL(string: "file://"))
        do {
            _ = try await core.validate(assetsDirectory: emptyPath)
            Issue.record("an empty assets path must be refused")
        } catch let failure as CoreFailure {
            #expect(failure.code == .invalidArgument)
            #expect(!failure.message.localizedCaseInsensitiveContains("engine"))
        }
    }
}
