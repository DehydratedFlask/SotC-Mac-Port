import SwiftUI

@main
struct SoCLauncherApp: App {
    @State private var model = LauncherModel()

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
}
