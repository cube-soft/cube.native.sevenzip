/* ------------------------------------------------------------------------- */
///
/// Conversion.h
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
#ifndef CUBE_ENCODING_CONVERSION_H
#define CUBE_ENCODING_CONVERSION_H

#include <string>
#include <winnls.h>
#include <babel/babel.h>
#include "Encoding.h"
#include "Utf8Mac.h"

namespace Cube {
namespace Encoding {
    namespace Details {
        class Initializer {
        public:
            Initializer() { babel::init_babel(); }
        };
    }

    /* --------------------------------------------------------------------- */
    ///
    /// Conversion
    ///
    /// <summary>
    /// 文字コードを変換するためのクラスです。
    /// </summary>
    ///
    /* --------------------------------------------------------------------- */
    class Conversion {
    public:
        typedef std::basic_string<char>    ascii_string;
        typedef std::basic_string<wchar_t> unicode_string;

        /* ----------------------------------------------------------------- */
        ///
        /// Initialize
        ///
        /// <summary>
        /// 初期化を行います。
        /// </summary>
        ///
        /// <remarks>
        /// TODO: 最終的には Initialize 関数をなくす方法を検討する。
        /// </remarks>
        ///
        /* ----------------------------------------------------------------- */
        static void Initialize() {
            static Details::Initializer initializer;
        }

        /* ----------------------------------------------------------------- */
        ///
        /// Guess
        ///
        /// <summary>
        /// 文字コードを推測します。
        /// </summary>
        ///
        /* ----------------------------------------------------------------- */
        static int Guess(const ascii_string& src) {
            auto result = babel::analyze_base_encoding(src).get_strict_result();
            switch (result) {
            case Ascii:
            case ShiftJis:
                return ShiftJis;
            case EucJp:
            case Jis:
            case Utf8:
            case Unicode:
                return result;
            default:
                break;
            }
            return Unknown;
        }

        /* ----------------------------------------------------------------- */
        ///
        /// ToUnicode
        ///
        /// <summary>
        /// Unicode に変換します。
        /// </summary>
        ///
        /* ----------------------------------------------------------------- */
        static unicode_string ToUnicode(const ascii_string& src, int encoding) {
            switch(encoding) {
            case ShiftJis:
                return babel::sjis_to_unicode(src);
            case EucJp:
                return babel::euc_to_unicode(src);
            case Jis:
                return babel::jis_to_unicode(src);
            case Utf8:
            {
                auto copy = src;
                Utf8Mac::Normalize(copy);
                return babel::utf8_to_unicode(copy);
            }
            case Unicode:
            default:
                break;
            }
            return Widen(src);
        }

        /* ----------------------------------------------------------------- */
        ///
        /// ToUnicode
        ///
        /// <summary>
        /// Unicode に変換します。
        /// </summary>
        ///
        /* ----------------------------------------------------------------- */
        static unicode_string ToUnicode(const ascii_string& src) {
            auto encoding = Guess(src);
            return ToUnicode(src, encoding);
        }

        /* ----------------------------------------------------------------- */
        ///
        /// ToUtf8
        ///
        /// <summary>
        /// UTF-8 に変換します。
        /// </summary>
        ///
        /* ----------------------------------------------------------------- */
        static ascii_string ToUtf8(const unicode_string& src) {
            return babel::unicode_to_utf8(src);
        }

    private:
        /* ----------------------------------------------------------------- */
        ///
        /// Widen
        ///
        /// <summary>
        /// string を wstring に変換します。
        /// </summary>
        ///
        /* ----------------------------------------------------------------- */
        static unicode_string Widen(const ascii_string& src) {
            wchar_t buffer[2048] = {};
            auto result = ::MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, buffer, sizeof(buffer) / sizeof(wchar_t));
            return (result != 0) ? unicode_string(buffer) : unicode_string();
        }
    }; // Conversion
}} // Cube::Encoding

#endif // CUBE_ENCODING_CONVERSION_H