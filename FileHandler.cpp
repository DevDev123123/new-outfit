#include "FileHandler.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace OutfitConverter {

    // ============== JSON BUILDER IMPLEMENTATION ==============
    JsonBuilder::JsonBuilder() : indentLevel(0), needsComma(false) {}

    std::string JsonBuilder::GetIndent() const {
        return std::string(indentLevel * 4, ' ');
    }

    void JsonBuilder::AddCommaIfNeeded() {
        if (needsComma) {
            json += ",\n";
        }
        needsComma = false;
    }

    void JsonBuilder::StartObject() {
        AddCommaIfNeeded();
        json += GetIndent() + "{\n";
        indentLevel++;
        needsComma = false;
    }

    void JsonBuilder::EndObject() {
        json += "\n";
        indentLevel--;
        json += GetIndent() + "}";
        needsComma = true;
    }

    void JsonBuilder::StartObjectField(const std::string& name) {
        AddCommaIfNeeded();
        json += GetIndent() + "\"" + name + "\": {\n";
        indentLevel++;
        needsComma = false;
    }

    void JsonBuilder::EndObjectField() {
        json += "\n";
        indentLevel--;
        json += GetIndent() + "}";
        needsComma = true;
    }

    void JsonBuilder::StartArray() {
        AddCommaIfNeeded();
        json += GetIndent() + "[\n";
        indentLevel++;
        needsComma = false;
    }

    void JsonBuilder::EndArray() {
        json += "\n";
        indentLevel--;
        json += GetIndent() + "]";
        needsComma = true;
    }

    void JsonBuilder::StartArrayField(const std::string& name) {
        AddCommaIfNeeded();
        json += GetIndent() + "\"" + name + "\": [\n";
        indentLevel++;
        needsComma = false;
    }

    void JsonBuilder::EndArrayField() {
        json += "\n";
        indentLevel--;
        json += GetIndent() + "]";
        needsComma = true;
    }

    void JsonBuilder::AddField(const std::string& name, int value) {
        AddCommaIfNeeded();
        json += GetIndent() + "\"" + name + "\": " + std::to_string(value);
        needsComma = true;
    }

    void JsonBuilder::AddField(const std::string& name, float value) {
        AddCommaIfNeeded();
        std::ostringstream oss;
        oss << value;
        json += GetIndent() + "\"" + name + "\": " + oss.str();
        needsComma = true;
    }

    void JsonBuilder::AddField(const std::string& name, const std::string& value) {
        AddCommaIfNeeded();
        json += GetIndent() + "\"" + name + "\": \"" + value + "\"";
        needsComma = true;
    }

    void JsonBuilder::AddField(const std::string& name, uint32_t value) {
        AddCommaIfNeeded();
        json += GetIndent() + "\"" + name + "\": " + std::to_string(value);
        needsComma = true;
    }

    void JsonBuilder::AddArrayElement(int value) {
        AddCommaIfNeeded();
        json += GetIndent() + std::to_string(value);
        needsComma = true;
    }

    void JsonBuilder::AddArrayElement(float value) {
        AddCommaIfNeeded();
        std::ostringstream oss;
        oss << value;
        json += GetIndent() + oss.str();
        needsComma = true;
    }

    void JsonBuilder::AddArrayElement(const std::string& value) {
        AddCommaIfNeeded();
        json += GetIndent() + "\"" + value + "\"";
        needsComma = true;
    }

    void JsonBuilder::Clear() {
        json.clear();
        indentLevel = 0;
        needsComma = false;
    }

    // ============== JSON PARSER IMPLEMENTATION ==============
    JsonParser::JsonParser(const std::string& jsonContent) : json(jsonContent), position(0) {}

    char JsonParser::PeekChar() {
        if (position >= json.length()) return '\0';
        return json[position];
    }

    char JsonParser::GetChar() {
        if (position >= json.length()) return '\0';
        return json[position++];
    }

    void JsonParser::SkipWhitespace() {
        while (position < json.length() && std::isspace(json[position])) {
            position++;
        }
    }

    std::string JsonParser::ParseString() {
        SkipWhitespace();
        if (GetChar() != '"') return "";

        std::string result;
        char ch;
        while ((ch = GetChar()) != '"' && ch != '\0') {
            if (ch == '\\') {
                ch = GetChar();
                if (ch == 'n') result += '\n';
                else if (ch == 't') result += '\t';
                else if (ch == 'r') result += '\r';
                else result += ch;
            } else {
                result += ch;
            }
        }
        return result;
    }

    int JsonParser::ParseInt() {
        SkipWhitespace();
        std::string numStr;
        char ch = PeekChar();
        
        if (ch == '-') {
            numStr += GetChar();
        }

        while (std::isdigit(PeekChar())) {
            numStr += GetChar();
        }

        return numStr.empty() ? 0 : std::stoi(numStr);
    }

    float JsonParser::ParseFloat() {
        SkipWhitespace();
        std::string numStr;
        char ch = PeekChar();
        
        if (ch == '-') {
            numStr += GetChar();
        }

        while (std::isdigit(PeekChar()) || PeekChar() == '.') {
            numStr += GetChar();
        }

        return numStr.empty() ? 0.0f : std::stof(numStr);
    }

    bool JsonParser::FindKey(const std::string& key) {
        size_t startPos = position;
        std::string searchStr = "\"" + key + "\"";
        
        size_t found = json.find(searchStr, position);
        if (found != std::string::npos) {
            position = found + searchStr.length();
            SkipWhitespace();
            if (PeekChar() == ':') {
                GetChar();
                SkipWhitespace();
                return true;
            }
        }
        
        position = startPos;
        return false;
    }

    std::string JsonParser::GetString(const std::string& key) {
        size_t oldPos = position;
        if (FindKey(key)) {
            return ParseString();
        }
        position = oldPos;
        return "";
    }

    int JsonParser::GetInt(const std::string& key) {
        size_t oldPos = position;
        if (FindKey(key)) {
            return ParseInt();
        }
        position = oldPos;
        return 0;
    }

    float JsonParser::GetFloat(const std::string& key) {
        size_t oldPos = position;
        if (FindKey(key)) {
            return ParseFloat();
        }
        position = oldPos;
        return 0.0f;
    }

    uint32_t JsonParser::GetUInt32(const std::string& key) {
        size_t oldPos = position;
        if (FindKey(key)) {
            SkipWhitespace();
            std::string numStr;
            while (std::isdigit(PeekChar())) {
                numStr += GetChar();
            }
            return numStr.empty() ? 0 : std::stoul(numStr);
        }
        position = oldPos;
        return 0;
    }

    std::vector<int> JsonParser::GetIntArray(const std::string& key) {
        std::vector<int> result;
        size_t oldPos = position;
        
        if (FindKey(key)) {
            SkipWhitespace();
            if (GetChar() == '[') {
                SkipWhitespace();
                while (PeekChar() != ']' && PeekChar() != '\0') {
                    result.push_back(ParseInt());
                    SkipWhitespace();
                    if (PeekChar() == ',') GetChar();
                    SkipWhitespace();
                }
                if (PeekChar() == ']') GetChar();
            }
        }
        
        if (result.empty()) position = oldPos;
        return result;
    }

    // ============== CHERAX FILE OPERATIONS ==============
    bool FileHandler::LoadCheraxOutfit(const std::string& filepath, CheraxOutfit& outfit) {
        std::string content = ReadFileContent(filepath);
        if (content.empty()) return false;

        JsonParser parser(content);
        
        outfit.format = parser.GetString("format");
        outfit.type = parser.GetInt("type");
        outfit.model = parser.GetUInt32("model");
        outfit.baseFlags = parser.GetUInt32("baseFlags");

        // Parse components
        size_t compPos = content.find("\"components\"");
        if (compPos != std::string::npos) {
            JsonParser compParser(content.substr(compPos));
            
            const std::vector<std::string> componentNames = {
                "Head", "Beard", "Hair", "Torso", "Legs", "Hands", "Feet",
                "Teeth", "Special", "Special 2", "Decal", "Tuxedo/Jacket Bib"
            };

            for (const auto& name : componentNames) {
                compParser.Reset();
                if (compParser.FindKey(name)) {
                    Component comp;
                    comp.drawable = compParser.GetInt("drawable");
                    comp.texture = compParser.GetInt("texture");
                    comp.palette = compParser.GetInt("palette");
                    outfit.components[name] = comp;
                }
            }
        }

        // Parse props
        size_t propPos = content.find("\"props\"");
        if (propPos != std::string::npos) {
            JsonParser propParser(content.substr(propPos));
            
            const std::vector<std::string> propNames = {
                "Hat", "Glasses", "Earwear", "Watch", "Bracelet"
            };

            for (const auto& name : propNames) {
                propParser.Reset();
                if (propParser.FindKey(name)) {
                    Prop prop;
                    prop.drawable = propParser.GetInt("drawable");
                    prop.texture = propParser.GetInt("texture");
                    outfit.props[name] = prop;
                }
            }
        }

        return true;
    }

    bool FileHandler::SaveCheraxOutfit(const std::string& filepath, const CheraxOutfit& outfit) {
        JsonBuilder builder;
        
        builder.StartObject();
        builder.AddField("format", outfit.format);
        builder.AddField("type", outfit.type);
        builder.AddField("model", outfit.model);
        builder.AddField("baseFlags", outfit.baseFlags);

        // Components
        builder.StartObjectField("components");
        for (const auto& pair : outfit.components) {
            builder.StartObjectField(pair.first);
            builder.AddField("drawable", pair.second.drawable);
            builder.AddField("texture", pair.second.texture);
            builder.AddField("palette", pair.second.palette);
            builder.EndObjectField();
        }
        builder.EndObjectField();

        // Props
        builder.StartObjectField("props");
        for (const auto& pair : outfit.props) {
            builder.StartObjectField(pair.first);
            builder.AddField("drawable", pair.second.drawable);
            builder.AddField("texture", pair.second.texture);
            builder.EndObjectField();
        }
        builder.EndObjectField();

        builder.EndObject();

        return WriteFileContent(filepath, builder.GetJson());
    }

    // ============== UTILITY FUNCTIONS ==============
    bool FileHandler::FileExists(const std::string& filepath) {
        std::ifstream file(filepath);
        return file.good();
    }

    std::string FileHandler::ReadFileContent(const std::string& filepath) {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) return "";

        return std::string((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
    }

    bool FileHandler::WriteFileContent(const std::string& filepath, const std::string& content) {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) return false;

        file << content;
        return file.good();
    }
