import LauncherCore
import SwiftUI

struct ContentView: View {
    @Environment(LauncherModel.self) private var model
    @State private var isChoosingFolder = false

    var body: some View {
        @Bindable var model = model
        Form {
            Section("Game files") {
                LabeledContent("Extracted archives") {
                    Text(model.assetsDirectory?.lastPathComponent ?? "Not chosen")
                        .foregroundStyle(.secondary)
                }
                Button("Choose Folder…", systemImage: "folder") {
                    isChoosingFolder = true
                }
            }

            Section("Readiness") {
                if let report = model.report {
                    ForEach(report.items) { item in
                        ReadinessRow(item: item)
                    }
                } else {
                    Text("Choose your game folder to check readiness.")
                        .foregroundStyle(.secondary)
                }
            }

            Section {
                HStack {
                    Text(statusText)
                        .foregroundStyle(model.isFailed ? .red : .secondary)
                    Spacer()
                    if model.canStop {
                        Button("Stop", systemImage: "stop.fill") { model.stop() }
                    } else {
                        Button("Play", systemImage: "play.fill") { model.start() }
                            .keyboardShortcut(.defaultAction)
                            .disabled(!model.canStart)
                    }
                }
            }
        }
        .formStyle(.grouped)
        .fileImporter(isPresented: $isChoosingFolder, allowedContentTypes: [.folder]) { result in
            if case .success(let url) = result {
                model.assetsDirectory = url
                Task { await model.validate() }
            }
        }
        .task { await model.validate() }
    }

    private var statusText: String {
        switch model.state {
        case .idle:
            model.report?.playBlockedSummary ?? "Not ready to play"
        case .validating: "Checking…"
        case .ready: "Ready to play"
        case .starting: "Starting…"
        case .running: "Running"
        case .stopping: "Stopping…"
        case .failed(let message): message
        }
    }
}
