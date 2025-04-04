#pragma once
#include <random>
#include <filesystem>
#include <string>
#include <map>
#include <fstream>
#include <fmt/printf.h>
#include "ThirdParty/lzav/lzav.h"
#include <assert.h>
#include <regex>

namespace Utilities
{
    namespace FileHelper
    {
        static void EnsureDirectoryExists(const std::filesystem::path& path)
        {
            std::filesystem::create_directories(path);
        }
        
        static std::filesystem::path GetAbsolutePath( const std::filesystem::path& srcPath )
        {
            return std::filesystem::absolute(srcPath);
        }
        
        static std::string GetPlatformFilePath( const char* srcPath )
        {
#if ANDROID
            return std::string("/sdcard/Android/data/com.gknextrenderer/files/") + srcPath;
#else
            return std::string("../") + srcPath;
#endif
        }

        static std::string GetNormalizedFilePath( const char* srcPath )
        {
            std::string normlizedPath {};
#if ANDROID
            normlizedPath = std::string("/sdcard/Android/data/com.gknextrenderer/files/") + srcPath;
#else
            normlizedPath = std::string("../") + srcPath;
#endif
            std::filesystem::path fullPath(normlizedPath);
            std::filesystem::path directory = fullPath.parent_path();
            std::string pattern = fullPath.filename().string();

            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file() && entry.path().filename().string() == pattern) {
                    normlizedPath =  std::filesystem::absolute(entry.path()).string();
                    break;
                }
            }

            return normlizedPath;
        }
    }

    namespace NameHelper
    {
        static std::string RandomName(size_t length)
        {
            const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
            std::random_device rd;
            std::mt19937 generator(rd());
            std::uniform_int_distribution<> distribution(0, static_cast<int>(characters.size()) - 1);

            std::string randomName;
            for (size_t i = 0; i < length; ++i) {
                randomName += characters[distribution(generator)];
            }

            return randomName;
        }
    }

    namespace Package
    {
        enum EPackageRunMode
        {
            EPM_OsFile,
            EPM_PakFile
        };
        
        struct FPakEntry
        {
            std::string name;
            uint32_t pkgIdx;
            uint32_t offset;
            uint32_t size;
            uint32_t uncompressSize;
        };
        
        // PackageFileSystem for Mostly User Oriented Resource, like Texture, Model, etc.
        // Package mass files to one pak
        class FPackageFileSystem
        {
        public:
            // Construct
            FPackageFileSystem(EPackageRunMode RunMode);

            void SetRunMode(EPackageRunMode RunMode) { runMode_ = RunMode; }
            
            // Loading
            void Reset();
            void MountPak(const std::string& pakFile);
            bool LoadFile(const std::string& entry, std::vector<uint8_t>& outData);
            
            // Recording
            //void RecordUsage(const std::string& entry);
            //void SaveRecord(const std::string& recordFile);
            //void PakFromRecord(const std::string& pakFile, const std::string& recordFile);
            
            // Paking
            void PakAll(const std::string& pakFile, const std::string& srcDir, const std::string& rootPath, const std::string& regex = "");

            static FPackageFileSystem& GetInstance()
            {
                return *instance_;
            }
        private:
            // pak index
            std::map<std::string, FPakEntry> filemaps;
            std::vector<std::string> mountedPaks;
            EPackageRunMode runMode_;

            static FPackageFileSystem* instance_;
        };
    }
}