// ============== YIM FILE OPERATIONS ==============
bool FileHandler::LoadYimOutfit(const std::string& filepath, YimOutfit& outfit) {
    std::string content = ReadFileContent(filepath);
    if (content.empty()) return false;

    JsonParser parser(content);

    // Parse blend data
    if (parser.FindKey("blend_data")) {
        outfit.blend_data.is_parent = parser.GetInt("is_parent");
        outfit.blend_data.shape_first_id = parser.GetInt("shape_first_id");
        outfit.blend_data.shape_mix = parser.GetFloat("shape_mix");
        outfit.blend_data.shape_second_id = parser.GetInt("shape_second_id");
        outfit.blend_data.shape_third_id = parser.GetInt("shape_third_id");
        outfit.blend_data.skin_first_id = parser.GetInt("skin_first_id");
        outfit.blend_data.skin_mix = parser.GetFloat("skin_mix");
        outfit.blend_data.skin_second_id = parser.GetInt("skin_second_id");
        outfit.blend_data.skin_third_id = parser.GetInt("skin_third_id");
        outfit.blend_data.third_mix = parser.GetFloat("third_mix");
    }

    // Parse components
    size_t compPos = content.find("\"components\"");
    if (compPos != std::string::npos) {
        for (int i = 0; i < 12; i++) {
            JsonParser compParser(content.substr(compPos));
            if (compParser.FindKey(std::to_string(i))) {
                Component comp;
                comp.drawable = compParser.GetInt("drawable_id");
                comp.texture = compParser.GetInt("texture_id");
                comp.palette = 0;
                outfit.components[i] = comp;
            }
        }
    }

    // Parse props
    size_t propPos = content.find("\"props\"");
    if (propPos != std::string::npos) {
        for (int i = 0; i < 9; i++) {
            JsonParser propParser(content.substr(propPos));
            if (propParser.FindKey(std::to_string(i))) {
                Prop prop;
                prop.drawable = propParser.GetInt("drawable_id");
                prop.texture = propParser.GetInt("texture_id");
                outfit.props[i] = prop;
            }
        }
    }

    // Try to get model from file or use default
    parser.Reset();
    outfit.model = parser.GetUInt32("model");
    if (outfit.model == 0) {
        outfit.model = 1885233650; // Default male model
    }

    return true;
}

