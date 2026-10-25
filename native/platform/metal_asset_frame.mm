// Direct-Metal executor for the first typed indexed-mesh RenderOp subset.
// This is an explicitly supplied harness ABI; it is not the recovered retail resource binding.
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

#include "platform/metal_asset_frame.hpp"

#include "platform/metal_capture.hpp"
#include "platform/render_ops.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace sotc {
namespace {

constexpr RenderResourceHandle kColorTargetHandle = 1;
constexpr RenderResourceHandle kDepthTargetHandle = 2;
constexpr RenderResourceHandle kPipelineHandle = 3;
constexpr RenderResourceHandle kVertexBufferHandle = 4;
constexpr RenderResourceHandle kIndexBufferHandle = 5;
constexpr RenderResourceHandle kFirstTextureHandle = 6;
constexpr std::size_t kDepthCompareCount = 5;
constexpr std::size_t kDepthStateCount = 2 * 2 * kDepthCompareCount;
constexpr std::size_t kReadbackRowAlignment = 256;

struct DrawBinding {
    std::size_t vertex_offset = 0;
    std::size_t index_offset = 0;
    std::uint32_t index_count = 0;
    RenderResourceHandle texture_handle = kInvalidRenderResource;
    RenderResourceHandle sampler_handle = kInvalidRenderResource;
};

struct TextureInputBinding {
    RenderResourceHandle texture_handle = kInvalidRenderResource;
    RenderResourceHandle sampler_handle = kInvalidRenderResource;
    AssetTexture2D texture;
    AssetSamplerInput sampler;
};

struct TextureObjects {
    std::unordered_map<RenderResourceHandle, id<MTLTexture>> textures;
    std::unordered_map<RenderResourceHandle, id<MTLSamplerState>> samplers;

