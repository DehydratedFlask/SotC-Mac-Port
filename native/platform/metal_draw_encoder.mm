#include "metal_draw_encoder.hpp"

#import <Metal/Metal.h>

#include <array>
#include <cstring>
#include <string>
#include <vector>

namespace sotc {
namespace {

constexpr std::size_t kReadbackRowAlignment = 256;
constexpr std::uint32_t kIndicesPerBand = 4;

const char* kDrawMSLSource = R"MSL(
#include <metal_stdlib>
using namespace metal;

struct BandUniforms {
    float2 origin;   // normalized device origin (x, y)
    float2 size;     // normalized device size (x, y)
    float4 color;
};

struct VertexOut {
    float4 position [[position]];
};

vertex VertexOut band_vertex(uint vid [[vertex_id]],
                             constant BandUniforms& band [[buffer(0)]]) {
    // Indexed draw over a 4-entry index buffer: 0,1,2,3 -> strip corners.
    float2 corner = float2((vid == 1 || vid == 3) ? 1.0 : 0.0,
                           (vid >= 2) ? 1.0 : 0.0);
    float2 ndc = band.origin + corner * band.size;
    VertexOut out;
    out.position = float4(ndc, 0.0, 1.0);
    return out;
}

fragment float4 band_fragment(constant BandUniforms& band [[buffer(0)]]) {
    return band.color;
}
)MSL";

std::string describe_ns_error(NSError* error) {
    if (error == nil) {
        return "no error object";
    }
    NSString* description = [error localizedDescription];
    if (description == nil) {
        return "unnamed NSError";
    }
    return std::string([description UTF8String]);
}

std::array<float, 4> kind_color(pm4::DrawKind kind) {
    switch (kind) {
        case pm4::DrawKind::index_auto:
            return {1.0F, 0.0F, 0.0F, 1.0F};
        case pm4::DrawKind::index:
            return {0.0F, 1.0F, 0.0F, 1.0F};
        case pm4::DrawKind::index_offset:
            return {0.0F, 0.0F, 1.0F, 1.0F};
        case pm4::DrawKind::index_indirect:
            return {1.0F, 1.0F, 0.0F, 1.0F};
        case pm4::DrawKind::dispatch:
            return {1.0F, 0.0F, 1.0F, 1.0F};
        case pm4::DrawKind::dispatch_indirect:
            return {0.0F, 1.0F, 1.0F, 1.0F};
        case pm4::DrawKind::none:
            break;
    }
    return {0.0F, 0.0F, 0.0F, 1.0F};
}

struct BandUniforms {
    float origin[2];
    float size[2];
    float color[4];
};

}  // namespace

std::uint64_t metal_draw_digest(const std::vector<std::uint8_t>& rgba8) noexcept {
    std::uint64_t hash = 0xcbf29ce484222325ULL;
    for (const std::uint8_t byte : rgba8) {
        hash ^= byte;
        hash *= 0x100000001b3ULL;
    }
    return hash;
}

