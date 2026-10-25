// Direct-Metal offscreen capture provider. Objective-C++ with manual retain/release (no ARC).
//
// Device -> command queue -> MSL library compiled from the embedded source below -> render pipeline
// -> BGRA8Unorm render target -> full-screen triangle strip -> getBytes readback -> BGRA->RGBA
// conversion -> ImageIO PNG. No AppKit and no window: works in a plain CLI/ctest process.
#import <CoreGraphics/CoreGraphics.h>
#import <Foundation/Foundation.h>
#import <ImageIO/ImageIO.h>
#import <Metal/Metal.h>

#include "platform/metal_capture.hpp"

#include <cstddef>
#include <limits>
#include <string>
#include <utility>
#include <vector>

namespace sotc {
MetalCapture::~MetalCapture() = default;

namespace {

// The whole pattern lives here. `capture_fragment` derives red/green from the integer pixel
// position (`[[position]]` is in window pixels) and blue from the frame number, alpha 255.
NSString* const kCaptureMSLSource = @R"MSL(
#include <metal_stdlib>
using namespace metal;

struct CaptureVertexOut {
    float4 position [[position]];
};

constant float2 kCaptureQuad[4] = {
    float2(-1.0f, -1.0f),
    float2( 1.0f, -1.0f),
    float2(-1.0f,  1.0f),
    float2( 1.0f,  1.0f),
};

struct CaptureUniforms {
    uint frame_num;
};

vertex CaptureVertexOut capture_vertex(uint vid [[vertex_id]]) {
    CaptureVertexOut out;
    out.position = float4(kCaptureQuad[vid], 0.0f, 1.0f);
    return out;
}

fragment float4 capture_fragment(CaptureVertexOut in [[stage_in]],
                                 constant CaptureUniforms& uniforms [[buffer(0)]]) {
    uint2 pixel = uint2(floor(in.position.xy));
    float red = float(pixel.x & 255u) / 255.0f;
    float green = float(pixel.y & 255u) / 255.0f;
    float blue = float(64u + (uniforms.frame_num & 63u)) / 255.0f;
    return float4(red, green, blue, 1.0f);
}
)MSL";

// Conservative row alignment for `getBytes:bytesPerRow:fromRegion:mipmapLevel:`; the readback then
// gets repacked to the tight `width * 4` stride the header promises.
constexpr std::size_t kReadbackRowAlignment = 256;

std::string describe_ns_error(NSError* error) {
    if (error == nil) {
        return "unknown error";
    }
    const char* text = [[error localizedDescription] UTF8String];
    return text == nullptr ? std::string("unknown error") : std::string(text);
}

std::string capture_size_text(std::uint32_t width, std::uint32_t height) {
    return std::to_string(width) + "x" + std::to_string(height);
}

bool encode_png_rgba8(const std::vector<std::uint8_t>& rgba8, std::uint32_t width,
                      std::uint32_t height, std::vector<std::uint8_t>& png, std::string& error) {
    png.clear();
    error.clear();
    if (width == 0 || height == 0) {
        error = "invalid RGBA8 PNG dimensions";
        return false;
    }
    if (static_cast<std::size_t>(width) > std::numeric_limits<std::size_t>::max() / 4) {
        error = "RGBA8 PNG row size overflow";
        return false;
    }
    const std::size_t tight_row = static_cast<std::size_t>(width) * 4;
    if (static_cast<std::size_t>(height) > std::numeric_limits<std::size_t>::max() / tight_row) {
        error = "RGBA8 PNG image size overflow";
        return false;
    }
    const std::size_t expected_size = tight_row * static_cast<std::size_t>(height);
    if (rgba8.size() != expected_size) {
        error = "rgba8 buffer size mismatch";
        return false;
    }

    CGDataProviderRef provider =
        CGDataProviderCreateWithData(nullptr, rgba8.data(), rgba8.size(), nullptr);
    if (provider == nullptr) {
        error = "CGDataProviderCreateWithData failed";
        return false;
    }
    CGColorSpaceRef color_space = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
    if (color_space == nullptr) {
        CGDataProviderRelease(provider);
        error = "CGColorSpaceCreateWithName(kCGColorSpaceSRGB) failed";
        return false;
    }
    const CGBitmapInfo bitmap_info = static_cast<CGBitmapInfo>(
        static_cast<std::uint32_t>(kCGImageAlphaLast) |
        static_cast<std::uint32_t>(kCGBitmapByteOrder32Big));
    CGImageRef image = CGImageCreate(width, height, 8, 32, tight_row, color_space, bitmap_info,
                                     provider, nullptr, false, kCGRenderingIntentDefault);
    if (image == nullptr) {
        CGColorSpaceRelease(color_space);
        CGDataProviderRelease(provider);
        error = "CGImageCreate failed";
        return false;
    }

    bool encoded = false;
    CFMutableDataRef data = CFDataCreateMutable(kCFAllocatorDefault, 0);
    if (data != nullptr) {
        CGImageDestinationRef destination =
            CGImageDestinationCreateWithData(data, CFSTR("public.png"), 1, nullptr);
        if (destination != nullptr) {
            CGImageDestinationAddImage(destination, image, nullptr);
            if (CGImageDestinationFinalize(destination)) {
                const CFIndex length = CFDataGetLength(data);
                const UInt8* bytes = CFDataGetBytePtr(data);
                if (length > 0 && bytes != nullptr) {
                    png.assign(bytes, bytes + static_cast<std::size_t>(length));
                    encoded = true;
                } else {
                    error = "CGImageDestination produced no bytes";
                }
            } else {
                error = "CGImageDestinationFinalize failed";
            }
            CFRelease(destination);
        } else {
            error = "CGImageDestinationCreateWithData(public.png) failed";
        }
        CFRelease(data);
    } else {
        error = "CFDataCreateMutable failed";
    }

    CGImageRelease(image);
    CGColorSpaceRelease(color_space);
    CGDataProviderRelease(provider);
    return encoded;
}

class MetalCaptureImpl final : public MetalCapture {
public:
    MetalCaptureImpl(id<MTLDevice> device, id<MTLCommandQueue> queue,
                     id<MTLRenderPipelineState> pipeline, std::uint32_t width,
                     std::uint32_t height)
        // `device` is autoreleased (+0); the queue and pipeline come from `new...` (+1) and their
        // ownership is transferred to this object, so they are not retained a second time.
        : device_([device retain]),
          queue_(queue),
          pipeline_(pipeline),
          width_(width),
          height_(height) {}

