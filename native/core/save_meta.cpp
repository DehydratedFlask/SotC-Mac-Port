#include "core/save_meta.hpp"

#include <cstring>

namespace sotc {
namespace {

void put_u32(std::byte* p, std::uint32_t v) noexcept {
    for (int i = 0; i != 4; ++i) p[i] = static_cast<std::byte>((v >> (8 * i)) & 0xFFu);
}

void put_u64(std::byte* p, std::uint64_t v) noexcept {
    for (int i = 0; i != 8; ++i) p[i] = static_cast<std::byte>((v >> (8 * i)) & 0xFFu);
}

std::uint32_t get_u32(const std::byte* p) noexcept {
    std::uint32_t v = 0;
    for (int i = 0; i != 4; ++i) v |= std::to_integer<std::uint32_t>(p[i]) << (8 * i);
    return v;
}

std::uint64_t get_u64(const std::byte* p) noexcept {
    std::uint64_t v = 0;
    for (int i = 0; i != 8; ++i) v |= std::to_integer<std::uint64_t>(p[i]) << (8 * i);
    return v;
}

// One slot: a NUL-terminated text of at most `slot - 1` bytes.
SaveMetaError copy_text_out(const std::byte* slot, std::size_t limit, std::string& out) noexcept {
    const void* const nul = std::memchr(slot, 0, limit);
    if (nul == nullptr) return SaveMetaError::text_too_long;
    const auto length = static_cast<std::size_t>(static_cast<const std::byte*>(nul) - slot);
    out.assign(reinterpret_cast<const char*>(slot), length);
    return SaveMetaError::none;
}

SaveMetaError validate_text(const std::string& text, std::size_t slot) noexcept {
    if (text.size() >= slot) return SaveMetaError::text_too_long;  // strcpy writes len+1
    if (text.find('\0') != std::string::npos) return SaveMetaError::text_has_nul;
    return SaveMetaError::none;
}

void copy_text_in(std::byte* slot, const std::string& text) noexcept {
    std::memcpy(slot, text.data(), text.size());
    slot[text.size()] = std::byte{0};
}

}  // namespace

const char* to_string(SaveMetaError error) noexcept {
    switch (error) {
        case SaveMetaError::none: return "none";
        case SaveMetaError::text_too_long: return "text_too_long";
        case SaveMetaError::text_has_nul: return "text_has_nul";
        case SaveMetaError::raw_size: return "raw_size";
    }
    return "?";
}

SaveMetaError encode_save_meta(const SaveMeta& meta, std::vector<std::byte>& out) {
    SaveMetaError error = validate_text(meta.title, kSaveMetaTitleSlot);
    if (error != SaveMetaError::none) return error;
    error = validate_text(meta.subtitle, kSaveMetaSubtitleSlot);
    if (error != SaveMetaError::none) return error;
    error = validate_text(meta.detail, kSaveMetaDetailSlot);
    if (error != SaveMetaError::none) return error;

    std::vector<std::byte> block(kSaveParamBytes, std::byte{0});
    copy_text_in(block.data(), meta.title);
    copy_text_in(block.data() + kSaveMetaTitleSlot, meta.subtitle);
    copy_text_in(block.data() + kSaveMetaTitleSlot + kSaveMetaSubtitleSlot, meta.detail);
    put_u32(block.data() + kSaveMetaUserParamOffset, meta.user_param);
    std::memcpy(block.data() + kSaveMetaUserParamOffset + 4, meta.reserved.data(),
                kSaveMetaReservedBytes);
    out = std::move(block);
    return SaveMetaError::none;
}

SaveMetaError decode_save_meta(std::span<const std::byte> bytes, SaveMeta& out) {
    if (bytes.size() != kSaveParamBytes) return SaveMetaError::raw_size;
    SaveMeta value{};
    SaveMetaError error = copy_text_out(bytes.data(), kSaveMetaTitleSlot, value.title);
    if (error != SaveMetaError::none) return error;
    error = copy_text_out(bytes.data() + kSaveMetaTitleSlot, kSaveMetaSubtitleSlot,
                          value.subtitle);
    if (error != SaveMetaError::none) return error;
    error = copy_text_out(bytes.data() + kSaveMetaTitleSlot + kSaveMetaSubtitleSlot,
                          kSaveMetaDetailSlot, value.detail);
    if (error != SaveMetaError::none) return error;
    value.user_param = get_u32(bytes.data() + kSaveMetaUserParamOffset);
    std::memcpy(value.reserved.data(), bytes.data() + kSaveMetaUserParamOffset + 4,
                kSaveMetaReservedBytes);
    out = std::move(value);
    return SaveMetaError::none;
}

std::vector<std::byte> encode_meta_file(std::span<const std::byte> block, std::uint32_t data_hash,
                                        std::uint32_t descriptor_id) {
    if (block.size() != kSaveParamBytes) return {};
    std::vector<std::byte> file(kSaveMetaFileBytes, std::byte{0});
    std::memcpy(file.data(), kSaveMetaMagic.data(), kSaveMetaMagic.size());
    put_u32(file.data() + 0x08, kSaveMetaVersion);
    put_u32(file.data() + 0x0C, descriptor_id);
    put_u64(file.data() + 0x10, data_hash);
    put_u64(file.data() + 0x18, kSaveParamBytes);
    std::memcpy(file.data() + kSaveMetaHeaderBytes, block.data(), kSaveParamBytes);
    return file;
}

const char* to_string(SaveMetaFileError error) noexcept {
    switch (error) {
        case SaveMetaFileError::none: return "none";
        case SaveMetaFileError::missing: return "missing";
        case SaveMetaFileError::bad_magic: return "bad_magic";
        case SaveMetaFileError::bad_version: return "bad_version";
        case SaveMetaFileError::bad_size: return "bad_size";
        case SaveMetaFileError::bad_param_bytes: return "bad_param_bytes";
    }
    return "?";
}

SaveMetaFileError decode_meta_file(std::span<const std::byte> bytes, std::uint32_t& data_hash,
                                   std::uint32_t& descriptor_id,
                                   std::vector<std::byte>& block) {
    data_hash = 0;
    descriptor_id = 0;
    block.clear();
    if (bytes.size() != kSaveMetaFileBytes) return SaveMetaFileError::bad_size;
    if (std::memcmp(bytes.data(), kSaveMetaMagic.data(), kSaveMetaMagic.size()) != 0) {
        return SaveMetaFileError::bad_magic;
    }
    if (get_u32(bytes.data() + 0x08) != kSaveMetaVersion) return SaveMetaFileError::bad_version;
    descriptor_id = get_u32(bytes.data() + 0x0C);
    data_hash = static_cast<std::uint32_t>(get_u64(bytes.data() + 0x10));
    if (get_u64(bytes.data() + 0x18) != kSaveParamBytes) return SaveMetaFileError::bad_param_bytes;
    block.assign(bytes.begin() + static_cast<std::ptrdiff_t>(kSaveMetaHeaderBytes), bytes.end());
    return SaveMetaFileError::none;
}

bool save_icon_ok(const SaveIcon& icon) noexcept {
    return icon.bytes.empty() || icon.bytes.size() <= kSaveIconMaxBytes;
}

}  // namespace sotc
