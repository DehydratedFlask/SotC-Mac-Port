import Testing
@testable import LauncherCore

struct CoreErrorCodeMappingTests {
    @Test func cAbiErrorCodesMapToNamedCases() {
        #expect(CoreErrorCode(code: 0) == .ok)
        #expect(CoreErrorCode(code: 1) == .invalidArgument)
        #expect(CoreErrorCode(code: 2) == .badState)
        #expect(CoreErrorCode(code: 3) == .assetsNotReady)
        #expect(CoreErrorCode(code: 4) == .engineNotImplemented)
        #expect(CoreErrorCode(code: 5) == .io)
        #expect(CoreErrorCode(code: 6) == .bufferTooSmall)
        #expect(CoreErrorCode(code: 7) == .abiMismatch)
        #expect(CoreErrorCode(code: 8) == .internalError)
        #expect(CoreErrorCode(code: 9) == .unknown(9))
        #expect(CoreErrorCode(code: UInt32.max) == .unknown(UInt32.max))
    }
}
