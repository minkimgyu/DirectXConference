#include <Windows.h>

#include "Application.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	Application app;

	if (app.Initialize(L"Game Engine", 600, 600, true))
	{
		app.Update();
	}

	app.Shutdown();

	return 0;
}