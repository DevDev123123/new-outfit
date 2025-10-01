#include "UIManager.h"
#include <windowsx.h>
#include <commctrl.h>
#include <sstream>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "gdi32.lib")

namespace OutfitConverter {
namespace UI {

    // ============== WINDOW MANAGER IMPLEMENTATION ==============
    WindowManager::WindowManager(HINSTANCE hInst) 
        : mainWindow(nullptr), hInstance(hInst), 
          fontRegular(nullptr), fontBold(nullptr), fontTitle(nullptr),
          backgroundBrush(nullptr), panelBrush(nullptr), buttonBrush(nullptr) {
        
        InitializeFonts();
        InitializeBrushes();
    }

    WindowManager::~WindowManager() {
        if (fontRegular) DeleteObject(fontRegular);
        if (fontBold) DeleteObject(fontBold);
        if (fontTitle) DeleteObject(fontTitle);
        if (backgroundBrush) DeleteObject(backgroundBrush);
        if (panelBrush) DeleteObject(panelBrush);
        if (buttonBrush) DeleteObject(buttonBrush);
    }

    void WindowManager::InitializeFonts() {
        // Regular font
        fontRegular = CreateFontW(
            16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
            L"Segoe UI"
        );

        // Bold font
        fontBold = CreateFontW(
            16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
            L"Segoe UI"
        );

        // Title font
        fontTitle = CreateFontW(
            24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
            L"Segoe UI"
        );
    }

    void WindowManager::InitializeBrushes() {
        backgroundBrush = CreateSolidBrush(ColorTheme::BACKGROUND_DARK);
        panelBrush = CreateSolidBrush(ColorTheme::BACKGROUND_PANEL);
        buttonBrush = CreateSolidBrush(ColorTheme::BUTTON_COLOR);
    }

    bool WindowManager::CreateMainWindow(const std::wstring& title, int width, int height) {
        // Register window class
        WNDCLASSEXW wc = {0};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = MainWindowProc;
        wc.hInstance = hInstance;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = backgroundBrush;
        wc.lpszClassName = L"OutfitConverterMainWindow";
        wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

        if (!RegisterClassExW(&wc)) {
            return false;
        }

        // Calculate window size for desired client area
        RECT rect = {0, 0, width, height};
        AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, TRUE, 0);

        // Create window centered on screen
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        int windowWidth = rect.right - rect.left;
        int windowHeight = rect.bottom - rect.top;
        int x = (screenWidth - windowWidth) / 2;
        int y = (screenHeight - windowHeight) / 2;

        mainWindow = CreateWindowExW(
            0,
            L"OutfitConverterMainWindow",
            title.c_str(),
            WS_OVERLAPPEDWINDOW,
            x, y, windowWidth, windowHeight,
            nullptr, nullptr, hInstance, this
        );

        if (!mainWindow) {
            return false;
        }

        CreateControls();
        ApplyTheme(mainWindow);

        return true;
    }

