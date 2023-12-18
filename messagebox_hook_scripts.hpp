#include <iostream>
#include <windows.h>
#include <winternl.h>

using prototype_message_box = int(WINAPI*)(HWND hwnd, LPCSTR lp_text, LPCSTR lp_caption,
	UINT u_type);

prototype_message_box original_message_box = MessageBoxA;

int hooked_message_box(HWND hwnd, LPCSTR lp_text, LPCSTR lp_caption, UINT u_type)
{
	std::wcout << "Title: " << lp_caption << std::endl;
	std::wcout << "Text: " << lp_text << std::endl;

	return original_message_box(hwnd, lp_text, lp_caption, u_type);
}

void install_message_box_hook()
{
	LPVOID image_base = GetModuleHandleA(nullptr);
	PIMAGE_DOS_HEADER dos_headers = (PIMAGE_DOS_HEADER)image_base;
	PIMAGE_NT_HEADERS nt_headers = (PIMAGE_NT_HEADERS)((DWORD_PTR)image_base + dos_headers->e_lfanew);
	PIMAGE_IMPORT_DESCRIPTOR import_descriptor = nullptr;
	IMAGE_DATA_DIRECTORY imports_directory =
		nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	import_descriptor = (PIMAGE_IMPORT_DESCRIPTOR)(imports_directory.VirtualAddress + (DWORD_PTR)image_base);
	LPCSTR library_name = nullptr;
	HMODULE library = nullptr;
	PIMAGE_IMPORT_BY_NAME function_name = nullptr;
	
	while (import_descriptor->Name != NULL)
	{
		library_name = (LPCSTR)import_descriptor->Name + (DWORD_PTR)image_base;
		library = LoadLibraryA(library_name);

		if (library)
		{
			PIMAGE_THUNK_DATA original_first_thunk = nullptr, first_thunk = nullptr;
			original_first_thunk = (PIMAGE_THUNK_DATA)((DWORD_PTR)image_base +
				import_descriptor->OriginalFirstThunk);
			first_thunk = (PIMAGE_THUNK_DATA)((DWORD_PTR)image_base + import_descriptor->FirstThunk);

			while (original_first_thunk->u1.AddressOfData != NULL)
			{
				function_name = (PIMAGE_IMPORT_BY_NAME)((DWORD_PTR)image_base
					+ original_first_thunk->u1.AddressOfData);

				if (std::string(function_name->Name).compare("MessageBoxA") == 0)
				{
					SIZE_T bytes_written = 0;
					DWORD old_protect = 0;
					VirtualProtect((LPVOID)(&first_thunk->u1.Function), 8, PAGE_READWRITE, &old_protect);
					first_thunk->u1.Function = (DWORD_PTR)hooked_message_box;
				}
				++original_first_thunk;
				++first_thunk;
			}
		}
		import_descriptor++;
	}
}