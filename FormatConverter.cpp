#include "FormatConverter.h"
#include <algorithm>

namespace OutfitConverter {

    // ============== CHERAX TO YIM ==============
    YimOutfit FormatConverter::CheraxToYim(const CheraxOutfit& cherax) {
        YimOutfit yim;
        yim.model = cherax.model;

        // Convert components
        for (const auto& pair : cherax.components) {
            int slot = GetSlotFromCheraxName(pair.first);
            if (slot >= 0 && slot < 12) {
                yim.components[slot] = pair.second;
            }
        }

        // Convert props
        for (const auto& pair : cherax.props) {
            int slot = GetPropSlotFromCheraxName(pair.first);
            if (slot >= 0 && slot < 9) {
                yim.props[slot] = pair.second;
            }
        }

        return yim;
    }

    // ============== CHERAX TO LEXIS ==============
    LexisOutfit FormatConverter::CheraxToLexis(const CheraxOutfit& cherax) {
        LexisOutfit lexis;
        lexis.model = cherax.model;

        // Map components
        for (const auto& pair : cherax.components) {
            int slot = GetSlotFromCheraxName(pair.first);
            if (slot >= 0 && slot < 12) {
                lexis.component[slot] = pair.second.drawable;
                lexis.component_variation[slot] = pair.second.texture;
            }
        }

        // Map props
        for (const auto& pair : cherax.props) {
            int slot = GetPropSlotFromCheraxName(pair.first);
            if (slot >= 0 && slot < 9) {
                lexis.prop[slot] = pair.second.drawable;
                lexis.prop_variation[slot] = pair.second.texture;
            }
        }

        return lexis;
    }

    // ============== CHERAX TO STAND ==============
    StandOutfit FormatConverter::CheraxToStand(const CheraxOutfit& cherax) {
        StandOutfit stand;
        stand.model_name = ModelHashToName(cherax.model);

        // Map all components
        auto getComp = [&](const std::string& name) -> Component {
            auto it = cherax.components.find(name);
            return (it != cherax.components.end()) ? it->second : Component();
        };

        auto getProp = [&](const std::string& name) -> Prop {
            auto it = cherax.props.find(name);
            return (it != cherax.props.end()) ? it->second : Prop();
        };

        Component head = getComp("Head");
        stand.head = head.drawable;
        stand.head_variation = head.texture;

        Component mask = getComp("Beard");
        stand.mask = mask.drawable;
        stand.mask_variation = mask.texture;

        Component hair = getComp("Hair");
        stand.hair = hair.drawable;
        stand.hair_colour = cherax.primary_hair_tint;
        stand.hair_colour_highlight = cherax.secondary_hair_tint;

        Component torso = getComp("Torso");
        stand.gloves_torso = torso.drawable;
        stand.gloves_torso_variation = torso.texture;

        Component legs = getComp("Legs");
        stand.pants = legs.drawable;
        stand.pants_variation = legs.texture;

        Component feet = getComp("Feet");
        stand.shoes = feet.drawable;
        stand.shoes_variation = feet.texture;

        Component special = getComp("Special");
        stand.top = special.drawable;
        stand.top_variation = special.texture;

        Component special2 = getComp("Special 2");
        stand.top2 = special2.drawable;
        stand.top2_variation = special2.texture;

        Component decal = getComp("Decal");
        stand.decals = decal.drawable;
        stand.decals_variation = decal.texture;

        // Props
        Prop hat = getProp("Hat");
        stand.hat = hat.drawable;
        stand.hat_variation = hat.texture;

        Prop glasses = getProp("Glasses");
        stand.glasses = glasses.drawable;
        stand.glasses_variation = glasses.texture;

        Prop earwear = getProp("Earwear");
        stand.earwear = earwear.drawable;
        stand.earwear_variation = earwear.texture;

        Prop watch = getProp("Watch");
        stand.watch = watch.drawable;
        stand.watch_variation = watch.texture;

        Prop bracelet = getProp("Bracelet");
        stand.bracelet = bracelet.drawable;
        stand.bracelet_variation = bracelet.texture;

        return stand;
    }

