import Foundation

/// Finds the child-process game core (`SoCGameCore`) without starting it.
///
/// Search order (first *valid* candidate wins):
/// 1. `SOTC_CORE_PATH` (empty values are ignored),
/// 2. `<app bundle>/Contents/MacOS/SoCGameCore`,
/// 3. `<app bundle>/Contents/Resources/SoCGameCore`,
/// 4. `SoCGameCore` next to the running executable (covers `swift run` and an unbundled binary).
///
/// A candidate is accepted only when it is an existing **regular executable file** with a thin
/// 64-bit ARM64 MH_EXECUTE header. Directories, scripts and other files use the same rejection wording
/// `SystemChecks.nativeCore(executable:)` uses, so the launcher's "NOT BUILT … Play disabled" copy
/// stays identical. Finding a core proves only that a file exists — it never means the engine is
/// implemented or that the game can start.
public struct CoreLocator: Sendable {
    public static let environmentVariable = "SOTC_CORE_PATH"
    public static let executableName = SystemChecks.expectedCoreExecutableName

    public enum Location: Equatable, Sendable {
        case found(URL)
        /// A full `NOT BUILT: …` sentence (`.notBuilt`) explaining why no core was usable.
        case notFound(ReadinessItem.Status)

        public var url: URL? {
            if case .found(let url) = self { return url }
            return nil
        }

        public var rejection: ReadinessItem.Status? {
            if case .notFound(let status) = self { return status }
            return nil
        }
    }

    public init() {}

    /// - Parameters:
    ///   - environment: process environment; only `SOTC_CORE_PATH` is read.
    ///   - bundleURL: the app bundle to inspect, or nil to skip bundle candidates.
    ///   - executableURL: the running executable, or nil to skip the sibling candidate.
    public func locate(
        environment: [String: String] = ProcessInfo.processInfo.environment,
        bundleURL: URL? = Bundle.main.bundleURL,
        executableURL: URL? = Bundle.main.executableURL
    ) -> Location {
        var candidates: [URL] = []
        var seen = Set<String>()

        func add(_ url: URL) {
            let key = url.standardizedFileURL.path(percentEncoded: false)
            guard seen.insert(key).inserted else { return }
            candidates.append(url)
        }

        if let raw = environment[Self.environmentVariable]?.trimmingCharacters(in: .whitespacesAndNewlines),
           !raw.isEmpty {
            add(URL(filePath: raw, directoryHint: .notDirectory))
        }
        if let bundleURL {
            add(bundleURL.appending(path: "Contents/MacOS/\(Self.executableName)", directoryHint: .notDirectory))
            add(bundleURL.appending(path: "Contents/Resources/\(Self.executableName)", directoryHint: .notDirectory))
        }
        if let executableURL {
            add(executableURL.deletingLastPathComponent().appending(path: Self.executableName, directoryHint: .notDirectory))
        }

        // A missing candidate is unremarkable (most of them are absent). The first candidate that
        // exists but is unusable is the useful rejection to report; a later valid candidate still wins.
        var firstExistingRejection: String?
        for candidate in candidates {
            switch Self.inspect(candidate) {
            case .success(let url):
                return .found(url)
            case .failure(let rejection):
                if rejection.fileExists, firstExistingRejection == nil {
                    firstExistingRejection = rejection.detail
                }
            }
        }

        if let firstExistingRejection {
            return .notFound(Self.notBuilt(firstExistingRejection))
        }
        return .notFound(Self.notBuilt("no \(Self.executableName) path (auxiliary executable absent)"))
    }

    /// Same sentence `SystemChecks` uses for an absent core. Kept in sync by `CoreLocatorTests`.
    static func notBuilt(_ detail: String) -> ReadinessItem.Status {
        .notBuilt("NOT BUILT: \(detail). Play disabled until a native SoCGameCore exists")
    }

    private struct Rejection: Error {
        let detail: String
        let fileExists: Bool
    }

    private static func inspect(_ url: URL) -> Result<URL, Rejection> {
        let name = url.lastPathComponent
        let path = url.path(percentEncoded: false)
        var isDirectory: ObjCBool = false
        guard FileManager.default.fileExists(atPath: path, isDirectory: &isDirectory) else {
            return .failure(Rejection(detail: "\(name) not found — native recovery incomplete", fileExists: false))
        }
        if isDirectory.boolValue {
            return .failure(Rejection(detail: "\(name) is a directory, not an executable", fileExists: true))
        }
        // A FIFO or device with the execute bit set would block the header read; only regular files
        // are accepted. `stat` follows symlinks, so a symlink to a regular core stays acceptable
        // (the same behaviour as `SystemChecks`).
        var fileInfo = stat()
        guard stat(path, &fileInfo) == 0, (fileInfo.st_mode & S_IFMT) == S_IFREG else {
            return .failure(Rejection(detail: "\(name) is not a regular file, not an executable", fileExists: true))
        }
        guard FileManager.default.isExecutableFile(atPath: path) else {
            return .failure(Rejection(detail: "\(name) is not executable", fileExists: true))
        }
        guard SystemChecks.isMachOExecutable(at: url) else {
            return .failure(Rejection(detail: "\(name) is not a Mach-O executable (refusing scripts/stubs)", fileExists: true))
        }
        return .success(url)
    }

}
