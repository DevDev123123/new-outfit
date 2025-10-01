#pragma once
#include "OutfitStructures.h"
#include <string>

namespace OutfitConverter {

    // ============== FORMAT CONVERTER CLASS ==============
    class FormatConverter {
    private:
        // Component slot mapping helpers
        static std::string GetCheraxComponentName(int slot);
        static int GetSlotFromCheraxName(const std::string& name);
        static std::string GetCheraxPropName(int slot);
        static int GetPropSlotFromCheraxName(const std::string& name);

        // Model name to hash conversion
        static uint32_t ModelNameToHash(const std::string& modelName);
        static std::string ModelHashToName(uint32_t hash);

    public:
        // ============== CHERAX CONVERSIONS ==============
        static YimOutfit CheraxToYim(const CheraxOutfit& cherax);
        static LexisOutfit CheraxToLexis(const CheraxOutfit& cherax);
        static StandOutfit CheraxToStand(const CheraxOutfit& cherax);

        // ============== YIM CONVERSIONS ==============
        static CheraxOutfit YimToCherax(const YimOutfit& yim);
        static LexisOutfit YimToLexis(const YimOutfit& yim);
        static StandOutfit YimToStand(const YimOutfit& yim);

        // ============== LEXIS CONVERSIONS ==============
        static CheraxOutfit LexisToCherax(const LexisOutfit& lexis);
        static YimOutfit LexisToYim(const LexisOutfit& lexis);
        static StandOutfit LexisToStand(const LexisOutfit& lexis);

        // ============== STAND CONVERSIONS ==============
        static CheraxOutfit StandToCherax(const StandOutfit& stand);
        static YimOutfit StandToYim(const StandOutfit& stand);
        static LexisOutfit StandToLexis(const StandOutfit& stand);

        // ============== UNIVERSAL CONVERTER ==============
        // Auto-detect format and convert to target
        enum class FormatType {
            CHERAX,
            YIM,
            LEXIS,
            STAND,
            UNKNOWN
        };

        static FormatType DetectFormat(const std::string& filepath);
        
        // Universal conversion interface
        static bool ConvertFile(const std::string& inputPath, 
                              const std::string& outputPath,
                              FormatType targetFormat);

        // Validation helpers
        static bool ValidateComponent(const Component& comp);
        static bool ValidateProp(const Prop& prop);
        static bool ValidateModel(uint32_t model);
    };

    // ============== COMPONENT MAPPING CONSTANTS ==============
    namespace ComponentMapping {
        // Cherax component names to slot indices
        const std::map<std::string, int> CHERAX_COMPONENT_MAP = {
            {"Head", SLOT_HEAD},
            {"Beard", SLOT_BEARD},
            {"Hair", SLOT_HAIR},
            {"Torso", SLOT_TORSO},
            {"Legs", SLOT_LEGS},
            {"Hands", SLOT_HANDS},
            {"Feet", SLOT_FEET},
            {"Teeth", SLOT_TEETH},
            {"Special", SLOT_SPECIAL},
            {"Special 2", SLOT_SPECIAL2},
            {"Decal", SLOT_DECAL},
            {"Tuxedo/Jacket Bib", SLOT_JACKET}
        };

        const std::map<std::string, int> CHERAX_PROP_MAP = {
            {"Hat", PROP_HEAD},
            {"Glasses", PROP_EYES},
            {"Earwear", PROP_EARS},
            {"Watch", PROP_LEFT_WRIST},
            {"Bracelet", PROP_RIGHT_WRIST}
        };

        // Model hash constants
        const uint32_t MODEL_MP_M_FREEMODE_01 = 1885233650; // Male
        const uint32_t MODEL_MP_F_FREEMODE_01 = 2627665880; // Female

        // Stand model name mapping
        const std::map<std::string, uint32_t> STAND_MODEL_MAP = {
            {"Online Male", MODEL_MP_M_FREEMODE_01},
            {"Online Female", MODEL_MP_F_FREEMODE_01}
        };
    }

} // namespace OutfitConverter
