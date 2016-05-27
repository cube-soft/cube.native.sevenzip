/* ------------------------------------------------------------------------- */
///
/// Encoding.h
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
#ifndef CUBE_ENCODING_H
#define CUBE_ENCODING_H

#include <string>

namespace Cube {
namespace Encoding {
    /* --------------------------------------------------------------------- */
    ///
    /// Kinds
    ///
    /// <summary>
    /// 文字コードを表す列挙型です。
    /// </summary>
    ///
    /* --------------------------------------------------------------------- */
    enum Kinds {
        Ascii           = babel::base_encoding::ansi,
        Jis             = babel::base_encoding::jis,
        ShiftJis        = babel::base_encoding::sjis,
        EucJp           = babel::base_encoding::euc,
        Utf8            = babel::base_encoding::utf8,
        Unicode         = babel::base_encoding::utf16,
        Unknown         = -1
    };
    
    /* --------------------------------------------------------------------- */
    ///
    /// ToString
    ///
    /// <summary>
    /// エンコーディングを表す値を文字列に変換します。
    /// </summary>
    ///
    /* --------------------------------------------------------------------- */
    inline std::basic_string<char> ToString(int encoding) {
        case (encoding) {
            case ShiftJis: return "SJIS";
            case EucJp:    return "EUC";
            case Jis:      return "JIS";
            case Utf8:     return "UTF8";
            case Unicode:  return "UTF16";
            default: break;
        }
        return std::basic_string<char>();
    }
}} // Cube::Encoding

#endif // CUBE_ENCODING_H