    ~TextureObjects() {
        for (const auto& [handle, texture] : textures) {
            (void)handle;
            [texture release];
        }
        for (const auto& [handle, sampler] : samplers) {
            (void)handle;
            [sampler release];
        }
    }
};

[[nodiscard]] bool valid_texture_filter(AssetTextureFilter filter) noexcept {
    return filter == AssetTextureFilter::nearest || filter == AssetTextureFilter::linear;
}

[[nodiscard]] bool valid_texture_address_mode(AssetTextureAddressMode mode) noexcept {
    return mode == AssetTextureAddressMode::clamp_to_edge ||
           mode == AssetTextureAddressMode::repeat ||
           mode == AssetTextureAddressMode::mirror_repeat;
}

[[nodiscard]] bool valid_texture_input(const AssetTexture2D& texture) noexcept {
    constexpr std::uint32_t kMaxTextureDimension = 8192;
    if (texture.width == 0 || texture.height == 0 ||
        texture.width > kMaxTextureDimension || texture.height > kMaxTextureDimension) {
        return false;
    }
    const std::size_t row_bytes = static_cast<std::size_t>(texture.width) * 4;
    if (texture.height > std::numeric_limits<std::size_t>::max() / row_bytes) return false;
    return texture.rgba8.data() != nullptr &&
           texture.rgba8.size() == row_bytes * texture.height;
}

[[nodiscard]] MTLSamplerMinMagFilter metal_filter(AssetTextureFilter filter) noexcept {
    return filter == AssetTextureFilter::linear ? MTLSamplerMinMagFilterLinear
                                                : MTLSamplerMinMagFilterNearest;
}

[[nodiscard]] MTLSamplerAddressMode metal_address_mode(AssetTextureAddressMode mode) noexcept {
    switch (mode) {
        case AssetTextureAddressMode::clamp_to_edge: return MTLSamplerAddressModeClampToEdge;
        case AssetTextureAddressMode::repeat: return MTLSamplerAddressModeRepeat;
        case AssetTextureAddressMode::mirror_repeat: return MTLSamplerAddressModeMirrorRepeat;
    }
    return MTLSamplerAddressModeClampToEdge;
}

[[nodiscard]] bool make_texture_objects(id<MTLDevice> device,
                                        const std::vector<TextureInputBinding>& inputs,
                                        TextureObjects& objects, std::string& error) {
    @autoreleasepool {
        for (const TextureInputBinding& input : inputs) {
            MTLTextureDescriptor* texture_descriptor = [MTLTextureDescriptor
                texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                             width:input.texture.width
                                            height:input.texture.height
                                         mipmapped:NO];
            texture_descriptor.usage = MTLTextureUsageShaderRead;
            texture_descriptor.storageMode = MTLStorageModeShared;
            id<MTLTexture> texture = [device newTextureWithDescriptor:texture_descriptor];
            if (texture == nil) {
                error = "Metal fragment texture creation failed";
                return false;
            }
            const std::size_t bytes_per_row = static_cast<std::size_t>(input.texture.width) * 4;
            [texture replaceRegion:MTLRegionMake2D(0, 0, input.texture.width, input.texture.height)
                       mipmapLevel:0
                         withBytes:input.texture.rgba8.data()
                       bytesPerRow:bytes_per_row];
            objects.textures.emplace(input.texture_handle, texture);

            MTLSamplerDescriptor* sampler_descriptor = [[MTLSamplerDescriptor alloc] init];
            sampler_descriptor.minFilter = metal_filter(input.sampler.min_filter);
            sampler_descriptor.magFilter = metal_filter(input.sampler.mag_filter);
            sampler_descriptor.mipFilter = MTLSamplerMipFilterNotMipmapped;
            sampler_descriptor.sAddressMode = metal_address_mode(input.sampler.address_u);
            sampler_descriptor.tAddressMode = metal_address_mode(input.sampler.address_v);
            sampler_descriptor.rAddressMode = MTLSamplerAddressModeClampToEdge;
            sampler_descriptor.lodMinClamp = 0.0F;
            sampler_descriptor.lodMaxClamp = 0.0F;
            id<MTLSamplerState> sampler = [device newSamplerStateWithDescriptor:sampler_descriptor];
            [sampler_descriptor release];
            if (sampler == nil) {
                error = "Metal fragment sampler creation failed";
                return false;
            }
            objects.samplers.emplace(input.sampler_handle, sampler);
        }
    }
    return true;
}

[[nodiscard]] std::string describe_ns_error(NSError* error) {
    if (error == nil) return "unknown error";
    const char* text = [[error localizedDescription] UTF8String];
    return text == nullptr ? std::string("unknown error") : std::string(text);
}

[[nodiscard]] MetalAssetFrameResult frame_error(std::uint32_t width, std::uint32_t height,
                                                std::string error) {
    MetalAssetFrameResult result;
    result.width = width;
    result.height = height;
    result.stride = width * 4;
    result.error = std::move(error);
    return result;
}

[[nodiscard]] bool finite_shader_case(const DefaultShaderCase& shader) noexcept {
    const std::array<const ShaderFloat4*, 18> fields = {
        &shader.position,      &shader.encoded_normal, &shader.object_row0,
        &shader.object_row1,   &shader.object_row2,    &shader.clip_row0,
        &shader.clip_row1,     &shader.clip_row2,      &shader.clip_row3,
        &shader.camera,        &shader.material,       &shader.param0_p0,
        &shader.param0_p10,    &shader.param0_p20,     &shader.param1_p0,
        &shader.param1_p10,    &shader.param1_p20,     &shader.barycentric,
    };
    for (const ShaderFloat4* field : fields) {
        for (float value : *field) {
            if (!std::isfinite(value)) return false;
        }
    }
    return true;
}

[[nodiscard]] MTLCompareFunction metal_compare(DepthCompare compare) noexcept {
    switch (compare) {
        case DepthCompare::never: return MTLCompareFunctionNever;
        case DepthCompare::less: return MTLCompareFunctionLess;
        case DepthCompare::less_equal: return MTLCompareFunctionLessEqual;
        case DepthCompare::greater: return MTLCompareFunctionGreater;
        case DepthCompare::always: return MTLCompareFunctionAlways;
    }
    return MTLCompareFunctionNever;
}

[[nodiscard]] std::size_t depth_state_index(const SetDepthState& state) noexcept {
    const std::size_t enabled = state.enabled ? 1 : 0;
    const std::size_t write = state.write_enabled ? 1 : 0;
    return (enabled * 2 + write) * kDepthCompareCount + static_cast<std::size_t>(state.compare);
}

class MetalAssetFrameRendererImpl final : public MetalAssetFrameRenderer {
public:
    MetalAssetFrameRendererImpl(id<MTLDevice> device, id<MTLCommandQueue> queue,
                                id<MTLRenderPipelineState> pipeline,
                                const MetalAssetFrameConfig& config)
        : device_([device retain]), queue_(queue), pipeline_(pipeline), config_(config) {}

