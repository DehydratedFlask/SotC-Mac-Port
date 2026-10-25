// Offline test harness: compile supplied private MSL and compare its numeric readback externally.
// No original shader decoding, runtime guest GPU, game data or fallback shader is included.
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#include <cstdio>
#include <cstring>

static int fail(NSString* text) {
    fprintf(stderr,"FAILED: %s\n",[text UTF8String]); return 1;
}
int main(int argc,char** argv) {
    @autoreleasepool {
        if(argc<4 || argc>5) return fail(@"usage: metal_shader_verify private.metal inputs.bin outputs.bin [offscreen.rgba32f]");
        NSError* error=nil;
        NSString* source=[NSString stringWithContentsOfFile:[NSString stringWithUTF8String:argv[1]] encoding:NSUTF8StringEncoding error:&error];
        if(!source) return fail([error description]);
        NSData* data=[NSData dataWithContentsOfFile:[NSString stringWithUTF8String:argv[2]]];
        constexpr NSUInteger caseBytes=18*4*sizeof(float), outputBytes=4*4*sizeof(float);
        if(!data || [data length]==0 || [data length]%caseBytes) return fail(@"input must contain complete explicit ShaderCase records");
        NSUInteger count=[data length]/caseBytes;
        id<MTLDevice> device=MTLCreateSystemDefaultDevice();
        if(!device) return fail(@"no Metal device");
        MTLCompileOptions* options=[MTLCompileOptions new];
        options.fastMathEnabled=NO;
        options.languageVersion=MTLLanguageVersion3_1;
        id<MTLLibrary> library=[device newLibraryWithSource:source options:options error:&error];
        if(!library) return fail([error description]);
        id<MTLFunction> fn=[library newFunctionWithName:@"verify_pair"];
        if(!fn) return fail(@"verify_pair function missing");
        id<MTLComputePipelineState> compute=[device newComputePipelineStateWithFunction:fn error:&error];
        if(!compute) return fail([error description]);
        id<MTLCommandQueue> queue=[device newCommandQueue];
        id<MTLBuffer> input=[device newBufferWithBytes:[data bytes] length:[data length] options:MTLResourceStorageModeShared];
        id<MTLBuffer> output=[device newBufferWithLength:count*outputBytes options:MTLResourceStorageModeShared];
        if(!queue || !input || !output) return fail(@"Metal queue/buffer allocation failed");
        memset([output contents],0xa5,count*outputBytes);
        id<MTLCommandBuffer> command=[queue commandBuffer];
        id<MTLComputeCommandEncoder> encoder=[command computeCommandEncoder];
        [encoder setComputePipelineState:compute];
        [encoder setBuffer:input offset:0 atIndex:0]; [encoder setBuffer:output offset:0 atIndex:1];
        NSUInteger group=MIN((NSUInteger)32,compute.maxTotalThreadsPerThreadgroup);
        [encoder dispatchThreads:MTLSizeMake(count,1,1) threadsPerThreadgroup:MTLSizeMake(group,1,1)];
        [encoder endEncoding]; [command commit]; [command waitUntilCompleted];
        if(command.status!=MTLCommandBufferStatusCompleted) return fail([command.error description]);
        NSData* out=[NSData dataWithBytes:[output contents] length:count*outputBytes];
        if(![out writeToFile:[NSString stringWithUTF8String:argv[3]] atomically:YES]) return fail(@"output write failed");
        if(argc==5) {
            if(count<3) return fail(@"offscreen render requires at least three case records");
            MTLRenderPipelineDescriptor* pd=[MTLRenderPipelineDescriptor new];
            pd.vertexFunction=[library newFunctionWithName:@"asset_vertex"];
            pd.fragmentFunction=[library newFunctionWithName:@"asset_fragment"];
            pd.colorAttachments[0].pixelFormat=MTLPixelFormatRGBA32Float;
            id<MTLRenderPipelineState> render=[device newRenderPipelineStateWithDescriptor:pd error:&error];
            if(!render) return fail([error description]);
            MTLTextureDescriptor* td=[MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA32Float width:128 height:128 mipmapped:NO];
            td.usage=MTLTextureUsageRenderTarget; td.storageMode=MTLStorageModeShared;
            id<MTLTexture> texture=[device newTextureWithDescriptor:td];
            if(!texture) return fail(@"offscreen texture allocation failed");
            MTLRenderPassDescriptor* pass=[MTLRenderPassDescriptor renderPassDescriptor];
            pass.colorAttachments[0].texture=texture;
            pass.colorAttachments[0].loadAction=MTLLoadActionClear;
            pass.colorAttachments[0].storeAction=MTLStoreActionStore;
            pass.colorAttachments[0].clearColor=MTLClearColorMake(0,0,0,0);
            id<MTLCommandBuffer> cb=[queue commandBuffer];
            id<MTLRenderCommandEncoder> re=[cb renderCommandEncoderWithDescriptor:pass];
            [re setRenderPipelineState:render]; [re setCullMode:MTLCullModeNone];
            [re setVertexBuffer:input offset:0 atIndex:0]; [re setFragmentBuffer:input offset:0 atIndex:0];
            [re drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
            [re endEncoding]; [cb commit]; [cb waitUntilCompleted];
            if(cb.status!=MTLCommandBufferStatusCompleted) return fail([cb.error description]);
            NSMutableData* pixels=[NSMutableData dataWithLength:128*128*4*sizeof(float)];
            [texture getBytes:[pixels mutableBytes] bytesPerRow:128*4*sizeof(float) fromRegion:MTLRegionMake2D(0,0,128,128) mipmapLevel:0];
            if(![pixels writeToFile:[NSString stringWithUTF8String:argv[4]] atomically:YES]) return fail(@"offscreen write failed");
        }
        printf("PASSED: device=%s cases=%lu library=compiled compute=completed offscreen=%s fastMath=NO\n",
               [[device name] UTF8String],(unsigned long)count,argc==5?"completed":"NOT RUN");
        return 0;
    }
}
