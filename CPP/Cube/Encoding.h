/* ------------------------------------------------------------------------- */
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
#pragma once

#include <string>
#include <winnls.h>
#include "babel/babel.h"
#include "EncodingMac.h"

namespace Cube {
namespace Encoding {

namespace Details {
    class Initializer {
    public:
        Initializer() { babel::init_babel(); }
    };
}

/* ------------------------------------------------------------------------- */
///
/// Kinds
///
/// <summary>
/// 文字コードを表す列挙型です。
/// </summary>
///
/* ------------------------------------------------------------------------- */
enum Kinds {
    Ascii           = babel::base_encoding::ansi,
    Jis             = babel::base_encoding::jis,
    ShiftJis        = babel::base_encoding::sjis,
    EucJp           = babel::base_encoding::euc,
    Utf8            = babel::base_encoding::utf8,
    Unicode         = babel::base_encoding::utf16,
    Unknown         = -1
};

/* ------------------------------------------------------------------------- */
///
/// Conversion
///
/// <summary>
/// 文字コードを変換するためのクラスです。
/// </summary>
///
/* ------------------------------------------------------------------------- */
class Conversion {
public:
    typedef std::basic_string<char>    ascii_string;
    typedef std::basic_string<wchar_t> unicode_string;

    /* --------------------------------------------------------------------- */
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
    /* --------------------------------------------------------------------- */
    static void Initialize() {
        static Details::Initializer initializer;
    }

    /* --------------------------------------------------------------------- */
    ///
    /// Guess
    ///
    /// <summary>
    /// 文字コードを推測します。
    /// </summary>
    ///
    /* --------------------------------------------------------------------- */
    static int Guess(const ascii_string& src) {
        auto result = babel::analyze_base_encoding(src).get_strict_result();
        switch (result) {
        case babel::base_encoding::ansi:
        case babel::base_encoding::sjis:
        case babel::base_encoding::euc:
        case babel::base_encoding::jis:
        case babel::base_encoding::utf8:
        case babel::base_encoding::utf16:
            return result;
        default:
            return Unknown;
        }
    }

    /* --------------------------------------------------------------------- */
    ///
    /// ToUnicode
    ///
    /// <summary>
    /// Unicode に変換します。
    /// </summary>
    ///
    /* --------------------------------------------------------------------- */
    static unicode_string ToUnicode(const ascii_string& src, int encoding) {
        switch(encoding) {
        case Ascii:
        case ShiftJis:
            return babel::sjis_to_unicode(src);
        case EucJp:
            return babel::euc_to_unicode(src);
        case Jis:
            return babel::jis_to_unicode(src);
        case Utf8:
        {
            auto copy = src;
            Mac::Normalize(copy);
            return babel::utf8_to_unicode(copy);
        }
        case Unicode:
        default:
            break;
        }
        return Widen(src);
    }

    /* --------------------------------------------------------------------- */
    ///
    /// ToUnicode
    ///
    /// <summary>
    /// Unicode に変換します。
    /// </summary>
    ///
    /* --------------------------------------------------------------------- */
    static unicode_string ToUnicode(const ascii_string& src) {
        return ToUnicode(src, Guess(src));
    }

    /* --------------------------------------------------------------------- */
    ///
    /// ToUtf8
    ///
    /// <summary>
    /// UTF-8 に変換します。
    /// </summary>
    ///
    /* --------------------------------------------------------------------- */
    static ascii_string ToUtf8(const unicode_string& src) {
        return babel::unicode_to_utf8(src);
    }

    /* --------------------------------------------------------------------- */
    ///
    /// ToShiftJis
    ///
    /// <summary>
    /// Shift_JIS に変換します。
    /// </summary>
    ///
    /* --------------------------------------------------------------------- */
    static ascii_string ToShiftJis(const unicode_string& src) {
        return babel::unicode_to_sjis(src);
    }
    
    /* --------------------------------------------------------------------- */
    ///
    /// Widen
    ///
    /// <summary>
    /// 標準 API を用いて string を wstring に変換します。
    /// </summary>
    ///
    /* --------------------------------------------------------------------- */
    static unicode_string Widen(const ascii_string& src, UINT cp = CP_OEMCP) {
        wchar_t buffer[2048] = {};
        auto count  = static_cast<int>(sizeof(buffer) / sizeof(buffer[0]));
        auto result = ::MultiByteToWideChar(cp, 0, src.c_str(), -1, buffer, count);
        return (result != 0) ? unicode_string(buffer) : unicode_string();
    }

    /* --------------------------------------------------------------------- */
    ///
    /// Narrow
    ///
    /// <summary>
    /// 標準 API を用いて wstring を string に変換します。
    /// </summary>
    ///
    /* --------------------------------------------------------------------- */
    static ascii_string Narrow(const unicode_string& src, UINT cp = CP_OEMCP) {
        char buffer[2048] = {};
        auto count  = static_cast<int>(sizeof(buffer) / sizeof(buffer[0]));
        auto result = WideCharToMultiByte(cp, 0, src.c_str(), -1, buffer, count, nullptr, nullptr);
        return result != 0 ? ascii_string(buffer) : ascii_string();
    }
}; // Conversion

}} // Cube::Encoding
