// Copyright 2019 The Dawn Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "tests/DawnTest.h"

#include "utils/ComboRenderPipelineDescriptor.h"
#include "utils/WGPUHelpers.h"

constexpr uint32_t kRTSize = 400;
constexpr uint32_t kBufferElementsCount = kMinUniformBufferOffsetAlignment / sizeof(uint32_t) + 2;
constexpr uint32_t kBufferSize = kBufferElementsCount * sizeof(uint32_t);
constexpr uint32_t kBindingSize = 8;

class DynamicBufferOffsetTests : public DawnTest {
  protected:
    void SetUp() override {
        DawnTest::SetUp();

        // Mix up dynamic and non dynamic resources in one bind group and using not continuous
        // binding number to cover more cases.
        std::array<uint32_t, kBufferElementsCount> uniformData = {0};
        uniformData[0] = 1;
        uniformData[1] = 2;

        mUniformBuffers[0] = utils::CreateBufferFromData(device, uniformData.data(), kBufferSize,
                                                         wgpu::BufferUsage::Uniform);

        uniformData[uniformData.size() - 2] = 5;
        uniformData[uniformData.size() - 1] = 6;

        // Dynamic uniform buffer
        mUniformBuffers[1] = utils::CreateBufferFromData(device, uniformData.data(), kBufferSize,
                                                         wgpu::BufferUsage::Uniform);

        wgpu::BufferDescriptor storageBufferDescriptor;
        storageBufferDescriptor.size = kBufferSize;
        storageBufferDescriptor.usage =
            wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::CopySrc;

        mStorageBuffers[0] = device.CreateBuffer(&storageBufferDescriptor);

        // Dynamic storage buffer
        mStorageBuffers[1] = device.CreateBuffer(&storageBufferDescriptor);

        // Default bind group layout
        mBindGroupLayouts[0] = utils::MakeBindGroupLayout(
            device, {{0, wgpu::ShaderStage::Compute | wgpu::ShaderStage::Fragment,
                      wgpu::BufferBindingType::Uniform},
                     {1, wgpu::ShaderStage::Compute | wgpu::ShaderStage::Fragment,
                      wgpu::BufferBindingType::Storage},
                     {3, wgpu::ShaderStage::Compute | wgpu::ShaderStage::Fragment,
                      wgpu::BufferBindingType::Uniform, true},
                     {4, wgpu::ShaderStage::Compute | wgpu::ShaderStage::Fragment,
                      wgpu::BufferBindingType::Storage, true}});

        // Default bind group
        mBindGroups[0] = utils::MakeBindGroup(device, mBindGroupLayouts[0],
                                              {{0, mUniformBuffers[0], 0, kBindingSize},
                                               {1, mStorageBuffers[0], 0, kBindingSize},
                                               {3, mUniformBuffers[1], 0, kBindingSize},
                                               {4, mStorageBuffers[1], 0, kBindingSize}});

        // Extra uniform buffer for inheriting test
        mUniformBuffers[2] = utils::CreateBufferFromData(device, uniformData.data(), kBufferSize,
                                                         wgpu::BufferUsage::Uniform);

        // Bind group layout for inheriting test
        mBindGroupLayouts[1] = utils::MakeBindGroupLayout(
            device, {{0, wgpu::ShaderStage::Compute | wgpu::ShaderStage::Fragment,
                      wgpu::BufferBindingType::Uniform}});

        // Bind group for inheriting test
        mBindGroups[1] = utils::MakeBindGroup(device, mBindGroupLayouts[1],
                                              {{0, mUniformBuffers[2], 0, kBindingSize}});
    }
    // Create objects to use as resources inside test bind groups.

    wgpu::BindGroup mBindGroups[2];
    wgpu::BindGroupLayout mBindGroupLayouts[2];
    wgpu::Buffer mUniformBuffers[3];
    wgpu::Buffer mStorageBuffers[2];
    wgpu::Texture mColorAttachment;

