#include "MainClass.h"

MainClass::MainClass()
{
	m_windowClassName = L"HDRViewer";
	m_hInstance = nullptr;
}

HRESULT MainClass::CreateDesktopWindow()
{
	if (m_hInstance == nullptr)
	{
		m_hInstance = (HINSTANCE)GetModuleHandle(nullptr);
	}

	HICON hIcon = nullptr;
	WCHAR szExePath[MAX_PATH];
	GetModuleFileName(nullptr, szExePath, MAX_PATH);

	if (hIcon == NULL)
	{
		hIcon = ExtractIcon(m_hInstance, szExePath, MAX_PATH);
	}

	WNDCLASS wndClass;

	wndClass.style = CS_DBLCLKS;
	wndClass.lpfnWndProc = MainClass::StaticWindowProc;

	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = m_hInstance;

	wndClass.hIcon = hIcon;
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = m_windowClassName.c_str();

	if (!RegisterClass(&wndClass))
	{
		auto error = GetLastError();
		if (error != ERROR_CLASS_ALREADY_EXISTS)
		{
			return HRESULT_FROM_WIN32(error);
		}
	}

	m_rc;
	int x = CW_USEDEFAULT, y = CW_USEDEFAULT;
	m_hMenu = nullptr;

	int w = 640, h = 480;
	SetRect(&m_rc, 0, 0, w, h);

	AdjustWindowRect(&m_rc, WS_OVERLAPPEDWINDOW, m_hMenu != nullptr);

	m_hWnd = CreateWindow(
		m_windowClassName.c_str(),
		L"HDRViewer",
		WS_OVERLAPPEDWINDOW,
		x,
		y,
		m_rc.right - m_rc.left,
		m_rc.bottom - m_rc.top,
		nullptr,
		m_hMenu,
		m_hInstance,
		nullptr
	);

	if (m_hWnd == nullptr)
	{
		auto error = GetLastError();
		return HRESULT_FROM_WIN32(error);
	}

	return S_OK;
}

HRESULT MainClass::Run(shared_ptr<DeviceResources> deviceResources, shared_ptr<Renderer> renderer)
{
	auto hr = S_OK;

	if (!IsWindowVisible(m_hWnd))
	{
		ShowWindow(m_hWnd, SW_SHOW);
	}

	MSG msg;
	msg.message = WM_NULL;

	PeekMessage(&msg, nullptr, 0U, 0U, PM_NOREMOVE);

	while (WM_QUIT != msg.message)
	{
		auto bGotMsg = PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE) != 0;

		if (bGotMsg)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else
		{
			renderer->Update();
			renderer->Render();

			deviceResources->Present();
		}
	}

	return hr;
}

MainClass::~MainClass()
= default;

LRESULT MainClass::StaticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	EnableMenuItem(GetSystemMenu(hWnd, FALSE), SC_CLOSE,
		MF_BYCOMMAND | MF_ENABLED);

	switch (uMsg)
	{
	case WM_CLOSE:
		{
			const auto hMenu = GetMenu(hWnd);

			if (hMenu != nullptr)
			{
				DestroyMenu(hMenu);
			}

			DestroyWindow(hWnd);
			UnregisterClass(m_windowClassName.c_str(), m_hInstance);

			return 0;
		}

	case WM_DESTROY:
		{
		PostQuitMessage(0);
		break;
		}

	case WM_LBUTTONDOWN:

		// here you can add extra check and decide whether to start
		// the window move or not
		if (GetCursorPos(&g_OrigCursorPos))
		{
			RECT rt;
			GetWindowRect(hWnd, &rt);
			g_OrigWndPos.x = rt.left;
			g_OrigWndPos.y = rt.top;
			g_MovingMainWnd = true;
			SetCapture(hWnd);
			SetCursor(LoadCursor(NULL, IDC_SIZEALL));
		}
		return 0;

	case WM_LBUTTONUP:
		ReleaseCapture();
		return 0;

	case WM_CAPTURECHANGED:
		g_MovingMainWnd = (HWND)lParam == hWnd;
		return 0;

	case WM_MOUSEMOVE:
		if (g_MovingMainWnd)
		{
			POINT pt;
			if (GetCursorPos(&pt))
			{
				int wnd_x = g_OrigWndPos.x +
					(pt.x - g_OrigCursorPos.x);
				int wnd_y = g_OrigWndPos.y +
					(pt.y - g_OrigCursorPos.y);
				SetWindowPos(hWnd, NULL, wnd_x,
					wnd_y, 0, 0, SWP_NOACTIVATE |
					SWP_NOOWNERZORDER | SWP_NOZORDER |
					SWP_NOSIZE);
			}
		}
		return 0;

	default: ;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
