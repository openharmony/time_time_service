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

#ifndef SERVICES_INCLUDE_TIME_SERVICE_INTERFACE_H
#define SERVICES_INCLUDE_TIME_SERVICE_INTERFACE_H

#include "iremote_broker.h"
#include "itimer_info.h"
#include "want_agent_helper.h"
#include <unordered_set>

namespace OHOS {
namespace MiscServices {
class ITimeService : public IRemoteBroker {
public:

    enum APIVersion : int8_t {
        API_VERSION_7 = 0,
        API_VERSION_9 = 1,
    };
    /**
   * SetTime
   *
   * @param time int64_t set milliseconds
   * @return int32_t ERR_OK on success, other on failure.
   */
    virtual int32_t SetTime(int64_t time, APIVersion apiVersion = APIVersion::API_VERSION_7) = 0;
    /**
     * SetTimeZone
     *
     * @param timezoneId std::string &timezoneId string
     * @return int32_t ERR_OK on success, other on failure.
     */
    virtual int32_t SetTimeZone(const std::string &timezoneId, APIVersion apiVersion = APIVersion::API_VERSION_7) = 0;

    /**
     * GetTimeZone
     *
     * @param timezoneId std::string &timezoneId string
     * @return int32_t ERR_OK on success, other on failure.
     */
    virtual int32_t GetTimeZone(std::string &timezoneId) = 0;

    /**
    * GetThreadTimeMs
    *
    * @param times result of times ,unit: millisecond
    * @return int32_t ERR_OK on success, other on failure.
    */
    virtual int32_t GetThreadTimeMs(int64_t &times) = 0;

    /**
    * GetThreadTimeNs
    *
    * @param times result of times ,unit: Nanosecond
    * @return int32_t ERR_OK on success, other on failure.
    */
    virtual int32_t GetThreadTimeNs(int64_t &times) = 0;

    /**
     * CreateTimer
     *
     * @param type    timer type
     * @param repeat  is repeat or not
     * @param timerCallback remoteobject
     * @return uint64_t > 0 on success, == 0 failure.
     */
    virtual int32_t CreateTimer(const std::shared_ptr<ITimerInfo> &timerOptions, sptr<IRemoteObject> &timerCallback,
        uint64_t &timerId) = 0;

    /**
    * StartTimer
    *
    * @param timerId indicate timerId
    * @param treggerTime  trigger times
    * @return bool true on success, false on failure.
    */
    virtual int32_t StartTimer(uint64_t timerId, uint64_t triggerTime) = 0;

    /**
    * StopTimer
    *
    * @param timerId indicate timerId
    * @return bool true on success, false on failure.
    */
    virtual int32_t StopTimer(uint64_t timerId) = 0;

    /**
    * DestroyTimer
    *
    * @param timerId indicate timerId
    * @param isAsync indicate is async or not.
    * @return bool true on success, false on failure.
    */
    virtual int32_t DestroyTimer(uint64_t timerId, bool isAsync = false) = 0;

    /**
     * ProxyTimer
     * @param uid the uid
     * @param isProxy true if proxy, false if not proxy
     * @param needRetrigger true if need retrigger, false if not.
     * @return bool true on success, false on failure.
     */
    virtual bool ProxyTimer(int32_t uid, bool isProxy, bool needRetrigger) = 0;

    /**
     * ProxyTimer
     * @param pidList the pidlist
     * @param isProxy true if proxy, false if not proxy
     * @param needRetrigger true if need retrigger, false if not.
     * @return bool true on success, false on failure.
     */
    virtual bool ProxyTimer(int32_t uid, std::set<int> pidList, bool isProxy, bool needRetrigger) = 0;

    /**
     * AdjustTimer
     * @param isAdjust true if adjust, false if not adjust.
     * @param interval adjust period.
     * @return int32_t return error code.
     */
    virtual int32_t AdjustTimer(bool isAdjust, uint32_t interval) = 0;

    /**
     * SetTimerExemption
     * @param nameArr list for bundle name or proccess name.
     * @param isExemption exemption or ctrl.
     * @return int32_t return error code.
     */
    virtual int32_t SetTimerExemption(const std::unordered_set<std::string> &nameArr, bool isExemption) = 0;

    /**
     * ResetAllProxy
     * @return bool true on success, false on failure.
     */
    virtual bool ResetAllProxy() = 0;

    /**
     * @brief GetNtpTimeMs
     *
     * Obtain the wall time through ntp.
     *
     * @param time the wall time(the UTC time from 1970 0H:0M:0S) in milliseconds.
     * @return int32_t return error code.
     */
    virtual int32_t GetNtpTimeMs(int64_t &time) = 0;

    /**
     * @brief GetRealTimeMs
     *
     * Obtain the wall time calculated based on the last NTP time.
     *
     * @param time the wall time(the UTC time from 1970 0H:0M:0S) in milliseconds.
     * @return int32_t return error code.
     */
    virtual int32_t GetRealTimeMs(int64_t &time) = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.miscservices.time.ITimeService");
};
} // namespace MiscServices
} // namespace OHOS
#endif // SERVICES_INCLUDE_TIME_SERVICE_INTERFACE_H