bool FileHandler::SaveYimOutfit(const std::string& filepath, const YimOutfit& outfit) {
    JsonBuilder builder;
    
    builder.StartObject();

    // Blend data
    builder.StartObjectField("blend_data");
    builder.AddField("is_parent", outfit.blend_data.is_parent);
    builder.AddField("shape_first_id", outfit.blend_data.shape_first_id);
    builder.AddField("shape_mix", outfit.blend_data.shape_mix);
    builder.AddField("shape_second_id", outfit.blend_data.shape_second_id);
    builder.AddField("shape_third_id", outfit.blend_data.shape_third_id);
    builder.AddField("skin_first_id", outfit.blend_data.skin_first_id);
    builder.AddField("skin_mix", outfit.blend_data.skin_mix);
    builder.AddField("skin_second_id", outfit.blend_data.skin_second_id);
    builder.AddField("skin_third_id", outfit.blend_data.skin_third_id);
    builder.AddField("third_mix", outfit.blend_data.third_mix);
    builder.EndObjectField();

    // Components
    builder.StartObjectField("components");
    for (const auto& pair : outfit.components) {
        builder.StartObjectField(std::to_string(pair.first));
        builder.AddField("drawable_id", pair.second.drawable);
        builder.AddField("texture_id", pair.second.texture);
        builder.EndObjectField();
    }
    builder.EndObjectField();

    // Props
    builder.StartObjectField("props");
    for (const auto& pair : outfit.props) {
        builder.StartObjectField(std::to_string(pair.first));
        builder.AddField("drawable_id", pair.second.drawable);
        builder.AddField("texture_id", pair.second.texture);
        builder.EndObjectField();
    }
    builder.EndObjectField();

    builder.EndObject();

    return WriteFileContent(filepath, builder.GetJson());
}

