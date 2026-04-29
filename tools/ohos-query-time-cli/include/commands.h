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

#ifndef OHOS_QUERY_TIME_COMMANDS_H
#define OHOS_QUERY_TIME_COMMANDS_H

#include <functional>
#include <string>
#include <unordered_map>

namespace OHOS {
namespace QueryTime {

// Command handler type - no parameter version
using CommandHandler = std::function<int()>;

// Command table structure
struct Command {
    const char* description;
    CommandHandler handler;
};

// Command table
extern std::unordered_map<std::string, Command> g_commands;

// Register command function
inline void RegisterCmd(const std::string& name, const char* desc, const CommandHandler& handler)
{
    g_commands[name] = {desc, handler};
}

// Command implementations - no parameter
int CmdGetWallTime();
int CmdGetBootTime();
int CmdGetMonotonicTime();
int CmdGetTimeZone();
int CmdHelp();
int CmdVersion();

// Initialize command table
void InitCommands();

} // namespace QueryTime
} // namespace OHOS

#endif // OHOS_QUERY_TIME_COMMANDS_H
