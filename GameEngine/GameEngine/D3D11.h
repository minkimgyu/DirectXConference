#pragma once

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "dxguid.lib")

#include <Windows.h>
#include <wrl.h>
#include <d3d11.h>
#include <d3dcompiler.h>

class D3D11
{
public:
	D3D11();
	virtual ~D3D11();

	bool Initalize(HWND hwnd, unsigned int swapChainWidth, unsigned int swapChainHeight, bool isFullScreen,
		bool isVSync, unsigned int refreshRate, bool isMsaa, unsigned int msaaSampleCount);
	void Shutdown();

	void ClearRenderTarget(float red, float green, float blue, float alpha);
	void PresentSwapChain(bool isVSync);

private:

	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_immediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;
	D3D_FEATURE_LEVEL m_featureLevel;
};

