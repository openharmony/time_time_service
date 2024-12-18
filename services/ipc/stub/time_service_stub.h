/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SERVICES_INCLUDE_TIME_SERVICE_STUB_H
#define SERVICES_INCLUDE_TIME_SERVICE_STUB_H

#include <map>

#include "ipc_skeleton.h"
#include "iremote_stub.h"
#include "itimer_call_back.h"
#include "time_service_interface.h"
#include "time_permission.h"
#include "time_service_ipc_interface_code.h"

namespace OHOS {
namespace MiscServices {
class TimeServiceStub : public IRemoteStub<ITimeService> {
public:
    TimeServiceStub();
    ~TimeServiceStub();
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    using TimeServiceFunc = std::function<int32_t(MessageParcel &data, MessageParcel &reply)>;
    int32_t OnSetTime(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetTimeZone(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetTimeZone(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetThreadTimeMs(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetThreadTimeNs(MessageParcel &data, MessageParcel &reply);

    int32_t OnCreateTimer(MessageParcel &data, MessageParcel &reply);
    int32_t OnStartTimer(MessageParcel &data, MessageParcel &reply);
    int32_t OnStopTimer(MessageParcel &data, MessageParcel &reply);
    int32_t OnDestroyTimer(MessageParcel &data, MessageParcel &reply);
    int32_t OnNetworkTimeStatusOff(MessageParcel &data, MessageParcel &reply);
    int32_t OnNetworkTimeStatusOn(MessageParcel &data, MessageParcel &reply);
    int32_t OnTimerProxy(MessageParcel &data, MessageParcel &reply);
    int32_t OnPidTimerProxy(MessageParcel &data, MessageParcel &reply);
    int32_t OnAdjustTimer(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetTimerExemption(MessageParcel &data, MessageParcel &reply);
    int32_t OnAllProxyReset(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetNtpTimeMs(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetRealTimeMs(MessageParcel &data, MessageParcel &reply);
    std::map<TimeServiceIpcInterfaceCode, TimeServiceFunc> memberFuncMap_;
};
} // namespace MiscServices
} // namespace OHOS

#endif // SERVICES_INCLUDE_TIME_SERVICE_STUB_H