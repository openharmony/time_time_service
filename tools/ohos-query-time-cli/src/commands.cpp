/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "commands.h"

#include <iostream>

#include "time_service_client.h"
#include "utils.h"

using namespace OHOS::MiscServices;

namespace OHOS {
namespace QueryTime {

// Command table definition
std::unordered_map<std::string, Command> g_commands;

// Get TimeServiceClient instance
static sptr<TimeServiceClient> GetTimeClient()
{
    auto client = TimeServiceClient::GetInstance();
    if (client == nullptr) {
        CLI_ERROR("Failed to get TimeServiceClient instance");
    }
    return client;
}

// get-wall-time command
int CmdGetWallTime()
{
    auto client = GetTimeClient();
    if (client == nullptr) {
        return OutputError("E_CLIENT_INIT", "Failed to initialize TimeServiceClient",
                           "Check if time_service system ability is running");
    }
    int64_t time = 0;
    int32_t ret = client->GetWallTimeMs(time);
    if (ret != 0) {
        return OutputError("E_GET_WALL_TIME", "Failed to get wall time",
                           "Check system time service status and permissions");
    }
    return OutputSuccess("time", std::to_string(time), "ms");
}

// get-boot-time command
int CmdGetBootTime()
{
    auto client = GetTimeClient();
    if (client == nullptr) {
        return OutputError("E_CLIENT_INIT", "Failed to initialize TimeServiceClient",
                           "Check if time_service system ability is running");
    }
    int64_t time = 0;
    int32_t ret = client->GetBootTimeMs(time);
    if (ret != 0) {
        return OutputError("E_GET_BOOT_TIME", "Failed to get boot time",
                           "Check system time service status");
    }
    return OutputSuccess("time", std::to_string(time), "ms");
}

// get-monotonic-time command
int CmdGetMonotonicTime()
{
    auto client = GetTimeClient();
    if (client == nullptr) {
        return OutputError("E_CLIENT_INIT", "Failed to initialize TimeServiceClient",
                           "Check if time_service system ability is running");
    }
    int64_t time = 0;
    int32_t ret = client->GetMonotonicTimeMs(time);
    if (ret != 0) {
        return OutputError("E_GET_MONOTONIC_TIME", "Failed to get monotonic time",
                           "Check system time service status");
    }
    return OutputSuccess("time", std::to_string(time), "ms");
}

// get-time-zone command
int CmdGetTimeZone()
{
    auto client = GetTimeClient();
    if (client == nullptr) {
        return OutputError("E_CLIENT_INIT", "Failed to initialize TimeServiceClient",
                           "Check if time_service system ability is running");
    }
    std::string timezone;
    int32_t ret = client->GetTimeZone(timezone);
    if (ret != 0) {
        return OutputError("E_GET_TIME_ZONE", "Failed to get time zone",
                           "Check system time service status");
    }
    return OutputSuccess("timezone", timezone, "");
}

// help command
int CmdHelp()
{
    std::cerr << TOOL_NAME << " - Query system time information\n\n";
    std::cerr << "Usage: " << TOOL_NAME << " <command>\n\n";
    std::cerr << "Commands:\n";
    for (const auto& pair : g_commands) {
        if (strcmp(pair.first.c_str(), "--help") != 0 &&
            strcmp(pair.first.c_str(), "--version") != 0) {
            std::cerr << "  " << pair.first << " - " << pair.second.description << std::endl;
        }
    }
    std::cerr << "\nOptions:\n";
    std::cerr << "  --help     Show this help message\n";
    std::cerr << "  --version  Show version information\n";
    return 0;
}

// version command
int CmdVersion()
{
    std::cout << "{\"success\":true,\"data\":{\"version\":\"" << VERSION << "\","
              << "\"tool\":\"" << TOOL_NAME << "\"}}" << std::endl;
    return 0;
}

// Initialize command table
void InitCommands()
{
    RegisterCmd("get-wall-time", "Get wall time (UTC time in milliseconds)", CmdGetWallTime);
    RegisterCmd("get-boot-time", "Get boot time (including sleep time)", CmdGetBootTime);
    RegisterCmd("get-monotonic-time", "Get monotonic time (excluding sleep time)", CmdGetMonotonicTime);
    RegisterCmd("get-time-zone", "Get current time zone", CmdGetTimeZone);
    RegisterCmd("--help", "Show help message", CmdHelp);
    RegisterCmd("--version", "Show version information", CmdVersion);
}

} // namespace QueryTime
} // namespace OHOS
