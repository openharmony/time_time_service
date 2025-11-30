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
constexpr int TOGGLE_COUNT = 100;
constexpr size_t MAX_TOGGLE_BYTES = 100;
constexpr size_t MIN_REPEAT_SIZE = 2;
constexpr size_t MIN_MULTIPLE_INSTANCES_SIZE = 2;
constexpr size_t MIN_ALL_PROPERTIES_SIZE = 4;
constexpr size_t MIN_STATE_CONSISTENCY_SIZE = 1;
constexpr uint64_t INTERVAL_UNIT_MS = 1000;
constexpr uint64_t MIN_INTERVAL_MS = 5000;
constexpr int EVEN_DIVISOR = 2;
constexpr int BOOL_FALSE_VALUE = 0;
constexpr int BOOL_TRUE_VALUE = 1;
constexpr int BOOL_NON_ZERO_VALUE = 255;

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
    for (int i = 0; i < TOGGLE_COUNT; i++) {
        timerInfo->SetDisposable(i % EVEN_DIVISOR == 0);
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
    for (size_t i = 0; i < size && i < MAX_TOGGLE_BYTES; i++) {
        bool disposableValue = (data[i] % EVEN_DIVISOR == 0);
        timerInfo->SetDisposable(disposableValue);
    }
}

/**
 * Test ITimerInfo::SetDisposable with repeat flag interaction
 * According to spec: disposable does not take effect for repeat timer
 */
void TestSetDisposableWithRepeat(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < MIN_REPEAT_SIZE) {
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
    bool repeatValue = (data[0] % EVEN_DIVISOR == 0);
    bool disposableValue = (data[1] % EVEN_DIVISOR == 0);

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
    if (data == nullptr || size < MIN_MULTIPLE_INSTANCES_SIZE) {
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
    timer1->SetDisposable(data[0] % EVEN_DIVISOR == 0);
    timer2->SetDisposable(data[1] % EVEN_DIVISOR == 0);
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
    timerInfo->SetDisposable(static_cast<bool>(BOOL_FALSE_VALUE));      // false
    timerInfo->SetDisposable(static_cast<bool>(BOOL_TRUE_VALUE));       // true
    timerInfo->SetDisposable(static_cast<bool>(BOOL_NON_ZERO_VALUE));   // true
    timerInfo->SetDisposable(static_cast<bool>(-1));     // true
}

/**
 * Test ITimerInfo::SetDisposable state consistency
 */
void TestSetDisposableStateConsistency(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < MIN_STATE_CONSISTENCY_SIZE) {
        return;
    }

    auto timerInfo = std::make_shared<TestTimerInfo>();
    if (timerInfo == nullptr) {
        return;
    }

    // Set disposable with fuzzer data
    bool targetValue = (data[0] % EVEN_DIVISOR == 0);
    timerInfo->SetDisposable(targetValue);

    // Set again to test idempotency
    timerInfo->SetDisposable(targetValue);
}

/**
 * Test ITimerInfo::SetDisposable with all timer properties
 */
void TestSetDisposableWithAllProperties(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < MIN_ALL_PROPERTIES_SIZE) {
        return;
    }

    auto timerInfo = std::make_shared<TestTimerInfo>();
    if (timerInfo == nullptr) {
        return;
    }

    // Set all properties including disposable
    timerInfo->SetType(static_cast<int>(data[0]));
    timerInfo->SetRepeat(data[1] % EVEN_DIVISOR == 0);
    timerInfo->SetInterval(static_cast<uint64_t>(data[2]) * INTERVAL_UNIT_MS + MIN_INTERVAL_MS);  // >= 5000ms per spec
    timerInfo->SetDisposable(data[3] % EVEN_DIVISOR == 0);
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
    if (size >= MIN_REPEAT_SIZE) {
        TestSetDisposableWithRepeat(data, size);
    }

    // Test SetDisposable with multiple instances
    if (size >= MIN_MULTIPLE_INSTANCES_SIZE) {
        TestSetDisposableMultipleInstances(data, size);
    }

    // Test SetDisposable boundary conditions
    TestSetDisposableBoundary();

    // Test SetDisposable state consistency
    if (size >= MIN_STATE_CONSISTENCY_SIZE) {
        TestSetDisposableStateConsistency(data, size);
    }

    // Test SetDisposable with all properties
    if (size >= MIN_ALL_PROPERTIES_SIZE) {
        TestSetDisposableWithAllProperties(data, size);
    }

    return 0;
}
