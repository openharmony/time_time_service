/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "timer_call_back_proxy.h"

namespace OHOS {
namespace MiscServices {
TimerCallbackProxy::TimerCallbackProxy(const sptr<IRemoteObject> &object) : IRemoteProxy<ITimerCallback>(object)
{
}

TimerCallbackProxy::~TimerCallbackProxy()
{
    TIME_HILOGD(TIME_MODULE_CLIENT, "TimerCallbackProxy instance destoryed");
}

int32_t TimerCallbackProxy::NotifyTimer(const uint64_t timerId, const sptr<IRemoteObject> &timerCallback)
{
    TIME_HILOGD(TIME_MODULE_CLIENT, "start id: %{public}" PRId64 "", timerId);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return E_TIME_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TIME_HILOGE(TIME_MODULE_CLIENT, "write descriptor failed!");
        return E_TIME_WRITE_PARCEL_ERROR;
    }

    if (!data.WriteUint64(timerId)) {
        TIME_HILOGE(TIME_MODULE_CLIENT, "write timerId failed!");
        return E_TIME_WRITE_PARCEL_ERROR;
    }
    if (timerCallback != nullptr && !data.WriteRemoteObject(timerCallback)) {
        TIME_HILOGE(TIME_MODULE_CLIENT, "write timerCallback failed!");
        return E_TIME_WRITE_PARCEL_ERROR;
    }
    int ret = remote->SendRequest(static_cast<int>(ITimerCallback::Message::NOTIFY_TIMER), data, reply, option);
    if (ret != ERR_OK) {
        TIME_HILOGE(TIME_MODULE_CLIENT, "SendRequest is failed, error code: %{public}d", ret);
    }
    return ret;
}
} // namespace MiscServices
} // namespace OHOS
