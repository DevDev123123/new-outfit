#pragma once
#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace OutfitConverter {

    // ============== COMMON STRUCTURES ==============
    struct Component {
        int drawable;
        int texture;
        int palette;

        Component() : drawable(0), texture(0), palette(0) {}
        Component(int d, int t, int p = 0) : drawable(d), texture(t), palette(p) {}
    };

    struct Prop {
        int drawable;
        int texture;

        Prop() : drawable(-1), texture(-1) {}
        Prop(int d, int t) : drawable(d), texture(t) {}
    };

    // ============== CHERAX FORMAT ==============
    struct CheraxOutfit {
        std::string format;
        int type;
        uint32_t model;
        uint32_t baseFlags;

        // Components mapped by name
        std::map<std::string, Component> components;
        
        // Props mapped by name
        std::map<std::string, Prop> props;

        // Face features
        std::map<std::string, float> face_features;

        // Hair colors
        int primary_hair_tint;
        int secondary_hair_tint;

        CheraxOutfit() : format("Cherax Entity"), type(2), model(0), baseFlags(66855),
            primary_hair_tint(255), secondary_hair_tint(255) {}
    };

    // ============== YIMMENU FORMAT ==============
    struct BlendData {
        int is_parent;
        int shape_first_id;
        float shape_mix;
        int shape_second_id;
        int shape_third_id;
        int skin_first_id;
        float skin_mix;
        int skin_second_id;
        int skin_third_id;
        float third_mix;

        BlendData() : is_parent(0), shape_first_id(0), shape_mix(0.0f),
            shape_second_id(0), shape_third_id(0), skin_first_id(31),
            skin_mix(0.5f), skin_second_id(43), skin_third_id(0), third_mix(0.0f) {}
    };

    struct YimOutfit {
        BlendData blend_data;
        std::map<int, Component> components;  // Indexed 0-11
        std::map<int, Prop> props;            // Indexed 0-8
        uint32_t model;

        YimOutfit() : model(0) {}
    };

    // ============== LEXIS FORMAT ==============
    struct LexisOutfit {
        std::vector<int> component;           // 12 components
        std::vector<int> component_variation; // 12 variations
        uint32_t model;
        std::vector<int> prop;                // 9 props
        std::vector<int> prop_variation;      // 9 variations

        LexisOutfit() : model(0) {
            component.resize(12, 0);
            component_variation.resize(12, 0);
            prop.resize(9, -1);
            prop_variation.resize(9, -1);
        }
    };

    // ============== STAND FORMAT ==============
    struct StandOutfit {
        std::string model_name;
        int head, head_variation;
        int mask, mask_variation;
        int hair, hair_colour, hair_colour_highlight;
        int top, top_variation;
        int gloves_torso, gloves_torso_variation;
        int top2, top2_variation;
        int top3, top3_variation;
        int parachute_bag, parachute_bag_variation;
        int pants, pants_variation;
        int shoes, shoes_variation;
        int accessories, accessories_variation;
        int decals, decals_variation;
        int hat, hat_variation;
        int glasses, glasses_variation;
        int earwear, earwear_variation;
        int watch, watch_variation;
        int bracelet, bracelet_variation;

        StandOutfit() : model_name("Online Female"),
            head(0), head_variation(0),
            mask(0), mask_variation(0),
            hair(0), hair_colour(0), hair_colour_highlight(0),
            top(0), top_variation(0),
            gloves_torso(0), gloves_torso_variation(0),
            top2(0), top2_variation(0),
            top3(0), top3_variation(0),
            parachute_bag(0), parachute_bag_variation(0),
            pants(0), pants_variation(0),
            shoes(0), shoes_variation(0),
            accessories(-1), accessories_variation(0),
            decals(0), decals_variation(0),
            hat(-1), hat_variation(-1),
            glasses(-1), glasses_variation(-1),
            earwear(-1), earwear_variation(-1),
            watch(-1), watch_variation(0),
            bracelet(-1), bracelet_variation(0) {}
    };

    // ============== COMPONENT MAPPING ==============
    // GTA V component slot indices
    enum ComponentSlot {
        SLOT_HEAD = 0,
        SLOT_BEARD = 1,
        SLOT_HAIR = 2,
        SLOT_TORSO = 3,
        SLOT_LEGS = 4,
        SLOT_HANDS = 5,
        SLOT_FEET = 6,
        SLOT_TEETH = 7,
        SLOT_SPECIAL = 8,
        SLOT_SPECIAL2 = 9,
        SLOT_DECAL = 10,
        SLOT_JACKET = 11
    };

    // Prop slot indices
    enum PropSlot {
        PROP_HEAD = 0,
        PROP_EYES = 1,
        PROP_EARS = 2,
        PROP_MOUTH = 3,
        PROP_LEFT_HAND = 4,
        PROP_RIGHT_HAND = 5,
        PROP_LEFT_WRIST = 6,
        PROP_RIGHT_WRIST = 7,
        PROP_HIP = 8
    };

} // namespace OutfitConverter
