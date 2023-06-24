#include "pch.h"
#include "EngineUtils.h"

UINT ConvertToMinimumHardwareAllocationByteSize(UINT byteSize)
{
	return (byteSize + 255) & ~255;
}

wstring ConvertStringToWString(const string& value)
{
	int32 len;
	int32 strlength = static_cast<int32>(value.length()) + 1;
	len = ::MultiByteToWideChar(CP_ACP, 0, value.c_str(), strlength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	::MultiByteToWideChar(CP_ACP, 0, value.c_str(), strlength, buf, len);
	wstring ret(buf);
	delete[] buf;
	return ret;
}

string ConvertWStringToString(const wstring& value)
{
	int32 len;
	int32 strlength = static_cast<int32>(value.length()) + 1;
	len = ::WideCharToMultiByte(CP_ACP, 0, value.c_str(), strlength, 0, 0, 0, 0);
	string r(len, '\0');
	::WideCharToMultiByte(CP_ACP, 0, value.c_str(), strlength, &r[0], len, 0, 0);
	return r;
}
