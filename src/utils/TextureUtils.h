// Copyright 2020 The Dawn Authors
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

#ifndef UTILS_TEXTURE_UTILS_H_
#define UTILS_TEXTURE_UTILS_H_

#include <array>

#include <dawn/webgpu_cpp.h>

#include "common/Assert.h"

namespace utils {
    static constexpr std::array<wgpu::TextureFormat, 63> kAllTextureFormats = {
        wgpu::TextureFormat::R8Unorm,
        wgpu::TextureFormat::R8Snorm,
        wgpu::TextureFormat::R8Uint,
        wgpu::TextureFormat::R8Sint,
        wgpu::TextureFormat::R16Uint,
        wgpu::TextureFormat::R16Sint,
        wgpu::TextureFormat::R16Float,
        wgpu::TextureFormat::RG8Unorm,
        wgpu::TextureFormat::RG8Snorm,
        wgpu::TextureFormat::RG8Uint,
        wgpu::TextureFormat::RG8Sint,
        wgpu::TextureFormat::R32Float,
        wgpu::TextureFormat::R32Uint,
        wgpu::TextureFormat::R32Sint,
        wgpu::TextureFormat::RG16Uint,
        wgpu::TextureFormat::RG16Sint,
        wgpu::TextureFormat::RG16Float,
        wgpu::TextureFormat::RGBA8Unorm,
        wgpu::TextureFormat::RGBA8UnormSrgb,
        wgpu::TextureFormat::RGBA8Snorm,
        wgpu::TextureFormat::RGBA8Uint,
        wgpu::TextureFormat::RGBA8Sint,
        wgpu::TextureFormat::BGRA8Unorm,
        wgpu::TextureFormat::BGRA8UnormSrgb,
        wgpu::TextureFormat::RGB10A2Unorm,
        wgpu::TextureFormat::RG11B10Ufloat,
        wgpu::TextureFormat::RGB9E5Ufloat,
        wgpu::TextureFormat::RG32Float,
        wgpu::TextureFormat::RG32Uint,
        wgpu::TextureFormat::RG32Sint,
        wgpu::TextureFormat::RGBA16Uint,
        wgpu::TextureFormat::RGBA16Sint,
        wgpu::TextureFormat::RGBA16Float,
        wgpu::TextureFormat::RGBA32Float,
        wgpu::TextureFormat::RGBA32Uint,
        wgpu::TextureFormat::RGBA32Sint,
        wgpu::TextureFormat::Depth32Float,
        wgpu::TextureFormat::Depth24Plus,
        wgpu::TextureFormat::Depth24PlusStencil8,
        wgpu::TextureFormat::BC1RGBAUnorm,
        wgpu::TextureFormat::BC1RGBAUnormSrgb,
        wgpu::TextureFormat::BC2RGBAUnorm,
        wgpu::TextureFormat::BC2RGBAUnormSrgb,
        wgpu::TextureFormat::BC3RGBAUnorm,
        wgpu::TextureFormat::BC3RGBAUnormSrgb,
        wgpu::TextureFormat::BC4RUnorm,
        wgpu::TextureFormat::BC4RSnorm,
        wgpu::TextureFormat::BC5RGUnorm,
        wgpu::TextureFormat::BC5RGSnorm,
        wgpu::TextureFormat::BC6HRGBUfloat,
        wgpu::TextureFormat::BC6HRGBFloat,
        wgpu::TextureFormat::BC7RGBAUnorm,
        wgpu::TextureFormat::BC7RGBAUnormSrgb,
        wgpu::TextureFormat::ETC2RGB8Unorm,
        wgpu::TextureFormat::ETC2RGB8UnormSrgb,
        wgpu::TextureFormat::ETC2RGB8A1Unorm,
        wgpu::TextureFormat::ETC2RGB8A1UnormSrgb,
        wgpu::TextureFormat::ETC2RGBA8Unorm,
        wgpu::TextureFormat::ETC2RGBA8UnormSrgb,
        wgpu::TextureFormat::EACR11Unorm,
        wgpu::TextureFormat::EACR11Snorm,
        wgpu::TextureFormat::EACRG11Unorm,
        wgpu::TextureFormat::EACRG11Snorm};

