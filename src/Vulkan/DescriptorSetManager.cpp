#include "DescriptorSetManager.hpp"
#include "DescriptorPool.hpp"
#include "DescriptorSetLayout.hpp"
#include "DescriptorSets.hpp"
#include "Device.hpp"
#include "Utils/Exception.hpp"
#include <set>

namespace Vulkan {

DescriptorSetManager::DescriptorSetManager(const Device& device, const std::vector<DescriptorBinding>& descriptorBindings, const size_t maxSets)
{
	// Sanity check to avoid binding different resources to the same binding point.
	std::map<uint32_t, VkDescriptorType> bindingTypes;

	for (const auto& binding : descriptorBindings)
	{
		if (!bindingTypes.insert(std::make_pair(binding.Binding, binding.Type)).second)
		{
			Throw(std::invalid_argument("binding collision"));
		}
	}

	descriptorPool_.reset(new DescriptorPool(device, descriptorBindings, maxSets));
	descriptorSetLayout_.reset(new class DescriptorSetLayout(device, descriptorBindings));
	descriptorSets_.reset(new class DescriptorSets(*descriptorPool_, *descriptorSetLayout_, bindingTypes, maxSets));
}

DescriptorSetManager::~DescriptorSetManager()
{
	/*
	 * 梳理了一下这几个类之间的关系
	* VkDescriptorPool：用于分配描述符集。 应用程序可以从中分配和释放descriptor set
	* VkDescriptorSetLayout：定义描述符集的布局。 descriptor 中的bingding point 应当包含什么类型的描述符
	* VkDescriptorSet：实际的描述符集，包含绑定的资源。 绑定资源到着色器 由descriptor set layout 定义
	* VkWriteDescriptorSet：用于更新描述符集中的描述符 更新descriptorpool 中的 descriptorset
	 */
	descriptorSets_.reset();
	descriptorSetLayout_.reset();
	descriptorPool_.reset();
}

}
