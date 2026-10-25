import Foundation

/// Spoken wording for the copyable diagnostics block.
///
/// A SwiftUI `accessibilityLabel` on the text view *replaces* its content for assistive technology,
/// so the label names the block and the report itself is carried as the spoken value.
public enum DiagnosticsAccessibility {
    public static let label = "Diagnostics report"

    /// The report text, unchanged; an empty report is spoken as such rather than as silence.
    public static func value(for text: String) -> String {
        text.isEmpty ? "Empty" : text
    }
}
