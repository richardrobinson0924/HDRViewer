#include "DeviceResources.h"
#include "Renderer.h"

DeviceResources::DeviceResources()
{

};

HRESULT DeviceResources::CreateDeviceResources(HWND hWnd)
{
	HRESULT hr = S_OK;

	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_12_1
	};

	UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(DEBUG) || defined(_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));

	desc.Windowed = TRUE;
	desc.BufferCount = 2;
	desc.BufferDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM; //?

	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	desc.OutputWindow = hWnd;

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	ComPtr<IDXGISwapChain> swapChain;

	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flags,
		levels,
		ARRAYSIZE(levels),
		D3D11_SDK_VERSION,
		&desc,
		swapChain.GetAddressOf(),
		device.GetAddressOf(),
		&m_feature_level,
		context.GetAddressOf()
	);

	device.As(&m_device);
	context.As(&m_context);
	swapChain.As(&m_swapChain);

	auto hdr = Renderer::configureHDR();

	m_swapChain->SetHDRMetaData(
		DXGI_HDR_METADATA_TYPE_HDR10,
		sizeof(DXGI_HDR_METADATA_HDR10),
		&hdr
	);

	m_swapChain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);

	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)& m_backBuffer);
	m_backBuffer->GetDesc(&m_bbDesc);

	ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));

	m_viewport.Height = (float)m_bbDesc.Height;
	m_viewport.Width = (float)m_bbDesc.Width;

	m_viewport.MinDepth = 0;
	m_viewport.MaxDepth = 1;

	m_context->RSSetViewports(1, &m_viewport);

	hr = m_device->CreateRenderTargetView(
		m_backBuffer.Get(),
		nullptr, 
		m_renderTargetView.GetAddressOf()
	);

	return hr;
}

HRESULT DeviceResources::ConfigureBackBuffer()
{
	auto hr = S_OK;

	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)& m_backBuffer);

	hr = m_device->CreateRenderTargetView(
		m_backBuffer.Get(),
		nullptr, 
		m_renderTargetView.GetAddressOf()
	);

	m_backBuffer->GetDesc(&m_bbDesc);

	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D32_FLOAT, // change this maybe?
		static_cast<UINT>(m_bbDesc.Width),
		static_cast<UINT>(m_bbDesc.Height),
		1,
		1,
		D3D11_BIND_DEPTH_STENCIL
	);

	m_device->CreateTexture2D(&depthStencilDesc, nullptr, &m_depthStencil);

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

	m_device->CreateDepthStencilView(
		m_depthStencil.Get(),
		&depthStencilViewDesc,
		&m_depthStencilView
	);

	ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));

	m_viewport.Height = (float)m_bbDesc.Height;
	m_viewport.Width = (float)m_bbDesc.Width;

	m_viewport.MinDepth = 0;
	m_viewport.MaxDepth = 1;

	m_context->RSSetViewports(1, &m_viewport);

	return hr;
}

HRESULT DeviceResources::ReleaseBackBuffer()
{
	auto hr = S_OK;

	m_renderTargetView.Reset();
	m_backBuffer.Reset();

	m_depthStencilView.Reset();
	m_depthStencil.Reset();

	m_context->Flush();

	return hr;
}

HRESULT DeviceResources::GoFullScreen()
{
	auto hr = S_OK;

	hr = m_swapChain->SetFullscreenState(TRUE, nullptr);
	ReleaseBackBuffer();

	hr = m_swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	hr = ConfigureBackBuffer();

	auto hdr = Renderer::configureHDR();

	m_swapChain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);


	m_swapChain->SetHDRMetaData(
		DXGI_HDR_METADATA_TYPE_HDR10,
		sizeof(DXGI_HDR_METADATA_HDR10),
		&hdr
	);

	m_swapChain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);


	return hr;
}

HRESULT DeviceResources::GoWindowed()
{
	auto hr = S_OK;

	hr = m_swapChain->SetFullscreenState(FALSE, nullptr);
	ReleaseBackBuffer();

	hr = m_swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	hr = ConfigureBackBuffer();

	return hr;
}

float DeviceResources::GetAspectRatio() const
{
	return static_cast<float>(m_bbDesc.Width) / static_cast<float>(m_bbDesc.Height);
}

void DeviceResources::Present() const
{
	m_swapChain->Present(1, 0);
}

DeviceResources::~DeviceResources()
= default;


