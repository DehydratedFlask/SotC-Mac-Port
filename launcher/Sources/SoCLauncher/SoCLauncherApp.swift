import LauncherCore
import LauncherNativeCore
import SwiftUI

@main
struct SoCLauncherApp: App {
    @State private var model = LauncherModel(client: SoCLauncherApp.makeClient())

    var body: some Scene {
        Window("Shadow of the Colossus", id: "main") {
            ContentView()
                .environment(model)
                .frame(minWidth: 520, minHeight: 420)
        }
        .commands {
            CommandGroup(replacing: .newItem) {}
        }
    }

    /// Prefer a real `SoCGameCore` child process when the locator finds one; otherwise talk to the
    /// in-process C ABI. If the handshake fails, every request reports that failure
    /// (`UnavailableCoreClient`); the launcher never substitutes a fake that could say "running".
    private static func makeClient() -> any CoreClient {
        CoreClientFactory.make(fallback: { makeInProcessClient() })
    }

    nonisolated private static func makeInProcessClient() -> any CoreClient {
        do {
            return try NativeCoreClient()
        } catch let failure as CoreFailure {
            return UnavailableCoreClient(failure: failure)
        } catch {
            return UnavailableCoreClient(failure: CoreFailure(code: .internalError, message: error.localizedDescription))
        }
    }
}
