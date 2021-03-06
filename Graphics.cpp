#include "Graphics.h"
#include <cassert>


Graphics::Graphics(HWND hWND)
{
	UINT m4xMsaaQuality;
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWND;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&m_pSwap,
		&m_pDevice,
		nullptr,
		&m_pContext
	);
	ID3D11Resource* pBackBuffer = nullptr;
	m_pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
	m_pDevice->CreateRenderTargetView(
		pBackBuffer,
		nullptr,
		&m_pTarget
	);
	pBackBuffer->Release();
	m_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality);
	assert(m4xMsaaQuality > 0);
}

Graphics::~Graphics()
{
	if (m_pSwap != nullptr)
	{
		m_pSwap->Release();
	}
	if (m_pContext != nullptr)
	{
		m_pContext->Release();
	}
	if (m_pTarget != nullptr)
	{
		m_pTarget->Release();
	}
	if (m_pDevice != nullptr)
	{
		m_pDevice->Release();
	}
}

void Graphics::EndFrame()
{
	m_pSwap->Present(1u, 0u);
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red,green,blue,1.0f };

	m_pContext->ClearRenderTargetView(m_pTarget, color);
}