#include "D3D11.h"

D3D11::D3D11()
{
}

D3D11::~D3D11()
{
}

bool D3D11::Initalize (HWND hwnd, unsigned int swapChainWidth, unsigned int swapChainHeight, bool isFullScreen,
	bool isVSync, unsigned int refreshRate, bool isMsaa, unsigned int msaaSampleCount)
{
    D3D_FEATURE_LEVEL targetFeatureLevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
    const UINT targetFeatureLevelCount = 2;

    UINT deviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
    deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    if(FAILED(D3D11CreateDevice(
        nullptr,                    // Specify nullptr to use the default adapter.
        D3D_DRIVER_TYPE_HARDWARE,   // Create a device using the hardware graphics driver.
        0,                          // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
        deviceFlags,                // Set debug and Direct2D compatibility flags.
        targetFeatureLevels,        // List of feature levels this app can support.
        targetFeatureLevelCount,    // Size of the list above.
        D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
        m_device.ReleaseAndGetAddressOf (), // Returns the Direct3D device created.
        &m_featureLevel,            // Returns feature level of device created.
        m_immediateContext.ReleaseAndGetAddressOf() // Returns the device immediate context.
    )))
    {
        return false;
    }

    UINT msaaQuality = 0;
    m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, msaaSampleCount, &msaaQuality);

    Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
    m_device.As (&dxgiDevice);

    Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
    dxgiDevice->GetAdapter(dxgiAdapter.ReleaseAndGetAddressOf());

    Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory;
    dxgiAdapter->GetParent(__uuidof(IDXGIFactory), &dxgiFactory);

    DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
    swapChainDesc.BufferDesc.Width = swapChainWidth;
    swapChainDesc.BufferDesc.Height = swapChainHeight;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = isVSync ? refreshRate : 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.Windowed = isFullScreen == false;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    if (FAILED(dxgiFactory->CreateSwapChain(m_device.Get(), &swapChainDesc, m_swapChain.ReleaseAndGetAddressOf())))
    {
        return false;
    }

    // Swap chain의 Back buffer 가져온다.
    Microsoft::WRL::ComPtr<ID3D11Texture2D> swapChainBuffer;

    if (FAILED(m_swapChain->GetBuffer(0, __uuidof (ID3D11Texture2D), (void**)swapChainBuffer.ReleaseAndGetAddressOf())))
    {
        return false;
    }

    if (FAILED(m_device->CreateRenderTargetView(swapChainBuffer.Get(), nullptr, m_rtv.ReleaseAndGetAddressOf())))
    {
        return false;
    }

    D3D11_TEXTURE2D_DESC depthStencilDescription = { };
	depthStencilDescription.Width = swapChainWidth;
	depthStencilDescription.Height = swapChainHeight;
	depthStencilDescription.MipLevels = 1;
	depthStencilDescription.ArraySize = 1;
	depthStencilDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDescription.SampleDesc.Count = 1;
	depthStencilDescription.SampleDesc.Quality = 0;
	depthStencilDescription.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDescription.CPUAccessFlags = 0;
    depthStencilDescription.MiscFlags = 0;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
    if (FAILED(m_device->CreateTexture2D(&depthStencilDescription, nullptr, depthStencilBuffer.ReleaseAndGetAddressOf())))
    {
        return false;
    }

    // Depth stencil view 만든다.
    if (FAILED(m_device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, m_dsv.ReleaseAndGetAddressOf())))
    {
        return false;
    }

    m_immediateContext->OMSetRenderTargets(1, m_rtv.GetAddressOf(), m_dsv.Get());

    D3D11_VIEWPORT viewport;
    viewport.Width = static_cast<FLOAT> (swapChainWidth);
    viewport.Height = static_cast<FLOAT> (swapChainHeight);
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    m_immediateContext->RSSetViewports(1, &viewport);

	return true;
}

void D3D11::Shutdown()
{
    // 렌더 타겟 바인딩 해제한다.
    ID3D11RenderTargetView* nullRenderTargetViews[] = { nullptr };
    ID3D11DepthStencilView* nullDepthStencilView = nullptr;
    m_immediateContext->OMSetRenderTargets(1, nullRenderTargetViews, nullDepthStencilView);

    // 생성한 자원 레퍼런스 해제한다.
    m_dsv = nullptr;
    m_rtv = nullptr;
    m_swapChain = nullptr;

    m_immediateContext.Reset();
    m_device.Reset();
}

void D3D11::ClearRenderTarget(float red, float green, float blue, float alpha)
{
    // Render Target을 지정한 색으로 덮어쓴다.
    FLOAT color[4] = { red, green, blue, alpha };
    m_immediateContext->ClearRenderTargetView(m_rtv.Get(), color);
}

void D3D11::PresentSwapChain(bool isVSync)
{
    // Swap chain의 Back buffer와 Front Buffer를 교환한다.
    if (isVSync)
    {
        m_swapChain->Present(1, 0);
    }
    else
    {
        m_swapChain->Present(0, 0);
    }
}