    void WindowManager::CreateControls() {
        // Create menu bar
        HMENU menuBar = CreateMenu();
        HMENU fileMenu = CreatePopupMenu();
        HMENU convertMenu = CreatePopupMenu();
        HMENU memoryMenu = CreatePopupMenu();
        HMENU helpMenu = CreatePopupMenu();

        // File menu
        AppendMenuW(fileMenu, MF_STRING, ID_FILE_OPEN, L"&Open Outfit...\tCtrl+O");
        AppendMenuW(fileMenu, MF_STRING, ID_FILE_SAVE, L"&Save Outfit...\tCtrl+S");
        AppendMenuW(fileMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(fileMenu, MF_STRING, ID_FILE_EXIT, L"E&xit\tAlt+F4");

        // Convert menu - Cherax submenu
        HMENU cheraxMenu = CreatePopupMenu();
        AppendMenuW(cheraxMenu, MF_STRING, ID_CONVERT_CHERAX_TO_YIM, L"To YimMenu");
        AppendMenuW(cheraxMenu, MF_STRING, ID_CONVERT_CHERAX_TO_LEXIS, L"To Lexis");
        AppendMenuW(cheraxMenu, MF_STRING, ID_CONVERT_CHERAX_TO_STAND, L"To Stand");

        // Convert menu - YimMenu submenu
        HMENU yimMenu = CreatePopupMenu();
        AppendMenuW(yimMenu, MF_STRING, ID_CONVERT_YIM_TO_CHERAX, L"To Cherax");
        AppendMenuW(yimMenu, MF_STRING, ID_CONVERT_YIM_TO_LEXIS, L"To Lexis");
        AppendMenuW(yimMenu, MF_STRING, ID_CONVERT_YIM_TO_STAND, L"To Stand");

        // Convert menu - Lexis submenu
        HMENU lexisMenu = CreatePopupMenu();
        AppendMenuW(lexisMenu, MF_STRING, ID_CONVERT_LEXIS_TO_CHERAX, L"To Cherax");
        AppendMenuW(lexisMenu, MF_STRING, ID_CONVERT_LEXIS_TO_YIM, L"To YimMenu");
        AppendMenuW(lexisMenu, MF_STRING, ID_CONVERT_LEXIS_TO_STAND, L"To Stand");

        // Convert menu - Stand submenu
        HMENU standMenu = CreatePopupMenu();
        AppendMenuW(standMenu, MF_STRING, ID_CONVERT_STAND_TO_CHERAX, L"To Cherax");
        AppendMenuW(standMenu, MF_STRING, ID_CONVERT_STAND_TO_YIM, L"To YimMenu");
        AppendMenuW(standMenu, MF_STRING, ID_CONVERT_STAND_TO_LEXIS, L"To Lexis");

        AppendMenuW(convertMenu, MF_POPUP, (UINT_PTR)cheraxMenu, L"From Cherax");
        AppendMenuW(convertMenu, MF_POPUP, (UINT_PTR)yimMenu, L"From YimMenu");
        AppendMenuW(convertMenu, MF_POPUP, (UINT_PTR)lexisMenu, L"From Lexis");
        AppendMenuW(convertMenu, MF_POPUP, (UINT_PTR)standMenu, L"From Stand");
        AppendMenuW(convertMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(convertMenu, MF_STRING, ID_EDITOR_OPEN, L"&Outfit Editor...");

        // Memory menu
        AppendMenuW(memoryMenu, MF_STRING, ID_MEMORY_ATTACH, L"&Attach to GTA V");
        AppendMenuW(memoryMenu, MF_STRING, ID_MEMORY_READ, L"&Read Current Outfit");
        AppendMenuW(memoryMenu, MF_STRING, ID_MEMORY_WRITE, L"&Write Outfit to Game");

        // Help menu
        AppendMenuW(helpMenu, MF_STRING, ID_HELP_ABOUT, L"&About");

        // Add to menu bar
        AppendMenuW(menuBar, MF_POPUP, (UINT_PTR)fileMenu, L"&File");
        AppendMenuW(menuBar, MF_POPUP, (UINT_PTR)convertMenu, L"&Convert");
        AppendMenuW(menuBar, MF_POPUP, (UINT_PTR)memoryMenu, L"&Memory");
        AppendMenuW(menuBar, MF_POPUP, (UINT_PTR)helpMenu, L"&Help");

        SetMenu(mainWindow, menuBar);

        // Create status bar
        HWND statusBar = CreateWindowExW(
            0, STATUSCLASSNAMEW, nullptr,
            WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
            0, 0, 0, 0,
            mainWindow, (HMENU)ID_STATUS_BAR,
            hInstance, nullptr
        );

        SendMessageW(statusBar, WM_SETFONT, (WPARAM)fontRegular, TRUE);
        SetStatusText(L"Ready - Credit to @sizrox");
    }

    void WindowManager::ApplyTheme(HWND hwnd) {
        // Apply dark theme colors to all child windows
        EnumChildWindows(hwnd, [](HWND child, LPARAM lParam) -> BOOL {
            WCHAR className[256];
            GetClassNameW(child, className, 256);

            // Set font and colors
            SendMessageW(child, WM_SETFONT, (WPARAM)((WindowManager*)lParam)->fontRegular, TRUE);
            
            return TRUE;
        }, (LPARAM)this);
    }

    void WindowManager::ShowWindow() {
        ::ShowWindow(mainWindow, SW_SHOW);
        UpdateWindow(mainWindow);
    }

    void WindowManager::HideWindow() {
        ::ShowWindow(mainWindow, SW_HIDE);
    }

    bool WindowManager::IsVisible() const {
        return IsWindowVisible(mainWindow);
    }

    void WindowManager::SetStatusText(const std::wstring& text) {
        HWND statusBar = GetDlgItem(mainWindow, ID_STATUS_BAR);
        if (statusBar) {
            SendMessageW(statusBar, SB_SETTEXTW, 0, (LPARAM)text.c_str());
        }
    }

    // ============== WINDOW PROCEDURE ==============
    LRESULT CALLBACK WindowManager::MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        WindowManager* manager = nullptr;

        if (msg == WM_CREATE) {
            CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
            manager = reinterpret_cast<WindowManager*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)manager);
        } else {
            manager = reinterpret_cast<WindowManager*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        }