MetalDrawEncodeResult metal_encode_draw_list(const pm4::DrawCommand* draws, std::size_t count,
                                             MetalDrawEncoderConfig config) {
    MetalDrawEncodeResult result;
    result.width = config.width;
    result.height = config.height;
    result.stride = config.width * 4;
    result.draws_requested = static_cast<std::uint32_t>(count);

    if (draws == nullptr && count != 0) {
        result.error = "null draw list with non-zero count";
        return result;
    }
    if (config.width == 0 || config.height == 0) {
        result.error = "zero-sized render target";
        return result;
    }

    @autoreleasepool {
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if (device == nil) {
            result.error = kMetalDrawEncoderNoDeviceError;
            return result;
        }

        NSError* ns_error = nil;
        id<MTLLibrary> library = [device newLibraryWithSource:[NSString stringWithUTF8String:kDrawMSLSource]
                                                      options:nil
                                                        error:&ns_error];
        if (library == nil) {
            result.error = "Metal library compilation failed: " + describe_ns_error(ns_error);
            return result;
        }

        id<MTLFunction> vertex = [library newFunctionWithName:@"band_vertex"];
        id<MTLFunction> fragment = [library newFunctionWithName:@"band_fragment"];
        if (vertex == nil || fragment == nil) {
            [library release];
            result.error = "Metal shader function lookup failed";
            return result;
        }

        MTLRenderPipelineDescriptor* pipeline_descriptor = [[MTLRenderPipelineDescriptor alloc] init];
        pipeline_descriptor.vertexFunction = vertex;
        pipeline_descriptor.fragmentFunction = fragment;
        pipeline_descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        id<MTLRenderPipelineState> pipeline =
            [device newRenderPipelineStateWithDescriptor:pipeline_descriptor error:&ns_error];
        [pipeline_descriptor release];
        [vertex release];
        [fragment release];
        [library release];
        if (pipeline == nil) {
            result.error = "Metal pipeline creation failed: " + describe_ns_error(ns_error);
            return result;
        }

        MTLTextureDescriptor* texture_descriptor = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
                                         width:config.width
                                        height:config.height
                                     mipmapped:NO];
        texture_descriptor.usage = MTLTextureUsageRenderTarget;
        texture_descriptor.storageMode = MTLStorageModeShared;
        id<MTLTexture> texture = [device newTextureWithDescriptor:texture_descriptor];
        if (texture == nil) {
            [pipeline release];
            result.error = "Metal render target creation failed";
            return result;
        }

        const std::uint16_t indices[kIndicesPerBand] = {0, 1, 2, 3};
        id<MTLBuffer> index_buffer =
            [device newBufferWithBytes:indices
                                length:sizeof(indices)
                               options:MTLResourceStorageModeShared];
        if (index_buffer == nil) {
            [texture release];
            [pipeline release];
            result.error = "Metal index buffer creation failed";
            return result;
        }

        MTLRenderPassDescriptor* pass = [MTLRenderPassDescriptor renderPassDescriptor];
        pass.colorAttachments[0].texture = texture;
        pass.colorAttachments[0].loadAction = MTLLoadActionClear;
        pass.colorAttachments[0].storeAction = MTLStoreActionStore;
        pass.colorAttachments[0].clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 1.0);

        id<MTLCommandQueue> queue = [device newCommandQueue];
        id<MTLCommandBuffer> command_buffer = queue == nil ? nil : [queue commandBuffer];
        id<MTLRenderCommandEncoder> encoder =
            command_buffer == nil ? nil : [command_buffer renderCommandEncoderWithDescriptor:pass];
        if (encoder == nil) {
            [queue release];
            [index_buffer release];
            [texture release];
            [pipeline release];
            result.error = "Metal command buffer/encoder creation failed";
            return result;
        }

        [encoder setRenderPipelineState:pipeline];
        [encoder setVertexBuffer:index_buffer offset:0 atIndex:0];
        [encoder setFrontFacingWinding:MTLWindingClockwise];
        [encoder setCullMode:MTLCullModeNone];

        const float band_height = 2.0F / static_cast<float>(count == 0 ? 1 : count);
        for (std::size_t index = 0; index < count; ++index) {
            const pm4::DrawKind kind = draws[index].kind;
            if (kind == pm4::DrawKind::none) {
                ++result.draws_skipped;
                continue;
            }
            BandUniforms band{};
            band.origin[0] = -1.0F;
            band.origin[1] = 1.0F - band_height * static_cast<float>(index + 1);
            band.size[0] = 2.0F;
            band.size[1] = band_height;
            const std::array<float, 4> color = kind_color(kind);
            const float scale = draws[index].count == 0 ? 0.25F : 1.0F;
            for (int channel = 0; channel < 4; ++channel) {
                band.color[channel] = color[channel] * (channel == 3 ? 1.0F : scale);
            }
            [encoder setVertexBytes:&band length:sizeof(band) atIndex:0];
            [encoder setFragmentBytes:&band length:sizeof(band) atIndex:0];
            [encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangleStrip
                                indexCount:kIndicesPerBand
                                 indexType:MTLIndexTypeUInt16
                               indexBuffer:index_buffer
                         indexBufferOffset:0];
            ++result.draws_encoded;
        }
        [encoder endEncoding];
        [command_buffer commit];
        [command_buffer waitUntilCompleted];

        if (command_buffer.status != MTLCommandBufferStatusCompleted || command_buffer.error != nil) {
            result.error = "Metal command buffer failed (status " +
                           std::to_string(static_cast<int>(command_buffer.status)) + "): " +
                           describe_ns_error(command_buffer.error);
            [queue release];
            [index_buffer release];
            [texture release];
            [pipeline release];
            return result;
        }

        const std::size_t tight_row = static_cast<std::size_t>(config.width) * 4;
        const std::size_t read_row =
            (tight_row + (kReadbackRowAlignment - 1)) & ~(kReadbackRowAlignment - 1);
        std::vector<std::uint8_t> bgra(read_row * static_cast<std::size_t>(config.height));
        [texture getBytes:bgra.data()
              bytesPerRow:read_row
               fromRegion:MTLRegionMake2D(0, 0, config.width, config.height)
              mipmapLevel:0];

        [queue release];
        [index_buffer release];
        [texture release];
        [pipeline release];

        result.rgba8.resize(tight_row * static_cast<std::size_t>(config.height));
        for (std::uint32_t y = 0; y < config.height; ++y) {
            const std::uint8_t* source = bgra.data() + y * read_row;
            std::uint8_t* dest = result.rgba8.data() + y * tight_row;
            for (std::uint32_t x = 0; x < config.width; ++x) {
                dest[x * 4 + 0] = source[x * 4 + 2];
                dest[x * 4 + 1] = source[x * 4 + 1];
                dest[x * 4 + 2] = source[x * 4 + 0];
                dest[x * 4 + 3] = source[x * 4 + 3];
            }
        }
    }

    result.ok = true;
    return result;
}

}  // namespace sotc
