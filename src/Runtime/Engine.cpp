#include "Engine.hpp"
#define BUILDVER(x) std::string buildver(#x)
#include "build.version"

ENGINE_API Options* GOption = nullptr;

namespace  NextRenderer
{
	std::string GetBuildVersion()
	{
		return buildver;
	}

	Vulkan::VulkanBaseRenderer* CreateRenderer(uint32_t rendererType, Vulkan::Window* window, const VkPresentModeKHR presentMode, const bool enableValidationLayers)
	{
		switch (rendererType)
		{
		case 0:
		case 1:
		case 2:
		case 3:
			{
				
			}
			default: break;
		}

		auto fallbackptr = new Vulkan::VulkanBaseRenderer(window, presentMode, enableValidationLayers);
		fallbackptr->registerlogic
	}

}
