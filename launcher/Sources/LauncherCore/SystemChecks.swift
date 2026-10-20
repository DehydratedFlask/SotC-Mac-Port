import CoreAudio
import Foundation
import Metal

/// Host capability probes. Each returns a status for display; none starts the game.
public enum SystemChecks {
    /// Expected auxiliary executable name once a native core ships (child-process ownership).
    public static let expectedCoreExecutableName = "SoCGameCore"

    public static func metal() -> ReadinessItem.Status {
        guard let device = MTLCreateSystemDefaultDevice() else {
            return .missing("No Metal device available")
        }
        return device.supportsFamily(.apple7) ? .ready : .missing("\(device.name) lacks the Apple7 GPU family")
    }

    public static func audioOutput() -> ReadinessItem.Status {
        var address = AudioObjectPropertyAddress(
            mSelector: kAudioHardwarePropertyDefaultOutputDevice,
            mScope: kAudioObjectPropertyScopeGlobal,
            mElement: kAudioObjectPropertyElementMain
        )
        var device = AudioObjectID(kAudioObjectUnknown)
        var size = UInt32(MemoryLayout<AudioObjectID>.size)
        let result = AudioObjectGetPropertyData(AudioObjectID(kAudioObjectSystemObject), &address, 0, nil, &size, &device)
        return result == noErr && device != kAudioObjectUnknown ? .ready : .missing("No default audio output device")
    }

    /// Honest core presence probe. Never invents readiness: missing, non-executable, or non-Mach-O → NOT BUILT.
    /// A real ARM64 Mach-O game core does not exist yet (G1 NOT PASSED); Play must stay disabled.
    public static func nativeCore(executable: URL?) -> ReadinessItem.Status {
        guard let executable else {
            return .notBuilt(notBuiltMessage("no \(expectedCoreExecutableName) path (auxiliary executable absent)"))
        }

        let path = executable.path(percentEncoded: false)
        var isDirectory: ObjCBool = false
        guard FileManager.default.fileExists(atPath: path, isDirectory: &isDirectory) else {
            return .notBuilt(notBuiltMessage("\(executable.lastPathComponent) not found — native recovery incomplete"))
        }
        if isDirectory.boolValue {
            return .notBuilt(notBuiltMessage("\(executable.lastPathComponent) is a directory, not an executable"))
        }
        guard FileManager.default.isExecutableFile(atPath: path) else {
            return .notBuilt(notBuiltMessage("\(executable.lastPathComponent) is not executable"))
        }
        guard isMachOExecutable(at: executable) else {
            return .notBuilt(notBuiltMessage("\(executable.lastPathComponent) is not a Mach-O executable (refusing scripts/stubs)"))
        }
        return .ready
    }

    private static func notBuiltMessage(_ detail: String) -> String {
        "NOT BUILT: \(detail). Play disabled until a native SoCGameCore exists"
    }

    /// Lightweight header check only — does not execute the file or claim game readiness.
    private static func isMachOExecutable(at url: URL) -> Bool {
        guard let handle = try? FileHandle(forReadingFrom: url) else { return false }
        defer { try? handle.close() }
        guard let header = try? handle.read(upToCount: 8), header.count >= 4 else { return false }
        let magic = header.withUnsafeBytes { $0.load(as: UInt32.self) }
        // MH_MAGIC_64, MH_CIGAM_64, FAT_MAGIC, FAT_CIGAM (incl. FAT_MAGIC_64 family via cafe babe)
        switch magic {
        case 0xFEEDFACF, 0xCFFAEDFE, 0xCAFEBABE, 0xBEBAFECA, 0xFEEDFACE, 0xCEFAEDFE:
            return true
        default:
            return false
        }
    }
}
