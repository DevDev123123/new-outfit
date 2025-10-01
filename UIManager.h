#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <functional>
#include "OutfitStructures.h"

namespace OutfitConverter {
namespace UI {

    // ============== COLOR SCHEME (PURPLE DARK THEME) ==============
    struct ColorTheme {
        // Primary colors
        static constexpr COLORREF BACKGROUND_DARK = RGB(20, 15, 30);      // Deep dark purple
        static constexpr COLORREF BACKGROUND_PANEL = RGB(35, 25, 50);     // Panel background
        static constexpr COLORREF ACCENT_PURPLE = RGB(147, 112, 219);     // Medium purple
        static constexpr COLORREF ACCENT_LIGHT = RGB(186, 157, 255);      // Light purple
        static constexpr COLORREF ACCENT_DARK = RGB(98, 71, 170);         // Dark purple
        
        // Text colors
        static constexpr COLORREF TEXT_PRIMARY = RGB(240, 240, 245);      // White/light
        static constexpr COLORREF TEXT_SECONDARY = RGB(180, 180, 200);    // Gray text
        static constexpr COLORREF TEXT_DISABLED = RGB(100, 100, 120);     // Disabled text
        
        // UI elements
        static constexpr COLORREF BORDER_COLOR = RGB(100, 80, 140);       // Border purple
        static constexpr COLORREF HOVER_COLOR = RGB(60, 45, 90);          // Hover state
        static constexpr COLORREF BUTTON_COLOR = RGB(120, 90, 180);       // Button color
        static constexpr COLORREF BUTTON_HOVER = RGB(147, 112, 219);      // Button hover
        static constexpr COLORREF INPUT_BG = RGB(40, 30, 60);             // Input background
        
        // Status colors
        static constexpr COLORREF SUCCESS_COLOR = RGB(100, 200, 100);     // Success green
        static constexpr COLORREF ERROR_COLOR = RGB(220, 80, 100);        // Error red
        static constexpr COLORREF WARNING_COLOR = RGB(255, 180, 100);     // Warning orange
    };

//     // ============== CUSTOM CONTROL IDS ==============
// enum ControlID {
//     // Menu bar
//     ID_FILE_OPEN = 1001,
//     ID_FILE_SAVE = 1002,
//     ID_FILE_EXIT = 1003,
//     ID_CONVERT_CHERAX_TO_YIM = 1010,
//     ID_CONVERT_CHERAX_TO_LEXIS = 1011,
//     ID_CONVERT_CHERAX_TO_STAND = 1012,
//     ID_CONVERT_YIM_TO_CHERAX = 1013,
//     ID_CONVERT_YIM_TO_LEXIS = 1014,
//     ID_CONVERT_YIM_TO_STAND = 1015,
//     ID_CONVERT_LEXIS_TO_CHERAX = 1016,
//     ID_CONVERT_LEXIS_TO_YIM = 1017,
//     ID_CONVERT_LEXIS_TO_STAND = 1018,
//     ID_CONVERT_STAND_TO_CHERAX = 1019,
//     ID_CONVERT_STAND_TO_YIM = 1020,
//     ID_CONVERT_STAND_TO_LEXIS = 1021,
//     ID_EDITOR_OPEN = 1030,
//     ID_MEMORY_ATTACH = 1040,
//     ID_MEMORY_READ = 1041,
//     ID_MEMORY_WRITE = 1042,
//     ID_HELP_ABOUT = 1050,

//     // Main window controls
//     ID_BTN_LOAD_FILE = 2001,
//     ID_BTN_SAVE_FILE = 2002,
//     ID_BTN_CONVERT = 2003,
//     ID_BTN_CLEAR = 2004,
//     ID_COMBO_SOURCE_FORMAT = 2010,
//     ID_COMBO_TARGET_FORMAT = 2011,
//     ID_EDIT_SOURCE_PATH = 2020,
//     ID_EDIT_TARGET_PATH = 2021,
//     ID_BTN_BROWSE_SOURCE = 2022,
//     ID_BTN_BROWSE_TARGET = 2023,
//     ID_LIST_COMPONENTS = 2030,
//     ID_STATUS_BAR = 2040,

//     // Editor controls
//     ID_EDITOR_LIST = 3001,
//     ID_EDITOR_DRAWABLE = 3010,
//     ID_EDITOR_TEXTURE = 3011,
//     ID_EDITOR_PALETTE = 3012,
//     ID_BTN_EDITOR_APPLY = 3020,
//     ID_BTN_EDITOR_RESET = 3021,
//     ID_BTN_EDITOR_SAVE = 3022,

//     // Memory editor controls
//     ID_MEMORY_LIST = 4001,
//     ID_BTN_ATTACH_GAME = 4010,
//     ID_BTN_READ_OUTFIT = 4011,
//     ID_BTN_WRITE_OUTFIT = 4012,
//     ID_BTN_BACKUP_OUTFIT = 4013,
//     ID_EDIT_MEMORY_STATUS = 4020
// };


    // ============== WINDOW MANAGER ==============
    class WindowManager {
    private:
        HWND mainWindow;
        HINSTANCE hInstance;
        HFONT fontRegular;
        HFONT fontBold;
        HFONT fontTitle;
        HBRUSH backgroundBrush;
        HBRUSH panelBrush;
        HBRUSH buttonBrush;

        static LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK EditorWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        void InitializeFonts();
        void InitializeBrushes();
        void CreateControls();
        void ApplyTheme(HWND hwnd);

    public:
        WindowManager(HINSTANCE hInst);
        ~WindowManager();

        bool CreateMainWindow(const std::wstring& title, int width, int height);
        void ShowWindow();
        void HideWindow();
        bool IsVisible() const;

        HWND GetHandle() const { return mainWindow; }
        void SetStatusText(const std::wstring& text);
    };

    // ============== CUSTOM BUTTON CLASS ==============
    class CustomButton {
    private:
        HWND hwnd;
        HWND parent;
        int controlID;
        std::wstring text;
        bool isHovered;
        bool isPressed;
        std::function<void()> onClick;

        static LRESULT CALLBACK ButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        void Paint(HDC hdc);

    public:
        CustomButton();
        bool Create(HWND parentHwnd, const std::wstring& buttonText, int x, int y, int width, int height, int id);
        void SetCallback(std::function<void()> callback);
        void SetEnabled(bool enabled);
        HWND GetHandle() const { return hwnd; }
    };

    // ============== CUSTOM PANEL CLASS ==============
    class CustomPanel {
    private:
        HWND hwnd;
        HWND parent;
        std::wstring title;
        
        static LRESULT CALLBACK PanelProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        void Paint(HDC hdc);

    public:
        CustomPanel();
        bool Create(HWND parentHwnd, const std::wstring& panelTitle, int x, int y, int width, int height);
        HWND GetHandle() const { return hwnd; }
    };

    // ============== DIALOG HELPERS ==============
    class DialogHelper {
    public:
        static std::wstring OpenFileDialog(HWND parent, const wchar_t* filter, const wchar_t* title);
        static std::wstring SaveFileDialog(HWND parent, const wchar_t* filter, const wchar_t* title);
        static void ShowMessage(HWND parent, const std::wstring& message, const std::wstring& title, UINT type);
        static bool ConfirmDialog(HWND parent, const std::wstring& message, const std::wstring& title);
    };

    // ============== ABOUT DIALOG ==============
    class AboutDialog {
    public:
        static void Show(HWND parent);
    private:
        static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    };

} // namespace UI
} // namespace OutfitConverter
