//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <string>
#include <memory>
#include <ppltasks.h>

#include "Renderer.h"
#include <iostream>


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Renderer::Renderer(std::shared_ptr<DeviceResources> deviceResources)
	:
	m_frameCount(0),
	m_deviceResources(deviceResources)
{
	m_frameCount = 0; // init frame count
}


//-----------------------------------------------------------------------------
// Create device-dependent resources for rendering.
//-----------------------------------------------------------------------------
void Renderer::CreateDeviceDependentResources()
{

}

void Renderer::CreateWindowSizeDependentResources()
{

}


//-----------------------------------------------------------------------------
// Update the scene.
//-----------------------------------------------------------------------------
void Renderer::Update()
{
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		m_deviceResources->GoWindowed();
	}

	if (GetAsyncKeyState(VK_TAB))
	{
		m_deviceResources->GoFullScreen();
	}
}

//-----------------------------------------------------------------------------
// Render the cube.
//-----------------------------------------------------------------------------
void Renderer::Render()
{
	// Use the Direct3D device context to draw.
	ID3D11DeviceContext* context = m_deviceResources->GetDeviceContext();

	ID3D11RenderTargetView* renderTarget = m_deviceResources->GetRenderTarget();
	ID3D11DepthStencilView* depthStencil = m_deviceResources->GetDepthStencil();


	const float clearColor[4] = { 1.10f, 0.00f, 1.00f, 1.0f };
	context->ClearRenderTargetView(renderTarget, clearColor);
}

//-----------------------------------------------------------------------------
// Clean up cube resources when the Direct3D device is lost or destroyed.
//-----------------------------------------------------------------------------
Renderer::~Renderer()
{
	// ComPtr will clean up references for us. But be careful to release
	// references to anything you don't need whenever you call Flush or Trim.
	// As always, clean up your system (CPU) memory resources before exit.
}

DXGI_HDR_METADATA_HDR10 Renderer::configureHDR()
{
	DXGI_HDR_METADATA_HDR10 hdr = {};

	const DisplayChromacities chroma = {
		0.70800f,
		0.29200f,
		0.17000f,
		0.79700f,
		0.13100f,
		0.04600f,
		0.31270f,
		0.32900f
	};

	hdr.RedPrimary[0] = static_cast<UINT16>(chroma.RedX * 50000.0f);
	hdr.RedPrimary[1] = static_cast<UINT16>(chroma.RedY * 50000.0f);

	hdr.GreenPrimary[0] = static_cast<UINT16>(chroma.GreenX * 50000.0f);
	hdr.GreenPrimary[1] = static_cast<UINT16>(chroma.GreenY * 50000.0f);

	hdr.BluePrimary[0] = static_cast<UINT16>(chroma.BlueX * 50000.0f);
	hdr.BluePrimary[1] = static_cast<UINT16>(chroma.BlueY * 50000.0f);

	hdr.WhitePoint[0] = static_cast<UINT16>(chroma.WhiteX * 50000.0f);
	hdr.WhitePoint[1] = static_cast<UINT16>(chroma.WhiteY * 50000.0f);

	hdr.MaxMasteringLuminance = static_cast<UINT>(1000.0f * 10000.0f);
	hdr.MinMasteringLuminance = static_cast<UINT>(0.001f * 10000.0f);

	hdr.MaxContentLightLevel = static_cast<UINT16>(2000.0f);
	hdr.MaxFrameAverageLightLevel = static_cast<UINT16>(500.0f);

	return hdr;
}
