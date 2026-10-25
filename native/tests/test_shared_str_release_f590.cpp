#include <array>
#include <atomic>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <type_traits>
#include <vector>

#include "core/shared_str_release_f590.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {
using sotc::SharedStr;
using Kind = sotc::SharedStrDeallocationKindF590;
using Deallocator = sotc::SharedStrDeallocatorF590;
static_assert(std::is_abstract_v<Deallocator>);
static_assert(static_cast<unsigned>(Kind::data) == 0 && static_cast<unsigned>(Kind::control) == 1);

const std::uint8_t text[] = {0x61, 0x80, 0xff, 0};
enum class Mutation { none, clear_ctl, replace_ctl };
struct Case { const char* name; std::uint64_t old; bool null_ctl; Mutation mutation; bool null_data = false; };
constexpr Case cases[] = {
    {"null", 0x123456789abcdef0, true, Mutation::none},
    {"two", 2, false, Mutation::none},
    {"one", 1, false, Mutation::none},
    {"clear", 1, false, Mutation::clear_ctl},
    {"replace", 1, false, Mutation::replace_ctl},
    {"zero", 0, false, Mutation::none},
    {"negative", UINT64_MAX, false, Mutation::none},
    {"minimum", UINT64_C(0x8000000000000000), false, Mutation::none},
    {"maximum", UINT64_C(0x7fffffffffffffff), false, Mutation::none},
    {"null_data", 1, false, Mutation::none, true},
};

struct Event { const void* allocation; Kind kind; std::uint64_t counter; };
struct Recorder final : Deallocator {
    SharedStr* object{};
    std::int64_t* initial{};
    std::int64_t* replacement{};
    Mutation mutation{};
    std::array<Event, 2> events{};
    std::size_t count{};

    void deallocate(const void* allocation, Kind kind) noexcept override {
        CHECK(count < events.size());
        events[count++] = {allocation, kind, std::bit_cast<std::uint64_t>(
            std::atomic_ref<std::int64_t>(*initial).load(std::memory_order_seq_cst))};
        if (kind == Kind::data) {
            if (mutation == Mutation::clear_ctl) object->ctl = nullptr;
            if (mutation == Mutation::replace_ctl) object->ctl = replacement;
        }
    }
};

const char* pointer_name(const void* p, const std::int64_t* initial, const std::int64_t* replacement) {
    if (p == nullptr) return "null";
    if (p == text) return "data";
    if (p == initial) return "initial";
    if (p == replacement) return "replacement";
    CHECK(false);
    return "unknown";
}

