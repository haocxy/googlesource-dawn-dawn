// Copyright 2021 The Dawn Authors
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

#ifndef DAWNWIRE_CLIENT_SHADER_MODULE_H_
#define DAWNWIRE_CLIENT_SHADER_MODULE_H_

#include <dawn/webgpu.h>

#include "common/SerialMap.h"
#include "dawn_wire/client/ObjectBase.h"

namespace dawn_wire { namespace client {

    class ShaderModule final : public ObjectBase {
      public:
        using ObjectBase::ObjectBase;
        ~ShaderModule();

        void GetCompilationInfo(WGPUCompilationInfoCallback callback, void* userdata);
        bool GetCompilationInfoCallback(uint64_t requestSerial,
                                        WGPUCompilationInfoRequestStatus status,
                                        const WGPUCompilationInfo* info);

        void CancelCallbacksForDisconnect() override;

      private:
        struct CompilationInfoRequest {
            WGPUCompilationInfoCallback callback = nullptr;
            void* userdata = nullptr;
        };
        uint64_t mCompilationInfoRequestSerial = 0;
        std::map<uint64_t, CompilationInfoRequest> mCompilationInfoRequests;
    };

}}  // namespace dawn_wire::client

#endif  // DAWNWIRE_CLIENT_SHADER_MODULE_H_