    // ============== YIM TO CHERAX ==============
    CheraxOutfit FormatConverter::YimToCherax(const YimOutfit& yim) {
        CheraxOutfit cherax;
        cherax.model = yim.model;

        // Convert components
        for (const auto& pair : yim.components) {
            std::string name = GetCheraxComponentName(pair.first);
            if (!name.empty()) {
                cherax.components[name] = pair.second;
            }
        }

        // Convert props
        for (const auto& pair : yim.props) {
            std::string name = GetCheraxPropName(pair.first);
            if (!name.empty()) {
                cherax.props[name] = pair.second;
            }
        }

        return cherax;
    }

    // ============== YIM TO LEXIS ==============
    LexisOutfit FormatConverter::YimToLexis(const YimOutfit& yim) {
        LexisOutfit lexis;
        lexis.model = yim.model;

        for (const auto& pair : yim.components) {
            if (pair.first >= 0 && pair.first < 12) {
                lexis.component[pair.first] = pair.second.drawable;
                lexis.component_variation[pair.first] = pair.second.texture;
            }
        }

        for (const auto& pair : yim.props) {
            if (pair.first >= 0 && pair.first < 9) {
                lexis.prop[pair.first] = pair.second.drawable;
                lexis.prop_variation[pair.first] = pair.second.texture;
            }
        }

        return lexis;
    }

    // ============== YIM TO STAND ==============
    StandOutfit FormatConverter::YimToStand(const YimOutfit& yim) {
        CheraxOutfit cherax = YimToCherax(yim);
        return CheraxToStand(cherax);
    }

    // ============== LEXIS TO CHERAX ==============
    CheraxOutfit FormatConverter::LexisToCherax(const LexisOutfit& lexis) {
        CheraxOutfit cherax;
        cherax.model = lexis.model;

        for (int i = 0; i < 12 && i < lexis.component.size(); i++) {
            std::string name = GetCheraxComponentName(i);
            if (!name.empty()) {
                cherax.components[name] = Component(
                    lexis.component[i],
                    lexis.component_variation[i],
                    0
                );
            }
        }

        for (int i = 0; i < 9 && i < lexis.prop.size(); i++) {
            std::string name = GetCheraxPropName(i);
            if (!name.empty()) {
                cherax.props[name] = Prop(
                    lexis.prop[i],
                    lexis.prop_variation[i]
                );
            }
        }

        return cherax;
    }

    // ============== LEXIS TO YIM ==============
    YimOutfit FormatConverter::LexisToYim(const LexisOutfit& lexis) {
        YimOutfit yim;
        yim.model = lexis.model;

        for (int i = 0; i < 12 && i < lexis.component.size(); i++) {
            yim.components[i] = Component(
                lexis.component[i],
                lexis.component_variation[i],
                0
            );
        }

        for (int i = 0; i < 9 && i < lexis.prop.size(); i++) {
            yim.props[i] = Prop(
                lexis.prop[i],
                lexis.prop_variation[i]
            );
        }

        return yim;
    }

    // ============== LEXIS TO STAND ==============
    StandOutfit FormatConverter::LexisToStand(const LexisOutfit& lexis) {
        CheraxOutfit cherax = LexisToCherax(lexis);
        return CheraxToStand(cherax);
    }

    // ============== STAND TO CHERAX ==============
    CheraxOutfit FormatConverter::StandToCherax(const StandOutfit& stand) {
        CheraxOutfit cherax;
        cherax.model = ModelNameToHash(stand.model_name);

        cherax.components["Head"] = Component(stand.head, stand.head_variation);
        cherax.components["Beard"] = Component(stand.mask, stand.mask_variation);
        cherax.components["Hair"] = Component(stand.hair, 0);
        cherax.components["Torso"] = Component(stand.gloves_torso, stand.gloves_torso_variation);
        cherax.components["Legs"] = Component(stand.pants, stand.pants_variation);
        cherax.components["Feet"] = Component(stand.shoes, stand.shoes_variation);
        cherax.components["Special"] = Component(stand.top, stand.top_variation);
        cherax.components["Special 2"] = Component(stand.top2, stand.top2_variation);
        cherax.components["Decal"] = Component(stand.decals, stand.decals_variation);

        cherax.props["Hat"] = Prop(stand.hat, stand.hat_variation);
        cherax.props["Glasses"] = Prop(stand.glasses, stand.glasses_variation);
        cherax.props["Earwear"] = Prop(stand.earwear, stand.earwear_variation);
        cherax.props["Watch"] = Prop(stand.watch, stand.watch_variation);
        cherax.props["Bracelet"] = Prop(stand.bracelet, stand.bracelet_variation);

        cherax.primary_hair_tint = stand.hair_colour;
        cherax.secondary_hair_tint = stand.hair_colour_highlight;

        return cherax;
    }