    ~MetalAssetFrameRendererImpl() override {
        for (id<MTLDepthStencilState> state : depth_states_) {
            [state release];
        }
        [pipeline_ release];
        [queue_ release];
        [device_ release];
    }

    MetalAssetFrameResult render(std::span<const AssetMeshDraw> draws) override {
        if (draws.empty()) return frame_error(config_.width, config_.height, "no mesh draws supplied");

        std::vector<DefaultShaderCase> vertex_cases;
        std::vector<std::uint16_t> index_data;
        std::vector<DrawBinding> bindings;
        std::vector<TextureInputBinding> texture_inputs;
        bindings.reserve(draws.size());
        texture_inputs.reserve(draws.size());

        for (std::size_t draw_index = 0; draw_index < draws.size(); ++draw_index) {
            const AssetMeshDraw& draw = draws[draw_index];
            if (draw.positions.empty() || draw.indices.empty()) {
                return frame_error(config_.width, config_.height,
                                   "draw " + std::to_string(draw_index) +
                                       " has empty positions or indices");
            }
            if (draw.indices.size() % 3 != 0 ||
                draw.indices.size() > std::numeric_limits<std::uint32_t>::max()) {
                return frame_error(config_.width, config_.height,
                                   "draw " + std::to_string(draw_index) +
                                       " index count is not a U16 triangle list");
            }
            if (!finite_shader_case(draw.shader)) {
                return frame_error(config_.width, config_.height,
                                   "draw " + std::to_string(draw_index) +
                                       " shader case contains a non-finite value");
            }
            const bool has_texture = draw.fragment_texture0.has_value();
            const bool has_sampler = draw.fragment_sampler0.has_value();
            if (has_texture != has_sampler ||
                has_texture != config_.fragment_texture_sampler_slot0) {
                return frame_error(config_.width, config_.height,
                                   "draw fragment texture/sampler inputs do not match the explicit slot-0 ABI");
            }
            if (has_texture && !valid_texture_input(*draw.fragment_texture0)) {
                return frame_error(config_.width, config_.height,
                                   "draw fragment texture must have exact nonempty RGBA8 2D bytes");
            }
            if (has_sampler &&
                (!valid_texture_filter(draw.fragment_sampler0->min_filter) ||
                 !valid_texture_filter(draw.fragment_sampler0->mag_filter) ||
                 !valid_texture_address_mode(draw.fragment_sampler0->address_u) ||
                 !valid_texture_address_mode(draw.fragment_sampler0->address_v))) {
                return frame_error(config_.width, config_.height,
                                   "draw fragment sampler contains an unsupported filter or address mode");
            }
            for (std::uint16_t index : draw.indices) {
                if (static_cast<std::size_t>(index) >= draw.positions.size()) {
                    return frame_error(config_.width, config_.height,
                                       "draw " + std::to_string(draw_index) +
                                           " index refers past its position array");
                }
            }
            const std::size_t max_vertex_cases =
                std::numeric_limits<std::size_t>::max() / sizeof(DefaultShaderCase);
            if (vertex_cases.size() > max_vertex_cases ||
                draw.positions.size() > max_vertex_cases - vertex_cases.size()) {
                return frame_error(config_.width, config_.height, "vertex buffer size overflow");
            }
            const std::size_t max_indices =
                std::numeric_limits<std::size_t>::max() / sizeof(std::uint16_t);
            if (index_data.size() > max_indices ||
                draw.indices.size() > max_indices - index_data.size()) {
                return frame_error(config_.width, config_.height, "index buffer size overflow");
            }

            DrawBinding binding;
            binding.vertex_offset = vertex_cases.size() * sizeof(DefaultShaderCase);
            binding.index_offset = index_data.size() * sizeof(std::uint16_t);
            binding.index_count = static_cast<std::uint32_t>(draw.indices.size());
            if (has_texture) {
                if (draw_index >
                    (std::numeric_limits<RenderResourceHandle>::max() - kFirstTextureHandle) / 2) {
                    return frame_error(config_.width, config_.height,
                                       "fragment resource handle range overflow");
                }
                binding.texture_handle = kFirstTextureHandle +
                    static_cast<RenderResourceHandle>(draw_index * 2);
                binding.sampler_handle = binding.texture_handle + 1;
                texture_inputs.push_back({binding.texture_handle, binding.sampler_handle,
                                          *draw.fragment_texture0, *draw.fragment_sampler0});
            }
            bindings.push_back(binding);

            for (const CmshPosition& position : draw.positions) {
                if (!std::isfinite(position.x) || !std::isfinite(position.y) ||
                    !std::isfinite(position.z)) {
                    return frame_error(config_.width, config_.height,
                                       "mesh position contains a non-finite value");
                }
                DefaultShaderCase vertex = draw.shader;
                vertex.position = {position.x, position.y, position.z, 0.0F};
                vertex_cases.push_back(vertex);
            }
            index_data.insert(index_data.end(), draw.indices.begin(), draw.indices.end());
        }

        if (vertex_cases.size() > std::numeric_limits<std::size_t>::max() /
                                      sizeof(DefaultShaderCase) ||
            index_data.size() > std::numeric_limits<std::size_t>::max() /
                                    sizeof(std::uint16_t)) {
            return frame_error(config_.width, config_.height, "GPU buffer size overflow");
        }

        const std::size_t vertex_bytes = vertex_cases.size() * sizeof(DefaultShaderCase);
        const std::size_t index_bytes = index_data.size() * sizeof(std::uint16_t);
        if (vertex_bytes == 0 || index_bytes == 0) {
            return frame_error(config_.width, config_.height, "GPU buffer is empty");
        }

        id<MTLBuffer> vertex_buffer = [device_ newBufferWithBytes:vertex_cases.data()
                                                            length:vertex_bytes
                                                           options:MTLResourceStorageModeShared];
        id<MTLBuffer> index_buffer = [device_ newBufferWithBytes:index_data.data()
                                                           length:index_bytes
                                                          options:MTLResourceStorageModeShared];
        if (vertex_buffer == nil || index_buffer == nil) {
            [vertex_buffer release];
            [index_buffer release];
            return frame_error(config_.width, config_.height, "Metal vertex/index buffer creation failed");
        }

        TextureObjects texture_objects;
        std::string texture_error;
        if (!make_texture_objects(device_, texture_inputs, texture_objects, texture_error)) {
            [vertex_buffer release];
            [index_buffer release];
            return frame_error(config_.width, config_.height, std::move(texture_error));
        }

        MTLTextureDescriptor* color_descriptor = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
                                         width:config_.width
                                        height:config_.height
                                     mipmapped:NO];
        color_descriptor.usage = MTLTextureUsageRenderTarget;
        color_descriptor.storageMode = MTLStorageModeShared;
        id<MTLTexture> color_texture = [device_ newTextureWithDescriptor:color_descriptor];

