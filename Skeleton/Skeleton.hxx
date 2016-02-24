#pragma once

#include "ws/WindowBase.h"


class Skeleton : public ws::WindowBase
{
public:
	Skeleton() {}
	virtual ~Skeleton() {}

protected:
	void GetWndClassEx(WNDCLASSEXW& wcex) const {
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW|CS_VREDRAW;
		wcex.lpszClassName = L"0E11FABD-2CE8-44B4-A453-61DBAF9B055C";
	}

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg)
		{
			case WM_CREATE:
				return 0;
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
			default:
				break;
		}
		return DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}

	void GetWindowStyles(WindowStyles& styles) const {
		WindowStyles(WS_OVERLAPPEDWINDOW, 0).swap(styles);
	}

private:
};
