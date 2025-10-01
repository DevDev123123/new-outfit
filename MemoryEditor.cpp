#include "MemoryEditor.h"
#include <tlhelp32.h>
#include <psapi.h>
#include <sstream>
#include <algorithm>

namespace OutfitConverter {

    // ============== CONSTRUCTOR / DESTRUCTOR ==============
    MemoryEditor::MemoryEditor() 
        : processHandle(nullptr), worldBase(0), outfitBase(0), initialized(false) {
    }

    MemoryEditor::~MemoryEditor() {
        Detach();
    }

    // ============== INITIALIZATION ==============
    bool MemoryEditor::Initialize() {
        if (!AttachToProcess(L"GTA5.exe")) {
            return false;
        }

        // Scan for World base
        worldBase = PatternScan(MemoryOffsets::WORLD_PATTERN);
        if (worldBase == 0) {
            Detach();
            return false;
        }

        // Resolve World pointer
        int offset = 0;
        if (ReadMemory(worldBase + 3, offset)) {
            worldBase = worldBase + offset + 7;
            ReadMemory(worldBase, worldBase);
        }

        // Scan for Outfit base
        outfitBase = PatternScan(MemoryOffsets::OUTFIT_PATTERN);
        if (outfitBase == 0) {
            Detach();
            return false;
        }

        // Resolve Outfit pointer
        if (ReadMemory(outfitBase + 3, offset)) {
            outfitBase = outfitBase + offset + 7;
        }

        initialized = (worldBase != 0 && outfitBase != 0);
        return initialized;
    }

    bool MemoryEditor::AttachToProcess(const std::wstring& processName) {
        DWORD processID = 0;
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        
        if (snapshot == INVALID_HANDLE_VALUE) {
            return false;
        }

        PROCESSENTRY32W processEntry = { sizeof(PROCESSENTRY32W) };
        
        if (Process32FirstW(snapshot, &processEntry)) {
            do {
                if (processName == processEntry.szExeFile) {
                    processID = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32NextW(snapshot, &processEntry));
        }

        CloseHandle(snapshot);

        if (processID == 0) {
            return false;
        }

        processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
        return (processHandle != nullptr);
    }

    void MemoryEditor::Detach() {
        if (processHandle) {
            CloseHandle(processHandle);
            processHandle = nullptr;
        }
        worldBase = 0;
        outfitBase = 0;
        initialized = false;
    }

    // ============== PATTERN SCANNING ==============
    uintptr_t MemoryEditor::PatternScan(const char* pattern) {
        if (!processHandle) return 0;

        MODULEINFO moduleInfo;
        HMODULE hModule = nullptr;

        if (!EnumProcessModules(processHandle, &hModule, sizeof(hModule), nullptr)) {
            return 0;
        }

        if (!GetModuleInformation(processHandle, hModule, &moduleInfo, sizeof(moduleInfo))) {
            return 0;
        }

        uintptr_t baseAddress = reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll);
        size_t moduleSize = moduleInfo.SizeOfImage;

        std::vector<uint8_t> patternBytes = ParsePattern(pattern);
        std::vector<uint8_t> buffer(moduleSize);

        SIZE_T bytesRead;
        if (!ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(baseAddress), 
                              buffer.data(), moduleSize, &bytesRead)) {
            return 0;
        }

        for (size_t i = 0; i < bytesRead - patternBytes.size(); i++) {
            if (ComparePattern(buffer.data() + i, patternBytes)) {
                return baseAddress + i;
            }
        }

