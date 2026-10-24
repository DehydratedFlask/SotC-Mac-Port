// SoCGameCore - native arm64 Mach-O auxiliary executable for the SotC Mac port
// (APP-01-SOCGAMECORE-EXE-01).
//
// It owns the recovered constructor chain through the versioned C ABI (native/api) and reports
// typed readiness as one JSON object per line on stdout. Diagnostics go to stderr. There is no
// engine behind this executable: --start always reports SOTC_ERR_ENGINE_NOT_IMPLEMENTED and no
// code path here can claim a running game.
//
// Protocol v1 (fixed; the launcher's child-process client is written against it):
//   --status                 sotc.core.status.v1                exit 0
//   --coverage               sotc.core.coverage.v1              exit 0
//   --validate <dir>         sotc.core.validate.v1              exit 0, 5 on hard I/O
//   --start                  sotc.core.start.v1, ok=false       exit 3
//   --serve                  `<id> <command> [argument]` stdin  exit 0 on quit/EOF
//   --fixture <path>         SOTC_SLOT23_NAMES_FIXTURE before the core runs any constructor
//   --assets <dir>           default assets directory used by --start
// Exit codes: 0 ok, 2 usage, 3 engine_not_implemented, 4 internal, 5 validate hard error.
#include "core_status_json.hpp"

#include <array>
#include <charconv>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

#include "core/startup_ctors.hpp"
#include "sotc_core.h"

namespace {

constexpr const char* kProgram = "SoCGameCore";

constexpr int kExitOk = 0;
constexpr int kExitUsage = 2;
constexpr int kExitEngineNotImplemented = 3;
constexpr int kExitInternal = 4;
constexpr int kExitValidateHardError = 5;

// The core reads this name (core/slot23_names_fixture.hpp). The protocol text names the shorter
// alias, so both are exported for clients that set either.
constexpr const char* kSlot23FixtureAlias = "SOTC_SLOT23_FIXTURE";

void write_line(std::string_view line) {
    std::cout << line << '\n';
    std::cout.flush();
}

void diagnostic(std::string_view text) { std::cerr << kProgram << ": " << text << '\n'; }

class Emit {
  public:
    explicit Emit(std::optional<std::int64_t> id) : id_(id) {}

    void operator()(std::string line) const {
        if (id_) {
            line = soc::status_json::with_request_id(line, *id_);
        }
        write_line(line);
    }