        switch (msg) {
            case WM_CREATE:
                return 0;

            case WM_CTLCOLORSTATIC:
            case WM_CTLCOLOREDIT:
            case WM_CTLCOLORLISTBOX: {
                HDC hdc = (HDC)wParam;
                SetTextColor(hdc, ColorTheme::TEXT_PRIMARY);
                SetBkColor(hdc, ColorTheme::BACKGROUND_PANEL);
                return (LRESULT)CreateSolidBrush(ColorTheme::BACKGROUND_PANEL);
            }

            case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                
                // Fill background
                RECT rect;
                GetClientRect(hwnd, &rect);
                FillRect(hdc, &rect, CreateSolidBrush(ColorTheme::BACKGROUND_DARK));
                
                EndPaint(hwnd, &ps);
                return 0;
            }

            case WM_SIZE: {
                // Resize status bar
                HWND statusBar = GetDlgItem(hwnd, ID_STATUS_BAR);
                if (statusBar) {
                    SendMessageW(statusBar, WM_SIZE, 0, 0);
                }
                return 0;
            }

            case WM_COMMAND: {
                int wmId = LOWORD(wParam);
                switch (wmId) {
                    case ID_FILE_EXIT:
                        PostQuitMessage(0);
                        return 0;
                    
                    case ID_HELP_ABOUT:
                        AboutDialog::Show(hwnd);
                        return 0;
                }
                break;
            }

            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
        }

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    // ============== DIALOG HELPER IMPLEMENTATION ==============
    std::wstring DialogHelper::OpenFileDialog(HWND parent, const wchar_t* filter, const wchar_t* title) {
        WCHAR filename[MAX_PATH] = {0};
        
        OPENFILENAMEW ofn = {0};
        ofn.lStructSize = sizeof(OPENFILENAMEW);
        ofn.hwndOwner = parent;
        ofn.lpstrFilter = filter;
        ofn.lpstrFile = filename;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrTitle = title;
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

        if (GetOpenFileNameW(&ofn)) {
            return std::wstring(filename);
        }

        return L"";
    }

    std::wstring DialogHelper::SaveFileDialog(HWND parent, const wchar_t* filter, const wchar_t* title) {
        WCHAR filename[MAX_PATH] = {0};
        
        OPENFILENAMEW ofn = {0};
        ofn.lStructSize = sizeof(OPENFILENAMEW);
        ofn.hwndOwner = parent;
        ofn.lpstrFilter = filter;
        ofn.lpstrFile = filename;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrTitle = title;
        ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

        if (GetSaveFileNameW(&ofn)) {
            return std::wstring(filename);
        }

        return L"";
    }

    void DialogHelper::ShowMessage(HWND parent, const std::wstring& message, 
                                   const std::wstring& title, UINT type) {
        MessageBoxW(parent, message.c_str(), title.c_str(), type);
    }

    bool DialogHelper::ConfirmDialog(HWND parent, const std::wstring& message, 
                                     const std::wstring& title) {
        int result = MessageBoxW(parent, message.c_str(), title.c_str(), 
                                MB_YESNO | MB_ICONQUESTION);
        return (result == IDYES);
    }

    // ============== ABOUT DIALOG ==============
    void AboutDialog::Show(HWND parent) {
        MessageBoxW(parent, 
            L"GTA V Outfit Converter & Editor\n\n"
            L"Version 1.0\n\n"
            L"Supports: Cherax, YimMenu, Lexis, Stand\n\n"
            L"Features:\n"
            L"- Format conversion between all menu types\n"
            L"- Real-time outfit editing\n"
            L"- Memory injection support\n\n"
            L"Credit to @sizrox on Discord\n\n"
            L"© 2025 All Rights Reserved",
            L"About Outfit Converter",
            MB_OK | MB_ICONINFORMATION);
    }

} // namespace UI
} // namespace OutfitConverter
// ============== CUSTOM BUTTON IMPLEMENTATION ==============
namespace OutfitConverter {
namespace UI {

    CustomButton::CustomButton() : hwnd(nullptr), parent(nullptr), controlID(0), 
                                   isHovered(false), isPressed(false) {}

