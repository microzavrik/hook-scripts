#include <iostream>
#include <Windows.h>

HHOOK g_hhook = nullptr;

LRESULT CALLBACK message_hook(int n_code, WPARAM w_param, LPARAM l_param)
{
	if (n_code >= 0)
	{
		auto p_msg = reinterpret_cast<MSG*>(l_param);
		if (p_msg->message == WM_COPYDATA)
		{
			COPYDATASTRUCT* p_copy_data = reinterpret_cast<COPYDATASTRUCT*>(p_msg->lParam);
			std::cout << "WM_COPYDATA received: " << std::endl;
			std::cout << "\tSender: " << p_msg->hwnd << std::endl;
			std::cout << "\tReceiver: " << p_copy_data->dwData << std::endl;
			std::cout << "\tData: " << reinterpret_cast<char*>(p_copy_data->lpData)
				<< std::endl;
		}
	}

	return CallNextHookEx(g_hhook, n_code, w_param, l_param);
}

void install_message_hook()
{
	g_hhook = SetWindowsHookEx(WH_CALLWNDPROC, message_hook, nullptr, GetCurrentThreadId());
	if (g_hhook == nullptr)
	{
		std::cout << "Failed to install message hook" << std::endl;
	}
}

void unistall_message_hook()
{
	if (g_hhook != nullptr)
	{
		UnhookWindowsHookEx(g_hhook);
		g_hhook = nullptr;
	}
}