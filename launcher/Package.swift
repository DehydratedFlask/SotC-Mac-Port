// swift-tools-version: 6.0
import PackageDescription

let package = Package(
    name: "SoCLauncher",
    platforms: [.macOS(.v15)],
    targets: [
        .target(name: "LauncherCore", path: "Sources/LauncherCore"),
        .executableTarget(name: "SoCLauncher", dependencies: ["LauncherCore"], path: "Sources/SoCLauncher"),
        .testTarget(name: "LauncherCoreTests", dependencies: ["LauncherCore"], path: "Tests/LauncherCoreTests"),
    ]
)
