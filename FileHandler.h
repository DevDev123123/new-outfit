#pragma once
#include "OutfitStructures.h"
#include <string>
#include <fstream>
#include <memory>

namespace OutfitConverter {

    // ============== FILE HANDLER CLASS ==============
    class FileHandler {
    public:
        // ============== CHERAX FILE OPERATIONS ==============
        static bool LoadCheraxOutfit(const std::string& filepath, CheraxOutfit& outfit);
        static bool SaveCheraxOutfit(const std::string& filepath, const CheraxOutfit& outfit);

        // ============== YIM FILE OPERATIONS ==============
        static bool LoadYimOutfit(const std::string& filepath, YimOutfit& outfit);
        static bool SaveYimOutfit(const std::string& filepath, const YimOutfit& outfit);

        // ============== LEXIS FILE OPERATIONS ==============
        static bool LoadLexisOutfit(const std::string& filepath, LexisOutfit& outfit);
        static bool SaveLexisOutfit(const std::string& filepath, const LexisOutfit& outfit);

        // ============== STAND FILE OPERATIONS ==============
        static bool LoadStandOutfit(const std::string& filepath, StandOutfit& outfit);
        static bool SaveStandOutfit(const std::string& filepath, const StandOutfit& outfit);

        // ============== UTILITY FUNCTIONS ==============
        static bool FileExists(const std::string& filepath);
        static std::string GetFileExtension(const std::string& filepath);
        static std::string ReadFileContent(const std::string& filepath);
        static bool WriteFileContent(const std::string& filepath, const std::string& content);
        
        // JSON helper functions
        static std::string EscapeJsonString(const std::string& input);
        static std::string UnescapeJsonString(const std::string& input);
        
        // File validation
        static bool ValidateJsonFile(const std::string& filepath);
        static bool ValidateTextFile(const std::string& filepath);

    private:
        // JSON parsing helpers (simple implementation without external dependencies)
        static std::string TrimWhitespace(const std::string& str);
        static std::string GetJsonValue(const std::string& json, const std::string& key);
        static std::string GetJsonObject(const std::string& json, const std::string& key);
        static std::vector<std::string> GetJsonArray(const std::string& json, const std::string& key);
        
        // JSON building helpers
        static std::string BuildJsonNumber(int value);
        static std::string BuildJsonNumber(float value);
        static std::string BuildJsonString(const std::string& value);
        static std::string BuildJsonArray(const std::vector<int>& values);
        static std::string BuildJsonObject(const std::map<std::string, std::string>& pairs);

        // Stand format parsing
        static std::string GetStandValue(const std::string& line);
        static int ParseStandInt(const std::string& value);
    };

    // ============== JSON BUILDER UTILITY ==============
    class JsonBuilder {
    private:
        std::string json;
        int indentLevel;
        bool needsComma;

        std::string GetIndent() const;
        void AddCommaIfNeeded();

    public:
        JsonBuilder();

        // Object operations
        void StartObject();
        void EndObject();
        void StartObjectField(const std::string& name);
        void EndObjectField();

        // Array operations
        void StartArray();
        void EndArray();
        void StartArrayField(const std::string& name);
        void EndArrayField();

        // Value operations
        void AddField(const std::string& name, int value);
        void AddField(const std::string& name, float value);
        void AddField(const std::string& name, const std::string& value);
        void AddField(const std::string& name, uint32_t value);
        void AddArrayElement(int value);
        void AddArrayElement(float value);
        void AddArrayElement(const std::string& value);

        // Get result
        std::string GetJson() const { return json; }
        void Clear();
    };

    // ============== JSON PARSER UTILITY ==============
    class JsonParser {
    private:
        std::string json;
        size_t position;

        char PeekChar();
        char GetChar();
        void SkipWhitespace();
        std::string ParseString();
        int ParseInt();
        float ParseFloat();
        bool ParseBool();

    public:
        JsonParser(const std::string& jsonContent);

        bool FindKey(const std::string& key);
        bool EnterObject(const std::string& key);
        bool EnterArray(const std::string& key);
        void ExitScope();

        std::string GetString(const std::string& key);
        int GetInt(const std::string& key);
        float GetFloat(const std::string& key);
        bool GetBool(const std::string& key);
        uint32_t GetUInt32(const std::string& key);

        std::vector<int> GetIntArray(const std::string& key);
        std::vector<float> GetFloatArray(const std::string& key);

        bool IsValid() const { return !json.empty(); }
        void Reset() { position = 0; }
    };

} // namespace OutfitConverter