        MTLTextureDescriptor* depth_descriptor = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                                         width:config_.width
                                        height:config_.height
                                     mipmapped:NO];
        depth_descriptor.usage = MTLTextureUsageRenderTarget;
        depth_descriptor.storageMode = MTLStorageModePrivate;
        id<MTLTexture> depth_texture = [device_ newTextureWithDescriptor:depth_descriptor];
        if (color_texture == nil || depth_texture == nil) {
            [color_texture release];
            [depth_texture release];
            [vertex_buffer release];
            [index_buffer release];
            return frame_error(config_.width, config_.height, "Metal color/depth target creation failed");
        }

        std::vector<RenderOp> operations;
        operations.reserve(5 + draws.size() *
                               (config_.fragment_texture_sampler_slot0 ? 6 : 4));
        operations.emplace_back(BeginPass{kColorTargetHandle, kDepthTargetHandle,
                                          {0.0F, 0.0F, 0.0F, 1.0F}, 1.0F});
        operations.emplace_back(SetViewport{0.0F, 0.0F, static_cast<float>(config_.width),
                                            static_cast<float>(config_.height), 0.0F, 1.0F});
        operations.emplace_back(SetDepthState{true, true, DepthCompare::less});
        operations.emplace_back(SetPipeline{kPipelineHandle});
        for (const DrawBinding& binding : bindings) {
            operations.emplace_back(BindVertexStream{kVertexBufferHandle, 0, binding.vertex_offset,
                                                     sizeof(DefaultShaderCase)});
            operations.emplace_back(BindConstants{RenderStage::fragment, kVertexBufferHandle, 0,
                                                  binding.vertex_offset, sizeof(DefaultShaderCase)});
            if (binding.texture_handle != kInvalidRenderResource) {
                operations.emplace_back(BindTexture{RenderStage::fragment,
                                                    binding.texture_handle, 0});
                operations.emplace_back(BindSampler{RenderStage::fragment,
                                                    binding.sampler_handle, 0});
            }
            operations.emplace_back(BindIndexStream{kIndexBufferHandle, binding.index_offset});
            operations.emplace_back(DrawIndexed{binding.index_count, 0});
        }
        operations.emplace_back(EndPass{});

