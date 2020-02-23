#include "Window.h"

INT CALLBACK WinMain
(
   HINSTANCE hInstance, 
   HINSTANCE hPrevInstance,
   LPSTR lpCmdLine, 
   INT nCmdShow )
{
    try {
        Window wnd(800, 300, "D3D Window");

        MSG msg;
        BOOL gResult;

        while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (gResult == -1)
        {
            return -1;
        }

        return msg.wParam;
    }
    catch (const WndExceptionHandler & e)
    {
        MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
    }
    catch (const std::exception & e)
    {
        MessageBox(nullptr, e.what(), "Standart Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    catch (...)
    {
        MessageBox(nullptr,"No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    return -1;
}