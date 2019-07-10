#pragma once

#include <d3d12.h>
#include <d3d11_4.h>
#include <Windows.h>
#include <wrl.h>

using namespace Microsoft::WRL;

class DeviceResources
{
public:
	DeviceResources();
	~DeviceResources();

	HRESULT CreateDeviceResources(HWND hWnd);

	HRESULT ConfigureBackBuffer();
	HRESULT ReleaseBackBuffer();

	HRESULT GoFullScreen();
	HRESULT GoWindowed();

	float GetAspectRatio() const;

	ID3D11Device* GetDevice()
	{
		return m_device.Get();
	}

	ID3D11DeviceContext* GetDeviceContext()
	{
		return m_context.Get();
	}

	ID3D11RenderTargetView* GetRenderTarget()
	{
		return m_renderTargetView.Get();
	}

	ID3D11DepthStencilView* GetDepthStencil()
	{
		return m_depthStencilView.Get();
	}

	void Present() const;

private:

	// device
	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	ComPtr<IDXGISwapChain4> m_swapChain;

	// swap chain resources
	ComPtr<ID3D11Texture2D> m_backBuffer;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;

	// depth stencil
	ComPtr<ID3D11Texture2D> m_depthStencil;
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;

	// metadata
	D3D_FEATURE_LEVEL m_feature_level;
	D3D11_TEXTURE2D_DESC m_bbDesc{};
	D3D11_VIEWPORT m_viewport{};
};

