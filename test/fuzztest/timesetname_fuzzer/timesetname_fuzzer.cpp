/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "timesetname_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

#include "itimer_info.h"

using namespace OHOS::MiscServices;

namespace {
constexpr size_t MAX_INPUT_SIZE = 1024;
constexpr size_t MAX_STRING_LEN = 512;
constexpr size_t MIN_MULTIPLE_INSTANCES_SIZE = 2;
constexpr size_t MIN_NULL_CHARS_SIZE = 4;
constexpr size_t MAX_SPECIAL_FUZZ_NAME_LEN = 32;
constexpr size_t MAX_MULTI_INSTANCE_FUZZ_NAME_LEN = 64;

/**
 * Mock implementation of ITimerInfo for testing SetName
 */
class TestTimerInfo : public ITimerInfo {
public:
    TestTimerInfo() = default;
    virtual ~TestTimerInfo() = default;

    void SetType(const int &type) override
    {
        this->type = type;
    }

    void SetRepeat(bool repeat) override
    {
        this->repeat = repeat;
    }

    void SetInterval(const uint64_t &interval) override
    {
        this->interval = interval;
    }

    void SetWantAgent(std::shared_ptr<OHOS::AbilityRuntime::WantAgent::WantAgent> wantAgent) override
    {
        this->wantAgent = wantAgent;
    }

    void OnTrigger() override
    {
        // Mock implementation
    }
};

/**
 * Test ITimerInfo::SetName with raw string input
 */
void TestSetNameWithRawString(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0 || size > MAX_STRING_LEN) {
        return;
    }

    auto timerInfo = std::make_shared<TestTimerInfo>();
    if (timerInfo == nullptr) {
        return;
    }

    // Test SetName with raw fuzzer data
    std::string name(reinterpret_cast<const char*>(data), size);
    timerInfo->SetName(name);
}

/**
 * Test ITimerInfo::SetName with empty string
 */
void TestSetNameWithEmptyString()
{
    auto timerInfo = std::make_shared<TestTimerInfo>();
    if (timerInfo == nullptr) {
        return;
    }

    // Test with empty string
    timerInfo->SetName("");
}

/**
 * Test ITimerInfo::SetName with special characters
 */
void TestSetNameWithSpecialChars(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    auto timerInfo = std::make_shared<TestTimerInfo>();
    if (timerInfo == nullptr) {
        return;
    }

    // Test with various special characters
    std::string specialNames[] = {
        "\n\r\t",
        "timer@#$%",
        "timer_test",
        "timer-test",
        "TIMER_123"
    };

    for (const auto& name : specialNames) {
        timerInfo->SetName(name);
    }

    // Test with fuzzer data
    if (size > 0) {
        std::string fuzzName(reinterpret_cast<const char*>(data),
            std::min(size, static_cast<size_t>(MAX_SPECIAL_FUZZ_NAME_LEN)));
        timerInfo->SetName(fuzzName);
    }
}

/**
 * Test ITimerInfo::SetName with boundary conditions
 */
void TestSetNameBoundary()
{
    auto timerInfo = std::make_shared<TestTimerInfo>();
    if (timerInfo == nullptr) {
        return;
    }

    // Test with maximum length string
    std::string maxName(MAX_STRING_LEN, 'X');
    timerInfo->SetName(maxName);

    // Test with single character
    timerInfo->SetName("X");

    // Test overwriting previous name
    timerInfo->SetName("FirstTimer");
    timerInfo->SetName("SecondTimer");
}

/**
 * Test ITimerInfo::SetName with multiple timer instances
 */
void TestSetNameMultipleInstances(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < MIN_MULTIPLE_INSTANCES_SIZE) {
        return;
    }

    // Create multiple timer instances
    auto timer1 = std::make_shared<TestTimerInfo>();
    auto timer2 = std::make_shared<TestTimerInfo>();
    if (timer1 == nullptr || timer2 == nullptr) {
        return;
    }

    // Set same name for multiple timers
    timer1->SetName("SameTimerName");
    timer2->SetName("SameTimerName");

    // Set different names
    timer1->SetName("Timer1");
    timer2->SetName("Timer2");

    // Use fuzzer data
    size_t len = std::min(size / 2, static_cast<size_t>(MAX_MULTI_INSTANCE_FUZZ_NAME_LEN));
    if (len > 0) {
        std::string fuzzName(reinterpret_cast<const char*>(data), len);
        timer1->SetName(fuzzName);
    }
}

/**
 * Test ITimerInfo::SetName with null characters in string
 */
void TestSetNameWithNullChars(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < MIN_NULL_CHARS_SIZE) {
        return;
    }

    auto timerInfo = std::make_shared<TestTimerInfo>();
    if (timerInfo == nullptr) {
        return;
    }

    // Create string with embedded null characters
    std::string nameWithNull;
    nameWithNull.push_back('A');
    nameWithNull.push_back('\0');
    nameWithNull.push_back('B');

    timerInfo->SetName(nameWithNull);

    // Test with raw data that may contain nulls
    std::string rawName(reinterpret_cast<const char*>(data), std::min(size, MAX_STRING_LEN));
    timerInfo->SetName(rawName);
}

} // namespace

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Validate input parameters
    if (data == nullptr || size == 0) {
        return 0;
    }

    // Prevent excessive memory allocation
    if (size > MAX_INPUT_SIZE) {
        return 0;
    }

    // Test SetName with raw string input
    TestSetNameWithRawString(data, size);

    // Test SetName with empty string
    TestSetNameWithEmptyString();

    // Test SetName with special characters
    TestSetNameWithSpecialChars(data, size);

    // Test SetName with boundary conditions
    TestSetNameBoundary();

    // Test SetName with multiple instances
    if (size >= MIN_MULTIPLE_INSTANCES_SIZE) {
        TestSetNameMultipleInstances(data, size);
    }

    // Test SetName with null characters
    if (size >= MIN_NULL_CHARS_SIZE) {
        TestSetNameWithNullChars(data, size);
    }

    return 0;
}
