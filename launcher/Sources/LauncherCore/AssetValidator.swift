import Foundation

/// Checks an extracted-asset directory for the archives the game needs. Reads names and sizes only.
public struct AssetValidator: Sendable {
    public static let requiredArchives = [
        "cachedFiles.psarc", "pak0.psarc", "pak1.psarc", "pak2.psarc", "pak3.psarc", "pakN.psarc",
    ]

    public init() {}

    /// Returns `.ready` only when every required archive exists, is a regular file, and starts with the PSARC magic.
    public func validate(directory: URL) -> ReadinessItem.Status {
        let fm = FileManager.default
        var problems: [String] = []
        for name in Self.requiredArchives {
            let url = directory.appending(path: name, directoryHint: .notDirectory)
            var isDirectory: ObjCBool = false
            guard fm.fileExists(atPath: url.path(percentEncoded: false), isDirectory: &isDirectory), !isDirectory.boolValue else {
                problems.append("\(name) not found")
                continue
            }
            guard let handle = try? FileHandle(forReadingFrom: url) else {
                problems.append("\(name) is not readable")
                continue
            }
            defer { try? handle.close() }
            let magic = (try? handle.read(upToCount: 4)) ?? Data()
            if magic != Data("PSAR".utf8) {
                problems.append("\(name) is not a PSARC archive")
            }
        }
        return problems.isEmpty ? .ready : .missing(problems.joined(separator: "; "))
    }
}
