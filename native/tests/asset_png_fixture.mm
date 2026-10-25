#include "asset_png_fixture.hpp"

#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <ImageIO/ImageIO.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <new>
#include <stdexcept>
#include <string>
#include <utility>

namespace sotc::test {
namespace {

constexpr std::array<std::uint8_t, 8> kPngSignature = {
    0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A,
};
constexpr std::size_t kMinPngHeaderBytes = 8 + 4 + 4 + 13 + 4;
constexpr std::size_t kMaxEncodedPngBytes = 16U * 1024U * 1024U;
constexpr std::size_t kMaxDecodedRgbaBytes = 64U * 1024U * 1024U;
constexpr std::uint32_t kMaxImageDimension = 4096;

template <typename Ref>
class ScopedCFRef {
public:
    explicit ScopedCFRef(Ref ref = nullptr) noexcept : ref_(ref) {}
    ~ScopedCFRef() {
        if (ref_ != nullptr) CFRelease(ref_);
    }

    ScopedCFRef(const ScopedCFRef&) = delete;
    ScopedCFRef& operator=(const ScopedCFRef&) = delete;

    [[nodiscard]] Ref get() const noexcept { return ref_; }

private:
    Ref ref_;
};

[[nodiscard]] std::uint32_t read_be32(const std::uint8_t* bytes) noexcept {
    return (static_cast<std::uint32_t>(bytes[0]) << 24U) |
           (static_cast<std::uint32_t>(bytes[1]) << 16U) |
           (static_cast<std::uint32_t>(bytes[2]) << 8U) |
           static_cast<std::uint32_t>(bytes[3]);
}

[[nodiscard]] std::string dimensions(std::uint32_t width, std::uint32_t height) {
    return std::to_string(width) + "x" + std::to_string(height);
}

[[nodiscard]] bool fail(std::string& error, std::string message) {
    error = std::move(message);
    return false;
}

}  // namespace

bool decode_asset_png_rgba8(std::span<const std::uint8_t> png,
                            std::uint32_t expected_width,
                            std::uint32_t expected_height,
                            std::vector<std::uint8_t>& rgba8,
                            std::string& error) {
    rgba8.clear();
    error.clear();

    if (png.empty()) return fail(error, "PNG input is empty");
    if (png.size() > kMaxEncodedPngBytes) {
        return fail(error, "PNG input exceeds the 16 MiB fixture limit");
    }
    if (png.size() > static_cast<std::size_t>(std::numeric_limits<CFIndex>::max())) {
        return fail(error, "PNG input size exceeds ImageIO's data limit");
    }
    if (expected_width == 0 || expected_height == 0) {
        return fail(error, "expected PNG dimensions must be nonzero");
    }

    const std::size_t width = static_cast<std::size_t>(expected_width);
    const std::size_t height = static_cast<std::size_t>(expected_height);
    if (width > std::numeric_limits<std::size_t>::max() / 4U) {
        return fail(error, "RGBA8 PNG row size overflow");
    }
    const std::size_t row_bytes = width * 4U;
    if (height > std::numeric_limits<std::size_t>::max() / row_bytes) {
        return fail(error, "RGBA8 PNG image size overflow");
    }
    const std::size_t decoded_bytes = row_bytes * height;
    if (decoded_bytes > kMaxDecodedRgbaBytes || expected_width > kMaxImageDimension ||
        expected_height > kMaxImageDimension) {
        return fail(error, "expected PNG dimensions exceed the 64 MiB fixture limit");
    }

    if (png.size() < kPngSignature.size() ||
        !std::equal(kPngSignature.begin(), kPngSignature.end(), png.begin())) {
        return fail(error, "PNG input has an invalid signature");
    }
    if (png.size() < kMinPngHeaderBytes || read_be32(png.data() + 8) != 13U ||
        png[12] != 'I' || png[13] != 'H' || png[14] != 'D' || png[15] != 'R') {
        return fail(error, "malformed PNG: missing or truncated IHDR chunk");
    }

    const std::uint32_t png_width = read_be32(png.data() + 16);
    const std::uint32_t png_height = read_be32(png.data() + 20);
    if (png_width == 0 || png_height == 0) {
        return fail(error, "malformed PNG: IHDR dimensions must be nonzero");
    }
    if (png_width != expected_width || png_height != expected_height) {
        return fail(error, "PNG dimensions mismatch: expected " +
                               dimensions(expected_width, expected_height) + ", found " +
                               dimensions(png_width, png_height));
    }
    if (png_width > kMaxImageDimension || png_height > kMaxImageDimension) {
        return fail(error, "PNG dimensions exceed the 4096-pixel fixture limit");
    }

    ScopedCFRef<CFDataRef> data(
        CFDataCreate(kCFAllocatorDefault, png.data(), static_cast<CFIndex>(png.size())));
    if (data.get() == nullptr) return fail(error, "ImageIO could not allocate PNG input data");

    ScopedCFRef<CGImageSourceRef> source(CGImageSourceCreateWithData(data.get(), nullptr));
    if (source.get() == nullptr) return fail(error, "ImageIO could not read PNG input");
    if (CGImageSourceGetCount(source.get()) != 1) {
        return fail(error, "PNG fixture must contain exactly one image");
    }

    ScopedCFRef<CGImageRef> image(CGImageSourceCreateImageAtIndex(source.get(), 0, nullptr));
    if (image.get() == nullptr) {
        return fail(error, "ImageIO failed to decode PNG (status " +
                               std::to_string(static_cast<int>(CGImageSourceGetStatus(source.get()))) +
                               ")");
    }
    const CGImageSourceStatus source_status = CGImageSourceGetStatus(source.get());
    if (source_status != kCGImageStatusComplete) {
        return fail(error, "ImageIO reported an incomplete or malformed PNG source (status " +
                               std::to_string(static_cast<int>(source_status)) + ")");
    }
    const CGImageSourceStatus image_status = CGImageSourceGetStatusAtIndex(source.get(), 0);
    if (image_status != kCGImageStatusComplete) {
        return fail(error, "ImageIO reported an incomplete or malformed PNG (status " +
                               std::to_string(static_cast<int>(image_status)) + ")");
    }

    const std::size_t image_width = CGImageGetWidth(image.get());
    const std::size_t image_height = CGImageGetHeight(image.get());
    if (image_width != width || image_height != height) {
        const auto actual_width = image_width > std::numeric_limits<std::uint32_t>::max()
                                      ? 0U
                                      : static_cast<std::uint32_t>(image_width);
        const auto actual_height = image_height > std::numeric_limits<std::uint32_t>::max()
                                       ? 0U
                                       : static_cast<std::uint32_t>(image_height);
        return fail(error, "PNG dimensions mismatch after ImageIO decode: expected " +
                               dimensions(expected_width, expected_height) + ", found " +
                               dimensions(actual_width, actual_height));
    }

    ScopedCFRef<CGColorSpaceRef> color_space(CGColorSpaceCreateWithName(kCGColorSpaceSRGB));
    if (color_space.get() == nullptr) return fail(error, "could not create the sRGB output color space");

    std::vector<std::uint8_t> decoded;
    try {
        decoded.assign(decoded_bytes, 0);
    } catch (const std::bad_alloc&) {
        return fail(error, "could not allocate the RGBA8 PNG output buffer");
    } catch (const std::length_error&) {
        return fail(error, "RGBA8 PNG output size is not representable");
    }

    const CGBitmapInfo bitmap_info =
        static_cast<CGBitmapInfo>(kCGBitmapByteOrder32Big) |
        static_cast<CGBitmapInfo>(kCGImageAlphaPremultipliedLast);
    ScopedCFRef<CGContextRef> context(CGBitmapContextCreate(
        decoded.data(), width, height, 8, row_bytes, color_space.get(), bitmap_info));
    if (context.get() == nullptr) return fail(error, "CoreGraphics could not create an RGBA8 bitmap");
    if (CGBitmapContextGetData(context.get()) != decoded.data() ||
        CGBitmapContextGetBytesPerRow(context.get()) != row_bytes ||
        CGBitmapContextGetBitsPerPixel(context.get()) != 32U ||
        CGBitmapContextGetBitsPerComponent(context.get()) != 8U) {
        return fail(error, "CoreGraphics did not create the requested tightly packed RGBA8 bitmap");
    }

    // CoreGraphics draws in a bottom-left coordinate space. Flip the CTM so byte row zero
    // is the PNG's top scanline, matching Metal's conventional texture (0, 0) row.
    CGContextSetBlendMode(context.get(), kCGBlendModeCopy);
    CGContextSetInterpolationQuality(context.get(), kCGInterpolationNone);
    CGContextTranslateCTM(context.get(), 0.0, static_cast<CGFloat>(height));
    CGContextScaleCTM(context.get(), 1.0, -1.0);
    CGContextDrawImage(context.get(), CGRectMake(0.0, 0.0, static_cast<CGFloat>(width),
                                                 static_cast<CGFloat>(height)),
                       image.get());
    CGContextFlush(context.get());

    if (decoded.size() != decoded_bytes) {
        return fail(error, "RGBA8 PNG output byte count mismatch");
    }
    for (std::size_t offset = 3; offset < decoded.size(); offset += 4) {
        if (decoded[offset] != 255U) {
            return fail(error, "PNG fixture is not opaque after RGBA8 decode");
        }
    }

    rgba8 = std::move(decoded);
    return true;
}

}  // namespace sotc::test
