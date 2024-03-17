#pragma once

#include <cstdio>
#include <string>
#include <utility>
#include <vector>
#include "IAssetLoader.hpp"
#include "Buffer.hpp"

namespace My {
	class AssetLoader : public IAssetLoader {
    public:
        ~AssetLoader() override = default;

        int Initialize() override;
        void Finalize() override;

        void Tick() override;

        bool AddSearchPath(const char *path);

        bool RemoveSearchPath(const char *path);

        bool FileExists(const char *filePath);

        AssetFilePtr OpenFile(const char* name, AssetOpenMode mode) override;

        Buffer SyncOpenAndReadText(const char *filePath) override;

        Buffer SyncOpenAndReadBinary(const char *filePath) override;

        size_t SyncRead(const AssetFilePtr& fp, Buffer& buf);

        void CloseFile(AssetFilePtr& fp);

        size_t GetSize(const AssetFilePtr& fp);

        int32_t Seek(AssetFilePtr fp, long offset, AssetSeekBase where);

        inline std::string SyncOpenAndReadTextFileToString(const char* fileName) override
        {
            std::string result;
            Buffer buffer = SyncOpenAndReadText(fileName);
            char* content = reinterpret_cast<char*>(buffer.m_pData);

            if (content)
            {
                result = std::string(std::move(content));
            }

            return result;
        }
    private:
        std::vector<std::string> m_strSearchPath;
    public:
        std::string m_AssetPath;
	};
}
