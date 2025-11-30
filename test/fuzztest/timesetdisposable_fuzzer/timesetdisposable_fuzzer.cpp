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

#include "timesetdisposable_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <memory>

#include "itimer_info.h"

using namespace OHOS::MiscServices;

namespace {
constexpr size_t MAX_INPUT_SIZE = 1024;
constexpr size_t MIN_INPUT_SIZE = 1;

/**
 * Mock implementation of ITimerInfo for testing SetDisposable
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
 * Test ITimerInfo::SetDisposable with true value
 */
void TestSetDisposableTrue()
{
    auto timerInfo = std::make_shared<TestTimerInfo>();
    if (timerInfo == nullptr) {
        return;
    }

    // Test setting disposable to true
    timerInfo->SetDisposable(true);
}

/**
 * Test ITimerInfo::SetDisposable with false value
 */
void TestSetDisposableFalse()
{
    auto timerInfo = std::make_shared<TestTimerInfo>();
    if (timerInfo == nullptr) {
        return;
    }

    // Test setting disposable to false
    timerInfo->SetDisposable(false);
}

/**
 * Test ITimerInfo::SetDisposable with toggle behavior
 */
void TestSetDisposableToggle()
{
    auto timerInfo = std::make_shared<TestTimerInfo>();
    if (timerInfo == nullptr) {
        return;
    }

    // Toggle between true and false
    timerInfo->SetDisposable(true);
    timerInfo->SetDisposable(false);
    timerInfo->SetDisposable(true);
    timerInfo->SetDisposable(false);

    // Rapid toggling
    for (int i = 0; i < 100; i++) {
        timerInfo->SetDisposable(i % 2 == 0);
    }
}

/**
 * Test ITimerInfo::SetDisposable with fuzzer data
 */
void TestSetDisposableWithFuzzerData(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    auto timerInfo = std::make_shared<TestTimerInfo>();
    if (timerInfo == nullptr) {
        return;
    }

    // Use each byte of fuzzer data as a boolean value
    for (size_t i = 0; i < size && i < 100; i++) {
        bool disposableValue = (data[i] % 2 == 0);
        timerInfo->SetDisposable(disposableValue);
    }
}

/**
 * Test ITimerInfo::SetDisposable with repeat flag interaction
 * According to spec: disposable does not take effect for repeat timer
 */
void TestSetDisposableWithRepeat(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < 2) {
        return;
    }

    auto timerInfo = std::make_shared<TestTimerInfo>();
    if (timerInfo == nullptr) {
        return;
    }

    // Test disposable with non-repeat timer
    timerInfo->SetRepeat(false);
    timerInfo->SetDisposable(true);

    // Test disposable with repeat timer (should not take effect per spec)
    timerInfo->SetRepeat(true);
    timerInfo->SetDisposable(true);

    // Toggle repeat and disposable with fuzzer data
    bool repeatValue = (data[0] % 2 == 0);
    bool disposableValue = (data[1] % 2 == 0);

    timerInfo->SetRepeat(repeatValue);
    timerInfo->SetDisposable(disposableValue);

    // Test various combinations
    timerInfo->SetRepeat(true);
    timerInfo->SetDisposable(false);

    timerInfo->SetRepeat(false);
    timerInfo->SetDisposable(true);
}

/**
 * Test ITimerInfo::SetDisposable with multiple timer instances
 */
void TestSetDisposableMultipleInstances(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < 2) {
        return;
    }

    // Create multiple timer instances
    auto timer1 = std::make_shared<TestTimerInfo>();
    auto timer2 = std::make_shared<TestTimerInfo>();

    if (timer1 == nullptr || timer2 == nullptr) {
        return;
    }

    // Set different disposable values for each timer
    timer1->SetDisposable(true);
    timer2->SetDisposable(false);

    // Use fuzzer data to set disposable
    timer1->SetDisposable(data[0] % 2 == 0);
    timer2->SetDisposable(data[1] % 2 == 0);
}

/**
 * Test ITimerInfo::SetDisposable with boundary conditions
 */
void TestSetDisposableBoundary()
{
    auto timerInfo = std::make_shared<TestTimerInfo>();
    if (timerInfo == nullptr) {
        return;
    }

    // Test setting to true
    timerInfo->SetDisposable(true);

    // Test resetting to false
    timerInfo->SetDisposable(false);

    // Test with various integer values cast to bool
    timerInfo->SetDisposable(static_cast<bool>(0));      // false
    timerInfo->SetDisposable(static_cast<bool>(1));      // true
    timerInfo->SetDisposable(static_cast<bool>(255));    // true
    timerInfo->SetDisposable(static_cast<bool>(-1));     // true
}

/**
 * Test ITimerInfo::SetDisposable state consistency
 */
void TestSetDisposableStateConsistency(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < 1) {
        return;
    }

    auto timerInfo = std::make_shared<TestTimerInfo>();
    if (timerInfo == nullptr) {
        return;
    }

    // Set disposable with fuzzer data
    bool targetValue = (data[0] % 2 == 0);
    timerInfo->SetDisposable(targetValue);

    // Set again to test idempotency
    timerInfo->SetDisposable(targetValue);
}

/**
 * Test ITimerInfo::SetDisposable with all timer properties
 */
void TestSetDisposableWithAllProperties(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < 4) {
        return;
    }

    auto timerInfo = std::make_shared<TestTimerInfo>();
    if (timerInfo == nullptr) {
        return;
    }

    // Set all properties including disposable
    timerInfo->SetType(static_cast<int>(data[0]));
    timerInfo->SetRepeat(data[1] % 2 == 0);
    timerInfo->SetInterval(static_cast<uint64_t>(data[2]) * 1000 + 5000);  // >= 5000ms per spec
    timerInfo->SetDisposable(data[3] % 2 == 0);
}

} // namespace

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Validate input parameters
    if (data == nullptr || size < MIN_INPUT_SIZE) {
        return 0;
    }

    // Prevent excessive memory allocation
    if (size > MAX_INPUT_SIZE) {
        return 0;
    }

    // Test SetDisposable with true value
    TestSetDisposableTrue();

    // Test SetDisposable with false value
    TestSetDisposableFalse();

    // Test SetDisposable toggle behavior
    TestSetDisposableToggle();

    // Test SetDisposable with fuzzer data
    TestSetDisposableWithFuzzerData(data, size);

    // Test SetDisposable with repeat flag interaction
    if (size >= 2) {
        TestSetDisposableWithRepeat(data, size);
    }

    // Test SetDisposable with multiple instances
    if (size >= 2) {
        TestSetDisposableMultipleInstances(data, size);
    }

    // Test SetDisposable boundary conditions
    TestSetDisposableBoundary();

    // Test SetDisposable state consistency
    if (size >= 1) {
        TestSetDisposableStateConsistency(data, size);
    }

    // Test SetDisposable with all properties
    if (size >= 4) {
        TestSetDisposableWithAllProperties(data, size);
    }

    return 0;
}
