// Common/StdOutStream.cpp

#include "StdAfx.h"

#include <tchar.h>

#include "IntToString.h"
#include "StdOutStream.h"
#include "StringConvert.h"
#include "UTFConvert.h"

#include <string>
#include "Cube/Conversion.h"

#ifdef SEVENZIP_ORIGINAL
static const char kNewLineChar = '\n';
#else
static const wchar_t* kNewLineChar =  L"\r\n";
#endif

static const char *kFileOpenMode = "wt";

extern int g_CodePage;

CStdOutStream g_StdOut(stdout);
CStdOutStream g_StdErr(stderr);

bool CStdOutStream::Open(const char *fileName) throw()
{
  Close();
  _stream = fopen(fileName, kFileOpenMode);
  _streamIsOpen = (_stream != 0);
  return _streamIsOpen;
}

bool CStdOutStream::Close() throw()
{
  if (!_streamIsOpen)
    return true;
  if (fclose(_stream) != 0)
    return false;
  _stream = 0;
  _streamIsOpen = false;
  return true;
}

bool CStdOutStream::Flush() throw()
{
  return (fflush(_stream) == 0);
}

CStdOutStream & endl(CStdOutStream & outStream) throw()
{
  return outStream << kNewLineChar;
}

#ifdef SEVENZIP_ORIGINAL

CStdOutStream & CStdOutStream::operator<<(const wchar_t *s)
{
  int codePage = g_CodePage;
  if (codePage == -1)
    codePage = CP_OEMCP;
  AString dest;
  if (codePage == CP_UTF8)
    ConvertUnicodeToUTF8(s, dest);
  else
    UnicodeStringToMultiByte2(dest, s, (UINT)codePage);
  return operator<<((const char *)dest);
}

#else

CStdOutStream & CStdOutStream::operator<<(char c)
{
    char str[2] = {};
    str[0] = c;
    str[1] = '\0';
    *this << str;
    this->Flush();
    return *this;
}

CStdOutStream & CStdOutStream::operator<<(const char *s)
{
    std::basic_string<char> str(s);

    if (str.empty()) return *this;

    if (str[0] == '\n') str.insert(0, "\r");
    for (auto i = str.find("\n"); i != std::string::npos; i = str.find("\n", i + 1)) {
        if (str[i - 1] != '\r') {
            str.insert(i, "\r");
            ++i;
        }
    }

    Cube::Encoding::Conversion::Initialize();
    *this << Cube::Encoding::Conversion::ToUnicode(str).c_str();
    this->Flush();
    return *this;
}

CStdOutStream & CStdOutStream::operator<<(const wchar_t *s)
{
    std::basic_string<wchar_t> wstr(s);

    if (wstr.empty()) return *this;

    if (wstr[0] == L'\n') wstr.insert(0, L"\r");
    for (auto i = wstr.find(L"\n"); i != std::wstring::npos; i = wstr.find(L"\n", i + 1)) {
        if (wstr[i - 1] != L'\r') {
            wstr.insert(i, L"\r");
            ++i;
        }
    }

    fwrite(wstr.c_str(), sizeof(wchar_t), wstr.size(), _stream);
    this->Flush();
    return *this;
}

#endif

void StdOut_Convert_UString_to_AString(const UString &s, AString &temp)
{
    int codePage = g_CodePage;
    if (codePage == -1)
        codePage = CP_OEMCP;
    if (codePage == CP_UTF8)
        ConvertUnicodeToUTF8(s, temp);
    else
        UnicodeStringToMultiByte2(temp, s, (UINT)codePage);
}

#ifdef SEVENZIP_ORIGINAL

void CStdOutStream::PrintUString(const UString &s, AString &temp)
{
  StdOut_Convert_UString_to_AString(s, temp);
  *this << (const char *)temp;
}

#else

void CStdOutStream::PrintUString(const UString &s, AString & /*temp */)
{
    *this << s;
}

#endif

CStdOutStream & CStdOutStream::operator<<(Int32 number) throw()
{
  char s[32];
  ConvertInt64ToString(number, s);
  return operator<<(s);
}

CStdOutStream & CStdOutStream::operator<<(Int64 number) throw()
{
  char s[32];
  ConvertInt64ToString(number, s);
  return operator<<(s);
}

CStdOutStream & CStdOutStream::operator<<(UInt32 number) throw()
{
  char s[16];
  ConvertUInt32ToString(number, s);
  return operator<<(s);
}

CStdOutStream & CStdOutStream::operator<<(UInt64 number) throw()
{
  char s[32];
  ConvertUInt64ToString(number, s);
  return operator<<(s);
}
