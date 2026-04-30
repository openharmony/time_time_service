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

#include <iostream>

#include "commands.h"
#include "utils.h"

using namespace OHOS::QueryTime;

static void PrintUsage(const char* prog)
{
    CLI_ERROR(std::string("Usage: ") + prog + " <command>");
    CLI_ERROR(std::string("Run '") + prog + " --help' for more information");
}

constexpr int MIN_ARGC = 2;
constexpr int COMMAND_ARG_INDEX = 1;
constexpr int CMD_NAME_ARG_COUNT = 2;  // Number of args to skip (program name + command name)

int main(int argc, char* argv[])
{
    if (argc < MIN_ARGC) {
        PrintUsage(argv[0]);
        return OutputError("E_NO_COMMAND", "No command specified",
            "Specify a command or use --help for usage");
    }

    InitCommands();

    std::string cmdName = argv[COMMAND_ARG_INDEX];
    auto it = g_commands.find(cmdName);
    if (it == g_commands.end()) {
        std::cout << "{\"success\":false,\"error\":{\"code\":\"E_UNKNOWN_COMMAND\", " <<
            "\"message\":\"Unknown command: " << cmdName << "\"}, " <<
            "\"suggestion\":\"Use --help to see available commands\"}" << std::endl;
        return 1;
    }

    // Invoke command handler with remaining parameters
    return it->second.handler(argc - CMD_NAME_ARG_COUNT, argv + CMD_NAME_ARG_COUNT);
}
