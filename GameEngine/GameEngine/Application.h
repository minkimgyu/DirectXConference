#pragma once

#include <string>
#include <memory>
#include <Windows.h>

using WindowProcedure = LRESULT(*) (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class Application
{
public:
	Application();
	virtual ~Application();

	bool Initialize(LPCTSTR name, unsigned int width, unsigned int height, bool isFullScreen);
	void Shutdown();

	void Update();

private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	std::unique_ptr<TCHAR[]> m_className;
};

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);