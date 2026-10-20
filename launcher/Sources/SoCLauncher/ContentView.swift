import LauncherCore
import SwiftUI

struct ContentView: View {
    @Environment(LauncherModel.self) private var model
    @State private var isChoosingFolder = false
    @State private var folderImportError: String?

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
            switch result {
            case .success(let url):
                model.assetsDirectory = url
                Task { await model.validate() }
            case .failure(let error):
                folderImportError = error.localizedDescription
            }
        }
        .alert("Couldn’t Open Folder", isPresented: Binding(
            get: { folderImportError != nil },
            set: { if !$0 { folderImportError = nil } }
        )) {
            Button("OK", role: .cancel) { folderImportError = nil }
        } message: {
            Text(folderImportError ?? "")
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
