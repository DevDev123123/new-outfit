#pragma once

// Control IDs accessible across the entire application
namespace OutfitConverter {

    // Menu bar IDs
    constexpr int ID_FILE_OPEN = 1001;
    constexpr int ID_FILE_SAVE = 1002;
    constexpr int ID_FILE_EXIT = 1003;
    constexpr int ID_CONVERT_CHERAX_TO_YIM = 1010;
    constexpr int ID_CONVERT_CHERAX_TO_LEXIS = 1011;
    constexpr int ID_CONVERT_CHERAX_TO_STAND = 1012;
    constexpr int ID_CONVERT_YIM_TO_CHERAX = 1013;
    constexpr int ID_CONVERT_YIM_TO_LEXIS = 1014;
    constexpr int ID_CONVERT_YIM_TO_STAND = 1015;
    constexpr int ID_CONVERT_LEXIS_TO_CHERAX = 1016;
    constexpr int ID_CONVERT_LEXIS_TO_YIM = 1017;
    constexpr int ID_CONVERT_LEXIS_TO_STAND = 1018;
    constexpr int ID_CONVERT_STAND_TO_CHERAX = 1019;
    constexpr int ID_CONVERT_STAND_TO_YIM = 1020;
    constexpr int ID_CONVERT_STAND_TO_LEXIS = 1021;
    constexpr int ID_EDITOR_OPEN = 1030;
    constexpr int ID_MEMORY_ATTACH = 1040;
    constexpr int ID_MEMORY_READ = 1041;
    constexpr int ID_MEMORY_WRITE = 1042;
    constexpr int ID_HELP_ABOUT = 1050;

    // Main window control IDs
    constexpr int ID_BTN_LOAD_FILE = 2001;
    constexpr int ID_BTN_SAVE_FILE = 2002;
    constexpr int ID_BTN_CONVERT = 2003;
    constexpr int ID_BTN_CLEAR = 2004;
    constexpr int ID_COMBO_SOURCE_FORMAT = 2010;
    constexpr int ID_COMBO_TARGET_FORMAT = 2011;
    constexpr int ID_EDIT_SOURCE_PATH = 2020;
    constexpr int ID_EDIT_TARGET_PATH = 2021;
    constexpr int ID_BTN_BROWSE_SOURCE = 2022;
    constexpr int ID_BTN_BROWSE_TARGET = 2023;
    constexpr int ID_LIST_COMPONENTS = 2030;
    constexpr int ID_STATUS_BAR = 2040;

    // Editor control IDs
    constexpr int ID_EDITOR_LIST = 3001;
    constexpr int ID_EDITOR_DRAWABLE = 3010;
    constexpr int ID_EDITOR_TEXTURE = 3011;
    constexpr int ID_EDITOR_PALETTE = 3012;
    constexpr int ID_BTN_EDITOR_APPLY = 3020;
    constexpr int ID_BTN_EDITOR_RESET = 3021;
    constexpr int ID_BTN_EDITOR_SAVE = 3022;

    // Memory editor control IDs
    constexpr int ID_MEMORY_LIST = 4001;
    constexpr int ID_BTN_ATTACH_GAME = 4010;
    constexpr int ID_BTN_READ_OUTFIT = 4011;
    constexpr int ID_BTN_WRITE_OUTFIT = 4012;
    constexpr int ID_BTN_BACKUP_OUTFIT = 4013;
    constexpr int ID_EDIT_MEMORY_STATUS = 4020;

} // namespace OutfitConverter
