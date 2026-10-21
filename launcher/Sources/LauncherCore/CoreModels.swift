import Foundation

// Swift mirror of the native core C ABI (native/api/sotc_core.h, ABI 1.x). Plain values only:
// no C types, so LauncherCore stays testable without the native library.

/// ABI version as `major.minor`. A major change breaks the ABI; a minor change only appends.
public struct CoreABIVersion: Equatable, Hashable, Sendable, Comparable, CustomStringConvertible {
    public let major: UInt16
    public let minor: UInt16

    public init(major: UInt16, minor: UInt16) {
        self.major = major
        self.minor = minor
    }

    /// Unpacks the C `(major << 16) | minor` encoding.
    public init(packed: UInt32) {
        self.init(major: UInt16(truncatingIfNeeded: packed >> 16), minor: UInt16(truncatingIfNeeded: packed))
    }

    public var packed: UInt32 { UInt32(major) << 16 | UInt32(minor) }
    public var description: String { "\(major).\(minor)" }

    public static func < (lhs: CoreABIVersion, rhs: CoreABIVersion) -> Bool { lhs.packed < rhs.packed }
}

/// Core lifecycle. `.running` is part of the vocabulary but the native core cannot enter it yet.
public enum CoreState: Equatable, Sendable {
    case idle, validating, ready, starting, running, stopping, failed

    /// nil for a numeric state this launcher does not know (a newer core): callers must treat that as an error.
    public init?(code: UInt32) {
        switch code {
        case 0: self = .idle
        case 1: self = .validating
        case 2: self = .ready
        case 3: self = .starting
        case 4: self = .running
        case 5: self = .stopping
        case 6: self = .failed
        default: return nil
        }
    }
}

public enum CoreErrorCode: Equatable, Sendable {
    case ok
    case invalidArgument
    case badState
    case assetsNotReady
    /// `start()` on a core with no engine. Always the honest answer today; never success.
    case engineNotImplemented
    case io
    case bufferTooSmall
    case abiMismatch
    case internalError
    case unknown(UInt32)

    public init(code: UInt32) {
        switch code {
        case 0: self = .ok
        case 1: self = .invalidArgument
        case 2: self = .badState
        case 3: self = .assetsNotReady
        case 4: self = .engineNotImplemented
        case 5: self = .io
        case 6: self = .bufferTooSmall
        case 7: self = .abiMismatch
        case 8: self = .internalError
        default: self = .unknown(code)
        }
    }
}

/// A non-success answer from the core, carrying the core's own message copy.
public struct CoreFailure: Error, Equatable, Sendable, CustomStringConvertible {
    public let code: CoreErrorCode
    public let message: String

    public init(code: CoreErrorCode, message: String) {
        self.code = code
        self.message = message
    }

    public var description: String { message }
}

public struct CoreArchiveCheck: Equatable, Sendable {
    public enum Status: Equatable, Sendable {
        case ok, missing, unreadable, badMagic, unknown(UInt32)

        public init(code: UInt32) {
            switch code {
            case 0: self = .ok
            case 1: self = .missing
            case 2: self = .unreadable
            case 3: self = .badMagic
            default: self = .unknown(code)
            }
        }
    }

    public let name: String
    public let status: Status
    public let sizeBytes: UInt64

    public init(name: String, status: Status, sizeBytes: UInt64) {
        self.name = name
        self.status = status
        self.sizeBytes = sizeBytes
    }
}

/// Structured result of `validate`. `assetsReady` and `engineReady` are independent facts.
public struct CoreValidation: Equatable, Sendable {
    public let verdict: CoreErrorCode
    public let assetsReady: Bool
    public let engineReady: Bool
    public let startWouldSucceed: Bool
    public let problemCount: Int
    public let engineError: CoreErrorCode
    public let archives: [CoreArchiveCheck]
    public let detail: String

    public init(
        verdict: CoreErrorCode, assetsReady: Bool, engineReady: Bool, startWouldSucceed: Bool,
        problemCount: Int, engineError: CoreErrorCode, archives: [CoreArchiveCheck], detail: String
    ) {
        self.verdict = verdict
        self.assetsReady = assetsReady
        self.engineReady = engineReady
        self.startWouldSucceed = startWouldSucceed
        self.problemCount = problemCount
        self.engineError = engineError
        self.archives = archives
        self.detail = detail
    }
}

/// Constructor-runner prefix facts. Only meaningful when `linked`.
public struct CoreCtorPrefix: Equatable, Sendable {
    public let linked: Bool
    public let slotsRegistered: Int
    public let slotsWithCallback: Int
    public let completed: Int
    public let runOK: Bool
    public let callbackFailed: Bool
    /// Original RVA of the first unrecovered constructor; 0 when the run reached the end of the table.
    public let blockedOnRVA: UInt32

    public init(
        linked: Bool, slotsRegistered: Int, slotsWithCallback: Int, completed: Int,
        runOK: Bool, callbackFailed: Bool, blockedOnRVA: UInt32
    ) {
        self.linked = linked
        self.slotsRegistered = slotsRegistered
        self.slotsWithCallback = slotsWithCallback
        self.completed = completed
        self.runOK = runOK
        self.callbackFailed = callbackFailed
        self.blockedOnRVA = blockedOnRVA
    }

    public static let notLinked = CoreCtorPrefix(
        linked: false, slotsRegistered: 0, slotsWithCallback: 0, completed: 0,
        runOK: false, callbackFailed: false, blockedOnRVA: 0
    )
}

public struct CoreStatus: Equatable, Sendable {
    public let state: CoreState
    public let lastError: CoreErrorCode
    public let assetsValidated: Bool
    public let assetsReady: Bool
    public let engineImplemented: Bool
    public let ctor: CoreCtorPrefix

    public init(
        state: CoreState, lastError: CoreErrorCode, assetsValidated: Bool, assetsReady: Bool,
        engineImplemented: Bool, ctor: CoreCtorPrefix
    ) {
        self.state = state
        self.lastError = lastError
        self.assetsValidated = assetsValidated
        self.assetsReady = assetsReady
        self.engineImplemented = engineImplemented
        self.ctor = ctor
    }
}

public struct CoreSubsystem: Equatable, Sendable {
    public enum Status: Equatable, Sendable {
        case notImplemented, partial, implemented, notLinked, unknown(UInt32)

        public init(code: UInt32) {
            switch code {
            case 0: self = .notImplemented
            case 1: self = .partial
            case 2: self = .implemented
            case 3: self = .notLinked
            default: self = .unknown(code)
            }
        }
    }

    public let name: String
    public let status: Status
    public let detail: String

    public init(name: String, status: Status, detail: String) {
        self.name = name
        self.status = status
        self.detail = detail
    }
}

public struct CoreCoverageReport: Equatable, Sendable {
    public let abiVersion: CoreABIVersion
    public let engineImplemented: Bool
    public let ctor: CoreCtorPrefix
    public let subsystems: [CoreSubsystem]

    public init(abiVersion: CoreABIVersion, engineImplemented: Bool, ctor: CoreCtorPrefix, subsystems: [CoreSubsystem]) {
        self.abiVersion = abiVersion
        self.engineImplemented = engineImplemented
        self.ctor = ctor
        self.subsystems = subsystems
    }
}
