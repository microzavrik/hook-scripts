#include <iostream>
#include <windows.h>

bool add_to_autorun(const std::wstring& program_path)
{
	HKEY hkey;
	const wchar_t* sub_key = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

	if (RegOpenKeyEx(HKEY_CURRENT_USER, sub_key, 0, KEY_SET_VALUE, &hkey) != ERROR_SUCCESS)
	{
		std::cerr << "Failed to open registry key" << std::endl;
		return false;
	}

	if (RegSetValueEx(hkey, L"program", 0, REG_SZ, reinterpret_cast<const BYTE*>(program_path.c_str()),
		static_cast<DWORD>(program_path.length())) != ERROR_SUCCESS)
	{
		std::cerr << "Failed to set registry value" << std::endl;
		RegCloseKey(hkey);
		return false;
	}

	RegCloseKey(hkey);
	return true;
}