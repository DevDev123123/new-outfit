#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <cstdint>
#include "OutfitStructures.h"

namespace OutfitConverter {

    // ============== MEMORY OFFSETS ==============
    struct MemoryOffsets {
        // AOB Patterns
        static constexpr const char* WORLD_PATTERN = "48 8B 05 ? ? ? ? 45 0F C6 C0";
        static constexpr const char* OUTFIT_PATTERN = "48 8D 3D ? ? ? ? 80 3E 00 0F 84 ? ? ? ?";

        // Component Offsets
        static constexpr uint32_t OFFSET_IMP = 0x5D78;
        static constexpr uint32_t OFFSET_NAME = 0x6250;
        static constexpr uint32_t OFFSET_MASK = 0x3F40;
        static constexpr uint32_t OFFSET_HAIR = 0x3F48;
        static constexpr uint32_t OFFSET_GLOVES = 0x3F50;
        static constexpr uint32_t OFFSET_LEGS = 0x3F58;
        static constexpr uint32_t OFFSET_PARA = 0x3F60;
        static constexpr uint32_t OFFSET_SHOES = 0x3F68;
        static constexpr uint32_t OFFSET_MISC = 0x3F70;
        static constexpr uint32_t OFFSET_TOP2 = 0x3F78;
        static constexpr uint32_t OFFSET_ARMOR = 0x3F80;
        static constexpr uint32_t OFFSET_DECALS = 0x3F88;
        static constexpr uint32_t OFFSET_TORSO = 0x3F90;
        static constexpr uint32_t OFFSET_HAT = 0x5058;
        static constexpr uint32_t OFFSET_GLASSES = 0x5060;
        static constexpr uint32_t OFFSET_EARRINGS = 0x5068;
        static constexpr uint32_t OFFSET_WATCHES = 0x5088;
        static constexpr uint32_t OFFSET_BRACELETS = 0x5090;
        static constexpr uint32_t OFFSET_TEXTURE = 0x890;
        static constexpr uint32_t OFFSET_TEXTURE_HEAD = 0x698;

        // Spoof Offsets (chain)
        static constexpr uint32_t SPOOF_NAME_1 = 0x8;
        static constexpr uint32_t SPOOF_NAME_2 = 0x10A8;
        static constexpr uint32_t SPOOF_NAME_3 = 0xFC;
    };

    // ============== MEMORY EDITOR CLASS ==============
    class MemoryEditor {
    private:
        HANDLE processHandle;
        uintptr_t worldBase;
        uintptr_t outfitBase;
        bool initialized;

        // Pattern scanning helper
        uintptr_t PatternScan(const char* pattern);
        std::vector<uint8_t> ParsePattern(const char* pattern);
        bool ComparePattern(const uint8_t* data, const std::vector<uint8_t>& pattern);

        // Memory read/write helpers
        template<typename T>
        bool ReadMemory(uintptr_t address, T& value);
        
        template<typename T>
        bool WriteMemory(uintptr_t address, const T& value);

        // Pointer chain resolver
        uintptr_t ResolvePointerChain(uintptr_t base, const std::vector<uint32_t>& offsets);

    public:
        MemoryEditor();
        ~MemoryEditor();

        // Initialization
        bool Initialize();
        bool AttachToProcess(const std::wstring& processName);
        void Detach();

        // Outfit reading
        bool ReadCurrentOutfit(YimOutfit& outfit);
        bool ReadComponentData(uint32_t slot, Component& component);
        bool ReadPropData(uint32_t slot, Prop& prop);

        // Outfit writing
        bool WriteOutfit(const YimOutfit& outfit);
        bool WriteComponent(uint32_t slot, const Component& component);
        bool WriteProp(uint32_t slot, const Prop& prop);

        // Advanced features
        bool BackupCurrentOutfit(const std::string& filepath);
        bool RestoreOutfit(const std::string& filepath);
        
        // Model switching
        bool SetPedModel(uint32_t modelHash);
        uint32_t GetCurrentModel();

        // Status checks
        bool IsInitialized() const { return initialized; }
        uintptr_t GetWorldBase() const { return worldBase; }
        uintptr_t GetOutfitBase() const { return outfitBase; }

        // Outfit name operations
        bool ReadOutfitName(std::string& name);
        bool WriteOutfitName(const std::string& name);
    };

    // ============== TEMPLATE IMPLEMENTATIONS ==============
    template<typename T>
    bool MemoryEditor::ReadMemory(uintptr_t address, T& value) {
        if (!processHandle || address == 0) return false;
        
        SIZE_T bytesRead;
        return ReadProcessMemory(processHandle, 
                                reinterpret_cast<LPCVOID>(address), 
                                &value, 
                                sizeof(T), 
                                &bytesRead) && bytesRead == sizeof(T);
    }

    template<typename T>
    bool MemoryEditor::WriteMemory(uintptr_t address, const T& value) {
        if (!processHandle || address == 0) return false;
        
        SIZE_T bytesWritten;
        return WriteProcessMemory(processHandle, 
                                 reinterpret_cast<LPVOID>(address), 
                                 &value, 
                                 sizeof(T), 
                                 &bytesWritten) && bytesWritten == sizeof(T);
    }

} // namespace OutfitConverter