    wgpu::RenderPipeline CreateRenderPipeline(bool isInheritedPipeline = false) {
        wgpu::ShaderModule vsModule = utils::CreateShaderModule(device, R"(
            [[stage(vertex)]]
            fn main([[builtin(vertex_index)]] VertexIndex : u32) -> [[builtin(position)]] vec4<f32> {
                var pos = array<vec2<f32>, 3>(
                    vec2<f32>(-1.0, 0.0),
                    vec2<f32>(-1.0, 1.0),
                    vec2<f32>( 0.0, 1.0));
                return vec4<f32>(pos[VertexIndex], 0.0, 1.0);
            })");

        // Construct fragment shader source
        std::ostringstream fs;
        std::string multipleNumber = isInheritedPipeline ? "2" : "1";
        fs << R"(
            [[block]] struct Buf {
                value : vec2<u32>;
            };

            [[group(0), binding(0)]] var<uniform> uBufferNotDynamic : Buf;
            [[group(0), binding(1)]] var<storage, read_write> sBufferNotDynamic : Buf;
            [[group(0), binding(3)]] var<uniform> uBuffer : Buf;
            [[group(0), binding(4)]] var<storage, read_write> sBuffer : Buf;
        )";

        if (isInheritedPipeline) {
            fs << R"(
                [[group(1), binding(0)]] var<uniform> paddingBlock : Buf;
            )";
        }

        fs << "let multipleNumber : u32 = " << multipleNumber << "u;\n";
        fs << R"(
            [[stage(fragment)]] fn main() -> [[location(0)]] vec4<f32> {
                sBufferNotDynamic.value = uBufferNotDynamic.value.xy;
                sBuffer.value = vec2<u32>(multipleNumber, multipleNumber) * (uBuffer.value.xy + uBufferNotDynamic.value.xy);
                return vec4<f32>(f32(uBuffer.value.x) / 255.0, f32(uBuffer.value.y) / 255.0,
                                      1.0, 1.0);
            }
        )";

        wgpu::ShaderModule fsModule = utils::CreateShaderModule(device, fs.str().c_str());

        utils::ComboRenderPipelineDescriptor pipelineDescriptor;
        pipelineDescriptor.vertex.module = vsModule;
        pipelineDescriptor.cFragment.module = fsModule;
        pipelineDescriptor.cTargets[0].format = wgpu::TextureFormat::RGBA8Unorm;

        wgpu::PipelineLayoutDescriptor pipelineLayoutDescriptor;
        if (isInheritedPipeline) {
            pipelineLayoutDescriptor.bindGroupLayoutCount = 2;
        } else {
            pipelineLayoutDescriptor.bindGroupLayoutCount = 1;
        }
        pipelineLayoutDescriptor.bindGroupLayouts = mBindGroupLayouts;
        pipelineDescriptor.layout = device.CreatePipelineLayout(&pipelineLayoutDescriptor);

        return device.CreateRenderPipeline(&pipelineDescriptor);
    }

    wgpu::ComputePipeline CreateComputePipeline(bool isInheritedPipeline = false) {
        // Construct compute shader source
        std::ostringstream cs;
        std::string multipleNumber = isInheritedPipeline ? "2" : "1";
        cs << R"(
            [[block]] struct Buf {
                value : vec2<u32>;
            };

            [[group(0), binding(0)]] var<uniform> uBufferNotDynamic : Buf;
            [[group(0), binding(1)]] var<storage, read_write> sBufferNotDynamic : Buf;
            [[group(0), binding(3)]] var<uniform> uBuffer : Buf;
            [[group(0), binding(4)]] var<storage, read_write> sBuffer : Buf;
        )";

        if (isInheritedPipeline) {
            cs << R"(
                [[group(1), binding(0)]] var<uniform> paddingBlock : Buf;
            )";
        }

        cs << "let multipleNumber : u32 = " << multipleNumber << "u;\n";
        cs << R"(
            [[stage(compute), workgroup_size(1)]] fn main() {
                sBufferNotDynamic.value = uBufferNotDynamic.value.xy;
                sBuffer.value = vec2<u32>(multipleNumber, multipleNumber) * (uBuffer.value.xy + uBufferNotDynamic.value.xy);
            }
        )";

        wgpu::ShaderModule csModule = utils::CreateShaderModule(device, cs.str().c_str());

        wgpu::ComputePipelineDescriptor csDesc;
        csDesc.compute.module = csModule;
        csDesc.compute.entryPoint = "main";

        wgpu::PipelineLayoutDescriptor pipelineLayoutDescriptor;
        if (isInheritedPipeline) {
            pipelineLayoutDescriptor.bindGroupLayoutCount = 2;
        } else {
            pipelineLayoutDescriptor.bindGroupLayoutCount = 1;
        }
        pipelineLayoutDescriptor.bindGroupLayouts = mBindGroupLayouts;
        csDesc.layout = device.CreatePipelineLayout(&pipelineLayoutDescriptor);

        return device.CreateComputePipeline(&csDesc);
    }
};

