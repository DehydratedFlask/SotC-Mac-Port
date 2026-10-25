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

    /// Honest core presence probe. Never invents readiness: missing, non-executable, wrong Mach-O
    /// shape, or non-Mach-O → NOT BUILT. Presence does not imply an implemented engine; CoreClient
    /// supplies that separate readiness fact.
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
        switch classifyCoreMachO(at: executable) {
        case .accepted:
            return .ready
        case .rejected(let detail):
            return .notBuilt(notBuiltMessage("\(executable.lastPathComponent) \(detail)"))
        }
    }

    private static func notBuiltMessage(_ detail: String) -> String {
        "NOT BUILT: \(detail). Play disabled until a native SoCGameCore exists"
    }

    enum MachOClassification {
        case accepted
        case rejected(String)
    }

    /// Lightweight header check only — does not execute the file or claim game readiness.
    /// Policy (user decision): require a thin 64-bit ARM64 Mach-O executable (MH_EXECUTE).
    /// Rejection copy names the real mismatch (fat / wrong CPU / wrong file type / non-Mach-O)
    /// instead of collapsing every failure into "not a Mach-O".
    static func classifyCoreMachO(at url: URL) -> MachOClassification {
        guard let handle = try? FileHandle(forReadingFrom: url) else {
            return .rejected("is not a Mach-O executable (refusing scripts/stubs)")
        }
        defer { try? handle.close() }
        guard let header = try? handle.read(upToCount: 16), header.count >= 4 else {
            return .rejected("is not a Mach-O executable (refusing scripts/stubs)")
        }
        func word(_ offset: Int) -> UInt32? {
            guard header.count >= offset + 4 else { return nil }
            return header.withUnsafeBytes { $0.loadUnaligned(fromByteOffset: offset, as: UInt32.self) }
        }
        guard let magic = word(0) else {
            return .rejected("is not a Mach-O executable (refusing scripts/stubs)")
        }

        // FAT_MAGIC / FAT_CIGAM and their 64-bit variants as read on little-endian hosts.
        let fatMagic: UInt32 = 0xCAFEBABE
        let fatCigam: UInt32 = 0xBEBAFECA
        let fatMagic64: UInt32 = 0xCAFEBABF
        let fatCigam64: UInt32 = 0xBFBAFECA
        if magic == fatMagic || magic == fatCigam || magic == fatMagic64 || magic == fatCigam64 {
            return .rejected("is a fat/universal Mach-O, not a thin ARM64 executable")
        }

        let mhMagic: UInt32 = 0xFEEDFACE       // 32-bit LE
        let mhCigam: UInt32 = 0xCEFAEDFE       // 32-bit swapped
        if magic == mhMagic || magic == mhCigam {
            return .rejected("is a 32-bit Mach-O, not a thin 64-bit ARM64 executable")
        }

        let mhMagic64: UInt32 = 0xFEEDFACF     // little-endian 64-bit thin header as read on arm64
        let mhCigam64: UInt32 = 0xCFFAEDFE
        if magic == mhCigam64 {
            return .rejected("is a byte-swapped 64-bit Mach-O, not a thin ARM64 executable")
        }
        guard magic == mhMagic64 else {
            return .rejected("is not a Mach-O executable (refusing scripts/stubs)")
        }
        guard header.count >= 16, let cpuType = word(4), let fileType = word(12) else {
            return .rejected("Mach-O header is truncated")
        }

        let cpuTypeARM64: UInt32 = 0x0100000C  // CPU_TYPE_ARM64
        let mhExecute: UInt32 = 2
        if cpuType != cpuTypeARM64 {
            return .rejected("is not a thin ARM64 Mach-O executable (wrong CPU type)")
        }
        if fileType != mhExecute {
            return .rejected("is not a Mach-O executable file type (MH_EXECUTE required)")
        }
        return .accepted
    }
}
