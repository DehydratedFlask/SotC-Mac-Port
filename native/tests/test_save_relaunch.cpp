// Cross-process save/load persistence fixture (SAV-02-RELAUNCH-01).
//
// The existing transaction tests round-trip through `save_io_posix()` inside one process. This one
// proves the stronger host claim: a save written by one killable process is loaded, validated and
// enumerated by a second, fresh process that shares only the directory on disk.
//
// It is still NOT a real platform save and NOT a retail-format import: both processes use the
// native `ProgressV1` encoder and the synthetic `USR-META` wrapper. G2/G5 remain NOT RUN.
//
//   test_save_relaunch                 parent: temp dir -> child write -> child read -> cleanup
//   test_save_relaunch --write <root>  child: write slot "0" with metadata and exit 0/1
//   test_save_relaunch --read <root>   child: load, compare, enumerate and exit 0/1
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>
#include <vector>

#include <mach-o/dyld.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "core/save_meta.hpp"
#include "core/save_progress_object.hpp"
#include "core/save_slot_store.hpp"
#include "core/save_transaction.hpp"

using namespace sotc;

namespace {

int g_checks = 0;

#define CHECK(condition)                                                             \
    do {                                                                             \
        ++g_checks;                                                                  \
        if (!(condition)) {                                                          \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #condition); \
            return 1;                                                                \
        }                                                                            \
    } while (0)

ProgressV1 expected_object() {
    ProgressV1 object = make_progress_v1();
    object.words[0] = 0xDEADBEEFu;
    object.words[4] = 0x01020304u;
    object.booleans[1] = true;
    object.floats[1] = 1.5f;
    object.ids = {0x11u, 0x22u, 0x33u};
    object.unlock[0] = 2;
    object.unlock[256] = 1;
    // Keep the blob bits and the flag array consistent: `encode_progress_body` rewrites the blob
    // from `flags`, so a mismatched fixture would legitimately differ after a load.
    const auto flags = flag_specs();
    (void)set_flag_bit(object.game_data, flags[0].flag_index, true);
    (void)set_flag_bit(object.game_data, flags[47].flag_index, true);
    object.flags[0] = true;
    object.flags[47] = true;
    return object;
}

SaveMeta expected_meta() {
    SaveMeta meta;
    meta.title = "SOTC-RELAUNCH";
    meta.subtitle = "SLOT-0";
    meta.detail = "cross-process persistence fixture";
    meta.user_param = 0x00010003u;
    return meta;
}

bool same_object(const ProgressV1& a, const ProgressV1& b) {
    return a.game_data == b.game_data && a.unlock == b.unlock && a.flags == b.flags &&
           a.words == b.words && a.booleans == b.booleans && a.floats == b.floats && a.ids == b.ids;
}

std::string executable_path() {
    char buffer[4096];
    std::uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) != 0) return {};
    char resolved[4096];
    if (::realpath(buffer, resolved) == nullptr) return {};
    return resolved;
}

int child_write(const std::string& root) {
    SaveStore store;
    store.root = root;
    store.ops = save_io_posix();
    const SaveIoResult init = save_store_init(store);
    CHECK(init.ok());
    SaveMeta meta = expected_meta();
    SaveIcon icon;
    icon.bytes = {std::byte{0x89}, std::byte{'P'}, std::byte{'N'}, std::byte{'G'}};
    const SaveStatus stored = save_store(store, "0", expected_object(), SaveAttachments{&meta, &icon});
    CHECK(stored.ok());
    CHECK(stored.data_committed);
    std::printf("child-write: %d checks, data committed\n", g_checks);
    return 0;
}

int child_read(const std::string& root) {
    SaveStore store;
    store.root = root;
    store.ops = save_io_posix();
    ProgressV1 loaded;
    const SaveStatus status = save_load(store, "0", loaded);
    CHECK(status.ok());
    CHECK(same_object(loaded, expected_object()));

    std::vector<SaveSlotInfo> slots;
    CHECK(save_enumerate(store, slots).ok());
    CHECK(slots.size() == 1);
    CHECK(slots[0].dir_name == std::string(kSaveDirPrefix) + "0");
    CHECK(slots[0].game_suffix == "0");
    CHECK(slots[0].has_meta);
    CHECK(slots[0].meta_matches_data);
    CHECK(slots[0].meta.title == expected_meta().title);
    CHECK(slots[0].meta.user_param == expected_meta().user_param);
    std::printf("child-read: %d checks, loaded and enumerated\n", g_checks);
    return 0;
}

int run_child(const std::string& exe, const char* mode, const std::string& root) {
    const pid_t pid = ::fork();
    if (pid < 0) return -1;
    if (pid == 0) {
        ::execl(exe.c_str(), exe.c_str(), mode, root.c_str(), static_cast<char*>(nullptr));
        std::perror("execl");
        ::_exit(127);
    }
    int status = 0;
    if (::waitpid(pid, &status, 0) != pid) return -1;
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc == 3 && std::strcmp(argv[1], "--write") == 0) return child_write(argv[2]);
    if (argc == 3 && std::strcmp(argv[1], "--read") == 0) return child_read(argv[2]);

    const std::string exe = executable_path();
    CHECK(!exe.empty());
    std::string tmpl = "/tmp/sotc_save_relaunch_XXXXXX";
    char* made = ::mkdtemp(tmpl.data());
    CHECK(made != nullptr);
    const std::string root = made;

    const int wrote = run_child(exe, "--write", root);
    CHECK(wrote == 0);
    CHECK(std::filesystem::is_regular_file(root + "/" + save_slot_path("0", kSaveDataName)));
    CHECK(std::filesystem::is_regular_file(root + "/" + save_slot_path("0", kSaveMetaName)));

    const int read = run_child(exe, "--read", root);
    CHECK(read == 0);

    // A second reader proves the file is not consumed or mutated by loading it.
    CHECK(run_child(exe, "--read", root) == 0);

    std::error_code ignored;
    std::filesystem::remove_all(root, ignored);
    std::printf("test_save_relaunch: %d parent checks + child processes passed\n", g_checks);
    return 0;
}
