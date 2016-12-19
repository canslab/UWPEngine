#pragma once

#include <dxgi1_2.h>
#include "IDrawable.h"

class CD3DRenderer
{
public:
	// 렌더러를 XAML 체인패널을 이용해 초기화 한다.
	bool Initialize(Windows::UI::Xaml::Controls::SwapChainPanel^ swapChainPanel);

	// 윈도우 사이즈나, 스케일 값이 변경되었을 때 호출한다.
	bool UpdateForWindowSizeOrScaleChanged(const Windows::Foundation::Size *newSize, float xScale, float yScale);
	
	// drawableObject를 그리기 이전에 해야하는 작업들을 수행한다.
	void BeReadyForDrawableObject(const IDrawable& drawableObject);
	
	// 드로잉을 준비한다!, 렌더타깃 설정, 뷰포트 설정, 렌더타깃 Clear, 스텐실뷰 Clear
	void BeginDraw();

	// drawable Object를 그린다!
	void Draw(const IDrawable& drawableObject);

	// 지금까지 드로잉한 내용을 Present한다
	void EndDraw() const;

public:
	CD3DRenderer();
	virtual ~CD3DRenderer();

private:
	bool _CreateD3D();
	bool _CreateSwapChain(Windows::UI::Xaml::Controls::SwapChainPanel^ swapChainPanel, UINT width, UINT height);
	void _SetCompositionScale(float fCompositionScaleX, float fCompositionScaley);
	
	bool _CreateRTV_DSV_VP(UINT width, UINT height);
	void _SetRTVandDSVtoContext(float defaultColor[]);
	void _SetVPToContext();

	bool _CreateShaders();
	bool _CreateInputLayout(const IDrawable& object);
	bool _CreateVertexBuffer(const IDrawable& object);
	bool _CreateIndexBuffer(const IDrawable& object);
	bool _CreateConstantBuffer();

	void _CleanRTVandDSV();

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