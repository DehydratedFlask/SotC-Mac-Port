#include "save_stream.hpp"

#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <limits>
#include <utility>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include "save_checksum.hpp"
#include "save_chunk_header.hpp"

namespace sotc {
namespace {

void put_u32(std::byte* dst, std::uint32_t value) noexcept {
    for (int i = 0; i != 4; ++i) dst[i] = static_cast<std::byte>((value >> (8 * i)) & 0xffu);
}

std::uint32_t get_u32(const std::byte* src) noexcept {
    std::uint32_t value = 0;
    for (int i = 0; i != 4; ++i) value |= std::to_integer<std::uint32_t>(src[i]) << (8 * i);
    return value;
}

SaveStreamReadResult fail(SaveStreamError error, int os_errno = 0) {
    SaveStreamReadResult result;
    result.error = error;
    result.os_errno = os_errno;
    return result;
}

}  // namespace

const char* to_string(SaveStreamError error) noexcept {
    switch (error) {
        case SaveStreamError::none: return "none";
        case SaveStreamError::duplicate_name: return "duplicate_name";
        case SaveStreamError::payload_too_large: return "payload_too_large";
        case SaveStreamError::body_too_large: return "body_too_large";
        case SaveStreamError::truncated_header: return "truncated_header";
        case SaveStreamError::bad_version: return "bad_version";
        case SaveStreamError::size_too_large: return "size_too_large";
        case SaveStreamError::truncated_body: return "truncated_body";
        case SaveStreamError::trailing_bytes: return "trailing_bytes";
        case SaveStreamError::bad_hash: return "bad_hash";
        case SaveStreamError::truncated_chunk_header: return "truncated_chunk_header";
        case SaveStreamError::bad_chunk_magic: return "bad_chunk_magic";
        case SaveStreamError::chunk_overrun: return "chunk_overrun";
        case SaveStreamError::reader_duplicate_name: return "reader_duplicate_name";
        case SaveStreamError::file_not_found: return "file_not_found";
        case SaveStreamError::file_too_large: return "file_too_large";
        case SaveStreamError::io_open: return "io_open";
        case SaveStreamError::io_read: return "io_read";
        case SaveStreamError::io_write: return "io_write";
        case SaveStreamError::io_fsync: return "io_fsync";
        case SaveStreamError::io_rename: return "io_rename";
    }
    return "unknown";
}

SaveStreamWriter::SaveStreamWriter(std::uint32_t descriptor_id, DuplicatePolicy policy,
                                   std::size_t max_body_size) noexcept
    : descriptor_id_(descriptor_id),
      policy_(policy),
      // The 0x6400000 load-time limit is a hard ceiling even if a caller asks for more.
      max_body_size_(std::min<std::size_t>(max_body_size, kSaveStreamMaxBodySize)) {}

SaveStreamError SaveStreamWriter::append_chunk(std::string_view name, std::uint16_t tag,
                                               std::span<const std::byte> payload) {
    return append_chunk_hashed(save_chunk_name_hash(name), tag, payload);
}

SaveStreamError SaveStreamWriter::append_chunk_hashed(std::uint32_t name_hash, std::uint16_t tag,
                                                      std::span<const std::byte> payload) {
    // Size checks come first and never touch the payload bytes.
    if (payload.size() > std::numeric_limits<std::uint32_t>::max()) {
        return SaveStreamError::payload_too_large;
    }
    const std::size_t room = max_body_size_ - body_.size();   // body_.size() <= max_body_size_
    if (kSaveChunkHeaderSize > room || payload.size() > room - kSaveChunkHeaderSize) {
        return SaveStreamError::body_too_large;
    }
    if (policy_ == DuplicatePolicy::reject &&
        std::find(hashes_.begin(), hashes_.end(), name_hash) != hashes_.end()) {
        return SaveStreamError::duplicate_name;
    }
    SaveChunkHeader header;
    header.name_hash = name_hash;
    header.tag = tag;
    header.payload_length = static_cast<std::uint32_t>(payload.size());
    const auto encoded = encode_save_chunk_header(header);
    body_.insert(body_.end(), encoded.begin(), encoded.end());
    body_.insert(body_.end(), payload.begin(), payload.end());
    hashes_.push_back(name_hash);
    return SaveStreamError::none;
}

std::vector<std::byte> SaveStreamWriter::serialize() const {
    std::vector<std::byte> out(kSaveStreamHeaderSize + body_.size());
    put_u32(out.data() + 0, kSaveStreamVersion);
    put_u32(out.data() + 4, fnv1a32(body_, kFnv1a32OffsetBasis));   // body only, never the header
    put_u32(out.data() + 8, static_cast<std::uint32_t>(body_.size()));
    put_u32(out.data() + 12, descriptor_id_);
    std::copy(body_.begin(), body_.end(), out.begin() + kSaveStreamHeaderSize);
    return out;
}

const SaveStreamChunk* ParsedSaveStream::find_first(std::uint32_t name_hash) const noexcept {
    for (const SaveStreamChunk& chunk : chunks) {
        if (chunk.name_hash == name_hash) return &chunk;
    }
    return nullptr;
}

const SaveStreamChunk* ParsedSaveStream::find_first(std::string_view name) const noexcept {
    return find_first(save_chunk_name_hash(name));
}

SaveStreamReadResult parse_save_stream(std::span<const std::byte> bytes, DuplicatePolicy policy) {
    if (bytes.size() < kSaveStreamHeaderSize) return fail(SaveStreamError::truncated_header);
    SaveStreamHeader header;
    header.version = get_u32(bytes.data() + 0);
    header.body_hash = get_u32(bytes.data() + 4);
    header.body_size = get_u32(bytes.data() + 8);
    header.descriptor_id = get_u32(bytes.data() + 12);
    if (header.version != kSaveStreamVersion) return fail(SaveStreamError::bad_version);
    if (header.body_size > kSaveStreamMaxBodySize) return fail(SaveStreamError::size_too_large);

    const std::size_t available = bytes.size() - kSaveStreamHeaderSize;
    if (available < header.body_size) return fail(SaveStreamError::truncated_body);
    if (available > header.body_size) return fail(SaveStreamError::trailing_bytes);

    const std::span<const std::byte> body = bytes.subspan(kSaveStreamHeaderSize, header.body_size);
    if (fnv1a32(body, kFnv1a32OffsetBasis) != header.body_hash) {
        return fail(SaveStreamError::bad_hash);
    }

    SaveStreamReadResult result;
    result.stream.header = header;
    std::size_t pos = 0;
    while (pos != body.size()) {
        const ChunkParseResult parsed = parse_save_chunk_header(body.subspan(pos));
        if (parsed.error == ChunkParseError::truncated_header) {
            return fail(SaveStreamError::truncated_chunk_header);
        }
        if (parsed.error == ChunkParseError::bad_magic) return fail(SaveStreamError::bad_chunk_magic);
        pos += kSaveChunkHeaderSize;
        if (parsed.header.payload_length > body.size() - pos) {
            return fail(SaveStreamError::chunk_overrun);
        }
        if (policy == DuplicatePolicy::reject &&
            result.stream.find_first(parsed.header.name_hash) != nullptr) {
            return fail(SaveStreamError::reader_duplicate_name);
        }
        SaveStreamChunk chunk;
        chunk.name_hash = parsed.header.name_hash;
        chunk.tag = parsed.header.tag;
        const auto payload = body.subspan(pos, parsed.header.payload_length);
        chunk.payload.assign(payload.begin(), payload.end());
        result.stream.chunks.push_back(std::move(chunk));
        pos += parsed.header.payload_length;
    }
    return result;
}

SaveStreamReadResult read_save_stream_file(const std::string& path, DuplicatePolicy policy) {
    int fd;
    do { fd = ::open(path.c_str(), O_RDONLY); } while (fd < 0 && errno == EINTR);
    if (fd < 0) {
        const int err = errno;
        return fail(err == ENOENT ? SaveStreamError::file_not_found : SaveStreamError::io_open, err);
    }
    struct stat st;
    if (::fstat(fd, &st) != 0) {
        const int err = errno;
        ::close(fd);
        return fail(SaveStreamError::io_read, err);
    }
    if (!S_ISREG(st.st_mode)) {
        ::close(fd);
        return fail(SaveStreamError::io_read, EISDIR);
    }
    if (static_cast<std::uint64_t>(st.st_size) >
        static_cast<std::uint64_t>(kSaveStreamHeaderSize) + kSaveStreamMaxBodySize) {
        ::close(fd);
        return fail(SaveStreamError::file_too_large);
    }
    std::vector<std::byte> bytes(static_cast<std::size_t>(st.st_size));
    std::size_t done = 0;
    while (done != bytes.size()) {
        const ssize_t n = ::read(fd, bytes.data() + done, bytes.size() - done);
        if (n < 0) {
            if (errno == EINTR) continue;
            const int err = errno;
            ::close(fd);
            return fail(SaveStreamError::io_read, err);
        }
        if (n == 0) break;   // file shrank underneath us: parse what we have (reports truncation)
        done += static_cast<std::size_t>(n);
    }
    ::close(fd);
    bytes.resize(done);
    return parse_save_stream(bytes, policy);
}

SaveStreamIoResult write_file_atomic(const std::string& path, std::span<const std::byte> bytes) {
    std::string tmp;
    int fd = -1;
    // Exclusive creation never follows an existing alias or takes another invocation's temp.
    // Pass the mode to open (rather than chmod after mkstemp) so the process umask still applies.
    for (unsigned attempt = 0; attempt != 128; ++attempt) {
        tmp = path + ".tmp." + std::to_string(::arc4random()) + "." +
              std::to_string(::arc4random());
        do { fd = ::open(tmp.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0644); }
        while (fd < 0 && errno == EINTR);
        if (fd >= 0 || errno != EEXIST) break;
    }
    if (fd < 0) return {SaveStreamError::io_open, errno};

    auto abort_with = [&](SaveStreamError error, int err) {
        ::close(fd);
        ::unlink(tmp.c_str());
        return SaveStreamIoResult{error, err};
    };

    std::size_t done = 0;
    while (done != bytes.size()) {
        const ssize_t n = ::write(fd, bytes.data() + done, bytes.size() - done);
        if (n < 0) {
            if (errno == EINTR) continue;
            return abort_with(SaveStreamError::io_write, errno);
        }
        done += static_cast<std::size_t>(n);
    }
    bool synced = false;
#ifdef F_FULLFSYNC
    synced = (::fcntl(fd, F_FULLFSYNC) == 0);   // not all filesystems support it; fall back
#endif
    if (!synced && ::fsync(fd) != 0) return abort_with(SaveStreamError::io_fsync, errno);
    if (::close(fd) != 0) {
        const int err = errno;
        ::unlink(tmp.c_str());
        return {SaveStreamError::io_write, err};
    }
    if (::rename(tmp.c_str(), path.c_str()) != 0) {
        const int err = errno;
        ::unlink(tmp.c_str());
        return {SaveStreamError::io_rename, err};
    }
    // Best effort: persist the directory entry. Failure here does not undo the committed rename.
    const std::size_t slash = path.find_last_of('/');
    const std::string dir = (slash == std::string::npos) ? "." : (slash == 0 ? "/" : path.substr(0, slash));
    const int dfd = ::open(dir.c_str(), O_RDONLY);
    if (dfd >= 0) {
        (void)::fsync(dfd);
        ::close(dfd);
    }
    return {};
}

}  // namespace sotc
