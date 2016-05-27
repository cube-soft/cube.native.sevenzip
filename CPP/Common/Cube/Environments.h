/* ------------------------------------------------------------------------- */
///
/// Environments.h
/// 
/// Copyright (c) 2010 CubeSoft, Inc.
/// 
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///  http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/* ------------------------------------------------------------------------- */
#ifndef CUBE_ARCHIVE_ENVIRONMENTS_H
#define CUBE_ARCHIVE_ENVIRONMENTS_H

#include <string>
#include <vector>
#include <windows.h>
#include "Encoding.h"

namespace Cube {
namespace Archive {
namespace Environments {
    /* --------------------------------------------------------------------- */
    //  GetEncoding
    /* --------------------------------------------------------------------- */
    inline int GetEncoding() {
        TCHAR buffer[32] = {};
        auto result = ::GetEnvironmentVariable(_T("Encoding"), buffer, sizeof(buffer) / sizeof(TCHAR));
        if (result == 0) return Cube::Encoding::Unknown;
        return _ttoi(buffer);
    }

    /* --------------------------------------------------------------------- */
    //  SetEncoding
    /* --------------------------------------------------------------------- */
    inline void SetEncoding(int encoding) {
        TCHAR buffer[32] = {};
        ::_itot_s(encoding , buffer, sizeof(buffer) / sizeof(TCHAR), 10);
        ::SetEnvironmentVariable(_T("Encoding"), buffer);
    }
    
    inline std::vector<std::basic_string<char>> GetWarning() {
        std::vector<std::basic_string<char>> dest;
        
    }
}}} // Cube::Archive::Environments

#endif // CUBE_ARCHIVE_ENVIRONMENTS_H