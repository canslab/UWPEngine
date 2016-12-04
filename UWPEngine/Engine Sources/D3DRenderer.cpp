﻿#include "pch.h"
#include "D3DRenderer.h"
#include "Util.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Xaml::Controls;
using namespace Microsoft::WRL;
using namespace Platform;

#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "DXGI.lib" )
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "D3DCompiler.lib" )

bool CD3DRenderer::Initialize(Windows::UI::Xaml::Controls::SwapChainPanel ^ swapChainPanel)
{
	assert(m_bInitialized == false);
	bool bResult = true;

	auto currentDisplayInformation = DisplayInformation::GetForCurrentView();
	float dpi = currentDisplayInformation->LogicalDpi;

	CoreWindow^ window = CoreWindow::GetForCurrentThread();
	m_nativeOrientation = currentDisplayInformation->NativeOrientation;
	m_currentOrientation = currentDisplayInformation->CurrentOrientation;
	m_logicalSize = Windows::Foundation::Size(static_cast<float>(swapChainPanel->ActualWidth), static_cast<float>(swapChainPanel->ActualHeight));
	m_dpi = dpi;
	m_compositionScaleX = swapChainPanel->CompositionScaleX;
	m_compositionScaleY = swapChainPanel->CompositionScaleY;

	// get effect width of output like below
	// ex) compositionScaleX = 2, logical width = 1500 
	//		=> outputSize width = 3000
	m_outputSize.Width = m_logicalSize.Width * m_compositionScaleX;
	m_outputSize.Height = m_logicalSize.Height * m_compositionScaleY;

	// outputSize should be at least larger than one
	m_outputSize.Width = max(m_outputSize.Width, 1);
	m_outputSize.Height = max(m_outputSize.Height, 1);

	// render target size is same as outputSize
	m_d3dRenderTargetSize.Width = m_outputSize.Width;
	m_d3dRenderTargetSize.Height = m_outputSize.Height;

	UINT width = lround(m_d3dRenderTargetSize.Width);
	UINT height = lround(m_d3dRenderTargetSize.Height);

	// Create D3D 
	bResult = CreateD3D();
	if (bResult == false)
	{
		__debugbreak();
		return false;
	}

	bResult = CreateSwapChain(swapChainPanel, width, height);
	if (bResult == false)
	{
		__debugbreak();
		return false;
	}

	bResult = CreateRTV_DSV_VP(width, height);
	if (bResult == false)
	{
		__debugbreak();
		return false;
	}

	bResult = CreateShaders();
	if (bResult == false)
	{
		__debugbreak();
		return false;
	}

	bResult = CreateInputLayout();
	if (bResult == false)
	{
		__debugbreak();
		return false;
	}

	bResult = CreateConstantBuffer();
	if (bResult == false)
	{
		__debugbreak();
		return false;
	}


	/*bResult = CreateVertexBuffer(
	if (bResult == false)
	{
		__debugbreak();
		return false;
	}*/

	float initColor[] = { 0,0,0,1 };

	// Bind render target view & depth stencil view to the OM Stage
	SetRTVandDSVtoContext(initColor);

	// Bind Viewport to RS stage
	SetVPToContext();

	// set initalized !
	m_bInitialized = true;
	return true;
}

