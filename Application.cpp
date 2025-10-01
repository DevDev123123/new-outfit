#include "Application.h"
#include <sstream>
#include <codecvt>
#include <locale>

namespace OutfitConverter {

    // ============== APPLICATION IMPLEMENTATION ==============
    Application::Application(HINSTANCE hInst) 
        : hInstance(hInst), outfitLoaded(false), memoryAttached(false),
          currentFormat(FormatConverter::FormatType::UNKNOWN),
          mainWindow(nullptr), sourcePathEdit(nullptr), targetPathEdit(nullptr),
          sourceFormatCombo(nullptr), targetFormatCombo(nullptr),
          componentList(nullptr), statusBar(nullptr) {
        
        windowManager = std::make_unique<UI::WindowManager>(hInst);
        memoryEditor = std::make_unique<MemoryEditor>();
    }

    Application::~Application() {
        Shutdown();
    }

    bool Application::Initialize() {
        if (!windowManager->CreateMainWindow(
            L"GTA V Outfit Converter - Credit to @sizrox", 1200, 800)) {
            return false;
        }

        mainWindow = windowManager->GetHandle();
        InitializeUI();
        SetupEventHandlers();
        
        return true;
    }

    void Application::InitializeUI() {
        // Create main panels
        RECT clientRect;
        GetClientRect(mainWindow, &clientRect);

        int panelWidth = (clientRect.right - 60) / 2;
        int panelHeight = clientRect.bottom - 150;

        // Source panel
        UI::CustomPanel sourcePanel;
        sourcePanel.Create(mainWindow, L"Source Outfit", 20, 20, panelWidth, panelHeight);

        // Target panel
        UI::CustomPanel targetPanel;
        targetPanel.Create(mainWindow, L"Target Format", 40 + panelWidth, 20, 
                          panelWidth, panelHeight);

        // Create source controls
        CreateWindowExW(0, L"STATIC", L"File Path:",
            WS_CHILD | WS_VISIBLE,
            40, 80, 100, 25,
            mainWindow, nullptr, hInstance, nullptr);

        sourcePathEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_READONLY,
            150, 80, panelWidth - 240, 25,
            mainWindow, (HMENU)ID_EDIT_SOURCE_PATH, hInstance, nullptr);

        UI::CustomButton browseSrcBtn;
        browseSrcBtn.Create(mainWindow, L"Browse...", 
            panelWidth - 70, 80, 80, 30, ID_BTN_BROWSE_SOURCE);
        browseSrcBtn.SetCallback([this]() { OnFileOpen(); });

        CreateWindowExW(0, L"STATIC", L"Format:",
            WS_CHILD | WS_VISIBLE,
            40, 120, 100, 25,
            mainWindow, nullptr, hInstance, nullptr);

        sourceFormatCombo = CreateWindowExW(0, L"COMBOBOX", L"",
            WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
            150, 120, 200, 200,
            mainWindow, (HMENU)ID_COMBO_SOURCE_FORMAT, hInstance, nullptr);

        // Add format options
        SendMessageW(sourceFormatCombo, CB_ADDSTRING, 0, (LPARAM)L"Cherax");
        SendMessageW(sourceFormatCombo, CB_ADDSTRING, 0, (LPARAM)L"YimMenu");
        SendMessageW(sourceFormatCombo, CB_ADDSTRING, 0, (LPARAM)L"Lexis");
        SendMessageW(sourceFormatCombo, CB_ADDSTRING, 0, (LPARAM)L"Stand");

        // Component list
        componentList = CreateWindowExW(WS_EX_CLIENTEDGE, L"LISTBOX", L"",
            WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL,
            40, 160, panelWidth - 60, panelHeight - 180,
            mainWindow, (HMENU)ID_LIST_COMPONENTS, hInstance, nullptr);

