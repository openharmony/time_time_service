/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "timesettimezone_fuzzer.h"
#include "time_common.h"

#include <cstddef>
#include <cstdint>
#include <string_ex.h>

#include "time_service_client.h"
#include <fuzzer/FuzzedDataProvider.h>

using namespace OHOS::MiscServices;

namespace OHOS {

bool FuzzTimeSetTimezone(const uint8_t *rawData, size_t size)
{
    std::string timeZone(reinterpret_cast<const char *>(rawData), size);
    TimeServiceClient::GetInstance()->SetTimeZone(timeZone);
    TimeServiceClient::GetInstance()->SetTimeZone(timeZone);
    TimeServiceClient::GetInstance()->SetTimeZoneV9(timeZone);
    int32_t code;
    TimeServiceClient::GetInstance()->SetTimeZone(timeZone, code);
    return true;
}

bool FuzzTimeGetTimezone(const uint8_t *rawData, size_t size)
{
    TimeServiceClient::GetInstance()->GetTimeZone();
    std::string timezoneId(reinterpret_cast<const char *>(rawData), size);
    TimeServiceClient::GetInstance()->GetTimeZone(timezoneId);
    return true;
}

bool FuzzTimeSetTime(const uint8_t *rawData, size_t size)
{
    int64_t time = static_cast<int64_t>(*rawData);
    TimeServiceClient::GetInstance()->SetTime(time);
    TimeServiceClient::GetInstance()->SetTime(time);
    TimeServiceClient::GetInstance()->SetTimeV9(time);
    TimeServiceClient::GetInstance()->SetTimeV9(time);
    int32_t code;
    TimeServiceClient::GetInstance()->SetTime(time, code);
    return true;
}

bool FuzzTimeGetTime(const uint8_t *rawData, size_t size)
{
    FuzzedDataProvider fdp(rawData, size);
    TimeServiceClient::GetInstance()->GetWallTimeMs();
    int64_t time = fdp.ConsumeIntegral<int64_t>();
    TimeServiceClient::GetInstance()->GetWallTimeMs(time);
    TimeServiceClient::GetInstance()->GetWallTimeNs();
    time = fdp.ConsumeIntegral<int64_t>();
    TimeServiceClient::GetInstance()->GetWallTimeNs(time);

    TimeServiceClient::GetInstance()->GetBootTimeMs();
    time = fdp.ConsumeIntegral<int64_t>();
    TimeServiceClient::GetInstance()->GetBootTimeMs(time);
    TimeServiceClient::GetInstance()->GetBootTimeNs();
    time = fdp.ConsumeIntegral<int64_t>();
    TimeServiceClient::GetInstance()->GetBootTimeNs(time);

    TimeServiceClient::GetInstance()->GetMonotonicTimeMs();
    time = fdp.ConsumeIntegral<int64_t>();
    TimeServiceClient::GetInstance()->GetMonotonicTimeMs(time);
    TimeServiceClient::GetInstance()->GetMonotonicTimeNs();
    time = fdp.ConsumeIntegral<int64_t>();
    TimeServiceClient::GetInstance()->GetMonotonicTimeNs(time);

    TimeServiceClient::GetInstance()->GetThreadTimeMs();
    time = fdp.ConsumeIntegral<int64_t>();
    TimeServiceClient::GetInstance()->GetThreadTimeMs(time);
    TimeServiceClient::GetInstance()->GetThreadTimeNs();
    time = fdp.ConsumeIntegral<int64_t>();
    TimeServiceClient::GetInstance()->GetThreadTimeNs(time);
    return true;
}

bool FuzzTimeGetNtpTimeMs(const uint8_t *rawData, size_t size)
{
    FuzzedDataProvider fdp(rawData, size);
    int64_t time = fdp.ConsumeIntegral<int64_t>();
    TimeServiceClient::GetInstance()->GetNtpTimeMs(time);
    return true;
}

bool FuzzTimeGetRealTimeMs(const uint8_t *rawData, size_t size)
{
    FuzzedDataProvider fdp(rawData, size);
    int64_t time = fdp.ConsumeIntegral<int64_t>();
    TimeServiceClient::GetInstance()->GetRealTimeMs(time);
    return true;
}

bool FuzzTimeTimeUtilsGetWallTimeMs(const uint8_t *rawData, size_t size)
{
    FuzzedDataProvider fdp(rawData, size);
    int64_t time = fdp.ConsumeIntegral<int64_t>();
    TimeUtils::GetWallTimeMs(time);
    return true;
}

bool FuzzTimeTimeUtilsGetBootTimeNs(const uint8_t *rawData, size_t size)
{
    FuzzedDataProvider fdp(rawData, size);
    int64_t time = fdp.ConsumeIntegral<int64_t>();
    TimeUtils::GetBootTimeNs(time);
    TimeUtils::GetBootTimeNs();
    return true;
}

bool FuzzTimeTimeUtilsGetBootTimeMs(const uint8_t *rawData, size_t size)
{
    FuzzedDataProvider fdp(rawData, size);
    int64_t time = fdp.ConsumeIntegral<int64_t>();
    TimeUtils::GetBootTimeNs(time);
    return true;
}

}
/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    OHOS::FuzzTimeSetTimezone(data, size);
    OHOS::FuzzTimeGetTimezone(data, size);
    OHOS::FuzzTimeSetTime(data, size);
    OHOS::FuzzTimeGetTime(data, size);
    OHOS::FuzzTimeGetNtpTimeMs(data, size);
    OHOS::FuzzTimeGetRealTimeMs(data, size);
    OHOS::FuzzTimeTimeUtilsGetWallTimeMs(data, size);
    OHOS::FuzzTimeTimeUtilsGetBootTimeNs(data, size);
    OHOS::FuzzTimeTimeUtilsGetBootTimeMs(data, size);
    return 0;
}