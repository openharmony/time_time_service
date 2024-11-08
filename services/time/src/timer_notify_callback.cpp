/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "timer_notify_callback.h"
#include "timer_manager.h"

namespace OHOS {
namespace MiscServices {
std::mutex TimerNotifyCallback::instanceLock_;
sptr<TimerNotifyCallback> TimerNotifyCallback::instance_;
TimerManager* TimerNotifyCallback::managerHandler_;

TimerNotifyCallback::TimerNotifyCallback() = default;

TimerNotifyCallback::~TimerNotifyCallback() = default;

sptr<TimerNotifyCallback> TimerNotifyCallback::GetInstance(TimerManager* timerManager)
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> autoLock(instanceLock_);
        if (instance_ == nullptr) {
            instance_ = new TimerNotifyCallback;
            instance_->managerHandler_ = timerManager;
        }
    }
    return instance_;
}

void TimerNotifyCallback::Finish(uint64_t timerId)
{
}
} // namespace MiscServices
} // namespace OHOS