    bool CustomButton::Create(HWND parentHwnd, const std::wstring& buttonText, 
                             int x, int y, int width, int height, int id) {
        parent = parentHwnd;
        text = buttonText;
        controlID = id;

        // Register custom button class
        WNDCLASSEXW wc = {0};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = ButtonProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.hCursor = LoadCursor(nullptr, IDC_HAND);
        wc.lpszClassName = L"CustomPurpleButton";

        static bool registered = false;
        if (!registered) {
            RegisterClassExW(&wc);
            registered = true;
        }

        hwnd = CreateWindowExW(
            0,
            L"CustomPurpleButton",
            buttonText.c_str(),
            WS_CHILD | WS_VISIBLE | WS_TABSTOP,
            x, y, width, height,
            parentHwnd,
            (HMENU)(INT_PTR)id,
            GetModuleHandleW(nullptr),
            this
        );

        return (hwnd != nullptr);
    }

    void CustomButton::SetCallback(std::function<void()> callback) {
        onClick = callback;
    }

    void CustomButton::SetEnabled(bool enabled) {
        EnableWindow(hwnd, enabled ? TRUE : FALSE);
    }

    LRESULT CALLBACK CustomButton::ButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        CustomButton* button = nullptr;

        if (msg == WM_CREATE) {
            CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
            button = reinterpret_cast<CustomButton*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)button);
            button->hwnd = hwnd;
        } else {
            button = reinterpret_cast<CustomButton*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        }

        if (!button) return DefWindowProcW(hwnd, msg, wParam, lParam);

        switch (msg) {
            case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                button->Paint(hdc);
                EndPaint(hwnd, &ps);
                return 0;
            }

            case WM_LBUTTONDOWN: {
                button->isPressed = true;
                SetCapture(hwnd);
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }

            case WM_LBUTTONUP: {
                if (button->isPressed) {
                    button->isPressed = false;
                    ReleaseCapture();
                    
                    RECT rect;
                    GetClientRect(hwnd, &rect);
                    POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                    
                    if (PtInRect(&rect, pt) && button->onClick) {
                        button->onClick();
                    }
                    
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
                return 0;
            }

            case WM_MOUSEMOVE: {
                if (!button->isHovered) {
                    button->isHovered = true;
                    InvalidateRect(hwnd, nullptr, FALSE);
                    
                    TRACKMOUSEEVENT tme = {0};
                    tme.cbSize = sizeof(TRACKMOUSEEVENT);
                    tme.dwFlags = TME_LEAVE;
                    tme.hwndTrack = hwnd;
                    TrackMouseEvent(&tme);
                }
                return 0;
            }

            case WM_MOUSELEAVE: {
                button->isHovered = false;
                button->isPressed = false;
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }

            case WM_SETFOCUS:
            case WM_KILLFOCUS:
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
        }

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    void CustomButton::Paint(HDC hdc) {
        RECT rect;
        GetClientRect(hwnd, &rect);

        // Determine button color
        COLORREF bgColor;
        if (!IsWindowEnabled(hwnd)) {
            bgColor = ColorTheme::BORDER_COLOR;
        } else if (isPressed) {
            bgColor = ColorTheme::ACCENT_DARK;
        } else if (isHovered) {
            bgColor = ColorTheme::BUTTON_HOVER;
        } else {
            bgColor = ColorTheme::BUTTON_COLOR;
        }

        // Draw button background with rounded corners
        HBRUSH brush = CreateSolidBrush(bgColor);
        HPEN pen = CreatePen(PS_SOLID, 2, ColorTheme::ACCENT_LIGHT);
        
        SelectObject(hdc, brush);
        SelectObject(hdc, pen);

        RoundRect(hdc, rect.left + 1, rect.top + 1, 
                 rect.right - 1, rect.bottom - 1, 8, 8);

        DeleteObject(brush);
        DeleteObject(pen);

        // Draw text
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, ColorTheme::TEXT_PRIMARY);
        
        HFONT font = CreateFontW(
            16, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
            L"Segoe UI"
        );
        
        SelectObject(hdc, font);
        DrawTextW(hdc, text.c_str(), -1, &rect, 
                 DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        
        DeleteObject(font);

        // Draw focus rectangle
        if (GetFocus() == hwnd) {
            RECT focusRect = rect;
            InflateRect(&focusRect, -4, -4);
            DrawFocusRect(hdc, &focusRect);
        }
    }

    // ============== CUSTOM PANEL IMPLEMENTATION ==============
    CustomPanel::CustomPanel() : hwnd(nullptr), parent(nullptr) {}

    bool CustomPanel::Create(HWND parentHwnd, const std::wstring& panelTitle, 
                            int x, int y, int width, int height) {
        parent = parentHwnd;
        title = panelTitle;

        // Register custom panel class
        WNDCLASSEXW wc = {0};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = PanelProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.lpszClassName = L"CustomPurplePanel";

        static bool registered = false;
        if (!registered) {
            RegisterClassExW(&wc);
            registered = true;
        }

        hwnd = CreateWindowExW(
            0,
            L"CustomPurplePanel",
            panelTitle.c_str(),
            WS_CHILD | WS_VISIBLE,
            x, y, width, height,
            parentHwnd,
            nullptr,
            GetModuleHandleW(nullptr),
            this
        );

        return (hwnd != nullptr);
    }

    LRESULT CALLBACK CustomPanel::PanelProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        CustomPanel* panel = nullptr;

        if (msg == WM_CREATE) {
            CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
            panel = reinterpret_cast<CustomPanel*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)panel);
            panel->hwnd = hwnd;
        } else {
            panel = reinterpret_cast<CustomPanel*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        }

        if (!panel) return DefWindowProcW(hwnd, msg, wParam, lParam);

        switch (msg) {
            case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                panel->Paint(hdc);
                EndPaint(hwnd, &ps);
                return 0;
            }

            case WM_ERASEBKGND:
                return 1;
        }

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    void CustomPanel::Paint(HDC hdc) {
        RECT rect;
        GetClientRect(hwnd, &rect);

        // Draw panel background
        HBRUSH bgBrush = CreateSolidBrush(ColorTheme::BACKGROUND_PANEL);
        FillRect(hdc, &rect, bgBrush);
        DeleteObject(bgBrush);

        // Draw border with accent color
        HPEN borderPen = CreatePen(PS_SOLID, 2, ColorTheme::ACCENT_PURPLE);
        SelectObject(hdc, borderPen);
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
        
        RoundRect(hdc, rect.left + 1, rect.top + 1, 
                 rect.right - 1, rect.bottom - 1, 12, 12);
        
        DeleteObject(borderPen);

        // Draw title bar if title exists
        if (!title.empty()) {
            RECT titleRect = {rect.left + 10, rect.top + 10, 
                            rect.right - 10, rect.top + 40};
            
            // Draw title background
            HBRUSH titleBrush = CreateSolidBrush(ColorTheme::ACCENT_DARK);
            HRGN titleRgn = CreateRoundRectRgn(titleRect.left, titleRect.top, 
                                              titleRect.right, titleRect.bottom, 8, 8);
            FillRgn(hdc, titleRgn, titleBrush);
            DeleteObject(titleBrush);
            DeleteObject(titleRgn);

            // Draw title text
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, ColorTheme::TEXT_PRIMARY);
            
            HFONT font = CreateFontW(
                18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                L"Segoe UI"
            );
            
            SelectObject(hdc, font);
            DrawTextW(hdc, title.c_str(), -1, &titleRect, 
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            
            DeleteObject(font);
        }

        // Draw subtle gradient effect at bottom
        RECT gradientRect = {rect.left, rect.bottom - 30, rect.right, rect.bottom};
        TRIVERTEX vertices[2];
        vertices[0].x = gradientRect.left;
        vertices[0].y = gradientRect.top;
        vertices[0].Red = GetRValue(ColorTheme::BACKGROUND_PANEL) << 8;
        vertices[0].Green = GetGValue(ColorTheme::BACKGROUND_PANEL) << 8;
        vertices[0].Blue = GetBValue(ColorTheme::BACKGROUND_PANEL) << 8;
        vertices[0].Alpha = 0xFF00;

        vertices[1].x = gradientRect.right;
        vertices[1].y = gradientRect.bottom;
        vertices[1].Red = GetRValue(ColorTheme::BACKGROUND_DARK) << 8;
        vertices[1].Green = GetGValue(ColorTheme::BACKGROUND_DARK) << 8;
        vertices[1].Blue = GetBValue(ColorTheme::BACKGROUND_DARK) << 8;
        vertices[1].Alpha = 0xFF00;

        GRADIENT_RECT gRect = {0, 1};
        GradientFill(hdc, vertices, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
    }

} // namespace UI
} // namespace OutfitConverter
