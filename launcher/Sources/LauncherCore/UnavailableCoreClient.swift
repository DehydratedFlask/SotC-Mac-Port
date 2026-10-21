import Foundation

/// Used when no real core client could be created (for example the C ABI refused the handshake).
/// It answers every request with the same `CoreFailure`; it can never validate, start or run, so it
/// is safe to ship and cannot fake anything.
public struct UnavailableCoreClient: CoreClient {
    public let failure: CoreFailure
    public let abiVersion = CoreABIVersion(major: 0, minor: 0)

    public init(failure: CoreFailure) { self.failure = failure }

    public func validate(assetsDirectory: URL) async throws -> CoreValidation { throw failure }
    public func start() async throws { throw failure }
    public func stop() async throws { throw failure }
    public func status() async throws -> CoreStatus { throw failure }
    public func coverage() async throws -> CoreCoverageReport { throw failure }
}
