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
      auto code = CheckUTF8_AString(src) ?
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

bool g_ForceToUTF8 = true; // false;

void MultiByteToUnicodeString2(UString &dest, const AString &src, UINT codePage)
{
  dest.Empty();
  if (src.IsEmpty())
    return;

  if (codePage == CP_UTF8 || g_ForceToUTF8)
  {
#if 1
    ConvertUTF8ToUnicode(src, dest);
    return;
#endif
  }

  const size_t limit = ((size_t)src.Len() + 1) * 2;
  wchar_t *d = dest.GetBuf((unsigned)limit);
  const size_t len = mbstowcs(d, src, limit);
  if (len != (size_t)-1)
  {
    dest.ReleaseBuf_SetEnd((unsigned)len);

#if WCHAR_MAX > 0xffff
    d = dest.GetBuf();
    for (size_t i = 0;; i++)
    {
      wchar_t c = d[i];
      // printf("\ni=%2d c = %4x\n", (unsigned)i, (unsigned)c);
      if (c == 0)
        break;
      if (c >= 0x10000 && c < 0x110000)
      {
        UString tempString = d + i;
        const wchar_t *t = tempString.Ptr();

        for (;;)
        {
          wchar_t w = *t++;
          // printf("\nchar=%x\n", w);
          if (w == 0)
            break;
          if (i == limit)
            break; // unexpected error
          if (w >= 0x10000 && w < 0x110000)
          {
#if 1
            if (i + 1 == limit)
              break; // unexpected error
            w -= 0x10000;
            d[i++] = (unsigned)0xd800 + (((unsigned)w >> 10) & 0x3ff);
            w = 0xdc00 + (w & 0x3ff);
#else
            // w = '_'; // for debug
#endif
          }
          d[i++] = w;
        }
        dest.ReleaseBuf_SetEnd((unsigned)i);
        break;
      }
    }

#endif
 
    /*
    printf("\nMultiByteToUnicodeString2 (%d) %s\n", (int)src.Len(),  src.Ptr());
    printf("char:    ");
    for (unsigned i = 0; i < src.Len(); i++)
      printf (" %02x", (int)(Byte)src[i]);
    printf("\n");
    printf("\n-> (%d) %ls\n", (int)dest.Len(), dest.Ptr());
    printf("wchar_t: ");
    for (unsigned i = 0; i < dest.Len(); i++)
    {
      printf (" %02x", (int)dest[i]);
    }
    printf("\n");
    */

    return;
  }

  /* if there is mbstowcs() error, we have two ways:
     
     1) change 0x80+ characters to some character: '_'
        in that case we lose data, but we have correct UString()
        and that scheme can show errors to user in early stages,
        when file converted back to mbs() cannot be found

     2) transfer bad characters in some UTF-16 range.
        it can be non-original Unicode character.
        but later we still can restore original character.
  */

  
  // printf("\nmbstowcs  ERROR !!!!!! s=%s\n", src.Ptr());
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
  if (src.IsEmpty())
    return;

  const size_t limit = ((size_t)src.Len() + 1) * 6;
  char *d = dest.GetBuf((unsigned)limit);

  const size_t len = wcstombs(d, src, limit);

  if (len != (size_t)-1)
  {
    dest.ReleaseBuf_SetEnd((unsigned)len);
    return;
  }
  dest.ReleaseBuf_SetEnd(0);
}


static void UnicodeStringToMultiByte2(AString &dest, const UString &src2, UINT codePage, char defaultChar, bool &defaultCharWasUsed)
{
  // if (codePage == 1234567) // for debug purposes
  if (codePage == CP_UTF8 || g_ForceToUTF8)
  {
#if 1
    defaultCharWasUsed = false;
    ConvertUnicodeToUTF8(src2, dest);
    return;
#endif
  }

  UString src = src2;
#if WCHAR_MAX > 0xffff
  {
    src.Empty();
    for (unsigned i = 0; i < src2.Len();)
    {
      wchar_t c = src2[i++];
      if (c >= 0xd800 && c < 0xdc00 && i != src2.Len())
      {
        const wchar_t c2 = src2[i];
        if (c2 >= 0xdc00 && c2 < 0xe000)
        {
#if 1
          // printf("\nSurragate [%d]: %4x %4x -> ", i, (int)c, (int)c2);
          c = 0x10000 + ((c & 0x3ff) << 10) + (c2 & 0x3ff);
          // printf("%4x\n", (int)c);
          i++;
#else
          // c = '_'; // for debug
#endif
        }
      }
      src += c;
    }
  }
#endif

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