void scenario(const Case& c, bool dump, bool first) {
    alignas(8) std::int64_t counter = std::bit_cast<std::int64_t>(c.old);
    alignas(8) std::int64_t replacement = 77;
    struct Frame { std::array<std::uint8_t, 16> before; SharedStr object; std::array<std::uint8_t, 16> after; } frame{};
    frame.before.fill(0xa5);
    frame.after.fill(0x5a);
    frame.object = {c.null_data ? nullptr : text, -7, INT64_C(0x123456789abcdef), c.null_ctl ? nullptr : &counter};
    std::array<std::uint8_t, sizeof(SharedStr)> original{};
    std::memcpy(original.data(), &frame.object, original.size());
    Recorder recorder;
    recorder.object = &frame.object;
    recorder.initial = &counter;
    recorder.replacement = &replacement;
    recorder.mutation = c.mutation;
    sotc::release_shared_str_f590(frame.object, recorder);

    const bool unchanged = std::memcmp(original.data(), &frame.object, original.size()) == 0;
    bool guards_unchanged = true;
    for (auto b : frame.before) guards_unchanged &= b == 0xa5;
    for (auto b : frame.after) guards_unchanged &= b == 0x5a;
    if (!dump) {
        const auto expected_counter = c.null_ctl ? c.old : c.old - UINT64_C(1);
        CHECK(std::bit_cast<std::uint64_t>(counter) == expected_counter);
        CHECK(replacement == 77);
        CHECK(frame.object.data == (c.null_data ? nullptr : text));
        CHECK(frame.object.size == -7 && frame.object.aux == INT64_C(0x123456789abcdef));
        const bool freed = !c.null_ctl && c.old == 1;
        auto* const expected_ctl = freed && c.mutation == Mutation::clear_ctl ? nullptr :
            freed && c.mutation == Mutation::replace_ctl ? &replacement : c.null_ctl ? nullptr : &counter;
        CHECK(frame.object.ctl == expected_ctl);
        CHECK(recorder.count == (freed ? c.mutation == Mutation::clear_ctl ? 1U : 2U : 0U));
        if (freed) {
            CHECK(recorder.events[0].allocation == frame.object.data && recorder.events[0].kind == Kind::data);
            CHECK(recorder.events[0].counter == 0);
            if (recorder.count == 2) {
                CHECK(recorder.events[1].allocation == expected_ctl && recorder.events[1].kind == Kind::control);
                CHECK(recorder.events[1].counter == 0);
            }
        }
        CHECK(unchanged == (!freed || c.mutation == Mutation::none));
        CHECK(guards_unchanged);
        return;
    }
    // Dump observed state without the literal semantic assertions above, so the
    // independent byte oracle must reject semantically wrong native mutants.
    std::printf("%s{\"name\":\"%s\",\"counter\":\"%016llx\",\"replacement_counter\":%lld,\"data\":\"%s\",\"size\":%lld,\"aux\":%lld,\"ctl\":\"%s\",\"object_unchanged\":%s,\"guards_unchanged\":%s,\"events\":[",
                first ? "" : ",", c.name, static_cast<unsigned long long>(std::bit_cast<std::uint64_t>(counter)),
                static_cast<long long>(replacement), pointer_name(frame.object.data, &counter, &replacement),
                static_cast<long long>(frame.object.size), static_cast<long long>(frame.object.aux),
                pointer_name(frame.object.ctl, &counter, &replacement), unchanged ? "true" : "false", guards_unchanged ? "true" : "false");
    for (std::size_t i = 0; i != recorder.count; ++i) {
        const auto& event = recorder.events[i];
        std::printf("%s{\"allocation\":\"%s\",\"kind\":%u,\"counter\":\"%016llx\"}", i ? "," : "",
                    pointer_name(event.allocation, &counter, &replacement), static_cast<unsigned>(event.kind),
                    static_cast<unsigned long long>(event.counter));
    }
    std::printf("]}");
}

void concurrent_counter() {
    constexpr unsigned count = 32;
    alignas(8) std::int64_t counter = count;
    SharedStr object{text, 3, 0, &counter};
    Recorder recorder;
    recorder.object = &object;
    recorder.initial = &counter;
    // Only the last release calls the recorder, after all atomic decrements.
    std::vector<std::thread> threads;
    for (unsigned i = 0; i != count; ++i) threads.emplace_back([&] { sotc::release_shared_str_f590(object, recorder); });
    for (auto& t : threads) t.join();
    CHECK(counter == 0 && recorder.count == 2);
    CHECK(object.ctl == &counter && object.data == text);
}

void real_deallocation() {
    struct RealDelete final : Deallocator {
        unsigned count{};
        void deallocate(const void* p, Kind kind) noexcept override {
            CHECK(static_cast<unsigned>(kind) == count++);
            if (kind == Kind::data) delete[] static_cast<const std::uint8_t*>(p);
            else delete static_cast<const std::int64_t*>(p);
        }
    } deallocator;
    SharedStr object{new std::uint8_t[4]{1, 2, 3, 0}, 3, 0, new std::int64_t(1)};
    std::array<std::uint8_t, sizeof(SharedStr)> original{};
    std::memcpy(original.data(), &object, original.size());
    sotc::release_shared_str_f590(object, deallocator);
    CHECK(deallocator.count == 2);
    // Read only the object representation; both allocations have been freed.
    CHECK(std::memcmp(original.data(), &object, original.size()) == 0);
}
}  // namespace

int main(int argc, char** argv) {
    const bool dump = argc == 2 && std::strcmp(argv[1], "--dump") == 0;
    if (dump) std::printf("{\"cases\":[");
    bool first = true;
    for (const auto& c : cases) { scenario(c, dump, first); first = false; }
    if (dump) { std::puts("]}"); return 0; }
    concurrent_counter();
    real_deallocation();
    std::puts("shared_str_release_f590: all checks passed");
}