// Create D3D Device
bool CD3DRenderer::CreateD3D()
{
	assert(m_pDevice == nullptr && m_pDeviceContext == nullptr);
	HRESULT hr;
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG) && !defined(ARM_ARCH)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	ComPtr<IDXGIFactory1> pFactory;
	IDXGIAdapter *pAdapter = nullptr;

	DXGI_ADAPTER_DESC adapterDesc = { 0 };
	ZeroMemory(&adapterDesc, sizeof(adapterDesc));

	if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory)))
	{
		__debugbreak();
		return false;
	}
	int nDeviceCount = 0;


	// choose dedicated graphics if possible
	while (pFactory->EnumAdapters(nDeviceCount, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		pAdapter->GetDesc(&adapterDesc);

		if (adapterDesc.VendorId == JHUtil::VENDOR_ID_QUALCOMM
			|| adapterDesc.VendorId == JHUtil::VENDOR_ID_NVIDIA
			|| adapterDesc.VendorId == JHUtil::VENDOR_ID_AMD )
		{
			break;
		}
		pAdapter->Release();
		pAdapter = NULL;
		nDeviceCount++;
	}

	D3D_DRIVER_TYPE driverType;

	// if there is dedicated graphics, driver type value should be D3D_DRIVER_TYPE_UNKNOWN
	driverType = (pAdapter) ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE;

	ComPtr<ID3D11Device> pDevice = nullptr;
	ComPtr<ID3D11DeviceContext> pDeviceContext = nullptr;

	/*pAdapter = nullptr;
	driverType = D3D_DRIVER_TYPE_HARDWARE;*/

	hr = D3D11CreateDevice(
		pAdapter,
		driverType,
		NULL,
		creationFlags,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		pDevice.GetAddressOf(),
		&m_featureLevel,
		pDeviceContext.GetAddressOf()
	);
	if (pAdapter)
	{
		pAdapter->Release();
		pAdapter = nullptr;
	}

	if (FAILED(hr))
	{
#ifdef _DEBUG
		WCHAR txt[512] = { 0 };
		swprintf_s(txt, L"File : %S , Line : %d \n", __FILE__, __LINE__);
#endif
		__debugbreak();
		return false;
	}

	hr = pDevice->QueryInterface(__uuidof(ID3D11Device1), (void**)&m_pDevice);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		WCHAR txt[512] = { 0 };
		swprintf_s(txt, L"File : %S , Line : %d \n", __FILE__, __LINE__);
#endif
		__debugbreak();
		return false;
	}

	hr = pDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), (void**)&m_pDeviceContext);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		WCHAR txt[512] = { 0 };
		swprintf_s(txt, L"File : %S , Line : %d \n", __FILE__, __LINE__);
#endif
		__debugbreak();
		return false;
	}

	return true;
}
bool CD3DRenderer::CreateSwapChain(Windows::UI::Xaml::Controls::SwapChainPanel ^ swapChainPanel, UINT width, UINT height)
{
	assert(swapChainPanel != nullptr && width >= 1 && height >= 1);
	assert(m_pSwapChain == nullptr && m_pSwapChainPanelNative == nullptr && m_pDevice != nullptr);
	HRESULT hr;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
	swapChainDesc.Height = height;
	swapChainDesc.Width = width;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;								// Don't use multi sampling
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// it binds to render target output
	swapChainDesc.BufferCount = 2;									// it should be greater than 1
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;					// UWP app settings
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;	// Windows store app settings
	swapChainDesc.Flags = 0;

	ComPtr<IDXGIDevice1> pdxgiDevice = nullptr;
	ComPtr<IDXGIAdapter> pdxgiAdapter = nullptr;
	ComPtr<IDXGIFactory2>pdxgiFactory = nullptr;

	hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&pdxgiDevice);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		WCHAR txt[512] = { 0 };
		swprintf_s(txt, L"File : %S , Line : %d \n", __FILE__, __LINE__);
#endif
		__debugbreak();
	}

	hr = pdxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pdxgiAdapter);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		WCHAR txt[512] = { 0 };
		swprintf_s(txt, L"File : %S , Line : %d \n", __FILE__, __LINE__);
#endif
		__debugbreak();
		return false;
	}

	hr = pdxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&pdxgiFactory);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		WCHAR txt[512] = { 0 };
		swprintf_s(txt, L"File : %S , Line : %d \n", __FILE__, __LINE__);
