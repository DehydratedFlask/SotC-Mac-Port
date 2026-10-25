import Foundation

/// Chooses the launcher's `CoreClient` transport.
///
/// - A valid `SoCGameCore` found by `CoreLocator` → `ProcessCoreClient` (child process).
/// - Anything else → the caller's existing in-process fallback (`NativeCoreClient`, or
///   `UnavailableCoreClient` when even the C ABI handshake fails). A core that is found but cannot be
///   launched also falls back, so the launcher keeps its honest NOT BUILT answer instead of pretending
///   it has a working core.
///
/// The factory lives in `LauncherCore` and takes the fallback as a closure so this target does not
/// depend on `LauncherNativeCore`/the C ABI. Readiness copy is unchanged: nothing here can make the
/// engine row ready; that still requires the core to report `engine_implemented`.
public enum CoreClientFactory {
    public static func make(
        environment: [String: String] = ProcessInfo.processInfo.environment,
        bundleURL: URL? = Bundle.main.bundleURL,
        executableURL: URL? = Bundle.main.executableURL,
        locator: CoreLocator = CoreLocator(),
        fallback: @Sendable () -> any CoreClient
    ) -> any CoreClient {
        select(environment: environment, bundleURL: bundleURL, executableURL: executableURL, locator: locator, fallback: fallback).client
    }

    /// Same choice as `make`, but also reports which transport was used and why, so the UI can say
    /// "SoCGameCore not found" instead of silently answering from the in-process fallback.
    public static func select(
        environment: [String: String] = ProcessInfo.processInfo.environment,
        bundleURL: URL? = Bundle.main.bundleURL,
        executableURL: URL? = Bundle.main.executableURL,
        locator: CoreLocator = CoreLocator(),
        fallback: @Sendable () -> any CoreClient
    ) -> (client: any CoreClient, transport: CoreTransport) {
        switch locator.locate(environment: environment, bundleURL: bundleURL, executableURL: executableURL) {
        case .found(let coreURL):
            do {
                return (try ProcessCoreClient(coreURL: coreURL), .childProcess(coreURL))
            } catch {
                let name = coreURL.lastPathComponent
                let reason = (error as? CoreFailure)?.message ?? error.localizedDescription
                return (fallback(), .inProcessFallback(CoreLocator.notBuilt("\(name) was found but could not be started (\(reason))")))
            }
        case .notFound(let status):
            return (fallback(), .inProcessFallback(status))
        }
    }
}
