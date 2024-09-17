#include "DebugUtils.hpp"
#include "Utils/Exception.hpp"

namespace Vulkan
{
	DebugUtils::DebugUtils(VkInstance instance):
	vkSetDebugUtilsObjectNameExt_(reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(instance,"vkSetDebugUtilsObjectNameExt")))
	{
		/*
		 * vkGetInstanceProcAddr 获取vulkan instance中指定函数的地址
		 * PFN_vkSetDebugUtilsObjectNameEXT 是一种类型定义 pfn就代表这是一种函数指针类型
		 */
		if(vkSetDebugUtilsObjectNameExt_ == nullptr)
		{
			Throw(std::runtime_error("failed to get address of 'vkSetDebugUtilsObjectNameExt"));
		}
	}

}