        return 0;
    }

    std::vector<uint8_t> MemoryEditor::ParsePattern(const char* pattern) {
        std::vector<uint8_t> result;
        std::stringstream ss(pattern);
        std::string token;

        while (ss >> token) {
            if (token == "?") {
                result.push_back(0x00); // Wildcard stored as 0 with special flag
            } else {
                result.push_back(static_cast<uint8_t>(std::stoul(token, nullptr, 16)));
            }
        }

        return result;
    }

    bool MemoryEditor::ComparePattern(const uint8_t* data, const std::vector<uint8_t>& pattern) {
        std::stringstream ss(MemoryOffsets::WORLD_PATTERN);
        std::string token;
        size_t index = 0;

        while (ss >> token && index < pattern.size()) {
            if (token != "?") {
                if (data[index] != pattern[index]) {
                    return false;
                }
            }
            index++;
        }

        return true;
    }

    // ============== POINTER CHAIN RESOLVER ==============
    uintptr_t MemoryEditor::ResolvePointerChain(uintptr_t base, const std::vector<uint32_t>& offsets) {
        uintptr_t address = base;

        for (size_t i = 0; i < offsets.size(); i++) {
            if (!ReadMemory(address, address)) {
                return 0;
            }
            
            if (address == 0) {
                return 0;
            }

            address += offsets[i];
        }

        return address;
    }

    // ============== OUTFIT READING ==============
    bool MemoryEditor::ReadCurrentOutfit(YimOutfit& outfit) {
        if (!initialized) return false;

        // Read model
        uintptr_t playerPed = 0;
        if (!ReadMemory(worldBase + 0x08, playerPed)) return false;
        if (!ReadMemory(playerPed + 0x20, outfit.model)) return false;

        // Read all components
        for (int i = 0; i < 12; i++) {
            Component comp;
            if (ReadComponentData(i, comp)) {
                outfit.components[i] = comp;
            }
        }

        // Read all props
        for (int i = 0; i < 9; i++) {
            Prop prop;
            if (ReadPropData(i, prop)) {
                outfit.props[i] = prop;
            }
        }

        return true;
    }

    bool MemoryEditor::ReadComponentData(uint32_t slot, Component& component) {
        if (!initialized || slot >= 12) return false;

        uintptr_t componentAddress = outfitBase;
        
        // Calculate offset based on slot
        switch (slot) {
            case 0: componentAddress += MemoryOffsets::OFFSET_MASK; break;
            case 2: componentAddress += MemoryOffsets::OFFSET_HAIR; break;
            case 3: componentAddress += MemoryOffsets::OFFSET_TORSO; break;
            case 4: componentAddress += MemoryOffsets::OFFSET_LEGS; break;
            case 5: componentAddress += MemoryOffsets::OFFSET_GLOVES; break;
            case 6: componentAddress += MemoryOffsets::OFFSET_SHOES; break;
            case 8: componentAddress += MemoryOffsets::OFFSET_TOP2; break;
            case 9: componentAddress += MemoryOffsets::OFFSET_ARMOR; break;
            case 10: componentAddress += MemoryOffsets::OFFSET_DECALS; break;
            default: return false;
        }

        if (!ReadMemory(componentAddress, component.drawable)) return false;
        if (!ReadMemory(componentAddress + MemoryOffsets::OFFSET_TEXTURE, component.texture)) return false;
        
        component.palette = 0;
        return true;
    }

    bool MemoryEditor::ReadPropData(uint32_t slot, Prop& prop) {
        if (!initialized || slot >= 9) return false;

        uintptr_t propAddress = outfitBase;

        switch (slot) {
            case 0: propAddress += MemoryOffsets::OFFSET_HAT; break;
            case 1: propAddress += MemoryOffsets::OFFSET_GLASSES; break;
            case 2: propAddress += MemoryOffsets::OFFSET_EARRINGS; break;
            case 6: propAddress += MemoryOffsets::OFFSET_WATCHES; break;
            case 7: propAddress += MemoryOffsets::OFFSET_BRACELETS; break;
            default: prop.drawable = -1; prop.texture = -1; return true;
        }

        if (!ReadMemory(propAddress, prop.drawable)) return false;
        if (!ReadMemory(propAddress + MemoryOffsets::OFFSET_TEXTURE, prop.texture)) return false;

        return true;
    }

    // ============== OUTFIT WRITING ==============
    bool MemoryEditor::WriteOutfit(const YimOutfit& outfit) {
        if (!initialized) return false;

        // Write all components
        for (const auto& pair : outfit.components) {
            if (!WriteComponent(pair.first, pair.second)) {
                return false;
            }
        }

        // Write all props
        for (const auto& pair : outfit.props) {
            if (!WriteProp(pair.first, pair.second)) {
                return false;
            }
        }

        return true;
    }

    bool MemoryEditor::WriteComponent(uint32_t slot, const Component& component) {
        if (!initialized || slot >= 12) return false;

        uintptr_t componentAddress = outfitBase;
        
        switch (slot) {
            case 0: componentAddress += MemoryOffsets::OFFSET_MASK; break;
            case 2: componentAddress += MemoryOffsets::OFFSET_HAIR; break;
            case 3: componentAddress += MemoryOffsets::OFFSET_TORSO; break;
            case 4: componentAddress += MemoryOffsets::OFFSET_LEGS; break;
            case 5: componentAddress += MemoryOffsets::OFFSET_GLOVES; break;
            case 6: componentAddress += MemoryOffsets::OFFSET_SHOES; break;
            case 8: componentAddress += MemoryOffsets::OFFSET_TOP2; break;
            case 9: componentAddress += MemoryOffsets::OFFSET_ARMOR; break;
            case 10: componentAddress += MemoryOffsets::OFFSET_DECALS; break;
            default: return false;
        }

        if (!WriteMemory(componentAddress, component.drawable)) return false;
        if (!WriteMemory(componentAddress + MemoryOffsets::OFFSET_TEXTURE, component.texture)) return false;

        return true;
    }

    bool MemoryEditor::WriteProp(uint32_t slot, const Prop& prop) {
        if (!initialized || slot >= 9) return false;

        uintptr_t propAddress = outfitBase;

        switch (slot) {
            case 0: propAddress += MemoryOffsets::OFFSET_HAT; break;
            case 1: propAddress += MemoryOffsets::OFFSET_GLASSES; break;
            case 2: propAddress += MemoryOffsets::OFFSET_EARRINGS; break;
            case 6: propAddress += MemoryOffsets::OFFSET_WATCHES; break;
            case 7: propAddress += MemoryOffsets::OFFSET_BRACELETS; break;
            default: return true;
        }

        if (!WriteMemory(propAddress, prop.drawable)) return false;
        if (!WriteMemory(propAddress + MemoryOffsets::OFFSET_TEXTURE, prop.texture)) return false;

        return true;
    }

    // ============== MODEL OPERATIONS ==============
    bool MemoryEditor::SetPedModel(uint32_t modelHash) {
        if (!initialized) return false;

        uintptr_t playerPed = 0;
        if (!ReadMemory(worldBase + 0x08, playerPed)) return false;
        
        return WriteMemory(playerPed + 0x20, modelHash);
    }

    uint32_t MemoryEditor::GetCurrentModel() {
        if (!initialized) return 0;

        uintptr_t playerPed = 0;
        uint32_t model = 0;
        
        if (!ReadMemory(worldBase + 0x08, playerPed)) return 0;
        if (!ReadMemory(playerPed + 0x20, model)) return 0;

        return model;
    }

    // ============== OUTFIT NAME OPERATIONS ==============
    bool MemoryEditor::ReadOutfitName(std::string& name) {
        if (!initialized) return false;

        std::vector<uint32_t> offsets = {
            MemoryOffsets::SPOOF_NAME_1,
            MemoryOffsets::SPOOF_NAME_2,
            MemoryOffsets::SPOOF_NAME_3
        };

        uintptr_t nameAddress = ResolvePointerChain(outfitBase, offsets);
        if (nameAddress == 0) return false;

        char buffer[256] = {0};
        SIZE_T bytesRead;
        
        if (!ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(nameAddress),
                              buffer, sizeof(buffer) - 1, &bytesRead)) {
            return false;
        }

        name = std::string(buffer);
        return true;
    }

    bool MemoryEditor::WriteOutfitName(const std::string& name) {
        if (!initialized || name.length() > 255) return false;

        std::vector<uint32_t> offsets = {
            MemoryOffsets::SPOOF_NAME_1,
            MemoryOffsets::SPOOF_NAME_2,
            MemoryOffsets::SPOOF_NAME_3
        };

        uintptr_t nameAddress = ResolvePointerChain(outfitBase, offsets);
        if (nameAddress == 0) return false;

        SIZE_T bytesWritten;
        return WriteProcessMemory(processHandle, reinterpret_cast<LPVOID>(nameAddress),
                                 name.c_str(), name.length() + 1, &bytesWritten);
    }

} // namespace OutfitConverter
