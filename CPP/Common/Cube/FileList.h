/* ------------------------------------------------------------------------- */
///
/// FileList.h
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
#ifndef CUBE_ARCHIVE_FILELIST_H
#define CUBE_ARCHIVE_FILELIST_H

namespace Cube {
namespace Archive {
    class FileList {
    public:
        typedef TCHAR   char_type;
        typedef wchar_t unicode_type;
        typedef char    utf8_type;

        typedef std::basic_string<TCHAR>   system_string;
        typedef std::basic_string<wchar_t> unicode_string;
        typedef std::basic_string<char>    utf8_string;

    private:
        string_type _path;
    }
}}} // Cube::Archive::Encoding

#endif