// Dynamic offsets are all zero and no effect to result.
TEST_P(DynamicBufferOffsetTests, BasicRenderPipeline) {
    wgpu::RenderPipeline pipeline = CreateRenderPipeline();
    utils::BasicRenderPass renderPass = utils::CreateBasicRenderPass(device, kRTSize, kRTSize);

    wgpu::CommandEncoder commandEncoder = device.CreateCommandEncoder();
    std::array<uint32_t, 2> offsets = {0, 0};
    wgpu::RenderPassEncoder renderPassEncoder =
        commandEncoder.BeginRenderPass(&renderPass.renderPassInfo);
    renderPassEncoder.SetPipeline(pipeline);
    renderPassEncoder.SetBindGroup(0, mBindGroups[0], offsets.size(), offsets.data());
    renderPassEncoder.Draw(3);
    renderPassEncoder.EndPass();
    wgpu::CommandBuffer commands = commandEncoder.Finish();
    queue.Submit(1, &commands);

    std::vector<uint32_t> expectedData = {2, 4};
    EXPECT_PIXEL_RGBA8_EQ(RGBA8(1, 2, 255, 255), renderPass.color, 0, 0);
    EXPECT_BUFFER_U32_RANGE_EQ(expectedData.data(), mStorageBuffers[1], 0, expectedData.size());
}

// Have non-zero dynamic offsets.
TEST_P(DynamicBufferOffsetTests, SetDynamicOffsetsRenderPipeline) {
    wgpu::RenderPipeline pipeline = CreateRenderPipeline();
    utils::BasicRenderPass renderPass = utils::CreateBasicRenderPass(device, kRTSize, kRTSize);

    wgpu::CommandEncoder commandEncoder = device.CreateCommandEncoder();
    std::array<uint32_t, 2> offsets = {kMinUniformBufferOffsetAlignment,
                                       kMinUniformBufferOffsetAlignment};
    wgpu::RenderPassEncoder renderPassEncoder =
        commandEncoder.BeginRenderPass(&renderPass.renderPassInfo);
    renderPassEncoder.SetPipeline(pipeline);
    renderPassEncoder.SetBindGroup(0, mBindGroups[0], offsets.size(), offsets.data());
    renderPassEncoder.Draw(3);
    renderPassEncoder.EndPass();
    wgpu::CommandBuffer commands = commandEncoder.Finish();
    queue.Submit(1, &commands);

    std::vector<uint32_t> expectedData = {6, 8};
    EXPECT_PIXEL_RGBA8_EQ(RGBA8(5, 6, 255, 255), renderPass.color, 0, 0);
    EXPECT_BUFFER_U32_RANGE_EQ(expectedData.data(), mStorageBuffers[1],
                               kMinUniformBufferOffsetAlignment, expectedData.size());
}

// Dynamic offsets are all zero and no effect to result.
TEST_P(DynamicBufferOffsetTests, BasicComputePipeline) {
    wgpu::ComputePipeline pipeline = CreateComputePipeline();

    std::array<uint32_t, 2> offsets = {0, 0};

    wgpu::CommandEncoder commandEncoder = device.CreateCommandEncoder();
    wgpu::ComputePassEncoder computePassEncoder = commandEncoder.BeginComputePass();
    computePassEncoder.SetPipeline(pipeline);
    computePassEncoder.SetBindGroup(0, mBindGroups[0], offsets.size(), offsets.data());
    computePassEncoder.Dispatch(1);
    computePassEncoder.EndPass();
    wgpu::CommandBuffer commands = commandEncoder.Finish();
    queue.Submit(1, &commands);

    std::vector<uint32_t> expectedData = {2, 4};
    EXPECT_BUFFER_U32_RANGE_EQ(expectedData.data(), mStorageBuffers[1], 0, expectedData.size());
}

