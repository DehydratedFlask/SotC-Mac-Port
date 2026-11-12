import Foundation
import Testing
@testable import LauncherCore

/// APP-01-CORE-ABI-01 follow-up: the child transport's wire names for numeric core statuses.
///
/// The golden table is the same table `native/tests/test_core_status_json.cpp` pins on the C++ emitter
/// (`wire_cases()`); keep them identical. C++ proves "code -> text", this file proves "text -> the
/// value the in-process transport derives from the same code".
enum CoreWireGolden {
    struct Case: Sendable {
        let code: UInt32
        let subsystem: String
        let archive: String
    }

    static let cases: [Case] = [
        Case(code: 0, subsystem: "not_implemented", archive: "ok"),
        Case(code: 1, subsystem: "partial", archive: "missing"),
        Case(code: 2, subsystem: "implemented", archive: "unreadable"),
        Case(code: 3, subsystem: "not_linked", archive: "bad_magic"),
        Case(code: 4, subsystem: "unknown:4", archive: "unknown:4"),
        Case(code: 7, subsystem: "unknown:7", archive: "unknown:7"),
        Case(code: 9, subsystem: "unknown:9", archive: "unknown:9"),
        Case(code: 10, subsystem: "unknown:10", archive: "unknown:10"),
        Case(code: 99, subsystem: "unknown:99", archive: "unknown:99"),
        Case(code: 255, subsystem: "unknown:255", archive: "unknown:255"),
        Case(code: 65536, subsystem: "unknown:65536", archive: "unknown:65536"),
        Case(code: 2_147_483_648, subsystem: "unknown:2147483648", archive: "unknown:2147483648"),
        Case(code: 4_294_967_294, subsystem: "unknown:4294967294", archive: "unknown:4294967294"),
        Case(code: UInt32.max, subsystem: "unknown:4294967295", archive: "unknown:4294967295"),
    ]

    /// Text the grammar must refuse. Each entry is a distinct way a malformed or non-canonical value
    /// could be silently coerced into a status.
    static let malformed: [String] = [
        "unknown",                    // bare: the number is lost
        "unknown:",                   // empty code
        "unknown:abc",
        "unknown:7x",
        "unknown:-1",                 // sign
        "unknown:+7",                 // UInt32("+7") would accept this
        "unknown: 7",                 // embedded space
        "unknown:7 ",                 // trailing space
        "unknown:7\n",                // trailing newline
        "unknown:007",                // leading zeros: non-canonical
        "unknown:00",
        "unknown:0x7",                // radix prefix
        "unknown:7.0",
        "unknown:1e1",
        "unknown:7:8",
        "unknown:4294967296",         // UInt32.max + 1
        "unknown:4294967303",         // 2^32 + 7: must not wrap around to 7
        "unknown:18446744073709551623", // 2^64 + 7
        "unknown:99999999999999999999",
        "unknown:\u{0667}",           // Arabic-Indic digit seven: not ASCII
        "unknown:\u{FF17}",           // fullwidth digit seven
        "UNKNOWN:7",
        "Unknown:7",
        " unknown:7",
        "unknown :7",
        "unknown::7",
        "unknown_7",
        "unknown:0",                  // known codes must use their names, not the unknown form
        "unknown:1",
        "unknown:2",
        "unknown:3",
        "",
        "Partial",                    // known names are case sensitive
        "not-implemented",
    ]
}

struct CoreWireStatusTests {
    @Test func goldenWireNamesDecodeToWhatTheInProcessTransportDerives() {
        for wire in CoreWireGolden.cases {
            #expect(
                CoreSubsystem.Status(wireName: wire.subsystem) == CoreSubsystem.Status(code: wire.code),
                "subsystem \(wire.code) -> \(wire.subsystem)"
            )
            #expect(
                CoreArchiveCheck.Status(wireName: wire.archive) == CoreArchiveCheck.Status(code: wire.code),
                "archive \(wire.code) -> \(wire.archive)"
            )
        }
    }

    /// Lossless in both directions for every sample, including the whole uint32 range's edges.
    @Test func encodingThenDecodingNeverChangesTheStatus() {
        var codes: [UInt32] = Array(0...300)
        codes += [1_000, 65_535, 65_536, 1 << 24, 1 << 31, UInt32.max - 1, UInt32.max]
        for code in codes {
            let subsystem = CoreSubsystem.Status(code: code)
            #expect(CoreSubsystem.Status(wireName: subsystem.wireName) == subsystem, "subsystem \(code)")
            let archive = CoreArchiveCheck.Status(code: code)
            #expect(CoreArchiveCheck.Status(wireName: archive.wireName) == archive, "archive \(code)")
        }
    }

    @Test func encodingMatchesTheGoldenTable() {
        for wire in CoreWireGolden.cases {
            #expect(CoreSubsystem.Status(code: wire.code).wireName == wire.subsystem)
            #expect(CoreArchiveCheck.Status(code: wire.code).wireName == wire.archive)
        }
    }

    /// An out-of-range code keeps its number: `.unknown(99)` is not `.unknown(98)`, and two different
    /// unknown codes never collapse to one value.
    @Test func distinctUnknownCodesStayDistinct() throws {
        let seven = try #require(CoreSubsystem.Status(wireName: "unknown:7"))
        let eight = try #require(CoreSubsystem.Status(wireName: "unknown:8"))
        #expect(seven == .unknown(7))
        #expect(eight == .unknown(8))
        #expect(seven != eight)
        #expect(CoreArchiveCheck.Status(wireName: "unknown:4294967295") == .unknown(UInt32.max))
    }

    @Test func malformedWireTextIsRejectedNotCoerced() {
        for text in CoreWireGolden.malformed {
            #expect(CoreSubsystem.Status(wireName: text) == nil, "subsystem accepted \(String(reflecting: text))")
            #expect(CoreArchiveCheck.Status(wireName: text) == nil, "archive accepted \(String(reflecting: text))")
        }
    }

    /// A subsystem name is not an archive name and vice versa; the two vocabularies stay separate.
    @Test func vocabulariesDoNotCrossOver() {
        #expect(CoreSubsystem.Status(wireName: "ok") == nil)
        #expect(CoreSubsystem.Status(wireName: "bad_magic") == nil)
        #expect(CoreArchiveCheck.Status(wireName: "partial") == nil)
        #expect(CoreArchiveCheck.Status(wireName: "not_linked") == nil)
    }

    /// An `.unknown(N)` built by hand for a code that has a name is not canonical on the wire.
    @Test func nonCanonicalInMemoryUnknownDoesNotRoundTrip() {
        #expect(CoreSubsystem.Status.unknown(1).wireName == "unknown:1")
        #expect(CoreSubsystem.Status(wireName: CoreSubsystem.Status.unknown(1).wireName) == nil)
        #expect(CoreArchiveCheck.Status(wireName: CoreArchiveCheck.Status.unknown(0).wireName) == nil)
    }
}
