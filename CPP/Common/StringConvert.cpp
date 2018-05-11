// Common/StringConvert.cpp

#include "StdAfx.h"

#include "StringConvert.h"

#ifndef _WIN32
#include <stdlib.h>
#endif

static const char k_DefultChar = '_';

#include "UTFConvert.h"
#include "../Cube/Encoding.h"
#pragma warning(disable:4100)

#ifdef _WIN32

void MultiByteToUnicodeString2(UString &dest, const AString &src, UINT codePage)
{
  dest.Empty();
  if (!src.IsEmpty())
  {
      auto code = CheckUTF8(src) ?
                  Cube::Encoding::Utf8 :
                  Cube::Encoding::Conversion::Guess((const char*)src);
      auto cvt  = code != Cube::Encoding::Unknown ?
                  Cube::Encoding::Conversion::ToUnicode((const char*)src, code) :
                  Cube::Encoding::Conversion::Widen((const char*)src, codePage);

      if ((code == Cube::Encoding::Jis ||
           code == Cube::Encoding::EucJp) &&
          !Cube::Encoding::Validator::IsValidPath(cvt))
      {
          code = Cube::Encoding::ShiftJis;
          cvt  = Cube::Encoding::Conversion::ToUnicode((const char*)src, code);
      }

      dest = cvt.c_str();
  }
}

static void UnicodeStringToMultiByte2(AString &dest, const UString &src, UINT codePage, char defaultChar, bool &defaultCharWasUsed)
{
  dest.Empty();
  defaultCharWasUsed = false;
  if (!src.IsEmpty())
  {
    auto utf8 = codePage == CP_UTF7 ||
                codePage == CP_UTF8 ||
                codePage == CP_MACCP;
    auto cvt  = utf8 ?
                Cube::Encoding::Conversion::ToUtf8((const wchar_t*)src) :
                Cube::Encoding::Conversion::Narrow((const wchar_t*)src, codePage);
    dest = cvt.c_str();
  }
}

#else

void MultiByteToUnicodeString2(UString &dest, const AString &src, UINT /* codePage */)
{
  dest.Empty();
  if (src.IsEmpty())
    return;

  size_t limit = ((size_t)src.Len() + 1) * 2;
  wchar_t *d = dest.GetBuf((unsigned)limit);
  size_t len = mbstowcs(d, src, limit);
  if (len != (size_t)-1)
  {
    dest.ReleaseBuf_SetEnd((unsigned)len);
    return;
  }

  {
    unsigned i;
    const char *s = (const char *)src;
    for (i = 0;;)
    {
      Byte c = (Byte)s[i];
      if (c == 0)
        break;
      d[i++] = (wchar_t)c;
    }
    d[i] = 0;
    dest.ReleaseBuf_SetLen(i);
  }
}

static void UnicodeStringToMultiByte2(AString &dest, const UString &src, UINT /* codePage */, char defaultChar, bool &defaultCharWasUsed)
{
  dest.Empty();
  defaultCharWasUsed = false;
  if (src.IsEmpty())
    return;

  size_t limit = ((size_t)src.Len() + 1) * 6;
  char *d = dest.GetBuf((unsigned)limit);
  size_t len = wcstombs(d, src, limit);
  if (len != (size_t)-1)
  {
    dest.ReleaseBuf_SetEnd((unsigned)len);
    return;
  }

  {
    const wchar_t *s = (const wchar_t *)src;
    unsigned i;
    for (i = 0;;)
    {
      wchar_t c = s[i];
      if (c == 0)
        break;
      if (c >= 0x100)
      {
        c = defaultChar;
        defaultCharWasUsed = true;
      }
      d[i++] = (char)c;
    }
    d[i] = 0;
    dest.ReleaseBuf_SetLen(i);
  }
}

#endif


UString MultiByteToUnicodeString(const AString &src, UINT codePage)
{
  UString dest;
  MultiByteToUnicodeString2(dest, src, codePage);
  return dest;
}

UString MultiByteToUnicodeString(const char *src, UINT codePage)
{
  return MultiByteToUnicodeString(AString(src), codePage);
}


void UnicodeStringToMultiByte2(AString &dest, const UString &src, UINT codePage)
{
  bool defaultCharWasUsed;
  UnicodeStringToMultiByte2(dest, src, codePage, k_DefultChar, defaultCharWasUsed);
}

AString UnicodeStringToMultiByte(const UString &src, UINT codePage, char defaultChar, bool &defaultCharWasUsed)
{
  AString dest;
  UnicodeStringToMultiByte2(dest, src, codePage, defaultChar, defaultCharWasUsed);
  return dest;
}

AString UnicodeStringToMultiByte(const UString &src, UINT codePage)
{
  AString dest;
  bool defaultCharWasUsed;
  UnicodeStringToMultiByte2(dest, src, codePage, k_DefultChar, defaultCharWasUsed);
  return dest;
}