  private:
    std::optional<std::int64_t> id_;
};

enum class Command { kNone, kStatus, kCoverage, kValidate, kStart, kServe };

struct Options {
    Command command = Command::kNone;
    std::string validate_dir;
    std::string fixture;
    bool fixture_set = false;
    std::string assets_dir;
    bool assets_set = false;
};

void usage(std::ostream& out) {
    out << "usage: " << kProgram
        << " (--status | --coverage | --validate <dir> | --start | --serve)\n"
           "                   [--fixture <path>] [--assets <dir>]\n"
           "\n"
           "  --status            one sotc.core.status.v1 line, exit 0\n"
           "  --coverage          one sotc.core.coverage.v1 line, exit 0\n"
           "  --validate <dir>    one sotc.core.validate.v1 line; exit 5 on hard I/O\n"
           "  --start             sotc.core.start.v1 with ok=false, exit 3 (no engine exists)\n"
           "  --serve             read '<id> <command> [argument]' lines from stdin\n"
           "  --fixture <path>    slot-23 name fixture for this process, before constructors run\n"
           "  --assets <dir>      default assets directory used by --start\n"
           "  --help              this text, exit 0\n"
           "\n"
           "Exit codes: 0 ok, 2 usage, 3 engine_not_implemented, 4 internal, 5 validate hard error.\n"
           "stdout carries only JSON lines; diagnostics are written to stderr.\n";
}

bool set_command(Options& options, Command command, std::string& error) {
    if (options.command != Command::kNone) {
        error = "only one command may be given";
        return false;
    }
    options.command = command;
    return true;
}

bool parse_options(int argc, char** argv, Options& options, bool& help, std::string& error) {
    for (int i = 1; i < argc; ++i) {
        const std::string_view arg = argv[i];
        if (arg == "--status") {
            if (!set_command(options, Command::kStatus, error)) return false;
        } else if (arg == "--coverage") {
            if (!set_command(options, Command::kCoverage, error)) return false;
        } else if (arg == "--start") {
            if (!set_command(options, Command::kStart, error)) return false;
        } else if (arg == "--serve") {
            if (!set_command(options, Command::kServe, error)) return false;
        } else if (arg == "--validate") {
            if (i + 1 >= argc) {
                error = "--validate requires a directory";
                return false;
            }
            if (!set_command(options, Command::kValidate, error)) return false;
            options.validate_dir = argv[++i];
            if (options.validate_dir.empty()) {
                error = "--validate requires a non-empty directory";
                return false;
            }
        } else if (arg == "--fixture") {
            if (i + 1 >= argc) {
                error = "--fixture requires a path";
                return false;
            }
            options.fixture = argv[++i];
            if (options.fixture.empty()) {
                error = "--fixture requires a non-empty path";
                return false;
            }
            options.fixture_set = true;
        } else if (arg == "--assets") {
            if (i + 1 >= argc) {
                error = "--assets requires a directory";
                return false;
            }
            options.assets_dir = argv[++i];
            if (options.assets_dir.empty()) {
                error = "--assets requires a non-empty directory";
                return false;
            }
            options.assets_set = true;
        } else if (arg == "--help" || arg == "-h") {
            help = true;
        } else {
            error = "unknown option: " + std::string(arg);
            return false;
        }
    }
    if (!help && options.command == Command::kNone) {
        error = "no command given";
        return false;
    }
    return true;
}

// ---- constructor-table measurement -------------------------------------------------------------

// The lowest original index in 0..572 that the recovered table does not represent. Computed from
// sotc::recovered_constructors(), never hard-coded, so a recovered slot changes the answer.
std::optional<std::uint32_t> first_unrecovered_index() {
    static const std::optional<std::uint32_t> measured = [] {
        std::array<bool, soc::status_json::kOriginalConstructorSlots> present{};
        for (const sotc::CtorSlot& slot : sotc::recovered_constructors()) {
            if (slot.original_index < present.size()) {
                present[slot.original_index] = true;
            }
        }
        for (std::uint32_t i = 0; i < present.size(); ++i) {
            if (!present[i]) return std::optional<std::uint32_t>(i);
        }
        return std::optional<std::uint32_t>{};
    }();
    return measured;
}

soc::status_json::CtorSnapshot make_ctor_snapshot(const sotc_ctor_prefix& prefix) {
    soc::status_json::CtorSnapshot ctor;
    ctor.linked = prefix.linked != 0;
    ctor.original_slots = soc::status_json::kOriginalConstructorSlots;
    ctor.slots_registered = prefix.slots_registered;
    ctor.slots_with_callback = prefix.slots_with_callback;
    ctor.completed = prefix.completed;
    ctor.run_ok = prefix.run_ok != 0;
    ctor.callback_failed = prefix.callback_failed != 0;
    ctor.blocked_on_rva = prefix.blocked_on_rva;
    if (ctor.linked) {
        ctor.first_unrecovered_index = first_unrecovered_index();
    }
    return ctor;
}

const char* subsystem_status_name(std::uint32_t status) {
    switch (status) {
        case SOTC_SUBSYSTEM_NOT_IMPLEMENTED: return "not_implemented";
        case SOTC_SUBSYSTEM_PARTIAL: return "partial";
        case SOTC_SUBSYSTEM_IMPLEMENTED: return "implemented";
        case SOTC_SUBSYSTEM_NOT_LINKED: return "not_linked";
        default: return "unknown";
    }
}

const char* archive_status_name(std::uint32_t status) {
    switch (status) {
        case SOTC_ARCHIVE_OK: return "ok";
        case SOTC_ARCHIVE_MISSING: return "missing";
        case SOTC_ARCHIVE_UNREADABLE: return "unreadable";
        case SOTC_ARCHIVE_BAD_MAGIC: return "bad_magic";
        default: return "unknown";
    }
}

// ---- process session ---------------------------------------------------------------------------

struct Session {
    sotc_core* core = nullptr;
    std::string default_assets_dir;
    bool validated = false;
};

int handle_status(const Session& session, const Emit& emit) {
    sotc_status status{};
    status.struct_size = sizeof status;
    const sotc_error error = sotc_core_status(session.core, &status);
    if (error != SOTC_OK) {
        diagnostic("status failed: " + std::string(sotc_core_error_name(error)));
        return kExitInternal;
    }
    soc::status_json::StatusSnapshot snapshot;
    snapshot.abi = sotc_core_abi_version();
    snapshot.state = sotc_core_state_name(status.state);
    snapshot.last_error = status.last_error;
    snapshot.last_error_name = sotc_core_error_name(status.last_error);
    snapshot.assets_validated = status.assets_validated != 0;
    snapshot.assets_ready = status.assets_ready != 0;
    snapshot.engine_implemented = status.engine_implemented != 0;
    snapshot.ctor = make_ctor_snapshot(status.ctor);
    emit(soc::status_json::format_status(snapshot));
    return kExitOk;
}

int handle_coverage(const Emit& emit) {
    sotc_coverage_report report{};
    report.struct_size = sizeof report;
    const sotc_error error = sotc_core_coverage(&report);
    if (error != SOTC_OK) {
        diagnostic("coverage failed: " + std::string(sotc_core_error_name(error)));
        return kExitInternal;
    }
    soc::status_json::CoverageSnapshot snapshot;
    snapshot.abi = report.abi_version;
    snapshot.engine_implemented = report.engine_implemented != 0;
    snapshot.subsystems.reserve(report.subsystem_count);
    for (std::uint32_t i = 0; i < report.subsystem_count && i < SOTC_SUBSYSTEM_COUNT; ++i) {
        soc::status_json::CoverageRowSnapshot row;
        row.name = report.subsystems[i].name;
        row.status = subsystem_status_name(report.subsystems[i].status);
        row.detail = report.subsystems[i].detail;
        snapshot.subsystems.push_back(std::move(row));
    }
    emit(soc::status_json::format_coverage(snapshot));
    return kExitOk;
}

int handle_validate(Session& session, const std::string& dir, const Emit& emit) {
    sotc_validation_result result{};
    result.struct_size = sizeof result;
    const sotc_error error = sotc_core_validate(session.core, dir.c_str(), &result);

    soc::status_json::ValidateSnapshot snapshot;
    int exit_code = kExitOk;
    if (error != SOTC_OK) {
        char message[SOTC_DETAIL_CAPACITY + 1] = {};
        sotc_core_last_error_message(session.core, message, sizeof message);
        snapshot.ok = false;
        snapshot.error = static_cast<std::uint32_t>(error);
        snapshot.error_name = sotc_core_error_name(static_cast<std::uint32_t>(error));
        snapshot.detail = message;
        if (error == SOTC_ERR_INTERNAL) exit_code = kExitInternal;
    } else {
        session.validated = true;
        snapshot.ok = result.error == SOTC_OK;
        snapshot.error = result.error;
        snapshot.error_name = sotc_core_error_name(result.error);
        snapshot.assets_ready = result.assets_ready != 0;
        snapshot.engine_ready = result.engine_ready != 0;
        snapshot.start_would_succeed = result.start_would_succeed != 0;
        snapshot.detail = result.detail;
        snapshot.archives.reserve(SOTC_ARCHIVE_COUNT);
        for (std::uint32_t i = 0; i < SOTC_ARCHIVE_COUNT; ++i) {
            soc::status_json::ArchiveSnapshot row;
            row.name = result.archives[i].name;
            row.status = archive_status_name(result.archives[i].status);
            row.size_bytes = result.archives[i].size_bytes;
            snapshot.archives.push_back(std::move(row));
        }
        if (result.error == SOTC_ERR_IO) exit_code = kExitValidateHardError;
    }
    emit(soc::status_json::format_validate(snapshot));
    return exit_code;
}

// The engine gap is unconditional: even with ready assets, start() can only fail. The pinned
// start line therefore always reports SOTC_ERR_ENGINE_NOT_IMPLEMENTED. A future engine would have
// to change this program (and its test) rather than silently pass; SOTC_OK is treated as an
// internal contradiction, never as success.
int handle_start(Session& session, const Emit& emit) {
    if (!session.validated && !session.default_assets_dir.empty()) {
        sotc_validation_result preflight{};
        preflight.struct_size = sizeof preflight;
        if (sotc_core_validate(session.core, session.default_assets_dir.c_str(), &preflight) == SOTC_OK) {
            session.validated = true;
        }
    }
    const sotc_error error = sotc_core_start(session.core);
    soc::status_json::StartSnapshot snapshot;
    if (error == SOTC_OK) {
        diagnostic("start() returned SOTC_OK; refusing to report a running game");
        snapshot.ok = false;
        snapshot.error = SOTC_ERR_INTERNAL;
        snapshot.error_name = sotc_core_error_name(SOTC_ERR_INTERNAL);
        emit(soc::status_json::format_start(snapshot));
        return kExitInternal;
    }
    if (error == SOTC_ERR_ENGINE_NOT_IMPLEMENTED || error == SOTC_ERR_BAD_STATE ||
        error == SOTC_ERR_ASSETS_NOT_READY) {
        if (error != SOTC_ERR_ENGINE_NOT_IMPLEMENTED) {
            diagnostic(std::string("start refused before the engine check (") +
                       sotc_core_error_name(error) + "); the engine gap is still the permanent cause");
        }
        snapshot.ok = false;
        snapshot.error = SOTC_ERR_ENGINE_NOT_IMPLEMENTED;
        snapshot.error_name = sotc_core_error_name(SOTC_ERR_ENGINE_NOT_IMPLEMENTED);
        emit(soc::status_json::format_start(snapshot));
        return kExitEngineNotImplemented;
    }
    diagnostic("start failed: " + std::string(sotc_core_error_name(error)));
    snapshot.ok = false;
    snapshot.error = static_cast<std::uint32_t>(error);
    snapshot.error_name = sotc_core_error_name(static_cast<std::uint32_t>(error));
    emit(soc::status_json::format_start(snapshot));
    return kExitInternal;
}

void handle_stop(Session& session, const Emit& emit) {
    const sotc_error error = sotc_core_stop(session.core);
    soc::status_json::StopSnapshot snapshot;
    snapshot.ok = error == SOTC_OK;
    snapshot.error = static_cast<std::uint32_t>(error);
    snapshot.error_name = sotc_core_error_name(static_cast<std::uint32_t>(error));
    emit(soc::status_json::format_stop(snapshot));
    if (error != SOTC_OK) diagnostic("stop failed: " + std::string(sotc_core_error_name(error)));
}

// ---- --serve request parsing -------------------------------------------------------------------

struct Request {
    bool has_id = false;
    std::int64_t id = 0;
    std::string command;
    std::string argument;
    bool malformed = false;
};

std::string_view trim(std::string_view text) {
    while (!text.empty() && std::isspace(static_cast<unsigned char>(text.front())) != 0) {
        text.remove_prefix(1);
    }
    while (!text.empty() && std::isspace(static_cast<unsigned char>(text.back())) != 0) {
        text.remove_suffix(1);
    }
    return text;
}

Request parse_request(std::string_view raw) {
    Request request;
    const std::string_view line = trim(raw);
    if (line.empty()) {
        request.malformed = true;
        return request;
    }
    const std::size_t id_end = line.find_first_of(" \t");
    const std::string_view id_text = line.substr(0, id_end);
    if (id_text.empty()) {
        request.malformed = true;
        return request;
    }
    std::int64_t id = 0;
    const char* const begin = id_text.data();
    const char* const end = begin + id_text.size();
    const std::from_chars_result parsed = std::from_chars(begin, end, id);
    if (parsed.ec != std::errc{} || parsed.ptr != end) {
        request.malformed = true;
        return request;
    }
    request.has_id = true;
    request.id = id;
    if (id_end == std::string_view::npos) {
        request.malformed = true;
        return request;
    }
    const std::string_view rest = trim(line.substr(id_end));
    if (rest.empty()) {
        request.malformed = true;
        return request;
    }
    const std::size_t command_end = rest.find_first_of(" \t");
    request.command = std::string(rest.substr(0, command_end));
    if (command_end != std::string_view::npos) {
        request.argument = std::string(trim(rest.substr(command_end)));
    }
    return request;
}

enum class ServeAction { kContinue, kQuit, kFatal };

void emit_bad_request(const Emit& emit) { emit("{\"ok\":false,\"error\":\"bad_request\"}"); }

ServeAction serve_one(Session& session, const Request& request) {
    // Every serve reply carries an id first; a request whose id token could not be parsed replies
    // with id 0 as the protocol requires.
    const Emit emit(std::optional<std::int64_t>(request.has_id ? request.id : 0));
    if (request.malformed) {
        emit_bad_request(emit);
        return ServeAction::kContinue;
    }
    if (request.command == "quit") {
        if (!request.argument.empty()) {
            emit_bad_request(emit);
            return ServeAction::kContinue;
        }
        emit("{\"ok\":true,\"bye\":true}");
        return ServeAction::kQuit;
    }
    if (request.command == "status" || request.command == "coverage" || request.command == "start" ||
        request.command == "stop") {
        if (!request.argument.empty()) {
            emit_bad_request(emit);
            return ServeAction::kContinue;
        }
        int code = kExitOk;
        if (request.command == "status") {
            code = handle_status(session, emit);
        } else if (request.command == "coverage") {
            code = handle_coverage(emit);
        } else if (request.command == "start") {
            code = handle_start(session, emit);
        } else {
            handle_stop(session, emit);
        }
        return code == kExitInternal ? ServeAction::kFatal : ServeAction::kContinue;
    }
    if (request.command == "validate") {
        if (request.argument.empty()) {
            emit_bad_request(emit);
            return ServeAction::kContinue;
        }
        const int code = handle_validate(session, request.argument, emit);
        return code == kExitInternal ? ServeAction::kFatal : ServeAction::kContinue;
    }
    emit_bad_request(emit);
    return ServeAction::kContinue;
}

int run_serve(Session& session) {
    std::string line;
    while (std::getline(std::cin, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        const Request request = parse_request(line);
        const ServeAction action = serve_one(session, request);
        if (action == ServeAction::kQuit) return kExitOk;
        if (action == ServeAction::kFatal) return kExitInternal;
    }
    return kExitOk;  // EOF is a clean end of the session
}

int run(const Options& options) {
    sotc_core* core = nullptr;
    const sotc_error created = sotc_core_create(SOTC_CORE_ABI_VERSION, &core);
    if (created != SOTC_OK || core == nullptr) {
        diagnostic("sotc_core_create failed: " + std::string(sotc_core_error_name(created)));
        return kExitInternal;
    }
    Session session;
    session.core = core;
    session.default_assets_dir = options.assets_set ? options.assets_dir : std::string();

    int code = kExitOk;
    switch (options.command) {
        case Command::kStatus: code = handle_status(session, Emit(std::nullopt)); break;
        case Command::kCoverage: code = handle_coverage(Emit(std::nullopt)); break;
        case Command::kValidate: code = handle_validate(session, options.validate_dir, Emit(std::nullopt)); break;
        case Command::kStart: code = handle_start(session, Emit(std::nullopt)); break;
        case Command::kServe: code = run_serve(session); break;
        case Command::kNone: code = kExitUsage; break;
    }
    sotc_core_destroy(core);
    return code;
}

}  // namespace

int main(int argc, char** argv) {
    Options options;
    bool help = false;
    std::string error;
    if (!parse_options(argc, argv, options, help, error)) {
        if (!error.empty()) diagnostic(error);
        usage(std::cerr);
        return kExitUsage;
    }
    if (help) {
        usage(std::cout);
        return kExitOk;
    }
    if (options.fixture_set) {
        if (::setenv(sotc::kSlot23NamesFixtureEnv, options.fixture.c_str(), 1) != 0 ||
            ::setenv(kSlot23FixtureAlias, options.fixture.c_str(), 1) != 0) {
            diagnostic("could not export the slot-23 fixture path");
            return kExitInternal;
        }
    }
    try {
        return run(options);
    } catch (const std::exception& exception) {
        diagnostic(std::string("internal error: ") + exception.what());
        return kExitInternal;
    } catch (...) {
        diagnostic("internal error");
        return kExitInternal;
    }
}