#endif
		__debugbreak();
		return false;
	}

	if (swapChainPanel)
	{
		// if there is something that points from m_pSwapChain,
		// it means CreateSwapChain() was already invoked. so it's abnormal case.
		if (m_pSwapChain)
		{
#ifdef _DEBUG
			WCHAR txt[512] = { 0 };
			swprintf_s(txt, L"File : %S , Line : %d \n", __FILE__, __LINE__);
#endif
			__debugbreak();
			return false;
		}

		// create swap chain 
		hr = pdxgiFactory->CreateSwapChainForComposition(m_pDevice.Get(), &swapChainDesc, nullptr, &m_pSwapChain);
		if (FAILED(hr))
		{
#ifdef _DEBUG
			WCHAR txt[512] = { 0 };
			swprintf_s(txt, L"File : %S , Line : %d \n", __FILE__, __LINE__);
#endif
			__debugbreak();
			return false;
		}
		
		// map created SwapChain(m_pSwapChain) to swapChainPanel(XAML controls)
		reinterpret_cast<IUnknown*>(swapChainPanel)->QueryInterface(__uuidof(ISwapChainPanelNative), (void**)&m_pSwapChainPanelNative);
		hr = m_pSwapChainPanelNative->SetSwapChain(m_pSwapChain.Get());
		if (FAILED(hr))
		{
#ifdef _DEBUG
			WCHAR txt[512] = { 0 };
			swprintf_s(txt, L"File : %S , Line : %d \n", __FILE__, __LINE__);
#endif
			__debugbreak();
			return false;
		}
	}
	else
	{
#ifdef _DEBUG
		WCHAR txt[512] = { 0 };
		swprintf_s(txt, L"File : %S , Line : %d \n", __FILE__, __LINE__);
#endif
		__debugbreak();
		return false;
	}

	// Sets the number of frames that the system is allowed to queue for rendering.
	pdxgiDevice->SetMaximumFrameLatency(1);

	// set no rotation of the back buffers for the swap chain
	m_pSwapChain->SetRotation(DXGI_MODE_ROTATION_IDENTITY);

	// inverse scale 필요
	SetCompositionScale(m_compositionScaleX, m_compositionScaleY);

	return true;
}
bool CD3DRenderer::CreateRTV_DSV_VP(UINT width, UINT height)
{
	assert(m_pSwapChain != nullptr && m_pSwapChainPanelNative != nullptr);
	assert(m_pDevice != nullptr && width >= 1 && height >= 1);
	HRESULT hr;

	// First Render target view and depth stencil view should be released
	// also, swap chain's backbuffer pointer & depth stencil buffer's pointer should be released
	CleanRTVandDSV();

	// Only is 0th index of swap chain readable & writable
	// Load the swap chain's backbuffer into m_pBackBuffer newly
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(m_pBackBuffer.GetAddressOf()));

	if (FAILED(hr))
	{
		__debugbreak();
		goto whenWrong;
	}

	// create render target view using backbuffer of the swap chain
	m_pDevice->CreateRenderTargetView(m_pBackBuffer.Get(), NULL, m_pRTV.GetAddressOf());
	if (FAILED(hr))
	{
		__debugbreak();
		goto whenWrong;
	}

	// Describe depth stencil buffer 
	D3D11_TEXTURE2D_DESC DSBufferDESC;
	ZeroMemory(&DSBufferDESC, sizeof(DSBufferDESC));

	DSBufferDESC.Usage = D3D11_USAGE_DEFAULT;
	DSBufferDESC.BindFlags = D3D11_BIND_DEPTH_STENCIL;		// it binds to depth stencil view
	DSBufferDESC.CPUAccessFlags = 0;
	DSBufferDESC.MiscFlags = 0;
	DSBufferDESC.SampleDesc.Count = 1;						// no multi sampling
	DSBufferDESC.SampleDesc.Quality = 0;					// no multi sampling
	DSBufferDESC.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// depth 24 bits, stencil 8 bits
	DSBufferDESC.Width = width;
	DSBufferDESC.Height = height;
	DSBufferDESC.MipLevels = 1;
	DSBufferDESC.ArraySize = 1;

	// allocate depth stencil buffer
	hr = m_pDevice->CreateTexture2D(&DSBufferDESC, nullptr, m_pDepthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		__debugbreak();
		goto whenWrong;
	}

	// Describe depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	ZeroMemory(&DSVDesc, sizeof(DSVDesc));
	DSVDesc.Format = DSBufferDESC.Format;
	DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSVDesc.Texture2D.MipSlice = 0;

	// allocate depth stencil view
	hr = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), &DSVDesc, m_pDSV.GetAddressOf());
	if (FAILED(hr))
	{
		__debugbreak();
		goto whenWrong;
	}

	m_dwWidth = width;
	m_dwHeight = height;

	// describe view port
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	// because viewport transformation is done at Rasterization Stage
	m_vp = vp;

	return true;

