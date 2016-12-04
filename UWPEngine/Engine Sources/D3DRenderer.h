#pragma once

#include <dxgi1_2.h>
#include "IDrawable.h"

class CD3DRenderer
{
public:
	bool Initialize(Windows::UI::Xaml::Controls::SwapChainPanel^ swapChainPanel);
	bool UpdateForWindowSizeOrScaleChanged(const Windows::Foundation::Size *newSize, float xScale, float yScale);
	void BeginDraw();
	void Draw();
	void Draw(const IDrawable& drawableObject, DirectX::XMFLOAT4X4 cameraMatrix);
	void EndDraw() const;
	void Present();

public:
	CD3DRenderer();
	virtual ~CD3DRenderer();

private:
	bool CreateD3D();
	bool CreateSwapChain(Windows::UI::Xaml::Controls::SwapChainPanel^ swapChainPanel, UINT width, UINT height);
	void SetCompositionScale(float fCompositionScaleX, float fCompositionScaley);
	
	bool CreateRTV_DSV_VP(UINT width, UINT height);
	void SetRTVandDSVtoContext(float defaultColor[]);
	void SetVPToContext();

	bool CreateShaders();
	bool CreateInputLayout();
	bool CreateVertexBuffer(const IDrawable& object);
	bool CreateIndexBuffer(const IDrawable& object);
	bool CreateConstantBuffer();

	void CleanRTVandDSV();

	DirectX::XMFLOAT4X4										m_projMatrix;

	bool													m_bInitialized;

	// DXGI SwapChain & XAML SwapChainPanel
	Microsoft::WRL::ComPtr<IDXGISwapChain1>					m_pSwapChain;
	Microsoft::WRL::ComPtr<ISwapChainPanelNative>			m_pSwapChainPanelNative;
	
	// D3D11 device, device context
	Microsoft::WRL::ComPtr<ID3D11Device>					m_pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>				m_pDeviceContext;
	
	// related to Output Merge Stage
	Microsoft::WRL::ComPtr<ID3D11Texture2D>					m_pBackBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>			m_pRTV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>			m_pDSV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>					m_pDepthStencilBuffer;
	D3D11_VIEWPORT											m_vp;

	DWORD													m_dwWidth;
	DWORD													m_dwHeight;

	// resources
	Microsoft::WRL::ComPtr<ID3D11Buffer>					m_pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>					m_pIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>					m_pConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>				m_pVertexInputLayout;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>				m_pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>				m_pPixelShader;

	// selected feature level
	D3D_FEATURE_LEVEL										m_featureLevel;

	// about display property
	float													m_dpi;
	float													m_compositionScaleX;
	float													m_compositionScaleY;
	Windows::Foundation::Size								m_logicalSize;
	Windows::Foundation::Size								m_outputSize;
	Windows::Foundation::Size								m_d3dRenderTargetSize;
	Windows::Graphics::Display::DisplayOrientations			m_nativeOrientation;
	Windows::Graphics::Display::DisplayOrientations			m_currentOrientation;
};