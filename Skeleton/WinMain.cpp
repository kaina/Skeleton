#define WINVER 0x0700
#define _WIN32_WINNT 0x0700
//#define UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <shlwapi.h>
#include "Skeleton.hxx"

#include <memory>


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
	HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);

	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if (FAILED(hr)) {
		return static_cast<int>(hr);
	}

	std::unique_ptr<Skeleton> p(new Skeleton);
	hr = p->Create(hInstance, nullptr);
	if (SUCCEEDED(hr)) {
		HWND hwnd = static_cast<HWND>(*p);
		ShowWindow(hwnd, nCmdShow);
		UpdateWindow(hwnd);

		MSG msg;
		while (GetMessageW(&msg, nullptr, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		hr = __HRESULT_FROM_WIN32(msg.wParam);

	}
	p.reset(nullptr);


	CoUninitialize();

	return static_cast<int>(hr);
}