    static constexpr std::array<wgpu::TextureFormat, 14> kBCFormats = {
        wgpu::TextureFormat::BC1RGBAUnorm,  wgpu::TextureFormat::BC1RGBAUnormSrgb,
        wgpu::TextureFormat::BC2RGBAUnorm,  wgpu::TextureFormat::BC2RGBAUnormSrgb,
        wgpu::TextureFormat::BC3RGBAUnorm,  wgpu::TextureFormat::BC3RGBAUnormSrgb,
        wgpu::TextureFormat::BC4RUnorm,     wgpu::TextureFormat::BC4RSnorm,
        wgpu::TextureFormat::BC5RGUnorm,    wgpu::TextureFormat::BC5RGSnorm,
        wgpu::TextureFormat::BC6HRGBUfloat, wgpu::TextureFormat::BC6HRGBFloat,
        wgpu::TextureFormat::BC7RGBAUnorm,  wgpu::TextureFormat::BC7RGBAUnormSrgb};

    static constexpr std::array<wgpu::TextureFormat, 10> kETC2Formats = {
        wgpu::TextureFormat::ETC2RGB8Unorm,   wgpu::TextureFormat::ETC2RGB8UnormSrgb,
        wgpu::TextureFormat::ETC2RGB8A1Unorm, wgpu::TextureFormat::ETC2RGB8A1UnormSrgb,
        wgpu::TextureFormat::ETC2RGBA8Unorm,  wgpu::TextureFormat::ETC2RGBA8UnormSrgb,
        wgpu::TextureFormat::EACR11Unorm,     wgpu::TextureFormat::EACR11Snorm,
        wgpu::TextureFormat::EACRG11Unorm,    wgpu::TextureFormat::EACRG11Snorm};

    // TODO(crbug.com/dawn/955) Add formats once implemented.
    static constexpr std::array<wgpu::TextureFormat, 0> kASTCFormats = {};

    static constexpr std::array<wgpu::TextureFormat, 24> kCompressedFormats = {
        wgpu::TextureFormat::BC1RGBAUnorm,    wgpu::TextureFormat::BC1RGBAUnormSrgb,
        wgpu::TextureFormat::BC2RGBAUnorm,    wgpu::TextureFormat::BC2RGBAUnormSrgb,
        wgpu::TextureFormat::BC3RGBAUnorm,    wgpu::TextureFormat::BC3RGBAUnormSrgb,
        wgpu::TextureFormat::BC4RUnorm,       wgpu::TextureFormat::BC4RSnorm,
        wgpu::TextureFormat::BC5RGUnorm,      wgpu::TextureFormat::BC5RGSnorm,
        wgpu::TextureFormat::BC6HRGBUfloat,   wgpu::TextureFormat::BC6HRGBFloat,
        wgpu::TextureFormat::BC7RGBAUnorm,    wgpu::TextureFormat::BC7RGBAUnormSrgb,
        wgpu::TextureFormat::ETC2RGB8Unorm,   wgpu::TextureFormat::ETC2RGB8UnormSrgb,
        wgpu::TextureFormat::ETC2RGB8A1Unorm, wgpu::TextureFormat::ETC2RGB8A1UnormSrgb,
        wgpu::TextureFormat::ETC2RGBA8Unorm,  wgpu::TextureFormat::ETC2RGBA8UnormSrgb,
        wgpu::TextureFormat::EACR11Unorm,     wgpu::TextureFormat::EACR11Snorm,
        wgpu::TextureFormat::EACRG11Unorm,    wgpu::TextureFormat::EACRG11Snorm};
    static_assert(kCompressedFormats.size() ==
                      kBCFormats.size() + kETC2Formats.size() + kASTCFormats.size(),
                  "Number of compressed format must equal number of BC, ETC2, and ASTC formats.");

    bool TextureFormatSupportsStorageTexture(wgpu::TextureFormat format);

    bool IsBCTextureFormat(wgpu::TextureFormat textureFormat);
    bool IsETC2TextureFormat(wgpu::TextureFormat textureFormat);
    bool IsASTCTextureFormat(wgpu::TextureFormat textureFormat);

    uint32_t GetTexelBlockSizeInBytes(wgpu::TextureFormat textureFormat);
    uint32_t GetTextureFormatBlockWidth(wgpu::TextureFormat textureFormat);
    uint32_t GetTextureFormatBlockHeight(wgpu::TextureFormat textureFormat);

    const char* GetWGSLColorTextureComponentType(wgpu::TextureFormat textureFormat);
    const char* GetWGSLImageFormatQualifier(wgpu::TextureFormat textureFormat);
    uint32_t GetWGSLRenderableColorTextureComponentCount(wgpu::TextureFormat textureFormat);

    wgpu::TextureDimension ViewDimensionToTextureDimension(
        const wgpu::TextureViewDimension dimension);
}  // namespace utils

#endif