        const RenderOpValidation validation = validate_render_ops(operations);
        if (!validation.ok) {
            [color_texture release];
            [depth_texture release];
            [vertex_buffer release];
            [index_buffer release];
            return frame_error(config_.width, config_.height,
                               "RenderOp validation failed at operation " +
                                   std::to_string(validation.operation_index) + ": " + validation.error);
        }

        MetalAssetFrameResult result;
        result.width = config_.width;
        result.height = config_.height;
        result.stride = config_.width * 4;

        @autoreleasepool {
            id<MTLCommandBuffer> command_buffer = [queue_ commandBuffer];
            if (command_buffer == nil) {
                result.error = "Metal command buffer creation failed";
            } else {
                id<MTLRenderCommandEncoder> encoder = nil;
                id<MTLDepthStencilState> active_depth_state = nil;
                std::size_t bound_index_offset = 0;
                bool index_bound = false;

                for (std::size_t operation_index = 0; operation_index < operations.size();
                     ++operation_index) {
                    const RenderOp& operation = operations[operation_index];

                    if (const auto* begin = std::get_if<BeginPass>(&operation)) {
                        if (begin->color_target != kColorTargetHandle ||
                            begin->depth_target != kDepthTargetHandle || encoder != nil) {
                            result.error = "BeginPass references an unknown target or nested encoder";
                            break;
                        }
                        MTLRenderPassDescriptor* pass = [MTLRenderPassDescriptor renderPassDescriptor];
                        pass.colorAttachments[0].texture = color_texture;
                        pass.colorAttachments[0].loadAction = MTLLoadActionClear;
                        pass.colorAttachments[0].storeAction = MTLStoreActionStore;
                        pass.colorAttachments[0].clearColor =
                            MTLClearColorMake(begin->clear_color[0], begin->clear_color[1],
                                              begin->clear_color[2], begin->clear_color[3]);
                        pass.depthAttachment.texture = depth_texture;
                        pass.depthAttachment.loadAction = MTLLoadActionClear;
                        pass.depthAttachment.storeAction = MTLStoreActionDontCare;
                        pass.depthAttachment.clearDepth = begin->clear_depth;
                        encoder = [command_buffer renderCommandEncoderWithDescriptor:pass];
                        if (encoder == nil) result.error = "Metal render encoder creation failed";
                    } else if (std::holds_alternative<EndPass>(operation)) {
                        if (encoder == nil) {
                            result.error = "EndPass reached without an active Metal encoder";
                            break;
                        }
                        [encoder endEncoding];
                        encoder = nil;
                    } else if (const auto* viewport = std::get_if<SetViewport>(&operation)) {
                        if (encoder == nil || viewport->x < 0.0F || viewport->y < 0.0F ||
                            viewport->x + viewport->width > static_cast<float>(config_.width) ||
                            viewport->y + viewport->height > static_cast<float>(config_.height)) {
                            result.error = "SetViewport is outside the offscreen target";
                            break;
                        }
                        MTLViewport metal_viewport;
                        metal_viewport.originX = viewport->x;
                        metal_viewport.originY = viewport->y;
                        metal_viewport.width = viewport->width;
                        metal_viewport.height = viewport->height;
                        metal_viewport.znear = viewport->min_depth;
                        metal_viewport.zfar = viewport->max_depth;
                        [encoder setViewport:metal_viewport];
                    } else if (const auto* depth = std::get_if<SetDepthState>(&operation)) {
                        if (encoder == nil) {
                            result.error = "SetDepthState reached outside an active encoder";
                            break;
                        }
                        const std::size_t state_index = depth_state_index(*depth);
                        if (state_index >= depth_states_.size()) {
                            result.error = "SetDepthState has an invalid state index";
                            break;
                        }
                        if (depth_states_[state_index] == nil) {
                            MTLDepthStencilDescriptor* descriptor =
                                [[MTLDepthStencilDescriptor alloc] init];
                            descriptor.depthCompareFunction = depth->enabled
                                ? metal_compare(depth->compare)
                                : MTLCompareFunctionAlways;
                            descriptor.depthWriteEnabled = depth->write_enabled;
                            depth_states_[state_index] =
                                [device_ newDepthStencilStateWithDescriptor:descriptor];
                            [descriptor release];
                            if (depth_states_[state_index] == nil) {
                                result.error = "Metal depth-stencil state creation failed";
                                break;
                            }
                        }
                        active_depth_state = depth_states_[state_index];
                        [encoder setDepthStencilState:active_depth_state];
                    } else if (const auto* pipeline = std::get_if<SetPipeline>(&operation)) {
                        if (encoder == nil || pipeline->pipeline != kPipelineHandle) {
                            result.error = "SetPipeline references an unknown Metal pipeline";
                            break;
                        }
                        [encoder setRenderPipelineState:pipeline_];
                    } else if (const auto* vertex = std::get_if<BindVertexStream>(&operation)) {
                        if (encoder == nil || vertex->buffer != kVertexBufferHandle ||
                            vertex->slot != 0 ||
                            vertex->offset > vertex_buffer.length ||
                            vertex->stride > vertex_buffer.length - vertex->offset) {
                            result.error = "BindVertexStream references an unsupported slot or span";
                            break;
                        }
                        [encoder setVertexBuffer:vertex_buffer
                                          offset:vertex->offset
                                         atIndex:vertex->slot];
                    } else if (const auto* constants = std::get_if<BindConstants>(&operation)) {
                        if (encoder == nil || constants->stage != RenderStage::fragment ||
                            constants->buffer != kVertexBufferHandle || constants->slot != 0 ||
                            constants->size != sizeof(DefaultShaderCase) ||
                            constants->offset > vertex_buffer.length ||
                            constants->size > vertex_buffer.length - constants->offset) {
                            result.error = "BindConstants references an unsupported stage or span";
                            break;
                        }
                        [encoder setFragmentBuffer:vertex_buffer
                                            offset:constants->offset
                                           atIndex:constants->slot];
                    } else if (const auto* texture = std::get_if<BindTexture>(&operation)) {
                        const auto resource = texture_objects.textures.find(texture->texture);
                        if (encoder == nil || texture->stage != RenderStage::fragment ||
                            texture->slot != 0 || resource == texture_objects.textures.end()) {
                            result.error = "BindTexture references an unsupported stage, slot, or host handle";
                            break;
                        }
                        [encoder setFragmentTexture:resource->second atIndex:texture->slot];
                    } else if (const auto* sampler = std::get_if<BindSampler>(&operation)) {
                        const auto resource = texture_objects.samplers.find(sampler->sampler);
                        if (encoder == nil || sampler->stage != RenderStage::fragment ||
                            sampler->slot != 0 || resource == texture_objects.samplers.end()) {
                            result.error = "BindSampler references an unsupported stage, slot, or host handle";
                            break;
                        }
                        [encoder setFragmentSamplerState:resource->second atIndex:sampler->slot];
                    } else if (const auto* indices = std::get_if<BindIndexStream>(&operation)) {
                        if (encoder == nil || indices->buffer != kIndexBufferHandle ||
                            indices->offset > index_buffer.length) {
                            result.error = "BindIndexStream references an unknown buffer span";
                            break;
                        }
                        bound_index_offset = indices->offset;
                        index_bound = true;
                    } else if (const auto* draw = std::get_if<DrawIndexed>(&operation)) {
                        if (encoder == nil || !index_bound || draw->first_index >
                                std::numeric_limits<std::size_t>::max() / sizeof(std::uint16_t)) {
                            result.error = "DrawIndexed has no active encoder/index stream";
                            break;
                        }
                        const std::size_t first_byte =
                            draw->first_index * sizeof(std::uint16_t);
                        if (bound_index_offset > index_buffer.length ||
                            first_byte > index_buffer.length - bound_index_offset ||
                            static_cast<std::size_t>(draw->index_count) >
                                (index_buffer.length - bound_index_offset - first_byte) /
                                    sizeof(std::uint16_t)) {
                            result.error = "DrawIndexed index span exceeds its host buffer";
                            break;
                        }
                        [encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                            indexCount:draw->index_count
                                             indexType:MTLIndexTypeUInt16
                                           indexBuffer:index_buffer
                                     indexBufferOffset:bound_index_offset + first_byte];
                    } else {
                        const auto* unsupported = std::get_if<UnsupportedRenderOp>(&operation);
                        result.error = std::string(render_op_name(unsupported->kind)) +
                                       " is not implemented by the Metal executor";
                        break;
                    }

                    if (!result.error.empty()) {
                        result.error += " at operation " + std::to_string(operation_index);
                        break;
                    }
                }

                if (encoder != nil) {
                    [encoder endEncoding];
                    encoder = nil;
                }
                active_depth_state = nil;

                if (result.error.empty()) {
                    [command_buffer commit];
                    [command_buffer waitUntilCompleted];
                    if (command_buffer.status != MTLCommandBufferStatusCompleted ||
                        command_buffer.error != nil) {
                        result.error = "Metal command buffer failed (status " +
                                       std::to_string(static_cast<int>(command_buffer.status)) +
                                       "): " + describe_ns_error(command_buffer.error);
                    }
                }
            }
        }