// Have non-zero dynamic offsets.
TEST_P(DynamicBufferOffsetTests, SetDynamicOffsetsComputePipeline) {
    wgpu::ComputePipeline pipeline = CreateComputePipeline();

    std::array<uint32_t, 2> offsets = {kMinUniformBufferOffsetAlignment,
                                       kMinUniformBufferOffsetAlignment};

    wgpu::CommandEncoder commandEncoder = device.CreateCommandEncoder();
    wgpu::ComputePassEncoder computePassEncoder = commandEncoder.BeginComputePass();
    computePassEncoder.SetPipeline(pipeline);
    computePassEncoder.SetBindGroup(0, mBindGroups[0], offsets.size(), offsets.data());
    computePassEncoder.Dispatch(1);
    computePassEncoder.EndPass();
    wgpu::CommandBuffer commands = commandEncoder.Finish();
    queue.Submit(1, &commands);

    std::vector<uint32_t> expectedData = {6, 8};
    EXPECT_BUFFER_U32_RANGE_EQ(expectedData.data(), mStorageBuffers[1],
                               kMinUniformBufferOffsetAlignment, expectedData.size());
}

// Test inherit dynamic offsets on render pipeline
TEST_P(DynamicBufferOffsetTests, InheritDynamicOffsetsRenderPipeline) {
    // Using default pipeline and setting dynamic offsets
    wgpu::RenderPipeline pipeline = CreateRenderPipeline();
    wgpu::RenderPipeline testPipeline = CreateRenderPipeline(true);

    utils::BasicRenderPass renderPass = utils::CreateBasicRenderPass(device, kRTSize, kRTSize);

    wgpu::CommandEncoder commandEncoder = device.CreateCommandEncoder();
    std::array<uint32_t, 2> offsets = {kMinUniformBufferOffsetAlignment,
                                       kMinUniformBufferOffsetAlignment};
    wgpu::RenderPassEncoder renderPassEncoder =
        commandEncoder.BeginRenderPass(&renderPass.renderPassInfo);
    renderPassEncoder.SetPipeline(pipeline);
    renderPassEncoder.SetBindGroup(0, mBindGroups[0], offsets.size(), offsets.data());
    renderPassEncoder.Draw(3);
    renderPassEncoder.SetPipeline(testPipeline);
    renderPassEncoder.SetBindGroup(1, mBindGroups[1]);
    renderPassEncoder.Draw(3);
    renderPassEncoder.EndPass();
    wgpu::CommandBuffer commands = commandEncoder.Finish();
    queue.Submit(1, &commands);

    std::vector<uint32_t> expectedData = {12, 16};
    EXPECT_PIXEL_RGBA8_EQ(RGBA8(5, 6, 255, 255), renderPass.color, 0, 0);
    EXPECT_BUFFER_U32_RANGE_EQ(expectedData.data(), mStorageBuffers[1],
                               kMinUniformBufferOffsetAlignment, expectedData.size());
}

// Test inherit dynamic offsets on compute pipeline
// TODO(shaobo.yan@intel.com) : Try this test on GTX1080 and cannot reproduce the failure.
// Suspect it is due to dawn doesn't handle sync between two dispatch and disable this case.
// Will double check root cause after got GTX1660.
TEST_P(DynamicBufferOffsetTests, InheritDynamicOffsetsComputePipeline) {
    DAWN_SUPPRESS_TEST_IF(IsWindows());
    wgpu::ComputePipeline pipeline = CreateComputePipeline();
    wgpu::ComputePipeline testPipeline = CreateComputePipeline(true);

    std::array<uint32_t, 2> offsets = {kMinUniformBufferOffsetAlignment,
                                       kMinUniformBufferOffsetAlignment};

    wgpu::CommandEncoder commandEncoder = device.CreateCommandEncoder();
    wgpu::ComputePassEncoder computePassEncoder = commandEncoder.BeginComputePass();
    computePassEncoder.SetPipeline(pipeline);
    computePassEncoder.SetBindGroup(0, mBindGroups[0], offsets.size(), offsets.data());
    computePassEncoder.Dispatch(1);
    computePassEncoder.SetPipeline(testPipeline);
    computePassEncoder.SetBindGroup(1, mBindGroups[1]);
    computePassEncoder.Dispatch(1);
    computePassEncoder.EndPass();
    wgpu::CommandBuffer commands = commandEncoder.Finish();
    queue.Submit(1, &commands);

    std::vector<uint32_t> expectedData = {12, 16};
    EXPECT_BUFFER_U32_RANGE_EQ(expectedData.data(), mStorageBuffers[1],
                               kMinUniformBufferOffsetAlignment, expectedData.size());
}

