#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <utility>
#include <xbyak/xbyak.h>


namespace ws {

// コンパイラが定義している変数
EXTERN_C IMAGE_DOS_HEADER __ImageBase;


class WindowBaseImpl
{
	typedef WindowBaseImpl _Myt;
	typedef std::unique_ptr<std::remove_pointer<LPVOID>::type, std::function<void (LPVOID)> > ThunkHolder;

public:
	WindowBaseImpl() {}
	virtual ~WindowBaseImpl() {}

	HRESULT Create(HINSTANCE hinstance, HWND hwndParent = nullptr) {
		const auto VMDeleter = [](LPVOID pv){ VirtualFreeEx(GetCurrentProcess(), pv, 0, MEM_RELEASE); };

		// Register class
		WNDCLASSEXW wcex = {};
		wcex.hInstance = hinstance;
		GetWndClassEx(wcex);

		// TODO: Common controlの場合は Subclassを使う
		if (wcex.lpfnWndProc) {
			// WndProcが設定されてるならそのままでいい
		} else {
			LRESULT (CALLBACK _Myt::*wndProc)(HWND,UINT,WPARAM,LPARAM) = &_Myt::WindowProcImpl;

			struct _Thunk : public Xbyak::CodeGenerator {
				_Thunk(_Myt* p, intptr_t addr) {
					// x64 は常に fastcall のため最初の 4引数が rcx,rdx,r8,r9 で渡される
					// ここでは rcx は this 用に使用する必要があり 3引数しか渡せなくなるため、1つずつずらして LPARAM は別に保持する
					mov(qword[&p->lParam_], r9);
					mov(r9, r8);
					mov(r8d, edx);
					mov(rdx, rcx);
					mov(rcx, reinterpret_cast<intptr_t>(p));
					mov(r11, addr);
				    jmp(r11);
				}
			} thunk(this, reinterpret_cast<intptr_t>(*reinterpret_cast<WNDPROC*>(&wndProc)));

			thunk_ = ThunkHolder(VirtualAllocEx(GetCurrentProcess(),nullptr,thunk.getSize(),MEM_COMMIT,PAGE_EXECUTE_READWRITE), VMDeleter);
			CopyMemory(thunk_.get(), thunk.getCode(), thunk.getSize());

			wcex.lpfnWndProc = static_cast<WNDPROC>(thunk_.get());
		}
		if (!RegisterClassExW(&wcex)) {
			return HRESULT_FROM_WIN32(GetLastError());
		}

		// Create window
		WindowStyles styles;
		GetWindowStyles(styles);

		// TODO: 初期位置の設定
		hwnd_ = CreateWindowExW(std::get<1>(styles),
							 wcex.lpszClassName,
							 nullptr,
							 std::get<0>(styles),
							 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
							 hwndParent,
							 nullptr,
							 wcex.hInstance,
							 nullptr);
		if (!hwnd_) {
			return HRESULT_FROM_WIN32(GetLastError());
		} else {
			return S_OK;
		}
	}

	inline operator HWND() const { return hwnd_; }
	inline operator HINSTANCE() const { return reinterpret_cast<HINSTANCE>(&__ImageBase); }

protected:
	typedef std::pair<DWORD, DWORD> WindowStyles;

	virtual void GetWndClassEx(WNDCLASSEXW& wcex) const = 0;
	virtual LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
	virtual void GetWindowStyles(WindowStyles& styles) const = 0;

private:
	LRESULT CALLBACK WindowProcImpl(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		return WindowProc(hwnd, uMsg, wParam, lParam_);
	}

	HWND hwnd_;
	LPARAM lParam_;
	ThunkHolder thunk_;
};

class WindowBase : public WindowBaseImpl
{
public:
	WindowBase() {}
	virtual ~WindowBase() {}
};

}	// namespace ws