        if (result.error.empty()) {
            const std::size_t tight_row = static_cast<std::size_t>(config_.width) * 4;
            if (tight_row > std::numeric_limits<std::size_t>::max() -
                                (kReadbackRowAlignment - 1)) {
                result.error = "readback row size overflow";
            } else {
                const std::size_t read_row =
                    (tight_row + (kReadbackRowAlignment - 1)) & ~(kReadbackRowAlignment - 1);
                if (config_.height > std::numeric_limits<std::size_t>::max() / read_row) {
                    result.error = "readback buffer size overflow";
                } else {
                    std::vector<std::uint8_t> bgra(read_row * config_.height);
                    [color_texture getBytes:bgra.data()
                                bytesPerRow:read_row
                                 fromRegion:MTLRegionMake2D(0, 0, config_.width, config_.height)
                                mipmapLevel:0];
                    result.rgba8.resize(tight_row * config_.height);
                    for (std::uint32_t y = 0; y < config_.height; ++y) {
                        const std::uint8_t* source = bgra.data() + y * read_row;
                        std::uint8_t* destination = result.rgba8.data() + y * tight_row;
                        for (std::uint32_t x = 0; x < config_.width; ++x) {
                            destination[x * 4 + 0] = source[x * 4 + 2];
                            destination[x * 4 + 1] = source[x * 4 + 1];
                            destination[x * 4 + 2] = source[x * 4 + 0];
                            destination[x * 4 + 3] = source[x * 4 + 3];
                        }
                    }
                    if (!metal_encode_rgba8_png(result.rgba8, config_.width, config_.height,
                                                result.png, result.error)) {
                        result.error = "PNG encoding failed: " + result.error;
                    }
                }
            }
        }