whenWrong:
	return false;
}

void CD3DRenderer::SetRTVandDSVtoContext(float defaultColor[])
{
	assert(m_pRTV != nullptr && m_pDSV != nullptr);

	m_pDeviceContext->OMSetRenderTargets(1, m_pRTV.GetAddressOf(), m_pDSV.Get());
	m_pDeviceContext->ClearRenderTargetView(m_pRTV.Get(), defaultColor);
}
void CD3DRenderer::SetVPToContext()
{
	assert(m_vp.Width >= 1 && m_vp.Height >= 1);
	m_pDeviceContext->RSSetViewports(1, &m_vp);
}
void CD3DRenderer::SetCompositionScale(float fCompositionScaleX, float fCompositionScaleY)
{
	assert(fCompositionScaleX > 0 && fCompositionScaleY > 0 && m_pSwapChain != nullptr);
	DXGI_MATRIX_3X2_F inverseScale = { 0 };
	float comp_scale_x = fCompositionScaleX;
	float comp_scale_y = fCompositionScaleY;
	inverseScale._11 = 1.0f / fCompositionScaleX;
	inverseScale._22 = 1.0f / fCompositionScaleY;

	ComPtr<IDXGISwapChain2> pSwapChain2 = nullptr;
	m_pSwapChain->QueryInterface(__uuidof(IDXGISwapChain2), (void**)&pSwapChain2);
	pSwapChain2->SetMatrixTransform(&inverseScale);
}

bool CD3DRenderer::CreateShaders()
{
	assert(m_pDevice != nullptr && m_pDeviceContext != nullptr);
	HRESULT hr;

	Array<byte>^ vsFileByteCode = JHUtil::LoadShaderFile("vertex.cso");
	Array<byte>^ psFileByteCode = JHUtil::LoadShaderFile("pixel.cso");

	if (vsFileByteCode == nullptr || psFileByteCode == nullptr)
	{
		__debugbreak();
		return false;
	}

	hr = m_pDevice->CreateVertexShader(vsFileByteCode->Data, vsFileByteCode->Length, nullptr, &m_pVertexShader);
	if (FAILED(hr))
	{
		__debugbreak();
		return false;
	}
	hr = m_pDevice->CreatePixelShader(psFileByteCode->Data, psFileByteCode->Length, nullptr, &m_pPixelShader);
	if (FAILED(hr))
	{
		__debugbreak();
		return false;
	}

	// resource deallocation manually
	vsFileByteCode = nullptr;
	psFileByteCode = nullptr;

	return true;
}
bool CD3DRenderer::CreateInputLayout()
{
	assert(m_pDevice != nullptr && m_pDeviceContext != nullptr);

	HRESULT hr;
	Array<byte>^ vsShaderByteCode = JHUtil::LoadShaderFile("vertex.cso");

	// before creating input layout, if there is something that points from m_pVertexInputLayout
	// release it before creating input layout.
	if (m_pVertexInputLayout)
	{
		m_pVertexInputLayout.Reset();
		m_pVertexInputLayout = nullptr;
	}

	// describe how vertex data looks like
	D3D11_INPUT_ELEMENT_DESC desc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// check whether input signature of vertex shader is identical to the description of the input element
	hr = m_pDevice->CreateInputLayout(desc, ARRAYSIZE(desc), vsShaderByteCode->Data, vsShaderByteCode->Length, &m_pVertexInputLayout);
	if (FAILED(hr))
	{
		__debugbreak();
		return false;
	}

	// deallocate resources manually
	vsShaderByteCode = nullptr;
	return true;
}
bool CD3DRenderer::CreateVertexBuffer(const IDrawable& object)
{
	HRESULT hr;
	D3D11_BUFFER_DESC vertexBufferDesc = { 0 };

	// byte width means the size of vertex buffer
	vertexBufferDesc.ByteWidth = object.GetVertexByteSize() * object.GetVertexCount();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.StructureByteStride = 0;

	void *pVertexData = object.GetAddressOfVertexArray();

	D3D11_SUBRESOURCE_DATA srd = { pVertexData, 0, 0 };
	
	hr = m_pDevice->CreateBuffer(&vertexBufferDesc, &srd, &m_pVertexBuffer);
	if (FAILED(hr))
	{
		__debugbreak();
		return false;
	}

	return true;
}
bool CD3DRenderer::CreateIndexBuffer(const IDrawable & object)
{
	HRESULT hr;
	D3D11_BUFFER_DESC indexBufferDesc = { 0 };

	indexBufferDesc.ByteWidth = sizeof(UINT) * object.GetIndexCount();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.StructureByteStride = 0;

	void *pIndexData = object.GetAddressOfIndexArray();
	D3D11_SUBRESOURCE_DATA srd = { pIndexData, 0, 0 };
	
	hr = m_pDevice->CreateBuffer(&indexBufferDesc, &srd, &m_pIndexBuffer);
	if (FAILED(hr))
	{
		__debugbreak();
		return false;
	}

	return true;
}

