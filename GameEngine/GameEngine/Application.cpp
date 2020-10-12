#include <tchar.h>
#include "Application.h"
#include "D3D11.h"

D3D11 g_d3d11;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        // Else: User canceled. Do nothing.
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

Application::Application()
{
}

Application::~Application()
{
}



bool Application::Initialize(LPCTSTR name, unsigned int width, unsigned int height, bool isFullScreen)
{
    size_t nameLength = _tcslen(name);
    m_className = std::make_unique<TCHAR[]>(nameLength + 1);
    _tcsncpy_s (m_className.get (), nameLength + 1, name, nameLength);

    m_hInstance = GetModuleHandle(NULL);

    // Register the window class.

    WNDCLASS wc = { };
    wc.style = 0;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hInstance;
    wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = name;

    if (FAILED(RegisterClass(&wc)))
    {
        return false;
    }

    DWORD style = 0;
    
    
    if (isFullScreen)
    {
        width = GetSystemMetrics(SM_CXSCREEN);
        height = GetSystemMetrics(SM_CYSCREEN);
        style |= WS_POPUP;
    }
    else
    {
        style |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    }

    UINT screenWidth = GetSystemMetrics(SM_CXSCREEN);
    UINT screenHeight = GetSystemMetrics(SM_CYSCREEN);

    RECT windowRect = { };
    windowRect.left = (screenWidth / 2) - (width / 2);
    windowRect.top = (screenHeight / 2) - (width / 2);
    windowRect.right = (screenWidth / 2) + (width / 2);
    windowRect.bottom = (screenHeight / 2) + (width / 2);

    AdjustWindowRect(&windowRect, style, false);

    UINT windowWidth = windowRect.right - windowRect.left;
    UINT windowHeight = windowRect.bottom - windowRect.top;

    
    
    // Create the window.
    m_hWnd = CreateWindowEx(
        0,                              // Optional window styles.
        name,                           // Window class
        name,                           // Window text
        style,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,

        NULL,       // Parent window    
        NULL,       // Menu
        m_hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (m_hWnd == NULL)
    {
        return false;
    }

    // 윈도우를 표시한다.
    ShowWindow(m_hWnd, SW_SHOW);

    // D3D11을 초기화한다.
    if (g_d3d11.Initalize(m_hWnd, width, height, false, true, 60, true, 4) == false)
    {
        return false;
    }


	return true;
}

void Application::Shutdown()
{
    // D3D11을 종료한다.
    g_d3d11.Shutdown();

    // 생성한 윈도우를 파괴한다.
    DestroyWindow(m_hWnd);

    m_hWnd = NULL;
    UnregisterClass(m_className.get(), m_hInstance);
    m_hInstance = NULL;

}

void Application::Update()
{
    MSG msg = { };

    while (true)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                return;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        g_d3d11.ClearRenderTarget(0.0f, 0.0f, 0.0f, 1.0f);
        g_d3d11.PresentSwapChain(true);
    }
}