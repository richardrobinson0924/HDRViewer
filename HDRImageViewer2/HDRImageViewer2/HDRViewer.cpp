
#include <memory>

#include "DeviceResources.h"
#include "Renderer.h"
#include "MainClass.h"
#include <dxgi1_6.h>
#include <iostream>

using namespace std;

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	auto hr = S_OK;

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	auto winMain = std::make_shared<MainClass>();

	hr = winMain->CreateDesktopWindow();

	if (SUCCEEDED(hr))
	{
		auto resources = make_shared<DeviceResources>();
		auto renderer = std::make_shared<Renderer>(resources);

		renderer->CreateDeviceDependentResources();
		resources->CreateDeviceResources(winMain->GetWindowHandle());

		renderer->CreateWindowSizeDependentResources();
		//resources->GoFullScreen();

		renderer->CreateWindowSizeDependentResources();

		hr = winMain->Run(resources, renderer);
	}

	return hr;
}