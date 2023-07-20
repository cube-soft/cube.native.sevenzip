// UTFConvert.cpp

#include "StdAfx.h"

// #include <stdio.h>

#include "MyTypes.h"
#include "UTFConvert.h"
#include "../Cube/Encoding.h"
#pragma warning(disable:4100)

#ifndef Z7_WCHART_IS_16BIT
#ifndef __APPLE__
  // we define it if the system supports files with non-utf8 symbols:
  #define MY_UTF8_RAW_NON_UTF8_SUPPORTED
#endif
#endif

/*
  MY_UTF8_START(n) - is a base value for start byte (head), if there are (n) additional bytes after start byte
  
  n : MY_UTF8_START(n) : Bits of code point

  0 : 0x80 :    : unused
  1 : 0xC0 : 11 :
  2 : 0xE0 : 16 : Basic Multilingual Plane
  3 : 0xF0 : 21 : Unicode space
  4 : 0xF8 : 26 :
  5 : 0xFC : 31 : UCS-4 : wcstombs() in ubuntu is limited to that value
  6 : 0xFE : 36 : We can use it, if we want to encode any 32-bit value
  7 : 0xFF :
*/

#define MY_UTF8_START(n) (0x100 - (1 << (7 - (n))))

#define MY_UTF8_HEAD_PARSE2(n) \
    if (c < MY_UTF8_START((n) + 1)) \
    { numBytes = (n); val -= MY_UTF8_START(n); }

#ifndef Z7_WCHART_IS_16BIT

/*
   if (wchar_t is 32-bit), we can support large points in long UTF-8 sequence,
   when we convert wchar_t strings to UTF-8:
     (_UTF8_NUM_TAIL_BYTES_MAX == 3) : (21-bits points) - Unicode
     (_UTF8_NUM_TAIL_BYTES_MAX == 5) : (31-bits points) - UCS-4
     (_UTF8_NUM_TAIL_BYTES_MAX == 6) : (36-bit hack)
*/

#define MY_UTF8_NUM_TAIL_BYTES_MAX 5
#endif

/*
#define MY_UTF8_HEAD_PARSE \
    UInt32 val = c; \
         MY_UTF8_HEAD_PARSE2(1) \
    else MY_UTF8_HEAD_PARSE2(2) \
    else MY_UTF8_HEAD_PARSE2(3) \
    else MY_UTF8_HEAD_PARSE2(4) \
    else MY_UTF8_HEAD_PARSE2(5) \
  #if MY_UTF8_NUM_TAIL_BYTES_MAX >= 6
    else MY_UTF8_HEAD_PARSE2(6)
  #endif
*/

#define MY_UTF8_HEAD_PARSE_MAX_3_BYTES \
    UInt32 val = c; \
         MY_UTF8_HEAD_PARSE2(1) \
    else MY_UTF8_HEAD_PARSE2(2) \
    else { numBytes = 3; val -= MY_UTF8_START(3); }


#define MY_UTF8_RANGE(n) (((UInt32)1) << ((n) * 5 + 6))


#define START_POINT_FOR_SURROGATE 0x10000


/* we use 128 bytes block in 16-bit BMP-PLANE to encode non-UTF-8 Escapes
   Also we can use additional HIGH-PLANE (we use 21-bit points above 0x1f0000)
   to simplify internal intermediate conversion in Linux:
   RAW-UTF-8 <-> internal wchar_t utf-16 strings <-> RAW-UTF-UTF-8
*/


#if defined(Z7_WCHART_IS_16BIT)

#define UTF_ESCAPE_PLANE 0

#else

/*
we can place 128 ESCAPE chars to
   ef 80 -    ee be 80 (3-bytes utf-8) : similar to WSL
   ef ff -    ee bf bf

1f ef 80 - f7 be be 80 (4-bytes utf-8) : last  4-bytes utf-8 plane (out of Unicode)
1f ef ff - f7 be bf bf (4-bytes utf-8) : last  4-bytes utf-8 plane (out of Unicode)
*/

