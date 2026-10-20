/// Lifecycle from the launcher's point of view (ROADMAP APP-01: validating/ready/starting/running/stopping/failed).
public enum LauncherState: Equatable, Sendable {
    case idle
    case validating
    case ready
    case starting
    case running
    case stopping
    case failed(String)
}