bool CD3DRenderer::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(float) * 48;	// multiples of 16
	desc.CPUAccessFlags = 0;				// it should be designatd
	desc.MiscFlags = 0;						// it should be designated

	HRESULT hr = m_pDevice->CreateBuffer(&desc, NULL, &m_pConstantBuffer);
	if (FAILED(hr))
	{
		__debugbreak();
		return false;
	}

	return true;
}

void CD3DRenderer::CleanRTVandDSV()
{
	assert(m_pDevice != nullptr && m_pDeviceContext != nullptr);
	if (m_pRTV)
	{
		m_pRTV.Reset();
		m_pRTV = nullptr;
	}

	if (m_pBackBuffer)
	{
		m_pBackBuffer.Reset();
		m_pBackBuffer = nullptr;
	}

	if (m_pDepthStencilBuffer)
	{
		m_pDepthStencilBuffer.Reset();
		m_pDepthStencilBuffer = nullptr;
	}

	if (m_pDSV)
	{
		m_pDSV.Reset();
		m_pDSV = nullptr;
	}
}

bool CD3DRenderer::UpdateForWindowSizeOrScaleChanged(const Windows::Foundation::Size *newSize, float xScale, float yScale)
{
	assert(m_bInitialized == true);
	assert(newSize != nullptr && newSize->Width >= 1 && newSize->Height >= 1 && xScale > 0 && yScale > 0);

	HRESULT hr;
	// If updated windows size and scale information are identical to the previous information,
	// There is no need to do any work such as resizing back buffer
	if (*newSize == m_logicalSize && m_compositionScaleX == xScale && m_compositionScaleY == yScale)
	{
		return false;
	}

	DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

	m_logicalSize = *newSize;
	m_compositionScaleX = xScale;
	m_compositionScaleY = yScale;

	m_nativeOrientation = currentDisplayInformation->NativeOrientation;
	m_currentOrientation = currentDisplayInformation->CurrentOrientation;

	// Reset RenderTargetView
	m_pDeviceContext->OMSetRenderTargets(0, NULL, NULL);
	CleanRTVandDSV();

	m_outputSize.Width = max(m_logicalSize.Width * m_compositionScaleX, 1);
	m_outputSize.Height = max(m_logicalSize.Height * m_compositionScaleY, 1);

	m_d3dRenderTargetSize = m_outputSize;

	UINT uiWidth = lround(m_d3dRenderTargetSize.Width);
	UINT uiHeight = lround(m_d3dRenderTargetSize.Height);

	// Swap Chain backbuffer도 resize해야지
	DXGI_SWAP_CHAIN_DESC desc;
	hr = m_pSwapChain->GetDesc(&desc);
	if (FAILED(hr))
	{
		__debugbreak();
		return false;
	}

	hr = m_pSwapChain->ResizeBuffers(desc.BufferCount, uiWidth, uiHeight, desc.BufferDesc.Format, 0);
	if (FAILED(hr))
	{
		__debugbreak();
		return false;
	}

	float defaultColor[] = { 0,0,0,1 };

	CreateRTV_DSV_VP(uiWidth, uiHeight);
	SetRTVandDSVtoContext(defaultColor);
	SetVPToContext();

	hr = m_pSwapChain->SetRotation(DXGI_MODE_ROTATION_IDENTITY);
	if (FAILED(hr))
	{
		__debugbreak();
		return false;
	}
	auto ratio = (float)uiWidth / uiHeight;
	DirectX::XMMATRIX p = DirectX::XMMatrixPerspectiveFovLH(0.25 * DirectX::XM_PI, ratio, 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&m_projMatrix, p);

	SetCompositionScale(m_compositionScaleX, m_compositionScaleY);	

	return true;
}

