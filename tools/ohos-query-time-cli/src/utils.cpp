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

#include "utils.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>

namespace OHOS {
namespace QueryTime {

namespace {
    const std::unordered_map<char, const char*> JSON_ESCAPE_MAP = {
        {'"', "\\\""},
        {'\\', "\\\\"},
        {'\b', "\\b"},
        {'\f', "\\f"},
        {'\n', "\\n"},
        {'\r', "\\r"},
        {'\t', "\\t"},
    };
    constexpr const char* JSON_ESC_UNICODE = "\\u";
    constexpr int UNICODE_ESCAPE_WIDTH = 4;
}

static std::string JsonEscape(const std::string& str)
{
    std::ostringstream oss;
    for (char c : str) {
        auto it = JSON_ESCAPE_MAP.find(c);
        if (it != JSON_ESCAPE_MAP.end()) {
            oss << it->second;
        } else if (static_cast<unsigned char>(c) < 0x20) {
            oss << JSON_ESC_UNICODE << std::hex << std::uppercase;
            oss << std::setw(UNICODE_ESCAPE_WIDTH) << std::setfill('0') << static_cast<int>(c);
            oss << std::dec << std::nouppercase;
        } else {
            oss << c;
        }
    }
    return oss.str();
}

const char* VERSION = "1.0.0";
const char* TOOL_NAME = "ohos-query-time";

int OutputSuccess(const std::string& dataField, const std::string& dataValue,
                  const std::string& unit)
{
    std::cout << "{\"success\":true,\"data\":{\"" << JsonEscape(dataField) << "\":\"" <<
        JsonEscape(dataValue) << "\"";
    if (!unit.empty()) {
        std::cout << ",\"unit\":\"" << JsonEscape(unit) << "\"";
    }
    std::cout << "}}" << std::endl;
    return 0;
}

int OutputError(const std::string& code, const std::string& message,
                const std::string& suggestion)
{
    std::cout << "{\"success\":false,\"error\":{\"code\":\"" << JsonEscape(code) << "\"," <<
        "\"message\":\"" << JsonEscape(message) << "\"},\"suggestion\":\"" <<
        JsonEscape(suggestion) << "\"}" << std::endl;
    return 1;
}

} // namespace QueryTime
} // namespace OHOS