        [color_texture release];
        [depth_texture release];
        [vertex_buffer release];
        [index_buffer release];
        if (!result.error.empty()) return result;
        result.ok = true;
        return result;
    }

private:
    id<MTLDevice> device_;
    id<MTLCommandQueue> queue_;
    id<MTLRenderPipelineState> pipeline_;
    MetalAssetFrameConfig config_;
    std::array<id<MTLDepthStencilState>, kDepthStateCount> depth_states_{};
};

}  // namespace

MetalAssetFrameRenderer::~MetalAssetFrameRenderer() = default;

DefaultShaderCase make_default_shader_case() noexcept {
    DefaultShaderCase shader{};
    shader.position = {0.0F, 0.0F, 0.25F, 0.0F};
    shader.encoded_normal = {0.5F, 0.5F, 1.0F, 0.0F};
    shader.object_row0 = {1.0F, 0.0F, 0.0F, 0.0F};
    shader.object_row1 = {0.0F, 1.0F, 0.0F, 0.0F};
    shader.object_row2 = {0.0F, 0.0F, 1.0F, 0.0F};
    shader.clip_row0 = {1.0F, 0.0F, 0.0F, 0.0F};
    shader.clip_row1 = {0.0F, 1.0F, 0.0F, 0.0F};
    shader.clip_row2 = {0.0F, 0.0F, 1.0F, 0.0F};
    shader.clip_row3 = {0.0F, 0.0F, 0.0F, 1.0F};
    shader.camera = {0.0F, 0.0F, 4.0F, 0.0F};
    shader.material = {0.8F, 0.4F, 0.2F, 1.0F};
    return shader;
}