    ~MetalCaptureImpl() override {
        [pipeline_ release];
        [queue_ release];
        [device_ release];
    }

    MetalCaptureResult capture(std::uint32_t frame_num) override {
        MetalCaptureResult result;
        result.width = width_;
        result.height = height_;
        result.stride = width_ * 4;
        result.frame_num = frame_num;

        @autoreleasepool {
            MTLTextureDescriptor* descriptor = [MTLTextureDescriptor
                texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
                                             width:width_
                                            height:height_
                                         mipmapped:NO];
            descriptor.usage = MTLTextureUsageRenderTarget;
            descriptor.storageMode = MTLStorageModeShared;
            id<MTLTexture> texture = [device_ newTextureWithDescriptor:descriptor];
            if (texture == nil) {
                result.error = "capture texture creation failed for " +
                               capture_size_text(width_, height_);
                return result;
            }

            MTLRenderPassDescriptor* pass = [MTLRenderPassDescriptor renderPassDescriptor];
            pass.colorAttachments[0].texture = texture;
            pass.colorAttachments[0].loadAction = MTLLoadActionClear;
            pass.colorAttachments[0].storeAction = MTLStoreActionStore;
            pass.colorAttachments[0].clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 1.0);

            id<MTLCommandBuffer> command_buffer = [queue_ commandBuffer];
            id<MTLRenderCommandEncoder> encoder =
                command_buffer == nil
                    ? nil
                    : [command_buffer renderCommandEncoderWithDescriptor:pass];
            if (command_buffer == nil || encoder == nil) {
                [texture release];
                result.error = "Metal command buffer/encoder creation failed";
                return result;
            }

            [encoder setRenderPipelineState:pipeline_];
            std::uint32_t uniforms = frame_num;
            [encoder setFragmentBytes:&uniforms length:sizeof(uniforms) atIndex:0];
            [encoder drawPrimitives:MTLPrimitiveTypeTriangleStrip vertexStart:0 vertexCount:4];
            [encoder endEncoding];
            [command_buffer commit];
            [command_buffer waitUntilCompleted];

            if (command_buffer.status != MTLCommandBufferStatusCompleted ||
                command_buffer.error != nil) {
                result.error = "Metal command buffer failed (status " +
                               std::to_string(static_cast<int>(command_buffer.status)) + "): " +
                               describe_ns_error(command_buffer.error);
                [texture release];
                return result;
            }

            const std::size_t tight_row = static_cast<std::size_t>(width_) * 4;
            const std::size_t read_row =
                (tight_row + (kReadbackRowAlignment - 1)) & ~(kReadbackRowAlignment - 1);
            std::vector<std::uint8_t> bgra(read_row * static_cast<std::size_t>(height_));
            [texture getBytes:bgra.data()
                  bytesPerRow:read_row
                   fromRegion:MTLRegionMake2D(0, 0, width_, height_)
                  mipmapLevel:0];
            [texture release];

            result.rgba8.resize(tight_row * static_cast<std::size_t>(height_));
            for (std::uint32_t y = 0; y < height_; ++y) {
                const std::uint8_t* source = bgra.data() + y * read_row;
                std::uint8_t* dest = result.rgba8.data() + y * tight_row;
                for (std::uint32_t x = 0; x < width_; ++x) {
                    // bgra8Unorm stores B,G,R,A; the interface promises R,G,B,A.
                    dest[x * 4 + 0] = source[x * 4 + 2];
                    dest[x * 4 + 1] = source[x * 4 + 1];
                    dest[x * 4 + 2] = source[x * 4 + 0];
                    dest[x * 4 + 3] = source[x * 4 + 3];
                }
            }

            std::string png_error;
            if (!encode_png_rgba8(result.rgba8, width_, height_, result.png, png_error)) {
                result.error = "PNG encoding failed: " + png_error;
                return result;
            }
        }