void CD3DRenderer::BeginDraw()
{
	assert(m_bInitialized == true);
	const FLOAT rgba[] = { 0,0,0,1 };

	m_pDeviceContext->OMSetRenderTargets(1, m_pRTV.GetAddressOf(), m_pDSV.Get());
	m_pDeviceContext->RSSetViewports(1, &m_vp);

	m_pDeviceContext->ClearRenderTargetView(m_pRTV.Get(), rgba);
	m_pDeviceContext->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}
void CD3DRenderer::Present()
{
	assert(m_bInitialized == true);
	UINT uiSyncInterval = 0;
	HRESULT hr;

	hr = m_pSwapChain->Present(uiSyncInterval, 0);

	if (FAILED(hr))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			__debugbreak();
		}
		else
		{
			__debugbreak();
		}
	}
}
void CD3DRenderer::Draw()
{
	assert(m_bInitialized == true);
	m_pDeviceContext->IASetInputLayout(m_pVertexInputLayout.Get());
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(float) * 3;
	UINT offset = 0;

	m_pDeviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);

	m_pDeviceContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	m_pDeviceContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
	m_pDeviceContext->Draw(3, 0);
}

void CD3DRenderer::Draw(const IDrawable & drawableObject, DirectX::XMFLOAT4X4 cameraMatrix)
{
	assert(m_bInitialized == true);
	CreateVertexBuffer(drawableObject);
	CreateIndexBuffer(drawableObject);
	
	UINT stride = drawableObject.GetVertexByteSize();
	UINT offset = 0;

	m_pDeviceContext->IASetInputLayout(m_pVertexInputLayout.Get());
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pDeviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	m_pDeviceContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	m_pDeviceContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
	m_pDeviceContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);

	auto tempWorldMatrix = drawableObject.GetWorldMatrix();
	auto tempCameraMatrix = DirectX::XMLoadFloat4x4(&cameraMatrix);
	auto tempProjMatrix = DirectX::XMLoadFloat4x4(&m_projMatrix);
	auto resultMat = tempWorldMatrix * tempCameraMatrix * tempProjMatrix;
	
	struct CIBAL
	{
		float worldMatrix[4][4];
		float viewMatrix[4][4];
		float projMatrix[4][4];
	};

	CIBAL ooo;
	memcpy(ooo.worldMatrix, &tempWorldMatrix, sizeof(float) * 16);
	memcpy(ooo.viewMatrix, &tempCameraMatrix, sizeof(float) * 16);
	memcpy(ooo.projMatrix, &tempProjMatrix, sizeof(float) * 16);

	m_pDeviceContext->UpdateSubresource(m_pConstantBuffer.Get(), 0, nullptr, &ooo, 0, 0);
	m_pDeviceContext->DrawIndexed(drawableObject.GetIndexCount(), 0, 0);
}

void CD3DRenderer::EndDraw() const
{
}

CD3DRenderer::CD3DRenderer()
{
	m_pSwapChain = nullptr;
	m_pSwapChainPanelNative = nullptr;
	m_pBackBuffer = nullptr;

	m_pRTV = nullptr;
	m_pDSV = nullptr;
	m_pDepthStencilBuffer = nullptr;

	m_dwHeight = m_dwWidth = 0;

	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_pConstantBuffer = nullptr;
	m_pVertexInputLayout = nullptr;

	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;

	m_pDevice = nullptr;
	m_pDeviceContext = nullptr;
	m_bInitialized = false;
}
CD3DRenderer::~CD3DRenderer()
{
	m_bInitialized = false;
	CleanRTVandDSV();
}