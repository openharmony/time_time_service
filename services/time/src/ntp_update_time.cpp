/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "ntp_update_time.h"

#include "init_param.h"
#include "ntp_trusted_time.h"
#include "parameters.h"
#include "time_system_ability.h"

using namespace std::chrono;

namespace OHOS {
namespace MiscServices {
namespace {
constexpr int64_t NANO_TO_MILLISECOND = 1000000;
constexpr int64_t TWO_SECOND_TO_MILLISECOND = 2000;
constexpr int64_t HALF_DAY_TO_MILLISECOND = 43200000;
constexpr const char* NTP_SERVER_SYSTEM_PARAMETER = "persist.time.ntpserver";
constexpr const char* NTP_SERVER_SPECIFIC_SYSTEM_PARAMETER = "persist.time.ntpserver_specific";
constexpr uint32_t NTP_MAX_SIZE = 5;
constexpr const char* AUTO_TIME_SYSTEM_PARAMETER = "persist.time.auto_time";
constexpr const char* AUTO_TIME_STATUS_ON = "ON";
constexpr const char* AUTO_TIME_STATUS_OFF = "OFF";
constexpr int64_t ONE_HOUR = 3600000;
constexpr const char* DEFAULT_NTP_SERVER = "1.cn.pool.ntp.org";
constexpr int32_t RETRY_TIMES = 2;
} // namespace

AutoTimeInfo NtpUpdateTime::autoTimeInfo_{};
std::mutex NtpUpdateTime::requestMutex_;

NtpUpdateTime::NtpUpdateTime() : timerId_(0), nitzUpdateTimeMilli_(0), nextTriggerTime_(0), lastNITZUpdateTime_(0){};

NtpUpdateTime& NtpUpdateTime::GetInstance()
{
    static NtpUpdateTime instance;
    return instance;
}

void NtpUpdateTime::Init()
{
    TIME_HILOGD(TIME_MODULE_SERVICE, "Ntp Update Time start");
    std::string ntpServer = system::GetParameter(NTP_SERVER_SYSTEM_PARAMETER, DEFAULT_NTP_SERVER);
    std::string ntpServerSpec = system::GetParameter(NTP_SERVER_SPECIFIC_SYSTEM_PARAMETER, "");
    std::string autoTime = system::GetParameter(AUTO_TIME_SYSTEM_PARAMETER, "ON");
    if ((ntpServer.empty() && ntpServerSpec.empty()) || autoTime.empty()) {
        TIME_HILOGW(TIME_MODULE_SERVICE, "No found parameter from system parameter");
        return;
    }
    RegisterSystemParameterListener();
    autoTimeInfo_.ntpServer = ntpServer;
    autoTimeInfo_.ntpServerSpec = ntpServerSpec;
    autoTimeInfo_.status = autoTime;
    auto callback = [this](uint64_t id) -> int32_t {
        this->RefreshNetworkTimeByTimer(id);
        return E_TIME_OK;
    };
    TimerPara timerPara{};
    timerPara.timerType = static_cast<int>(ITimerManager::TimerType::ELAPSED_REALTIME);
    timerPara.windowLength = 0;
    timerPara.interval = HALF_DAY_TO_MILLISECOND;
    timerPara.flag = 0;
    TimeSystemAbility::GetInstance()->CreateTimer(timerPara, callback, timerId_);
    RefreshNextTriggerTime();
    TimeSystemAbility::GetInstance()->StartTimer(timerId_, nextTriggerTime_);
    TIME_HILOGI(TIME_MODULE_SERVICE, "ntp update timerId: %{public}" PRIu64 "triggertime: %{public}" PRId64 "",
                timerId_, nextTriggerTime_);
}

void NtpUpdateTime::RefreshNetworkTimeByTimer(uint64_t timerId)
{
    TIME_HILOGI(TIME_MODULE_SERVICE, "The timer is up");
    if (!CheckStatus()) {
        TIME_HILOGI(TIME_MODULE_SERVICE, "Auto Sync Switch Off");
        return;
    }

    auto setSystemTime = [this]() { this->SetSystemTime(); };
    std::thread thread(setSystemTime);
    thread.detach();
    TIME_HILOGD(TIME_MODULE_SERVICE, "Ntp next triggertime: %{public}" PRId64 "", nextTriggerTime_);
}

void NtpUpdateTime::UpdateNITZSetTime()
{
    auto bootTimeNano = steady_clock::now().time_since_epoch().count();
    auto bootTimeMilli = bootTimeNano / NANO_TO_MILLISECOND;
    if (TimeUtils::GetBootTimeMs(lastNITZUpdateTime_) != ERR_OK) {
        TIME_HILOGE(TIME_MODULE_SERVICE, "get boot time fail");
    }
    TIME_HILOGD(TIME_MODULE_SERVICE, "nitz time changed");
    nitzUpdateTimeMilli_ = static_cast<uint64_t>(bootTimeMilli);
}

std::vector<std::string> NtpUpdateTime::SplitNtpAddrs(const std::string &ntpStr)
{
    std::vector<std::string> ntpList;
    size_t start = 0;
    do {
        if (ntpList.size() == NTP_MAX_SIZE) {
            break;
        }
        size_t end = ntpStr.find(',', start);
        if (end < start) {
            break;
        }
        std::string temp = ntpStr.substr(start, end - start);
        if (temp.empty()) {
            ++start;
            continue;
        }
        if (end == std::string::npos) {
            ntpList.emplace_back(temp);
            break;
        }
        ntpList.emplace_back(temp);
        start = end + 1;
    } while (start < ntpStr.size());
    return ntpList;
}

bool NtpUpdateTime::IsInUpdateInterval()
{
    // Determine the time interval between two NTP requests sent.
    int64_t curBootTime = 0;
    TimeUtils::GetBootTimeMs(curBootTime);
    auto lastBootTime = NtpTrustedTime::GetInstance().ElapsedRealtimeMillis();
    // If the time <= ONE_HOUR, do not send NTP requests.
    if ((lastBootTime > 0) && (curBootTime - lastBootTime <= ONE_HOUR)) {
        TIME_HILOGI(TIME_MODULE_SERVICE,
            "ntp updated bootTime: %{public}" PRId64 ", lastBootTime: %{public}" PRId64 "",
            curBootTime, lastBootTime);
        return true;
    }
    return false;
}

NtpRefreshCode NtpUpdateTime::GetNtpTimeInner()
{
    if (IsInUpdateInterval()) {
        return NO_NEED_REFRESH;
    }

    std::vector<std::string> ntpSpecList = SplitNtpAddrs(autoTimeInfo_.ntpServerSpec);
    std::vector<std::string> ntpList = SplitNtpAddrs(autoTimeInfo_.ntpServer);
    ntpSpecList.insert(ntpSpecList.end(), ntpList.begin(), ntpList.end());
    for (int i = 0; i < RETRY_TIMES; i++) {
        for (size_t j = 0; j < ntpSpecList.size(); j++) {
            TIME_HILOGI(TIME_MODULE_SERVICE, "ntpServer is : %{public}s", ntpSpecList[j].c_str());
            if (NtpTrustedTime::GetInstance().ForceRefresh(ntpSpecList[j])) {
                return REFRESH_SUCCESS;
            }
        }
    }
    return REFRESH_FAILED;
}

bool NtpUpdateTime::GetRealTimeInner(int64_t &time)
{
    time = NtpTrustedTime::GetInstance().CurrentTimeMillis();
    if (time <= 0) {
        TIME_HILOGE(TIME_MODULE_SERVICE, "current time is invalid: %{public}" PRId64 "", time);
        return false;
    }
    return true;
}

bool NtpUpdateTime::GetRealTime(int64_t &time)
{
    return GetRealTimeInner(time);
}

bool NtpUpdateTime::CheckNeedSetTime(NtpRefreshCode code, int64_t time)
{
    if (code == NO_NEED_REFRESH) {
        int64_t currentWallTime = 0;
        if (TimeUtils::GetWallTimeMs(currentWallTime) != ERR_OK) {
            TIME_HILOGE(TIME_MODULE_SERVICE, "get walltime fail");
            return false;
        }
    
        if (std::abs(currentWallTime - time) < TWO_SECOND_TO_MILLISECOND) {
            TIME_HILOGW(TIME_MODULE_SERVICE, "no need to refresh time");
            return false;
        }
    }
    return true;
}

bool NtpUpdateTime::GetNtpTime(int64_t &time)
{
    std::lock_guard<std::mutex> autoLock(requestMutex_);

    auto ret = GetNtpTimeInner();
    if (ret == REFRESH_FAILED) {
        TIME_HILOGE(TIME_MODULE_SERVICE, "get ntp time failed");
        return false;
    }

    if (!GetRealTimeInner(time)) {
        return false;
    }

    if (autoTimeInfo_.status == AUTO_TIME_STATUS_ON && CheckNeedSetTime(ret, time)) {
        TimeSystemAbility::GetInstance()->SetTimeInner(time);
    }
    return true;
}

void NtpUpdateTime::SetSystemTime()
{
    if (autoTimeInfo_.status != AUTO_TIME_STATUS_ON) {
        TIME_HILOGI(TIME_MODULE_SERVICE, "auto sync switch off");
        return;
    }

    if (!requestMutex_.try_lock()) {
        TIME_HILOGW(TIME_MODULE_SERVICE, "The NTP request is in progress");
        return;
    }

    auto ret = GetNtpTimeInner();
    if (ret == REFRESH_FAILED) {
        TIME_HILOGE(TIME_MODULE_SERVICE, "get ntp time failed");
        requestMutex_.unlock();
        return;
    }

    int64_t currentTime = NtpTrustedTime::GetInstance().CurrentTimeMillis();
    if (currentTime <= 0) {
        TIME_HILOGE(TIME_MODULE_SERVICE, "current time is invalid: %{public}" PRIu64 "", currentTime);
        requestMutex_.unlock();
        return;
    }
    if (!CheckNeedSetTime(ret, currentTime)) {
        requestMutex_.unlock();
        return;
    }

    TimeSystemAbility::GetInstance()->SetTimeInner(currentTime);
    requestMutex_.unlock();
}

void NtpUpdateTime::RefreshNextTriggerTime()
{
    auto bootTimeNano = steady_clock::now().time_since_epoch().count();
    auto bootTimeMilli = bootTimeNano / NANO_TO_MILLISECOND;
    nextTriggerTime_ = static_cast<uint64_t>(bootTimeMilli + HALF_DAY_TO_MILLISECOND);
}

bool NtpUpdateTime::CheckStatus()
{
    return autoTimeInfo_.status == AUTO_TIME_STATUS_ON;
}

bool NtpUpdateTime::IsValidNITZTime()
{
    if (nitzUpdateTimeMilli_ == 0) {
        return false;
    }
    int64_t bootTimeNano = static_cast<int64_t>(steady_clock::now().time_since_epoch().count());
    int64_t bootTimeMilli = bootTimeNano / NANO_TO_MILLISECOND;
    TIME_HILOGI(TIME_MODULE_SERVICE, "nitz update time: %{public}" PRIu64 " currentTime: %{public}" PRId64 "",
        nitzUpdateTimeMilli_, bootTimeMilli);
    return (bootTimeMilli - static_cast<int64_t>(nitzUpdateTimeMilli_)) < HALF_DAY_TO_MILLISECOND;
}

void NtpUpdateTime::StartTimer()
{
    TimeSystemAbility::GetInstance()->StartTimer(timerId_, nextTriggerTime_);
}

void NtpUpdateTime::Stop()
{
    TIME_HILOGD(TIME_MODULE_SERVICE, "start");
    TimeSystemAbility::GetInstance()->DestroyTimer(timerId_);
}

void NtpUpdateTime::RegisterSystemParameterListener()
{
    TIME_HILOGD(TIME_MODULE_SERVICE, "register system parameter modify lister");
    auto specificNtpResult = SystemWatchParameter(NTP_SERVER_SPECIFIC_SYSTEM_PARAMETER,
                                                  ChangeNtpServerCallback, nullptr);
    if (specificNtpResult != E_TIME_OK) {
        TIME_HILOGE(TIME_MODULE_SERVICE, "register specific ntp server lister fail: %{public}d", specificNtpResult);
    }

    auto netResult = SystemWatchParameter(NTP_SERVER_SYSTEM_PARAMETER, ChangeNtpServerCallback, nullptr);
    if (netResult != E_TIME_OK) {
        TIME_HILOGE(TIME_MODULE_SERVICE, "register ntp server lister fail: %{public}d", netResult);
    }

    auto switchResult = SystemWatchParameter(AUTO_TIME_SYSTEM_PARAMETER, ChangeAutoTimeCallback, nullptr);
    if (switchResult != E_TIME_OK) {
        TIME_HILOGE(TIME_MODULE_SERVICE, "register auto sync switch lister fail: %{public}d", switchResult);
    }
}

void NtpUpdateTime::ChangeNtpServerCallback(const char *key, const char *value, void *context)
{
    TIME_HILOGI(TIME_MODULE_SERVICE, "Ntp server changed");
    std::string ntpServer = system::GetParameter(NTP_SERVER_SYSTEM_PARAMETER, DEFAULT_NTP_SERVER);
    std::string ntpServerSpec = system::GetParameter(NTP_SERVER_SPECIFIC_SYSTEM_PARAMETER, "");
    if (ntpServer.empty() && ntpServerSpec.empty()) {
        TIME_HILOGW(TIME_MODULE_SERVICE, "No found ntp server from system parameter");
        return;
    }
    autoTimeInfo_.ntpServer = ntpServer;
    autoTimeInfo_.ntpServerSpec = ntpServerSpec;
    SetSystemTime();
}

void NtpUpdateTime::ChangeAutoTimeCallback(const char *key, const char *value, void *context)
{
    TIME_HILOGI(TIME_MODULE_SERVICE, "Auto sync switch changed");
    if (key == nullptr || value == nullptr) {
        TIME_HILOGE(TIME_MODULE_SERVICE, "key or value is nullptr");
        return;
    }
    if (std::string(AUTO_TIME_SYSTEM_PARAMETER).compare(key) != 0) {
        TIME_HILOGE(TIME_MODULE_SERVICE, "incorrect key:%{public}s", key);
        return;
    }

    if (std::string(AUTO_TIME_STATUS_ON).compare(value) != 0 && std::string(AUTO_TIME_STATUS_OFF).compare(value) != 0) {
        TIME_HILOGE(TIME_MODULE_SERVICE, "incorrect value:%{public}s", value);
        return;
    }
    autoTimeInfo_.status = std::string(value);
    SetSystemTime();
}

uint64_t NtpUpdateTime::GetNITZUpdateTime()
{
    return static_cast<uint64_t>(lastNITZUpdateTime_);
}
} // namespace MiscServices
} // namespace OHOS