        // Create target controls
        CreateWindowExW(0, L"STATIC", L"Save Path:",
            WS_CHILD | WS_VISIBLE,
            60 + panelWidth, 80, 100, 25,
            mainWindow, nullptr, hInstance, nullptr);

        targetPathEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            170 + panelWidth, 80, panelWidth - 240, 25,
            mainWindow, (HMENU)ID_EDIT_TARGET_PATH, hInstance, nullptr);

        UI::CustomButton browseTgtBtn;
        browseTgtBtn.Create(mainWindow, L"Browse...", 
            clientRect.right - 90, 80, 80, 30, ID_BTN_BROWSE_TARGET);
        browseTgtBtn.SetCallback([this]() {
            std::wstring path = UI::DialogHelper::SaveFileDialog(
                mainWindow, L"All Files\0*.*\0\0", L"Save Outfit As");
            if (!path.empty()) {
                SetWindowTextW(targetPathEdit, path.c_str());
            }
        });

        CreateWindowExW(0, L"STATIC", L"Target Format:",
            WS_CHILD | WS_VISIBLE,
            60 + panelWidth, 120, 100, 25,
            mainWindow, nullptr, hInstance, nullptr);

        targetFormatCombo = CreateWindowExW(0, L"COMBOBOX", L"",
            WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
            170 + panelWidth, 120, 200, 200,
            mainWindow, (HMENU)ID_COMBO_TARGET_FORMAT, hInstance, nullptr);

        SendMessageW(targetFormatCombo, CB_ADDSTRING, 0, (LPARAM)L"Cherax");
        SendMessageW(targetFormatCombo, CB_ADDSTRING, 0, (LPARAM)L"YimMenu");
        SendMessageW(targetFormatCombo, CB_ADDSTRING, 0, (LPARAM)L"Lexis");
        SendMessageW(targetFormatCombo, CB_ADDSTRING, 0, (LPARAM)L"Stand");

        // Convert button
        UI::CustomButton convertBtn;
        convertBtn.Create(mainWindow, L"Convert Outfit", 
            60 + panelWidth, 170, 200, 45, ID_BTN_CONVERT);
        convertBtn.SetCallback([this]() {
            int targetIdx = SendMessageW(targetFormatCombo, CB_GETCURSEL, 0, 0);
            if (targetIdx >= 0 && outfitLoaded) {
                FormatConverter::FormatType target = 
                    static_cast<FormatConverter::FormatType>(targetIdx);
                ConvertAndSave(target);
            }
        });

        // Memory controls
        UI::CustomButton attachBtn;
        attachBtn.Create(mainWindow, L"Attach to GTA V", 
            60 + panelWidth, 230, 200, 40, ID_BTN_ATTACH_GAME);
        attachBtn.SetCallback([this]() { AttachToMemory(); });

        UI::CustomButton readBtn;
        readBtn.Create(mainWindow, L"Read Outfit", 
            60 + panelWidth, 280, 200, 40, ID_BTN_READ_OUTFIT);
        readBtn.SetCallback([this]() { ReadFromMemory(); });

        UI::CustomButton writeBtn;
        writeBtn.Create(mainWindow, L"Write Outfit", 
            60 + panelWidth, 330, 200, 40, ID_BTN_WRITE_OUTFIT);
        writeBtn.SetCallback([this]() { WriteToMemory(); });

        // Editor button
        UI::CustomButton editorBtn;
        editorBtn.Create(mainWindow, L"Open Editor", 
            60 + panelWidth, 390, 200, 40, ID_EDITOR_OPEN);
        editorBtn.SetCallback([this]() { OnOpenEditor(); });

        UpdateUIState();
    }

    void Application::SetupEventHandlers() {
        // Additional event handling setup if needed
    }

    void Application::UpdateUIState() {
        EnableWindow(componentList, outfitLoaded ? TRUE : FALSE);
        
        if (outfitLoaded) {
            windowManager->SetStatusText(L"Outfit loaded - Ready to convert");
        } else if (memoryAttached) {
            windowManager->SetStatusText(L"Attached to GTA V - Ready to read/write");
        } else {
            windowManager->SetStatusText(L"Ready - Credit to @sizrox");
        }
    }

    int Application::Run() {
        windowManager->ShowWindow();

        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        return static_cast<int>(msg.wParam);
    }

    void Application::Shutdown() {
        if (memoryEditor && memoryAttached) {
            memoryEditor->Detach();
        }
    }

    // ============== FILE OPERATIONS ==============
    void Application::OnFileOpen() {
        std::wstring filter = 
            L"All Outfit Files\0*.json;*.txt\0"
            L"JSON Files (*.json)\0*.json\0"
            L"Text Files (*.txt)\0*.txt\0"
            L"All Files (*.*)\0*.*\0\0";

        std::wstring filepath = UI::DialogHelper::OpenFileDialog(
            mainWindow, filter.c_str(), L"Open Outfit File");

        if (!filepath.empty()) {
            LoadOutfitFile(filepath);
        }
    }

    void Application::LoadOutfitFile(const std::wstring& filepath) {
        std::string path = WStringToString(filepath);
        currentFormat = FormatConverter::DetectFormat(path);

        bool success = false;
        switch (currentFormat) {
            case FormatConverter::FormatType::CHERAX: {
                CheraxOutfit cherax;
                if (FileHandler::LoadCheraxOutfit(path, cherax)) {
                    currentOutfit = FormatConverter::CheraxToYim(cherax);
                    success = true;
                }
                break;
            }
            case FormatConverter::FormatType::YIM: {
                success = FileHandler::LoadYimOutfit(path, currentOutfit);
                break;
            }
            case FormatConverter::FormatType::LEXIS: {
                LexisOutfit lexis;
                if (FileHandler::LoadLexisOutfit(path, lexis)) {
                    currentOutfit = FormatConverter::LexisToYim(lexis);
                    success = true;
                }
                break;
            }
            case FormatConverter::FormatType::STAND: {
                StandOutfit stand;
                if (FileHandler::LoadStandOutfit(path, stand)) {
                    currentOutfit = FormatConverter::StandToYim(stand);
                    success = true;
                }
                break;
            }
            default:
                ShowError(L"Unknown outfit format!");
                return;
        }

        if (success) {
            outfitLoaded = true;
            currentFilePath = path;
            SetWindowTextW(sourcePathEdit, filepath.c_str());
            SendMessageW(sourceFormatCombo, CB_SETCURSEL, 
                        static_cast<int>(currentFormat), 0);
            PopulateComponentList();
            UpdateUIState();
            ShowSuccess(L"Outfit loaded successfully!");
        } else {
            ShowError(L"Failed to load outfit file!");
        }
    }

    void Application::ConvertAndSave(FormatConverter::FormatType targetFormat) {
        WCHAR targetPath[MAX_PATH];
        GetWindowTextW(targetPathEdit, targetPath, MAX_PATH);

        if (wcslen(targetPath) == 0) {
            ShowError(L"Please specify a target file path!");
            return;
        }

        SaveOutfitFile(targetPath, targetFormat);
    }

    void Application::SaveOutfitFile(const std::wstring& filepath, 
                                    FormatConverter::FormatType format) {
        std::string path = WStringToString(filepath);
        bool success = false;

        switch (format) {
            case FormatConverter::FormatType::CHERAX: {
                CheraxOutfit cherax = FormatConverter::YimToCherax(currentOutfit);
                success = FileHandler::SaveCheraxOutfit(path, cherax);
                break;
            }
            case FormatConverter::FormatType::YIM: {
                success = FileHandler::SaveYimOutfit(path, currentOutfit);
                break;
            }
            case FormatConverter::FormatType::LEXIS: {
                LexisOutfit lexis = FormatConverter::YimToLexis(currentOutfit);
                success = FileHandler::SaveLexisOutfit(path, lexis);
                break;
            }
            case FormatConverter::FormatType::STAND: {
                StandOutfit stand = FormatConverter::YimToStand(currentOutfit);
                success = FileHandler::SaveStandOutfit(path, stand);
                break;
            }
            default:
                ShowError(L"Invalid target format!");
                return;
        }

        if (success) {
            ShowSuccess(L"Outfit converted and saved successfully!");
        } else {
            ShowError(L"Failed to save outfit file!");
        }
    }

    // ============== MEMORY OPERATIONS ==============
    void Application::AttachToMemory() {
        if (memoryEditor->Initialize()) {
            memoryAttached = true;
            ShowSuccess(L"Successfully attached to GTA V!");
            UpdateUIState();
        } else {
            ShowError(L"Failed to attach to GTA V! Make sure the game is running.");
        }
    }

    void Application::ReadFromMemory() {
        if (!memoryAttached) {
            ShowError(L"Not attached to GTA V!");
            return;
        }

        if (memoryEditor->ReadCurrentOutfit(currentOutfit)) {
            outfitLoaded = true;
            currentFormat = FormatConverter::FormatType::YIM;
            PopulateComponentList();
            UpdateUIState();
            ShowSuccess(L"Outfit read from game memory!");
        } else {
            ShowError(L"Failed to read outfit from memory!");
        }
    }

    void Application::WriteToMemory() {
        if (!memoryAttached) {
            ShowError(L"Not attached to GTA V!");
            return;
        }

        if (!outfitLoaded) {
            ShowError(L"No outfit loaded to write!");
            return;
        }

        if (memoryEditor->WriteOutfit(currentOutfit)) {
            ShowSuccess(L"Outfit written to game successfully!");
        } else {
            ShowError(L"Failed to write outfit to memory!");
        }
    }

    void Application::PopulateComponentList() {
        SendMessageW(componentList, LB_RESETCONTENT, 0, 0);

        for (const auto& pair : currentOutfit.components) {
            std::wstring item = L"Component " + std::to_wstring(pair.first) + 
                               L": Drawable=" + std::to_wstring(pair.second.drawable) +
                               L", Texture=" + std::to_wstring(pair.second.texture);
            SendMessageW(componentList, LB_ADDSTRING, 0, (LPARAM)item.c_str());
        }

        for (const auto& pair : currentOutfit.props) {
            std::wstring item = L"Prop " + std::to_wstring(pair.first) + 
                               L": Drawable=" + std::to_wstring(pair.second.drawable) +
                               L", Texture=" + std::to_wstring(pair.second.texture);
            SendMessageW(componentList, LB_ADDSTRING, 0, (LPARAM)item.c_str());
        }
    }

    void Application::OnOpenEditor() {
        if (!outfitLoaded) {
            ShowError(L"Please load an outfit first!");
            return;
        }

        // Open editor window (to be implemented)
        ShowSuccess(L"Editor opening... (feature in development)");
    }

    void Application::OnAbout() {
        UI::AboutDialog::Show(mainWindow);
    }

    // ============== UTILITY FUNCTIONS ==============
    void Application::ShowError(const std::wstring& message) {
        UI::DialogHelper::ShowMessage(mainWindow, message, 
            L"Error", MB_OK | MB_ICONERROR);
    }

    void Application::ShowSuccess(const std::wstring& message) {
        UI::DialogHelper::ShowMessage(mainWindow, message, 
            L"Success", MB_OK | MB_ICONINFORMATION);
    }

    std::string Application::WStringToString(const std::wstring& wstr) {
        if (wstr.empty()) return std::string();
        
        int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
        std::string result(size - 1, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, nullptr, nullptr);
        
        return result;
    }

    std::wstring Application::StringToWString(const std::string& str) {
        if (str.empty()) return std::wstring();
        
        int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
        std::wstring result(size - 1, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
        
        return result;
    }

} // namespace OutfitConverter