std::unique_ptr<MetalAssetFrameRenderer> MetalAssetFrameRenderer::create(
    const MetalAssetFrameConfig& config, std::string_view msl_source, std::string& error) {
    error.clear();
    if (config.width == 0 || config.height == 0 || config.width > 8192 || config.height > 8192) {
        error = "invalid asset frame size";
        return nullptr;
    }
    if (msl_source.empty()) {
        error = "empty MSL source";
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

        NSString* source = [[NSString alloc] initWithBytes:msl_source.data()
                                                    length:msl_source.size()
                                                  encoding:NSUTF8StringEncoding];
        if (source == nil) {
            [queue release];
            error = "MSL source is not valid UTF-8";
            return nullptr;
        }

        NSError* ns_error = nil;
        id<MTLLibrary> library = [device newLibraryWithSource:source options:nil error:&ns_error];
        [source release];
        if (library == nil) {
            [queue release];
            error = "Metal MSL compilation failed: " + describe_ns_error(ns_error);
            return nullptr;
        }
        id<MTLFunction> vertex = [library newFunctionWithName:@"asset_vertex"];
        id<MTLFunction> fragment = [library newFunctionWithName:@"asset_fragment"];
        [library release];
        if (vertex == nil || fragment == nil) {
            [vertex release];
            [fragment release];
            [queue release];
            error = "MSL library is missing asset_vertex or asset_fragment";
            return nullptr;
        }

        MTLRenderPipelineDescriptor* descriptor = [[MTLRenderPipelineDescriptor alloc] init];
        descriptor.vertexFunction = vertex;
        descriptor.fragmentFunction = fragment;
        descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        descriptor.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
        ns_error = nil;
        id<MTLRenderPipelineState> pipeline =
            [device newRenderPipelineStateWithDescriptor:descriptor error:&ns_error];
        [descriptor release];
        [vertex release];
        [fragment release];
        if (pipeline == nil) {
            [queue release];
            error = "Metal pipeline creation failed: " + describe_ns_error(ns_error);
            return nullptr;
        }

        auto* renderer = new MetalAssetFrameRendererImpl(device, queue, pipeline, config);
        return std::unique_ptr<MetalAssetFrameRenderer>(renderer);
    }
}

}  // namespace sotc
