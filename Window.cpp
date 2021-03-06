#include "Window.h"
#include <sstream>
Window::WindowClass Window::WindowClass::m_wndClass;

Window::WindowClass::WindowClass() noexcept
:m_hInstance(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(m_wndClassName, GetInstance());
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return m_wndClass.m_hInstance;
}

const char* Window::WindowClass::GetName() noexcept
{
	return m_wndClassName;
}
Window::Window(int width, int height, const char* name) 
	:
	m_width(width),
	m_height(height)
{
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw WND_LAST_EXCEPT();
	}
	m_hWND = CreateWindow(
		WindowClass::GetName(),
		name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wr.right - wr.left,
		wr.bottom - wr.top,
		nullptr,
		nullptr,
		WindowClass::GetInstance(),
		this
	);
	if (m_hWND == nullptr)
	{
		throw WND_LAST_EXCEPT();
	}
	ShowWindow(m_hWND, SW_SHOWDEFAULT);
	// create graphics object
	m_pGfx = std::make_unique<Graphics>(m_hWND);
}

Window::~Window()
{
	DestroyWindow(m_hWND);
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}
void Window::SetTitle(const std::string& title)
{

	if (SetWindowText(m_hWND, title.c_str()) == 0)
	{
		throw WND_LAST_EXCEPT();
	}
}
std::optional <int> Window::ProcessMessages()
{
	MSG msg;

	while (PeekMessage(&msg, nullptr, 0, 0,PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return msg.wParam;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}
Graphics& Window::Gfx()
{
	return *m_pGfx;
}
LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}break;
		//****** START KEYBOARD MESSAGES ******//
		case WM_KILLFOCUS:
		{
			m_kybrd.ClearState();
		}break;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			if (!(lParam & 0x40000000) || m_kybrd.AutorepeatIsEnabled())
			{
				m_kybrd.OnKeyPressed(static_cast<unsigned char>(wParam));
			}


		}break;
		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			m_kybrd.OnKeyReleased(static_cast<unsigned char>(wParam));
		}break;

		case WM_CHAR:
		{
			m_kybrd.OnChar(static_cast<unsigned char>(wParam));
		}break;
		//****** END KEYBOARD MESSAGES ******//

		//****** MOUSE MESSAGES ******//
		case WM_MOUSEMOVE:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			// check to see if we are in client region
			if (pt.x >= 0 && pt.x < m_width && pt.y >= 0 && pt.y < m_height)
			{
				m_mouse.OnMouseMove(pt.x, pt.y);
				if (!m_mouse.IsInWindow())
				{
					SetCapture(m_hWND);
					m_mouse.OnMouseEnter();
				}
			}
			else
			{
				// we are not in client region, check to see is mouse left or right is beign pressed for checking drag events
				if (m_mouse.LeftIsPressed() || m_mouse.RightIsPressed() )
				{
					m_mouse.OnMouseMove(pt.x, pt.y);
				}
				// we left the region
				else
				{
					ReleaseCapture();
					m_mouse.OnMouseLeave();
				}
			}
		}break;
		case WM_LBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			m_mouse.OnLeftPressed(pt.x, pt.y);
		}break;
		case WM_RBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			m_mouse.OnRightPressed(pt.x, pt.y);
		}break;
		case WM_LBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			m_mouse.OnLeftReleased(pt.x, pt.y);
		}break;
		case WM_RBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			m_mouse.OnRightReleased(pt.x, pt.y);
		}break;
		case WM_MOUSEWHEEL:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
			m_mouse.OnWheelDelta(pt.x, pt.y, delta);
		}break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// ************* Windows exception handler ************* //

Window::Exception::Exception(int line, const char* file, HRESULT hr) noexcept
	:
	WndExceptionHandler(line,file),
	m_hr(hr)
{}


const char* Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[ErrorCode]" << GetErrorCode() << std::endl
		<< "[Description]" << GetErrorString() << std::endl
		<< GetOriginString();
	m_whatBuffer = oss.str();

	return m_whatBuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept
{
	return "Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr)
{
	char* pMsgBuf = nullptr;
	std::string error_code = "";
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr);

	if (0 == nMsgLen)
	{
		error_code = "Unknow error code";
	}
	else
	{
		error_code = pMsgBuf;
		LocalFree(pMsgBuf);
	}

	return error_code;
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return m_hr;
}

std::string Window::Exception::GetErrorString()const noexcept
{
	return TranslateErrorCode(m_hr);
}


