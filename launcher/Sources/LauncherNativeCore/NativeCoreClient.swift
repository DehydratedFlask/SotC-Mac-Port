import CSotcCore
import Foundation
import LauncherCore

/// `CoreClient` over the in-process C ABI (native/api/sotc_core.h).
///
/// Honest by construction: `start()` surfaces whatever the core answers. While the core has no
/// engine that is `SOTC_ERR_ENGINE_NOT_IMPLEMENTED`, thrown as `CoreFailure`; this type never
/// converts a failure into a running state.
///
/// This target links the ABI facade only (no constructor runner). The game core process
/// (`SoCGameCore`) will link the same facade plus `sotc_core`; a child-process `CoreClient` can then
/// replace this one without touching `LauncherCore`.
public final class NativeCoreClient: CoreClient, @unchecked Sendable {
    // The C handle takes an internal lock on every call, so sharing it across threads is safe.
    private let handle: OpaquePointer

    public init() throws {
        var created: OpaquePointer?
        let code = sotc_core_create(sotc_core_abi_version(), &created)
        guard code == SOTC_OK, let created else {
            throw CoreFailure(code: CoreErrorCode(code: code.rawValue), message: Self.message(for: code.rawValue))
        }
        handle = created
    }

    deinit { sotc_core_destroy(handle) }

    public var abiVersion: CoreABIVersion { CoreABIVersion(packed: sotc_core_abi_version()) }

    public func validate(assetsDirectory: URL) async throws -> CoreValidation {
        var result = sotc_validation_result()
        result.struct_size = UInt32(MemoryLayout<sotc_validation_result>.size)
        let code = sotc_core_validate(handle, assetsDirectory.path(percentEncoded: false), &result)
        guard code == SOTC_OK else { throw failure(code.rawValue) }
        return Self.validation(from: result)
    }

    public func start() async throws {
        let code = sotc_core_start(handle)
        guard code == SOTC_OK else { throw failure(code.rawValue) }
    }

    public func stop() async throws {
        let code = sotc_core_stop(handle)
        guard code == SOTC_OK else { throw failure(code.rawValue) }
    }

    public func status() async throws -> CoreStatus {
        var raw = sotc_status()
        raw.struct_size = UInt32(MemoryLayout<sotc_status>.size)
        let code = sotc_core_status(handle, &raw)
        guard code == SOTC_OK else { throw failure(code.rawValue) }
        guard let state = CoreState(code: raw.state) else {
            throw CoreFailure(code: .internalError, message: "Core reported unknown state \(raw.state)")
        }
        return CoreStatus(
            state: state,
            lastError: CoreErrorCode(code: raw.last_error),
            assetsValidated: raw.assets_validated != 0,
            assetsReady: raw.assets_ready != 0,
            engineImplemented: raw.engine_implemented != 0,
            ctor: Self.ctor(from: raw.ctor)
        )
    }

    public func coverage() async throws -> CoreCoverageReport {
        var raw = sotc_coverage_report()
        raw.struct_size = UInt32(MemoryLayout<sotc_coverage_report>.size)
        let code = sotc_core_coverage(&raw)
        guard code == SOTC_OK else { throw failure(code.rawValue) }
        let count = min(Int(raw.subsystem_count), Int(SOTC_SUBSYSTEM_COUNT))
        let subsystems = withUnsafeBytes(of: raw.subsystems) { bytes in
            bytes.bindMemory(to: sotc_subsystem_report.self).prefix(count).map {
                CoreSubsystem(
                    name: Self.string($0.name), status: .init(code: $0.status), detail: Self.string($0.detail)
                )
            }
        }
        return CoreCoverageReport(
            abiVersion: CoreABIVersion(packed: raw.abi_version),
            engineImplemented: raw.engine_implemented != 0,
            ctor: Self.ctor(from: raw.ctor),
            subsystems: subsystems
        )
    }

    // MARK: - Conversions

    private func failure(_ code: UInt32) -> CoreFailure {
        var detail = Self.lastError(handle)
        if detail.isEmpty { detail = Self.message(for: code) }
        return CoreFailure(code: CoreErrorCode(code: code), message: detail)
    }

    private static func message(for code: UInt32) -> String {
        var buffer = [CChar](repeating: 0, count: 256)
        _ = sotc_core_error_message(code, &buffer, buffer.count)
        return buffer.withUnsafeBytes { String(decoding: $0.prefix { $0 != 0 }, as: UTF8.self) }
    }

    private static func lastError(_ handle: OpaquePointer) -> String {
        var buffer = [CChar](repeating: 0, count: 512)
        _ = sotc_core_last_error_message(handle, &buffer, buffer.count)
        return buffer.withUnsafeBytes { String(decoding: $0.prefix { $0 != 0 }, as: UTF8.self) }
    }

    /// Fixed-size C `char[N]` arrives as a homogeneous tuple; read up to the first NUL.
    private static func string<T>(_ chars: T) -> String {
        withUnsafeBytes(of: chars) { String(decoding: $0.prefix { $0 != 0 }, as: UTF8.self) }
    }

    private static func ctor(from raw: sotc_ctor_prefix) -> CoreCtorPrefix {
        CoreCtorPrefix(
            linked: raw.linked != 0,
            slotsRegistered: Int(raw.slots_registered),
            slotsWithCallback: Int(raw.slots_with_callback),
            completed: Int(raw.completed),
            runOK: raw.run_ok != 0,
            callbackFailed: raw.callback_failed != 0,
            blockedOnRVA: raw.blocked_on_rva
        )
    }

    private static func validation(from raw: sotc_validation_result) -> CoreValidation {
        let archives = withUnsafeBytes(of: raw.archives) { bytes in
            bytes.bindMemory(to: sotc_archive_check.self).map {
                CoreArchiveCheck(name: string($0.name), status: .init(code: $0.status), sizeBytes: $0.size_bytes)
            }
        }
        return CoreValidation(
            verdict: CoreErrorCode(code: raw.error),
            assetsReady: raw.assets_ready != 0,
            engineReady: raw.engine_ready != 0,
            startWouldSucceed: raw.start_would_succeed != 0,
            problemCount: Int(raw.problem_count),
            engineError: CoreErrorCode(code: raw.engine_error),
            archives: archives,
            detail: string(raw.detail)
        )
    }
}
