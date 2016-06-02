/* ------------------------------------------------------------------------- */
///
/// Path.h
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
#ifndef CUBE_ARCHIVE_PATH_H
#define CUBE_ARCHIVE_PATH_H

#include <algorithm>
#include <string>
#include <vector>
#include <filesystem>

namespace Cube {
namespace Archive {
    /* --------------------------------------------------------------------- */
    ///
    /// Path
    ///
    /// <summary>
    /// パスを扱うクラスです。
    /// </summary>
    ///
    /* --------------------------------------------------------------------- */
    class Path {
    public:
        typedef wchar_t unicode_type;
        typedef std::basic_string<wchar_t> unicode_string;
        
        /* ----------------------------------------------------------------- */
        ///
        /// Normalize
        ///
        /// <summary>
        /// Windows のパスとして利用できるように正規化します。
        /// </summary>
        ///
        /* ----------------------------------------------------------------- */
        static unicode_string Normalize(const unicode_string& src) {
            namespace fs = std::tr2::sys;

            try {
                fs::path path(Escape(src));
                fs::path dest;

                for (auto& part : path) {
                    auto stem = part.stem().generic_wstring();
                    if (part == L"." || part == L"..") stem = L"\\";

                    auto pos = std::find(Reserved().begin(), Reserved().end(), stem);
                    if (pos != Reserved().end()) stem.insert(0, L"_");

                    dest /= fs::path(stem + part.extension().generic_wstring());
                }

                return dest.generic_wstring();
            }
            catch (...) { return src; }
        }

    private:
        /* ----------------------------------------------------------------- */
        ///
        /// Escape
        ///
        /// <summary>
        /// 使用不可能な文字を "_" に置き換えます。
        /// </summary>
        ///
        /* ----------------------------------------------------------------- */
        static unicode_string Escape(const unicode_string& src) {
            auto dest = src;
            for (auto& c : dest) {
                if (c == L'*' ||
                    c == L'"' ||
                    c == L'<' ||
                    c == L'>' ||
                    c == L'|' ||
                    c == L'?' ||
                    c == L':') c = L'_';
            }
            return dest;
        }

        /* ----------------------------------------------------------------- */
        ///
        /// Reserved
        ///
        /// <summary>
        /// 予約されている文字列の一覧を取得します。
        /// </summary>
        ///
        /* ----------------------------------------------------------------- */
        static const std::vector<unicode_string>& Reserved() {
            static const std::vector<unicode_string> map = {
                L"CON",
                L"PRN",
                L"AUX",
                L"NUL",

                L"COM0",
                L"COM1",
                L"COM2",
                L"COM3",
                L"COM4",
                L"COM5",
                L"COM6",
                L"COM7",
                L"COM8",
                L"COM9",

                L"LPT0",
                L"LPT1",
                L"LPT2",
                L"LPT3",
                L"LPT4",
                L"LPT5",
                L"LPT6",
                L"LPT7",
                L"LPT8",
                L"LPT9"
            };
            return map;
        }
    };
}} // Cube::Archive

#endif // CUBE_ARCHIVE_A