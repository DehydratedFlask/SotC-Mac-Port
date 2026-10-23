// Native owner-side wrapper around the recovered application-info registry list.
//
// The original container is the 0x60 B three-list object built by `0x1bdb10` (E534) and stored in
// `*(0x2ee68c8)`. Its list-1 sentinel (`+0x00`) receives the 0x58-byte named-entry nodes built by
// `0x1bdb50` (E560); list-3 (`+0x40`) receives the 0x60-byte callable nodes inserted by `0x1beeb0`
// through `0x1bde30`, and is the list `0x1290` uses. List-2 (`+0x20`) has no recovered writer yet.
//
// This wrapper exists because `0x1290` cannot run natively until the singleton owner exists: the
// A-insert needs a real `*(0x2ee68c8)` object. It provides that object, the entry-insert helper for
// the registry list, and the same list-3 view for the existing lifted insert.
//
// Layout compatibility (asserted below, not assumed):
//   Container1beeb0: three {prev,next,count(u64),pad} groups at 0x00/0x20/0x40.
//   List1bdeb0:      {prev,next,count(u32),pad} at 0x00; the 0x1bdb50 inserter increments the
//                    count 64-bit, the 0x1bdeb0 destructor decrements it 32-bit (E560/E559), so
//                    the native model keeps the 32-bit view and relies on counts < 2^32.
// The two list views share the same storage on purpose: the original stores both node types in
// the same 0x60 B object, and the destructor's `+0x50` test discriminates them.
//
// Entry payload: both callable slots are 0x20 B `{ptr,len,aux,control}` holders. String entries use
// `{data,size,0,null}`, exactly the non-owning string view the five observed `0x1bdb50` call sites
// build. A null control pointer makes the destructor's `0xf590` calls no-ops, so an entry does not
// own its text. Callers must keep the text alive for the registry's lifetime.
//
// NOT modelled: the allocator/release bodies (hooks), the 0x2ee68c8 construction site (owned by the
// future 0x2569d0 lift), the flag semantics beyond "non-zero entries are skipped by 0x1bdeb0", and
// any concurrent access. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

#include "container_1bdb10.hpp"
#include "insert_1bdb50.hpp"
#include "list_dtor_1bdeb0.hpp"

namespace sotc {

inline constexpr std::uint64_t kAppInfoRegistryOwnerRva = 0x2EE68C8;
inline constexpr std::uint64_t kAppInfoRegistryContainerRva = 0x1BDB10;
inline constexpr std::uint64_t kAppInfoRegistryInsertRva = 0x1BDB50;
inline constexpr std::uint64_t kAppInfoRegistryDtorRva = 0x1BDEB0;
inline constexpr std::size_t kAppInfoRegistryOwnerSize = 0x60;
inline constexpr std::size_t kAppInfoRegistryInfoListOffset = 0x00;
inline constexpr std::size_t kAppInfoRegistryHandlerListOffset = 0x40;

static_assert(sizeof(Container1beeb0) == kAppInfoRegistryOwnerSize);
static_assert(offsetof(Container1beeb0, list1_prev) == kAppInfoRegistryInfoListOffset);
static_assert(offsetof(Container1beeb0, list3_prev) == kAppInfoRegistryHandlerListOffset);
static_assert(sizeof(List1bdeb0) == 0x18);  // count/pad live at +0x10; +0x18 is container pad
static_assert(offsetof(List1bdeb0, count) == 0x10);

// The registry itself. Construction runs the lifted 0x1bdb10 ctor; destruction is deliberately not
// automatic because the original teardown is an explicit call (0x1bdeb0) with caller-supplied
// allocator hooks. Tests must call clear() with matching hooks before the storage goes away.
class AppInfoRegistry {
public:
    AppInfoRegistry() noexcept;

    AppInfoRegistry(const AppInfoRegistry&) = delete;
    AppInfoRegistry& operator=(const AppInfoRegistry&) = delete;

    // List-1 sentinel: the 0x58-byte named-entry nodes.
    [[nodiscard]] List1bdeb0& info_list() noexcept;
    [[nodiscard]] const List1bdeb0& info_list() const noexcept;

    // The whole 0x60 B object, as consumed by `insert_1beeb0` / `0x1290` (list-3 at +0x40).
    [[nodiscard]] Container1beeb0& container() noexcept { return container_; }
    [[nodiscard]] const Container1beeb0& container() const noexcept { return container_; }

    // Number of list-1 entries (32-bit view of the count at +0x10).
    [[nodiscard]] std::uint32_t entry_count() const noexcept;

    // Inserts a named entry whose two payload holders are string views. `flag` is the dword the
    // original stores at node+0x50; every observed registry insert passes 1. The text must outlive
    // the registry. Returns the same result shape as the lifted inserter, including a visible
    // failure when the allocation hook is null or fails.
    [[nodiscard]] Insert1bdb50Result add_string_entry(std::string_view name,
                                                      std::string_view value,
                                                      std::uint32_t flag,
                                                      const Insert1bdb50Hooks& hooks) noexcept;

    // Inserts an entry with caller-supplied 0x20 B holders, for values that are functors or
    // otherwise not string views (the original "Build Version" value is such a holder).
    [[nodiscard]] Insert1bdb50Result add_callable_entry(const Callable1bdeb0& name,
                                                        const Callable1bdeb0& value,
                                                        std::uint32_t flag,
                                                        const Insert1bdb50Hooks& hooks) noexcept;

    // Runs the lifted 0x1bdeb0 teardown over list-1. With the observed flag=1 entries the
    // destructor skips every node by design, so this reports skipped entries instead of erasing
    // them; flag=0 entries (no analysed path produces them yet) are released and freed.
    [[nodiscard]] ListDtor1bdeb0Result clear(const ListDtor1bdeb0Hooks& hooks) noexcept;

private:
    Container1beeb0 container_{};
};

// Builds the 0x20 B non-owning string view holder the five observed sites pass to 0x1bdb50.
[[nodiscard]] Callable1bdeb0 make_string_view_holder(std::string_view view) noexcept;

}  // namespace sotc