// ============== LEXIS FILE OPERATIONS ==============
bool FileHandler::LoadLexisOutfit(const std::string& filepath, LexisOutfit& outfit) {
    std::string content = ReadFileContent(filepath);
    if (content.empty()) return false;

    JsonParser parser(content);

    outfit.model = parser.GetUInt32("model");
    outfit.component = parser.GetIntArray("component");
    outfit.component_variation = parser.GetIntArray("component variation");
    outfit.prop = parser.GetIntArray("prop");
    outfit.prop_variation = parser.GetIntArray("prop variation");

    // Ensure proper sizes
    if (outfit.component.size() < 12) outfit.component.resize(12, 0);
    if (outfit.component_variation.size() < 12) outfit.component_variation.resize(12, 0);
    if (outfit.prop.size() < 9) outfit.prop.resize(9, -1);
    if (outfit.prop_variation.size() < 9) outfit.prop_variation.resize(9, -1);

    return true;
}

bool FileHandler::SaveLexisOutfit(const std::string& filepath, const LexisOutfit& outfit) {
    JsonBuilder builder;
    
    builder.StartObject();
    builder.StartObjectField("outfit");

    // Components array
    builder.StartArrayField("component");
    for (int val : outfit.component) {
        builder.AddArrayElement(val);
    }
    builder.EndArrayField();

    // Component variations array
    builder.StartArrayField("component variation");
    for (int val : outfit.component_variation) {
        builder.AddArrayElement(val);
    }
    builder.EndArrayField();

    // Model
    builder.AddField("model", outfit.model);

    // Props array
    builder.StartArrayField("prop");
    for (int val : outfit.prop) {
        builder.AddArrayElement(val);
    }
    builder.EndArrayField();

    // Prop variations array
    builder.StartArrayField("prop variation");
    for (int val : outfit.prop_variation) {
        builder.AddArrayElement(val);
    }
    builder.EndArrayField();

    builder.EndObjectField();
    builder.EndObject();

    return WriteFileContent(filepath, builder.GetJson());
}