// #define UTF_ESCAPE_PLANE_HIGH  (0x1f << 16)
// #define UTF_ESCAPE_PLANE        UTF_ESCAPE_PLANE_HIGH
#define UTF_ESCAPE_PLANE 0

/*
  if (Z7_UTF_FLAG_FROM_UTF8_USE_ESCAPE is set)
  {
    if (UTF_ESCAPE_PLANE is UTF_ESCAPE_PLANE_HIGH)
    {
      we can restore any 8-bit Escape from ESCAPE-PLANE-21 plane.
      But ESCAPE-PLANE-21 point cannot be stored to utf-16 (7z archive)
      So we still need a way to extract 8-bit Escapes and BMP-Escapes-8
      from same BMP-Escapes-16 stored in 7z.
      And if we want to restore any 8-bit from 7z archive,
      we still must use Z7_UTF_FLAG_FROM_UTF8_BMP_ESCAPE_CONVERT for (utf-8 -> utf-16)
      Also we need additional Conversions to tranform from utf-16 to utf-16-With-Escapes-21
    }
    else (UTF_ESCAPE_PLANE == 0)
    {
      we must convert original 3-bytes utf-8 BMP-Escape point to sequence
      of 3 BMP-Escape-16 points with Z7_UTF_FLAG_FROM_UTF8_BMP_ESCAPE_CONVERT
      so we can extract original RAW-UTF-8 from UTFD-16 later.
    }
  }
*/

#endif



#define UTF_ESCAPE_BASE 0xef00


#ifdef UTF_ESCAPE_BASE
#define IS_ESCAPE_POINT(v, plane) (((v) & (UInt32)0xffffff80) == (plane) + UTF_ESCAPE_BASE + 0x80)
#endif

#define IS_SURROGATE_POINT(v)     (((v) & (UInt32)0xfffff800) == 0xd800)
#define IS_LOW_SURROGATE_POINT(v) (((v) & (UInt32)0xfffffC00) == 0xdc00)


#define UTF_ERROR_UTF8_CHECK \
  { NonUtf = true; continue; }

void CUtf8Check::Check_Buf(const char *src, size_t size) throw()
{
  Clear();
  // Byte maxByte = 0;

  for (;;)
  {
    if (size == 0)
      break;

    const Byte c = (Byte)(*src++);
    size--;

    if (c == 0)
    {
      ZeroChar = true;
      continue;
    }

    /*
    if (c > maxByte)
      maxByte = c;
    */

    if (c < 0x80)
      continue;
    
    if (c < 0xc0 + 2) // it's limit for 0x140000 unicode codes : win32 compatibility
      UTF_ERROR_UTF8_CHECK

    unsigned numBytes;

    UInt32 val = c;
         MY_UTF8_HEAD_PARSE2(1)
    else MY_UTF8_HEAD_PARSE2(2)
    else MY_UTF8_HEAD_PARSE2(4)
    else MY_UTF8_HEAD_PARSE2(5)
    else
    {
      UTF_ERROR_UTF8_CHECK
    }

    unsigned pos = 0;
    do
    {
      if (pos == size)
        break;
      unsigned c2 = (Byte)src[pos];
      c2 -= 0x80;
      if (c2 >= 0x40)
        break;
      val <<= 6;
      val |= c2;
      if (pos == 0)
        if (val < (((unsigned)1 << 7) >> numBytes))
          break;
      pos++;
    }
    while (--numBytes);

    if (numBytes != 0)
    {
      if (pos == size)
        Truncated = true;
      else
        UTF_ERROR_UTF8_CHECK
    }

    #ifdef UTF_ESCAPE_BASE
      if (IS_ESCAPE_POINT(val, 0))
        Escape = true;
    #endif

    if (MaxHighPoint < val)
      MaxHighPoint = val;

    if (IS_SURROGATE_POINT(val))
      SingleSurrogate = true;

    src += pos;
    size -= pos;
  }

  // MaxByte = maxByte;
}