    // ============== STAND TO YIM ==============
    YimOutfit FormatConverter::StandToYim(const StandOutfit& stand) {
        CheraxOutfit cherax = StandToCherax(stand);
        return CheraxToYim(cherax);
    }

    // ============== STAND TO LEXIS ==============
    LexisOutfit FormatConverter::StandToLexis(const StandOutfit& stand) {
        CheraxOutfit cherax = StandToCherax(stand);
        return CheraxToLexis(cherax);
    }

    // ============== HELPER FUNCTIONS ==============
    std::string FormatConverter::GetCheraxComponentName(int slot) {
        const std::map<int, std::string> slotNames = {
            {0, "Head"}, {1, "Beard"}, {2, "Hair"}, {3, "Torso"},
            {4, "Legs"}, {5, "Hands"}, {6, "Feet"}, {7, "Teeth"},
            {8, "Special"}, {9, "Special 2"}, {10, "Decal"}, {11, "Tuxedo/Jacket Bib"}
        };
        auto it = slotNames.find(slot);
        return (it != slotNames.end()) ? it->second : "";
    }

    int FormatConverter::GetSlotFromCheraxName(const std::string& name) {
        for (const auto& pair : ComponentMapping::CHERAX_COMPONENT_MAP) {
            if (pair.first == name) return pair.second;
        }
        return -1;
    }

    std::string FormatConverter::GetCheraxPropName(int slot) {
        const std::map<int, std::string> propNames = {
            {0, "Hat"}, {1, "Glasses"}, {2, "Earwear"},
            {6, "Watch"}, {7, "Bracelet"}
        };
        auto it = propNames.find(slot);
        return (it != propNames.end()) ? it->second : "";
    }

    int FormatConverter::GetPropSlotFromCheraxName(const std::string& name) {
        for (const auto& pair : ComponentMapping::CHERAX_PROP_MAP) {
            if (pair.first == name) return pair.second;
        }
        return -1;
    }

    uint32_t FormatConverter::ModelNameToHash(const std::string& modelName) {
        auto it = ComponentMapping::STAND_MODEL_MAP.find(modelName);
        return (it != ComponentMapping::STAND_MODEL_MAP.end()) ? 
               it->second : ComponentMapping::MODEL_MP_M_FREEMODE_01;
    }

    std::string FormatConverter::ModelHashToName(uint32_t hash) {
        if (hash == ComponentMapping::MODEL_MP_M_FREEMODE_01) return "Online Male";
        if (hash == ComponentMapping::MODEL_MP_F_FREEMODE_01) return "Online Female";
        return "Online Male";
    }

    // ============== FORMAT DETECTION ==============
    FormatConverter::FormatType FormatConverter::DetectFormat(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) return FormatType::UNKNOWN;

        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();

        if (content.find("\"format\": \"Cherax Entity\"") != std::string::npos) {
            return FormatType::CHERAX;
        }
        if (content.find("\"blend_data\"") != std::string::npos) {
            return FormatType::YIM;
        }
        if (content.find("\"component variation\"") != std::string::npos) {
            return FormatType::LEXIS;
        }
        if (content.find("Model:") != std::string::npos && 
            content.find("Hair Colour") != std::string::npos) {
            return FormatType::STAND;
        }

        return FormatType::UNKNOWN;
    }

    // ============== VALIDATION ==============
    bool FormatConverter::ValidateComponent(const Component& comp) {
        return comp.drawable >= -1 && comp.drawable < 1000 &&
               comp.texture >= 0 && comp.texture < 100;
    }

    bool FormatConverter::ValidateProp(const Prop& prop) {
        return prop.drawable >= -1 && prop.drawable < 500 &&
               prop.texture >= -1 && prop.texture < 50;
    }

    bool FormatConverter::ValidateModel(uint32_t model) {
        return model == ComponentMapping::MODEL_MP_M_FREEMODE_01 ||
               model == ComponentMapping::MODEL_MP_F_FREEMODE_01;
    }

} // namespace OutfitConverter