// ============== STAND FILE OPERATIONS ==============
bool FileHandler::LoadStandOutfit(const std::string& filepath, StandOutfit& outfit) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        std::string key = TrimWhitespace(line.substr(0, colonPos));
        std::string value = TrimWhitespace(line.substr(colonPos + 1));

        if (key == "Model") outfit.model_name = value;
        else if (key == "Head") outfit.head = ParseStandInt(value);
        else if (key == "Head Variation") outfit.head_variation = ParseStandInt(value);
        else if (key == "Mask") outfit.mask = ParseStandInt(value);
        else if (key == "Mask Variation") outfit.mask_variation = ParseStandInt(value);
        else if (key == "Hair") outfit.hair = ParseStandInt(value);
        else if (key == "Hair Colour") outfit.hair_colour = ParseStandInt(value);
        else if (key == "Hair Colour\\: Highlight") outfit.hair_colour_highlight = ParseStandInt(value);
        else if (key == "Top") outfit.top = ParseStandInt(value);
        else if (key == "Top Variation") outfit.top_variation = ParseStandInt(value);
        else if (key == "Gloves / Torso") outfit.gloves_torso = ParseStandInt(value);
        else if (key == "Gloves / Torso Variation") outfit.gloves_torso_variation = ParseStandInt(value);
        else if (key == "Top 2") outfit.top2 = ParseStandInt(value);
        else if (key == "Top 2 Variation") outfit.top2_variation = ParseStandInt(value);
        else if (key == "Top 3") outfit.top3 = ParseStandInt(value);
        else if (key == "Top 3 Variation") outfit.top3_variation = ParseStandInt(value);
        else if (key == "Parachute / Bag") outfit.parachute_bag = ParseStandInt(value);
        else if (key == "Parachute / Bag Variation") outfit.parachute_bag_variation = ParseStandInt(value);
        else if (key == "Pants") outfit.pants = ParseStandInt(value);
        else if (key == "Pants Variation") outfit.pants_variation = ParseStandInt(value);
        else if (key == "Shoes") outfit.shoes = ParseStandInt(value);
        else if (key == "Shoes Variation") outfit.shoes_variation = ParseStandInt(value);
        else if (key == "Accessories") outfit.accessories = ParseStandInt(value);
        else if (key == "Accessories Variation") outfit.accessories_variation = ParseStandInt(value);
        else if (key == "Decals") outfit.decals = ParseStandInt(value);
        else if (key == "Decals Variation") outfit.decals_variation = ParseStandInt(value);
        else if (key == "Hat") outfit.hat = ParseStandInt(value);
        else if (key == "Hat Variation") outfit.hat_variation = ParseStandInt(value);
        else if (key == "Glasses") outfit.glasses = ParseStandInt(value);
        else if (key == "Glasses Variation") outfit.glasses_variation = ParseStandInt(value);
        else if (key == "Earwear") outfit.earwear = ParseStandInt(value);
        else if (key == "Earwear Variation") outfit.earwear_variation = ParseStandInt(value);
        else if (key == "Watch") outfit.watch = ParseStandInt(value);
        else if (key == "Watch Variation") outfit.watch_variation = ParseStandInt(value);
        else if (key == "Bracelet") outfit.bracelet = ParseStandInt(value);
        else if (key == "Bracelet Variation") outfit.bracelet_variation = ParseStandInt(value);
    }

    file.close();
    return true;
}