bool Check_UTF8_Buf(const char *src, size_t size, bool allowReduced) throw()
{
  CUtf8Check check;
  check.Check_Buf(src, size);
  return check.IsOK(allowReduced);
}

/*
bool CheckUTF8_chars(const char *src, bool allowReduced) throw()
{
  CUtf8Check check;
  check.CheckBuf(src, strlen(src));
  return check.IsOK(allowReduced);
}
*/

bool CheckUTF8_AString(const AString &s) throw()
{
  CUtf8Check check;
  check.Check_AString(s);
  return check.IsOK();
}


/*
bool CheckUTF8(const char *src, bool allowReduced) throw()
{
  // return Check_UTF8_Buf(src, strlen(src), allowReduced);

  for (;;)
  {
    const Byte c = (Byte)(*src++);
    if (c == 0)
      return true;

    if (c < 0x80)
      continue;
    if (c < 0xC0 + 2 || c >= 0xf5)
      return false;

    unsigned numBytes;
    MY_UTF8_HEAD_PARSE
    else
      return false;

    unsigned pos = 0;

    do
    {
      Byte c2 = (Byte)(*src++);
      if (c2 < 0x80 || c2 >= 0xC0)
        return allowReduced && c2 == 0;
      val <<= 6;
      val |= (c2 - 0x80);
      pos++;
    }
    while (--numBytes);

    if (val < MY_UTF8_RANGE(pos - 1))
      return false;

    if (val >= 0x110000)
      return false;
  }
}
*/

bool Convert_UTF8_Buf_To_Unicode(const char *src, size_t srcSize, UString &dest, unsigned flags)
{
  dest.Empty();
  auto tmp = Cube::Encoding::Conversion::ascii_string(src, src + srcSize);
  if (!tmp.empty())
  {
      auto code = Check_UTF8_Buf(src, srcSize, false) ?
                  Cube::Encoding::Utf8 :
                  Cube::Encoding::Conversion::Guess(tmp);
      auto cvt  = code != Cube::Encoding::Unknown ?
                  Cube::Encoding::Conversion::ToUnicode(tmp, code) :
                  Cube::Encoding::Conversion::Widen(tmp, CP_OEMCP);

      if ((code == Cube::Encoding::Jis ||
           code == Cube::Encoding::EucJp) &&
          !Cube::Encoding::Validator::IsValidPath(cvt))
      {
          code = Cube::Encoding::ShiftJis;
          cvt  = Cube::Encoding::Conversion::ToUnicode(tmp, code);
      }
      dest = cvt.c_str();
  }
  return true;
}

bool ConvertUTF8ToUnicode_Flags(const AString &src, UString &dest, unsigned flags)
{
  return Convert_UTF8_Buf_To_Unicode(src, src.Len(), dest,  flags);
}


static
unsigned g_UTF8_To_Unicode_Flags =
    Z7_UTF_FLAG_FROM_UTF8_USE_ESCAPE
  #ifndef Z7_WCHART_IS_16BIT
    | Z7_UTF_FLAG_FROM_UTF8_SURROGATE_ERROR
  #ifdef MY_UTF8_RAW_NON_UTF8_SUPPORTED
    | Z7_UTF_FLAG_FROM_UTF8_BMP_ESCAPE_CONVERT
  #endif
  #endif
    ;


/*
bool ConvertUTF8ToUnicode_boolRes(const AString &src, UString &dest)
{
  return ConvertUTF8ToUnicode_Flags(src, dest, g_UTF8_To_Unicode_Flags);
}
*/

bool ConvertUTF8ToUnicode(const AString &src, UString &dest)
{
  return ConvertUTF8ToUnicode_Flags(src, dest, g_UTF8_To_Unicode_Flags);
}

void Print_UString(const UString &a);

void ConvertUnicodeToUTF8_Flags(const UString &src, AString &dest, unsigned flags)
{
  dest.Empty();
  auto cvt = Cube::Encoding::Conversion::ToUtf8(src.Ptr());
  dest = cvt.c_str();
}


