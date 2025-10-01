#include "Application.h"
#include <windows.h>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// ============== ENTRY POINT ==============
int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES | ICC_STANDARD_CLASSES | ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    // Enable visual styles
    SetProcessDPIAware();

    // Create and initialize application
    OutfitConverter::Application app(hInstance);

    if (!app.Initialize()) {
        MessageBoxW(nullptr, 
            L"Failed to initialize application!", 
            L"Error", 
            MB_OK | MB_ICONERROR);
        return -1;
    }

    // Run application message loop
    int exitCode = app.Run();

    // Cleanup
    app.Shutdown();

    return exitCode;
}
