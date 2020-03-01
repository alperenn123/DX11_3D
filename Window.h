#pragma once
#include "windows_config.h"
#include "WndExceptionHandler.h"
#include "Keyboard.h"
#include "Mouse.h"
class Window {
public:
	class Exception : public WndExceptionHandler
	{
	public:
		Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		static std::string TranslateErrorCode(HRESULT hr);
		HRESULT GetErrorCode()const noexcept;
		std::string GetErrorString()const noexcept;
	private:
		HRESULT m_hr;
	};
private:
	class WindowClass
	{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;

	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator= (const WindowClass&) = delete;
		static constexpr const char* m_wndClassName = "Directx3D Engine Window";
		static WindowClass m_wndClass;
		HINSTANCE m_hInstance;

	};
public:
	Window(int width, int height, const char* name) ;
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::string& title);
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
	Keyboard m_kybrd;
	Mouse m_mouse;
private:
	int m_width;
	int m_height;
	HWND m_hWND;
};

// Macro for error expections
#define WND_EXCEPT(hr) (Window::Exception(__LINE__,__FILE__,hr))
#define WND_LAST_EXCEPT() Window::Exception( __LINE__,__FILE__,GetLastError() )