// swift-tools-version: 6.0
import PackageDescription

let package = Package(
    name: "SoCLauncher",
    platforms: [.macOS(.v15)],
    targets: [
        // C ABI facade of the native game core. Sources are symlinks to native/api/ (single source of truth).
        // Built WITHOUT the native core library (SOTC_API_WITH_CORE_LIB=0): it reports constructor facts as NOT_LINKED.
        .target(
            name: "CSotcCore",
            path: "Sources/CSotcCore",
            cxxSettings: [.define("SOTC_API_WITH_CORE_LIB", to: "0")]
        ),
        .target(name: "LauncherCore", path: "Sources/LauncherCore"),
        // Swift wrapper over the C ABI. Kept apart so LauncherCore stays free of native code.
        .target(name: "LauncherNativeCore", dependencies: ["LauncherCore", "CSotcCore"], path: "Sources/LauncherNativeCore"),
        .executableTarget(name: "SoCLauncher", dependencies: ["LauncherCore"], path: "Sources/SoCLauncher"),
        .testTarget(name: "LauncherCoreTests", dependencies: ["LauncherCore"], path: "Tests/LauncherCoreTests"),
        .testTarget(
            name: "LauncherNativeCoreTests",
            dependencies: ["LauncherNativeCore", "LauncherCore"],
            path: "Tests/LauncherNativeCoreTests"
        ),
    ],
    cxxLanguageStandard: .cxx20
)
