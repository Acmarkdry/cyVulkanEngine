#pragma once

#include "Vulkan/vulkan.hpp"

#define CONSOLE_GREEN_COLOR		"\033[1;32m-"
#define CONSOLE_DEFAULT_COLOR	"\033[0m"

namespace Utilities
{
	/*一个用于控制台输出的实用工具类*/
	enum class Severity
	{
		Verbos = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
		Info = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
		Warning = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
		Error = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		Fatal = VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT
	};

	class Console final
	{
	public:
		template <class Action>
		static void Write(const Severity severity,const Action action)
		{
			const auto attribute = SetColorBySeverity(severity);
			action();
			SetColorByAttributes(attribute);
		}

		static int SetColorBySeverity(const Severity severity)
		{
#ifdef WIN32
			const HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE); // 获取句柄

			CONSOLE_SCREEN_BUFFER_INFO info = {};
			GetConsoleScreenBufferInfo(hConsole, &info); // 控制台信息

			switch (severity)
			{
			case Severity::Verbos:
				SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
				break;
			case Severity::Info:
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				break;
			case Severity::Warning:
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				break;
			case Severity::Error:
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
				break;
			case Severity::Fatal:
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				break;
			default:;
			}

			return info.wAttributes;
#else
			return 0;
#endif
		}
		static void SetColorByAttributes(int attributes)
		{
# ifdef WIN32
			const HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
			SetConsoleTextAttribute(hConsole,attributes);
#endif
		}
	};
}