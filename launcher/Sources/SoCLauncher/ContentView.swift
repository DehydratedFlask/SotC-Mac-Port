import LauncherCore
import AppKit
import SwiftUI

struct ContentView: View {
    @Environment(LauncherModel.self) private var model
    @State private var isChoosingFolder = false
    @State private var folderImportError: String?
    @State private var showDiagnostics = false

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
                } else if model.state == .validating {
                    // The first check runs on appear without a folder, so the placeholder must not
                    // ask for one while the rows are already being collected.
                    Text("Checking readiness…")
                        .foregroundStyle(.secondary)
                } else {
                    Text("Choose your game folder to check readiness.")
                        .foregroundStyle(.secondary)
                }
            }

            Section("Status") {
                let presentation = model.presentation
                VStack(alignment: .leading, spacing: 4) {
                    Label(presentation.status, systemImage: statusSymbol(presentation.tone))
                        .foregroundStyle(statusTint(presentation.tone))
                    if let hint = presentation.hint {
                        Text(hint).font(.footnote).foregroundStyle(.secondary)
                    }
                }
                .accessibilityElement(children: .combine)
                .accessibilityLabel(Text(presentation.accessibilityLabel))
                .accessibilityValue(Text(presentation.accessibilityValue))
                // The combined element replaces its children, so the visible hint has to be spoken
                // explicitly or a screen-reader user never hears the recovery guidance.
                .accessibilityHint(Text(presentation.accessibilityHint))

                HStack {
                    Spacer()
                    if model.canStop {
                        Button("Stop", systemImage: "stop.fill") { model.stop() }
                            .accessibilityHint(Text("Stops the running game core."))
                    } else {
                        Button("Play", systemImage: "play.fill") { model.start() }
                            .keyboardShortcut(.defaultAction)
                            .disabled(!model.canStart)
                            .accessibilityHint(Text(model.canStart
                                ? "Starts the game core."
                                : "Unavailable. \(model.report?.playBlockedSummary ?? "Not ready to play.")"))
                    }
                }
            }

            Section("Diagnostics") {
                DisclosureGroup("Diagnostics report", isExpanded: $showDiagnostics) {
                    Text(model.diagnosticsText)
                        .font(.system(.footnote, design: .monospaced))
                        .textSelection(.enabled)
                        .frame(maxWidth: .infinity, alignment: .leading)
                        .accessibilityLabel(Text(DiagnosticsAccessibility.label))
                        .accessibilityValue(Text(DiagnosticsAccessibility.value(for: model.diagnosticsText)))
                    Button("Copy Diagnostics", systemImage: "doc.on.doc") {
                        NSPasteboard.general.clearContents()
                        NSPasteboard.general.setString(model.diagnosticsText, forType: .string)
                    }
                    .accessibilityHint(Text("Copies the diagnostics report to the clipboard."))
                }
            }
        }
        .formStyle(.grouped)
        .fileImporter(isPresented: $isChoosingFolder, allowedContentTypes: [.folder]) { result in
            switch model.handleFolderImport(result) {
            case .selected:
                Task { await model.validate() }
            case .cancelled:
                break
            case .failed(let message):
                folderImportError = message
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

    private func statusSymbol(_ tone: LauncherPresentation.Tone) -> String {
        switch tone {
        case .neutral: "circle.dashed"
        case .progress: "hourglass"
        case .success: "checkmark.circle.fill"
        case .unavailable: "nosign"
        case .failure: "exclamationmark.triangle.fill"
        }
    }

    private func statusTint(_ tone: LauncherPresentation.Tone) -> Color {
        switch tone {
        case .neutral, .progress: .secondary
        case .success: .green
        case .unavailable: .orange
        case .failure: .red
        }
    }
}
