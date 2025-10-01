#pragma once
#include "OutfitStructures.h"
#include "MemoryEditor.h"
#include "FormatConverter.h"
#include "FileHandler.h"
#include "UIManager.h"
#include <memory>
#include <string>

namespace OutfitConverter {

    // ============== APPLICATION MANAGER ==============
    class Application {
    private:
        std::unique_ptr<UI::WindowManager> windowManager;
        std::unique_ptr<MemoryEditor> memoryEditor;
        HINSTANCE hInstance;
        
        // Current loaded outfit (using YimOutfit as intermediate format)
        YimOutfit currentOutfit;
        std::string currentFilePath;
        FormatConverter::FormatType currentFormat;
        bool outfitLoaded;
        bool memoryAttached;

        // UI State
        HWND mainWindow;
        HWND sourcePathEdit;
        HWND targetPathEdit;
        HWND sourceFormatCombo;
        HWND targetFormatCombo;
        HWND componentList;
        HWND statusBar;

        // Private methods
        void InitializeUI();
        void SetupEventHandlers();
        void UpdateUIState();
        void LoadOutfitFile(const std::wstring& filepath);
        void SaveOutfitFile(const std::wstring& filepath, FormatConverter::FormatType format);
        void ConvertAndSave(FormatConverter::FormatType targetFormat);
        void AttachToMemory();
        void ReadFromMemory();
        void WriteToMemory();
        void PopulateComponentList();
        void ShowError(const std::wstring& message);
        void ShowSuccess(const std::wstring& message);
        
        // Conversion helpers
        std::string WStringToString(const std::wstring& wstr);
        std::wstring StringToWString(const std::string& str);

    public:
        Application(HINSTANCE hInst);
        ~Application();

        bool Initialize();
        int Run();
        void Shutdown();

        // Public interface for UI callbacks
        void OnFileOpen();
        void OnFileSave();
        void OnConvert(FormatConverter::FormatType sourceFormat, 
                      FormatConverter::FormatType targetFormat);
        void OnMemoryAttach();
        void OnMemoryRead();
        void OnMemoryWrite();
        void OnOpenEditor();
        void OnAbout();

        // Getters
        HWND GetMainWindow() const { return mainWindow; }
        bool IsOutfitLoaded() const { return outfitLoaded; }
        bool IsMemoryAttached() const { return memoryAttached; }
    };

    // ============== EDITOR WINDOW ==============
    class EditorWindow {
    private:
        HWND editorWindow;
        HWND componentList;
        HWND drawableEdit;
        HWND textureEdit;
        HWND paletteEdit;
        HINSTANCE hInstance;
        
        YimOutfit* workingOutfit;
        int selectedComponent;
        
        static LRESULT CALLBACK EditorProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        void CreateControls();
        void PopulateComponentList();
        void UpdateComponentControls();
        void ApplyChanges();
        void ResetComponent();

    public:
        EditorWindow(HINSTANCE hInst);
        ~EditorWindow();

        bool Create(HWND parent, YimOutfit* outfit);
        void Show();
        void Hide();
        bool IsVisible() const;
        HWND GetHandle() const { return editorWindow; }
    };

    // ============== MEMORY VIEWER WINDOW ==============
    class MemoryViewerWindow {
    private:
        HWND viewerWindow;
        HWND memoryList;
        HWND statusEdit;
        HINSTANCE hInstance;
        
        MemoryEditor* memoryEditor;
        
        static LRESULT CALLBACK ViewerProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        void CreateControls();
        void RefreshMemoryView();
        void UpdateStatus(const std::wstring& status);

    public:
        MemoryViewerWindow(HINSTANCE hInst);
        ~MemoryViewerWindow();

        bool Create(HWND parent, MemoryEditor* editor);
        void Show();
        void Hide();
        bool IsVisible() const;
        HWND GetHandle() const { return viewerWindow; }
    };

    // ============== CONVERSION DIALOG ==============
    class ConversionDialog {
    private:
        static FormatConverter::FormatType sourceFormat;
        static FormatConverter::FormatType targetFormat;
        static std::wstring sourcePath;
        static std::wstring targetPath;
        
        static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        static void OnBrowseSource(HWND hwnd);
        static void OnBrowseTarget(HWND hwnd);
        static void OnConvert(HWND hwnd);
        static const wchar_t* GetFormatFilter(FormatConverter::FormatType format);

    public:
        static bool Show(HWND parent);
        static bool GetResult(std::wstring& outSourcePath, std::wstring& outTargetPath,
                             FormatConverter::FormatType& outSourceFormat,
                             FormatConverter::FormatType& outTargetFormat);
    };

    // ============== UTILITY FUNCTIONS ==============
    namespace Utils {
        std::string GetComponentName(int slot);
        std::string GetPropName(int slot);
        std::wstring FormatToString(FormatConverter::FormatType format);
        FormatConverter::FormatType StringToFormat(const std::wstring& str);
        bool ValidateOutfitData(const YimOutfit& outfit);
        std::wstring GetLastErrorString();
    }

} // namespace OutfitConverter
