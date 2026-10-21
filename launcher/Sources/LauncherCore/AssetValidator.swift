import Foundation

/// Ordered, complete result of one asset-directory check.
///
/// `problems` follows `AssetValidator.requiredArchives` order, one entry per defective archive, so
/// the first reported problem is always the earliest required archive. `missingArchives` lists the
/// archives that had no regular file at the expected path (a directory with the archive's name
/// counts as missing, not as an archive).
public struct AssetValidationReport: Equatable, Sendable {
    public let problems: [String]
    public let missingArchives: [String]

    public init(problems: [String], missingArchives: [String]) {
        self.problems = problems
        self.missingArchives = missingArchives
    }

    public var isReady: Bool { problems.isEmpty }

    /// Same `.ready`/`.missing` contract as `AssetValidator.validate` (semicolon-joined problems).
    public var status: ReadinessItem.Status {
        problems.isEmpty ? .ready : .missing(problems.joined(separator: "; "))
    }
}

/// Checks an extracted-asset directory for the archives the game needs. Reads names and sizes only.
public struct AssetValidator: Sendable {
    public static let requiredArchives = [
        "cachedFiles.psarc", "pak0.psarc", "pak1.psarc", "pak2.psarc", "pak3.psarc", "pakN.psarc",
    ]

    public init() {}

    /// Returns `.ready` only when every required archive exists, is a regular file, and starts with the PSARC magic.
    public func validate(directory: URL) -> ReadinessItem.Status {
        report(directory: directory).status
    }

    /// Full ordered result behind `validate(directory:)`. Same checks, same wording, no extra I/O.
    public func report(directory: URL) -> AssetValidationReport {
        let fm = FileManager.default
        var problems: [String] = []
        var missingArchives: [String] = []
        for name in Self.requiredArchives {
            let url = directory.appending(path: name, directoryHint: .notDirectory)
            var isDirectory: ObjCBool = false
            guard fm.fileExists(atPath: url.path(percentEncoded: false), isDirectory: &isDirectory), !isDirectory.boolValue else {
                problems.append("\(name) not found")
                missingArchives.append(name)
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
        return AssetValidationReport(problems: problems, missingArchives: missingArchives)
    }
}