unsigned g_Unicode_To_UTF8_Flags =
      // Z7_UTF_FLAG_TO_UTF8_PARSE_HIGH_ESCAPE
      0
  #ifndef _WIN32
    #ifdef MY_UTF8_RAW_NON_UTF8_SUPPORTED
      | Z7_UTF_FLAG_TO_UTF8_EXTRACT_BMP_ESCAPE
    #else
      | Z7_UTF_FLAG_TO_UTF8_SURROGATE_ERROR
    #endif
  #endif
    ;

void ConvertUnicodeToUTF8(const UString &src, AString &dest)
{
  ConvertUnicodeToUTF8_Flags(src, dest, g_Unicode_To_UTF8_Flags);
}

void Convert_Unicode_To_UTF8_Buf(const UString &src, CByteBuffer &dest)
{
  dest.Free();
  auto cvt = Cube::Encoding::Conversion::ToUtf8(src.Ptr());
  dest.CopyFrom((const unsigned char*)(cvt.c_str()), cvt.size());
}

/*

#ifndef _WIN32
void Convert_UTF16_To_UTF32(const UString &src, UString &dest)
{
  dest.Empty();
  for (size_t i = 0; i < src.Len();)
  {
    wchar_t c = src[i++];
    if (c >= 0xd800 && c < 0xdc00 && i < src.Len())
    {
      const wchar_t c2 = src[i];
      if (c2 >= 0xdc00 && c2 < 0x10000)
      {
        // printf("\nSurragate [%d]: %4x %4x -> ", i, (int)c, (int)c2);
        c = 0x10000 + ((c & 0x3ff) << 10) + (c2 & 0x3ff);
        // printf("%4x\n", (int)c);
        i++;
      }
    }
    dest += c;
  }
}

void Convert_UTF32_To_UTF16(const UString &src, UString &dest)
{
  dest.Empty();
  for (size_t i = 0; i < src.Len();)
  {
    wchar_t w = src[i++];
    if (w >= 0x10000 && w < 0x110000)
    {
      w -= 0x10000;
      dest += (wchar_t)((unsigned)0xd800 + (((unsigned)w >> 10) & 0x3ff));
      w = 0xdc00 + (w & 0x3ff);
    }
    dest += w;
  }
}

bool UTF32_IsThere_BigPoint(const UString &src)
{
  for (size_t i = 0; i < src.Len();)
  {
    const UInt32 c = (UInt32)src[i++];
    if (c >= 0x110000)
      return true;
  }
  return false;
}

bool Unicode_IsThere_BmpEscape(const UString &src)
{
  for (size_t i = 0; i < src.Len();)
  {
    const UInt32 c = (UInt32)src[i++];
    if (IS_ESCAPE_POINT(c, 0))
      return true;
  }
  return false;
}


#endif

bool Unicode_IsThere_Utf16SurrogateError(const UString &src)
{
  for (size_t i = 0; i < src.Len();)
  {
    const UInt32 val = (UInt32)src[i++];
    if (IS_SURROGATE_POINT(val))
    {
      // it's hack to UTF-8 encoding
      if (val >= 0xdc00 || i == src.Len())
        return true;
      const UInt32 c2 = (UInt32)*src;
      if (!IS_LOW_SURROGATE_POINT(c2))
        return true;
    }
  }
  return false;
}
*/

#ifndef Z7_WCHART_IS_16BIT

void Convert_UnicodeEsc16_To_UnicodeEscHigh
#if UTF_ESCAPE_PLANE == 0
    (UString &) {}
#else
    (UString &s)
{
  const unsigned len = s.Len();
  for (unsigned i = 0; i < len; i++)
  {
    wchar_t c = s[i];
    if (IS_ESCAPE_POINT(c, 0))
    {
      c += UTF_ESCAPE_PLANE;
      s.ReplaceOneCharAtPos(i, c);
    }
  }
}
#endif
#endif
