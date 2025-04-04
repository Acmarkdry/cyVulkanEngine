#pragma once

#include "Vulkan/Vulkan.hpp"

#define CONSOLE_RED_COLOR		"\033[1;31m-"
#define CONSOLE_GREEN_COLOR		"\033[1;32m-"
#define CONSOLE_GOLD_COLOR		"\033[1;33m-"
#define CONSOLE_DEFAULT_COLOR	"\033[0m"

namespace Utilities
{
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
		static void Write(const Severity severity, const Action action)
		{
			const auto attributes = SetColorBySeverity(severity);
			action();
			SetColorByAttributes(attributes);
		}

		static int SetColorBySeverity(Severity severity) noexcept;
		static void SetColorByAttributes(int attributes) noexcept;
	}; 
}
