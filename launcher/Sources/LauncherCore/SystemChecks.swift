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
    /// Policy (user decision): require a thin 64-bit ARM64 Mach-O executable (MH_EXECUTE).
    /// Fat/universal files, other CPU types, and non-executable file types are rejected.
    private static func isMachOExecutable(at url: URL) -> Bool {
        guard let handle = try? FileHandle(forReadingFrom: url) else { return false }
        defer { try? handle.close() }
        guard let header = try? handle.read(upToCount: 16), header.count >= 16 else { return false }
        func word(_ offset: Int) -> UInt32 {
            header.withUnsafeBytes { $0.loadUnaligned(fromByteOffset: offset, as: UInt32.self) }
        }
        let mhMagic64: UInt32 = 0xFEEDFACF     // little-endian 64-bit thin header as read on arm64
        let cpuTypeARM64: UInt32 = 0x0100000C  // CPU_TYPE_ARM64
        let mhExecute: UInt32 = 2
        return word(0) == mhMagic64 && word(4) == cpuTypeARM64 && word(12) == mhExecute
    }
}
