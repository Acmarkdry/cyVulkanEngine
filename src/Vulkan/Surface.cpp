#include "Surface.hpp"

namespace Vulkan
{
	Surface::Surface(const Vulkan::Instance& instance)
	{
		Check(glfwCreateWindowSurface(instance.Handle(), instance.Window().Handle(), nullptr, &surface_),
	"create window surface");
	}

	Surface::~Surface()
	{
		if (surface_ != nullptr)
		{
			vkDestroySurfaceKHR(instance_.Handle(), surface_, nullptr);
			surface_ = nullptr;
		}
	}
}
