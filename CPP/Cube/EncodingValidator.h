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

namespace Cube {
namespace Encoding {

/* ------------------------------------------------------------------------- */
///
/// Validator
///
/// <summary>
/// エンコーディングに関するバリデーションを行うクラスです。
/// </summary>
///
/* ------------------------------------------------------------------------- */
class Validator {
public:
    typedef std::basic_string<char> utf8_string;
    typedef std::basic_string<wchar_t> unicode_string;

    /* --------------------------------------------------------------------- */
    ///
    /// IsValidPath
    ///
    /// <summary>
    /// 正常なパスかどうかを判別します。
    /// </summary>
    ///
    /* --------------------------------------------------------------------- */
    static bool IsValidPath(const unicode_string& src) {
        for (auto c : src) {
            auto n = static_cast<uint16_t>(c);
            if (n <= 31 ||
                n == 34 || // "
                n == 42 || // *
                n == 60 || // <
                n == 62 || // >
                n == 63 || // ?
                n == 124   // |
            ) return false;
        }
        return true;
    }

    /* --------------------------------------------------------------------- */
    ///
    /// NfdToNfc
    ///
    /// <summary>
    /// NFD を NFC に正規化します。
    /// </summary>
    ///
    /// <remarks>
    /// TODO: 現在、ひらがなおよびカタカナのみに対応しているので、
    /// それ以外にも対応する。
    /// </remarks>
    ///
    /* --------------------------------------------------------------------- */
    static void NfdToNfc(utf8_string& s) {
        static const char *utf8_mac_kana[][2] = {
            { "\xE3\x81\x8B\xE3\x82\x99", "\xE3\x81\x8C" },		// が
            { "\xE3\x81\x8D\xE3\x82\x99", "\xE3\x81\x8E" },		// ぎ
            { "\xE3\x81\x8F\xE3\x82\x99", "\xE3\x81\x90" },		// ぐ
            { "\xE3\x81\x91\xE3\x82\x99", "\xE3\x81\x92" },		// げ
            { "\xE3\x81\x93\xE3\x82\x99", "\xE3\x81\x94" },		// ご
            { "\xE3\x81\x95\xE3\x82\x99", "\xE3\x81\x96" },		// ざ
            { "\xE3\x81\x97\xE3\x82\x99", "\xE3\x81\x98" },		// じ
            { "\xE3\x81\x99\xE3\x82\x99", "\xE3\x81\x9A" },		// ず
            { "\xE3\x81\x9B\xE3\x82\x99", "\xE3\x81\x9C" },		// ぜ
            { "\xE3\x81\x9D\xE3\x82\x99", "\xE3\x81\x9E" },		// ぞ
            { "\xE3\x81\x9F\xE3\x82\x99", "\xE3\x81\xA0" },		// だ
            { "\xE3\x81\xA1\xE3\x82\x99", "\xE3\x81\xA2" },		// ぢ
            { "\xE3\x81\xA4\xE3\x82\x99", "\xE3\x81\xA5" },		// づ
            { "\xE3\x81\xA6\xE3\x82\x99", "\xE3\x81\xA7" },		// で
            { "\xE3\x81\xA8\xE3\x82\x99", "\xE3\x81\xA9" },		// ど
            { "\xE3\x81\xAF\xE3\x82\x99", "\xE3\x81\xB0" },		// ば
            { "\xE3\x81\xB2\xE3\x82\x99", "\xE3\x81\xB3" },		// び
            { "\xE3\x81\xB5\xE3\x82\x99", "\xE3\x81\xB6" },		// ぶ
            { "\xE3\x81\xB8\xE3\x82\x99", "\xE3\x81\xB9" },		// べ
            { "\xE3\x81\xBB\xE3\x82\x99", "\xE3\x81\xBC" },		// ぼ
            { "\xE3\x81\xAF\xE3\x82\x9A", "\xE3\x81\xB1" },		// ぱ
            { "\xE3\x81\xB2\xE3\x82\x9A", "\xE3\x81\xB4" },		// ぴ
            { "\xE3\x81\xB5\xE3\x82\x9A", "\xE3\x81\xB7" },		// ぷ
            { "\xE3\x81\xB8\xE3\x82\x9A", "\xE3\x81\xBA" },		// ぺ
            { "\xE3\x81\xBB\xE3\x82\x9A", "\xE3\x81\xBD" },		// ぽ
            { "\xE3\x81\x86\xE3\x82\x99", "\xE3\x82\x94" },		// ヴ（ひらがな）
            { "\xE3\x82\xAB\xE3\x82\x99", "\xE3\x82\xAC" },		// ガ
            { "\xE3\x82\xAD\xE3\x82\x99", "\xE3\x82\xAE" },		// ギ
            { "\xE3\x82\xAF\xE3\x82\x99", "\xE3\x82\xB0" },		// グ
            { "\xE3\x82\xB1\xE3\x82\x99", "\xE3\x82\xB2" },		// ゲ
            { "\xE3\x82\xB3\xE3\x82\x99", "\xE3\x82\xB4" },		// ゴ
            { "\xE3\x82\xB5\xE3\x82\x99", "\xE3\x82\xB6" },		// ザ
            { "\xE3\x82\xB7\xE3\x82\x99", "\xE3\x82\xB8" },		// ジ
            { "\xE3\x82\xB9\xE3\x82\x99", "\xE3\x82\xBA" },		// ズ
            { "\xE3\x82\xBB\xE3\x82\x99", "\xE3\x82\xBC" },		// ゼ
            { "\xE3\x82\xBD\xE3\x82\x99", "\xE3\x82\xBE" },		// ゾ
            { "\xE3\x82\xBF\xE3\x82\x99", "\xE3\x83\x80" },		// ダ
            { "\xE3\x83\x81\xE3\x82\x99", "\xE3\x83\x82" },		// ヂ
            { "\xE3\x83\x84\xE3\x82\x99", "\xE3\x83\x85" },		// ヅ
            { "\xE3\x83\x86\xE3\x82\x99", "\xE3\x83\x87" },		// デ
            { "\xE3\x83\x88\xE3\x82\x99", "\xE3\x83\x89" },		// ド
            { "\xE3\x83\x8F\xE3\x82\x99", "\xE3\x83\x90" },		// バ
            { "\xE3\x83\x92\xE3\x82\x99", "\xE3\x83\x93" },		// ビ
            { "\xE3\x83\x95\xE3\x82\x99", "\xE3\x83\x96" },		// ブ
            { "\xE3\x83\x98\xE3\x82\x99", "\xE3\x83\x99" },		// ベ
            { "\xE3\x83\x9B\xE3\x82\x99", "\xE3\x83\x9C" },		// ボ
            { "\xE3\x83\x8F\xE3\x82\x9A", "\xE3\x83\x91" },		// パ
            { "\xE3\x83\x92\xE3\x82\x9A", "\xE3\x83\x94" },		// ピ
            { "\xE3\x83\x95\xE3\x82\x9A", "\xE3\x83\x97" },		// プ
            { "\xE3\x83\x98\xE3\x82\x9A", "\xE3\x83\x9A" },		// ペ
            { "\xE3\x83\x9B\xE3\x82\x9A", "\xE3\x83\x9D" },		// ポ
            { "\xE3\x82\xA6\xE3\x82\x99", "\xE3\x83\xB4" },		// ヴ
        };

        if (s.size() < 6) return;

        for (auto i = 0u; i < sizeof(utf8_mac_kana) / sizeof(utf8_mac_kana[0]); ++i) {
            if (s.find("\xE3\x82\x99") == utf8_string::npos &&
                s.find("\xE3\x82\x9A") == utf8_string::npos) return; // hack
            ReplaceAll(s, utf8_mac_kana[i][0], utf8_mac_kana[i][1]);
        }
    }

private:
    /* --------------------------------------------------------------------- */
    ///
    /// ReplaceAll
    ///
    /// <summary>
    /// 一致する文字列を全て置換します。
    /// </summary>
    ///
    /* --------------------------------------------------------------------- */
    static void ReplaceAll(utf8_string& s, const utf8_string& sch, const utf8_string& rep) {
        if (s.empty()) return;

        auto pos = s.find(sch);
        while (pos != utf8_string::npos) {
            s.replace(pos, sch.size(), rep);
            pos = s.find(sch, pos + rep.size());
        }
    }
}; // Mac

}} // Cube::Encoding
