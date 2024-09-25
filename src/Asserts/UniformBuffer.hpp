#pragma once
#include <glm/vec4.hpp>

namespace Vulkan
{
	class Buffer;
	class Device;
	class DeviceMemory;
}

namespace Assets
{
	class alignas(16) UniformBufferObject
	{
		public:
	};

	struct alignas(16) LightObject final
{
	glm::vec4 p0;
	glm::vec4 p1;
	glm::vec4 p3;
	glm::vec4 normal_area;
};
	
}