        result.ok = true;
        return result;
    }

private:
    id<MTLDevice> device_;
    id<MTLCommandQueue> queue_;
    id<MTLRenderPipelineState> pipeline_;
    std::uint32_t width_;
    std::uint32_t height_;
};

}  // namespace

bool metal_encode_rgba8_png(const std::vector<std::uint8_t>& rgba8, std::uint32_t width,
                            std::uint32_t height, std::vector<std::uint8_t>& png,
                            std::string& error) {
    return encode_png_rgba8(rgba8, width, height, png, error);
}

std::unique_ptr<MetalCapture> MetalCapture::create(const MetalCaptureConfig& config,
                                                   std::string& error) {
    error.clear();
    if (config.width == 0 || config.height == 0) {
        error = "invalid capture size";
        return nullptr;
    }

    @autoreleasepool {
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if (device == nil) {
            error = kMetalCaptureNoDeviceError;
            return nullptr;
        }
        id<MTLCommandQueue> queue = [device newCommandQueue];
        if (queue == nil) {
            error = "Metal command queue creation failed";
            return nullptr;
        }

        NSError* ns_error = nil;
        id<MTLLibrary> library = [device newLibraryWithSource:kCaptureMSLSource
                                                      options:nil
                                                        error:&ns_error];
        if (library == nil) {
            [queue release];
            error = "Metal library compilation failed: " + describe_ns_error(ns_error);
            return nullptr;
        }

        id<MTLFunction> vertex_function = [library newFunctionWithName:@"capture_vertex"];
        id<MTLFunction> fragment_function = [library newFunctionWithName:@"capture_fragment"];
        if (vertex_function == nil || fragment_function == nil) {
            [vertex_function release];
            [fragment_function release];
            [library release];
            [queue release];
            error = "Metal capture shader functions not found";
            return nullptr;
        }

        MTLRenderPipelineDescriptor* pipeline_descriptor =
            [[MTLRenderPipelineDescriptor alloc] init];
        pipeline_descriptor.vertexFunction = vertex_function;
        pipeline_descriptor.fragmentFunction = fragment_function;
        pipeline_descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

        ns_error = nil;
        id<MTLRenderPipelineState> pipeline =
            [device newRenderPipelineStateWithDescriptor:pipeline_descriptor error:&ns_error];
        [pipeline_descriptor release];
        [vertex_function release];
        [fragment_function release];
        [library release];
        if (pipeline == nil) {
            [queue release];
            error = "Metal render pipeline creation failed: " + describe_ns_error(ns_error);
            return nullptr;
        }

        return std::unique_ptr<MetalCapture>(
            new MetalCaptureImpl(device, queue, pipeline, config.width, config.height));
    }
}

}  // namespace sotc
