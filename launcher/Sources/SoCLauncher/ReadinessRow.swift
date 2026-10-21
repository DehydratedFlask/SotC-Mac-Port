import LauncherCore
import SwiftUI

struct ReadinessRow: View {
    let item: ReadinessItem

    var body: some View {
        LabeledContent {
            Text(detail)
                .foregroundStyle(.secondary)
                .multilineTextAlignment(.trailing)
        } label: {
            Label(item.kind.rawValue, systemImage: symbol)
                .foregroundStyle(tint)
        }
        .accessibilityElement(children: .combine)
        .accessibilityLabel(Text(item.accessibilityLabel))
        .accessibilityValue(Text(item.accessibilityValue))
    }

    private var detail: String {
        switch item.status {
        case .ready: "Ready"
        case .missing(let message): message
        case .notBuilt(let message): message // includes "NOT BUILT" / "Play disabled"
        }
    }

    private var symbol: String {
        switch item.status {
        case .ready: "checkmark.circle.fill"
        case .missing: "exclamationmark.triangle.fill"
        case .notBuilt: "hammer.circle.fill"
        }
    }

    private var tint: Color {
        switch item.status {
        case .ready: .green
        case .missing: .orange
        case .notBuilt: .secondary
        }
    }
}
