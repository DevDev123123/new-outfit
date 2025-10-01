#pragma once
#include "OutfitStructures.h"
#include "MemoryEditor.h"
#include "FormatConverter.h"
#include "FileHandler.h"
#include <memory>
#include <string>
#include <windows.h>

namespace OutfitConverter {

    // Forward declare UI manager
    namespace UI {
        class WindowManager;
    }

    // ============== CONTROL IDS ==============
    #define ID_FILE_OPEN 1001
    #define ID_FILE_SAVE 1002
    #define ID_FILE_EXIT 1003
    #define ID_CONVERT_CHERAX_TO_YIM 1010
    #define ID_CONVERT_CHERAX_TO_LEXIS 1011
    #define ID_CONVERT_CHERAX_TO_STAND 1012
    #define ID_CONVERT_YIM_TO_CHERAX 1013
    #define ID_CONVERT_YIM_TO_LEXIS 1014
    #define ID_CONVERT_YIM_TO_STAND 1015
    #define ID_CONVERT_LEXIS_TO_CHERAX 1016
    #define ID_CONVERT_LEXIS_TO_YIM 1017
    #define ID_CONVERT_LEXIS_TO_STAND 1018
    #define ID_CONVERT_STAND_TO_CHERAX 1019
    #define ID_CONVERT_STAND_TO_YIM 1020
    #define ID_CONVERT_STAND_TO_LEXIS 1021
    #define ID_EDITOR_OPEN 1030
    #define ID_MEMORY_ATTACH 1040
    #define ID_MEMORY_READ 1041
    #define ID_MEMORY_WRITE 1042
    #define ID_HELP_ABOUT 1050
    #define ID_BTN_LOAD_FILE 2001
    #define ID_BTN_SAVE_FILE 2002
    #define ID_BTN_CONVERT 2003
    #define ID_BTN_CLEAR 2004
    #define ID_COMBO_SOURCE_FORMAT 2010
    #define ID_COMBO_TARGET_FORMAT 2011
    #define ID_EDIT_SOURCE_PATH 2020
    #define ID_EDIT_TARGET_PATH 2021
    #define ID_BTN_BROWSE_SOURCE 2022
    #define ID_BTN_BROWSE_TARGET 2023
    #define ID_LIST_COMPONENTS 2030
    #define ID_STATUS_BAR 2040
    #define ID_EDITOR_LIST 3001
    #define ID_EDITOR_DRAWABLE 3010
    #define ID_EDITOR_TEXTURE 3011
    #define ID_EDITOR_PALETTE 3012
    #define ID_BTN_EDITOR_APPLY 3020
    #define ID_BTN_EDITOR_RESET 3021
    #define ID_BTN_EDITOR_SAVE 3022
    #define ID_MEMORY_LIST 4001
    #define ID_BTN_ATTACH_GAME 4010
    #define ID_BTN_READ_OUTFIT 4011
    #define ID_BTN_WRITE_OUTFIT 4012
    #define ID_BTN_BACKUP_OUTFIT 4013
    #define ID_EDIT_MEMORY_STATUS 4020

    // ============== APPLICATION MANAGER ==============
    class Application {
    private:
        std::unique_ptr<UI::WindowManager> windowManager;
        std::unique_ptr<MemoryEditor> memoryEditor;
        HINSTANCE hInstance;
        
        YimOutfit currentOutfit;
        std::string currentFilePath;
        FormatConverter::FormatType currentFormat;
        bool outfitLoaded;
        bool memoryAttached;

        HWND mainWindow;
        HWND sourcePathEdit;
        HWND targetPathEdit;
        HWND sourceFormatCombo;
        HWND targetFormatCombo;
        HWND componentList;
        HWND statusBar;

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
        
        std::string WStringToString(const std::wstring& wstr);
        std::wstring StringToWString(const std::string& str);

    public:
        Application(HINSTANCE hInst);
        ~Application();

        bool Initialize();
        int Run();
        void Shutdown();

        void OnFileOpen();
        void OnFileSave();
        void OnConvert(FormatConverter::FormatType sourceFormat, 
                      FormatConverter::FormatType targetFormat);
        void OnMemoryAttach();
        void OnMemoryRead();
        void OnMemoryWrite();
        void OnOpenEditor();
        void OnAbout();

        HWND GetMainWindow() const { return mainWindow; }
        bool IsOutfitLoaded() const { return outfitLoaded; }
        bool IsMemoryAttached() const { return memoryAttached; }
    };

} // namespace OutfitConverter