bool FileHandler::SaveStandOutfit(const std::string& filepath, const StandOutfit& outfit) {
    std::ostringstream oss;
    
    oss << "Model: " << outfit.model_name << "\n";
    oss << "Head: " << outfit.head << "\n";
    oss << "Head Variation: " << outfit.head_variation << "\n";
    oss << "Mask: " << outfit.mask << "\n";
    oss << "Mask Variation: " << outfit.mask_variation << "\n";
    oss << "Hair: " << outfit.hair << "\n";
    oss << "Hair Colour: " << outfit.hair_colour << "\n";
    oss << "Hair Colour\\: Highlight: " << outfit.hair_colour_highlight << "\n";
    oss << "Top: " << outfit.top << "\n";
    oss << "Top Variation: " << outfit.top_variation << "\n";
    oss << "Gloves / Torso: " << outfit.gloves_torso << "\n";
    oss << "Gloves / Torso Variation: " << outfit.gloves_torso_variation << "\n";
    oss << "Top 2: " << outfit.top2 << "\n";
    oss << "Top 2 Variation: " << outfit.top2_variation << "\n";
    oss << "Top 3: " << outfit.top3 << "\n";
    oss << "Top 3 Variation: " << outfit.top3_variation << "\n";
    oss << "Parachute / Bag: " << outfit.parachute_bag << "\n";
    oss << "Parachute / Bag Variation: " << outfit.parachute_bag_variation << "\n";
    oss << "Pants: " << outfit.pants << "\n";
    oss << "Pants Variation: " << outfit.pants_variation << "\n";
    oss << "Shoes: " << outfit.shoes << "\n";
    oss << "Shoes Variation: " << outfit.shoes_variation << "\n";
    oss << "Accessories: " << outfit.accessories << "\n";
    oss << "Accessories Variation: " << outfit.accessories_variation << "\n";
    oss << "Decals: " << outfit.decals << "\n";
    oss << "Decals Variation: " << outfit.decals_variation << "\n";
    oss << "Hat: " << outfit.hat << "\n";
    oss << "Hat Variation: " << outfit.hat_variation << "\n";
    oss << "Glasses: " << outfit.glasses << "\n";
    oss << "Glasses Variation: " << outfit.glasses_variation << "\n";
    oss << "Earwear: " << outfit.earwear << "\n";
    oss << "Earwear Variation: " << outfit.earwear_variation << "\n";
    oss << "Watch: " << outfit.watch << "\n";
    oss << "Watch Variation: " << outfit.watch_variation << "\n";
    oss << "Bracelet: " << outfit.bracelet << "\n";
    oss << "Bracelet Variation: " << outfit.bracelet_variation << "\n";

    return WriteFileContent(filepath, oss.str());
}

// ============== HELPER FUNCTIONS ==============
std::string FileHandler::TrimWhitespace(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

int FileHandler::ParseStandInt(const std::string& value) {
    try {
        return std::stoi(value);
    } catch (...) {
        return 0;
    }
}

} // namespace OutfitConverter

} // namespace OutfitConverter