// Setting multiple dynamic offsets for the same bindgroup in one render pass.
TEST_P(DynamicBufferOffsetTests, UpdateDynamicOffsetsMultipleTimesRenderPipeline) {
    // Using default pipeline and setting dynamic offsets
    wgpu::RenderPipeline pipeline = CreateRenderPipeline();

    utils::BasicRenderPass renderPass = utils::CreateBasicRenderPass(device, kRTSize, kRTSize);

    wgpu::CommandEncoder commandEncoder = device.CreateCommandEncoder();
    std::array<uint32_t, 2> offsets = {kMinUniformBufferOffsetAlignment,
                                       kMinUniformBufferOffsetAlignment};
    std::array<uint32_t, 2> testOffsets = {0, 0};

    wgpu::RenderPassEncoder renderPassEncoder =
        commandEncoder.BeginRenderPass(&renderPass.renderPassInfo);
    renderPassEncoder.SetPipeline(pipeline);
    renderPassEncoder.SetBindGroup(0, mBindGroups[0], offsets.size(), offsets.data());
    renderPassEncoder.Draw(3);
    renderPassEncoder.SetBindGroup(0, mBindGroups[0], testOffsets.size(), testOffsets.data());
    renderPassEncoder.Draw(3);
    renderPassEncoder.EndPass();
    wgpu::CommandBuffer commands = commandEncoder.Finish();
    queue.Submit(1, &commands);

    std::vector<uint32_t> expectedData = {2, 4};
    EXPECT_PIXEL_RGBA8_EQ(RGBA8(1, 2, 255, 255), renderPass.color, 0, 0);
    EXPECT_BUFFER_U32_RANGE_EQ(expectedData.data(), mStorageBuffers[1], 0, expectedData.size());
}

// Setting multiple dynamic offsets for the same bindgroup in one compute pass.
TEST_P(DynamicBufferOffsetTests, UpdateDynamicOffsetsMultipleTimesComputePipeline) {
    wgpu::ComputePipeline pipeline = CreateComputePipeline();

    std::array<uint32_t, 2> offsets = {kMinUniformBufferOffsetAlignment,
                                       kMinUniformBufferOffsetAlignment};
    std::array<uint32_t, 2> testOffsets = {0, 0};

    wgpu::CommandEncoder commandEncoder = device.CreateCommandEncoder();
    wgpu::ComputePassEncoder computePassEncoder = commandEncoder.BeginComputePass();
    computePassEncoder.SetPipeline(pipeline);
    computePassEncoder.SetBindGroup(0, mBindGroups[0], offsets.size(), offsets.data());
    computePassEncoder.Dispatch(1);
    computePassEncoder.SetBindGroup(0, mBindGroups[0], testOffsets.size(), testOffsets.data());
    computePassEncoder.Dispatch(1);
    computePassEncoder.EndPass();
    wgpu::CommandBuffer commands = commandEncoder.Finish();
    queue.Submit(1, &commands);

    std::vector<uint32_t> expectedData = {2, 4};
    EXPECT_BUFFER_U32_RANGE_EQ(expectedData.data(), mStorageBuffers[1], 0, expectedData.size());
}

DAWN_INSTANTIATE_TEST(DynamicBufferOffsetTests,
                      D3D12Backend(),
                      MetalBackend(),
                      OpenGLBackend(),
                      OpenGLESBackend(),
                      VulkanBackend());
