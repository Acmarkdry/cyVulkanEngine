#pragma once
#include <complex.h>
#include <complex.h>
#include <filesystem>
#include <fstream>
#include <random>

namespace utilities
{
	namespace FileHelper
	{
		static std::string GetPlatformFilePath(const char* srcPath)
		{
#if ANDROID
            return std::string("/sdcard/Android/data/com.gknextrenderer/files/") + srcPath;
#else
			return std::string("../") + srcPath;
#endif
		}

		static std::string GetNormalizeFilePath(const char* srcPath)
		{
			std::string normalizePath {};
#if ANDROID
			normlizedPath = std::string("/sdcard/Android/data/com.gknextrenderer/files/") + srcPath;
#else
			normalizePath = std::string("../") + srcPath;
#endif

			std::filesystem::path fullPath(normalizePath);
			std::filesystem::path directory = fullPath.parent_path();
			std::string pattern = fullPath.filename().string();

			for(const auto& entry:std::filesystem::directory_iterator(directory))
			{
				if(entry.is_regular_file() && entry.path().filename().string() == pattern)
				{
					normalizePath = std::filesystem::absolute(entry.path()).string();
					break;
				}
			}
			return normalizePath;
		}
		
	}

	namespace NameHelper
	{
		static std::string RandomName(size_t length)
		{
			/*随机取名*/
			const std::string character = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			std::random_device rd;
			std::mt19937 generator(rd());
			std::uniform_int_distribution<> distribution(0,static_cast<int>(character.size()) - 1);

			std::string randomName;
			for(size_t i = 0;i < length;i++)
			{
				randomName += character[distribution(generator)];
			}

			return randomName;
		}
		
	}
}
