#pragma once

#include <d3d12.h>
#include <Windows.h>
#include <memory>
#include "DeviceResources.h"
#include "Renderer.h"
#include <string>

using namespace std;

class MainClass
{
public:

	MainClass();
	~MainClass();

	HRESULT CreateDesktopWindow();

	HWND GetWindowHandle()
	{
		return m_hWnd;
	}

	static LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HRESULT Run(shared_ptr<DeviceResources> deviceResources, shared_ptr<Renderer> renderer);

private:
	HMENU m_hMenu;
	RECT m_rc;
	HWND m_hWnd;
};

static HINSTANCE m_hInstance;
static wstring m_windowClassName;

static bool g_MovingMainWnd = false;
static POINT g_OrigCursorPos;
static POINT g_OrigWndPos;

