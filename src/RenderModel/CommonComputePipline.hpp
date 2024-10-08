#pragma once
#include <memory>
#include <vector>

#include "Vulkan/ImageView.hpp"
#include "Vulkan/Vulkan.hpp"

namespace Assets
{
	class Scene;
	class UniformBuffer;
}

namespace Vulkan
{
	class DepthBuffer;
	class PipelineLayout;
	class RenderPass; // todo 理解一下这里的设计
	class SwapChain;
	class DescriptorSetManager;
	class DeviceProcedures;
	class Buffer;
}

namespace Vulkan::PipelineCommon
{
	class FinalComposePipline final
	{
	public:
		VULKAN_NON_COPIABLE(FinalComposePipline)

		FinalComposePipline(const SwapChain& swapChain, const ImageView& sourceImageView,
		                    const std::vector<Assets::UniformBuffer>& uniformBuffers);
		~FinalComposePipline();

		VkDescriptorSet DescriptorSet(uint32_t imageIndex) const;
		const PipelineLayout& PipelineLayout() const { return *pipelineLayout_; }

	private:
		const SwapChain& swapChain_;
		VULKAN_HANDLE(VkPipeline, pipeline_)

		std::unique_ptr<DescriptorSetManager> descriptorSetManager_;
		std::unique_ptr<Vulkan::PipelineLayout> pipelineLayout_;
	};

	class BufferClearPipeline final
	{
	public:
		VULKAN_NON_COPIABLE(BufferClearPipeline)

		BufferClearPipeline(
			const SwapChain& swapChain);
		~BufferClearPipeline();

		VkDescriptorSet DescriptorSet(uint32_t index) const;
		const PipelineLayout& PipelineLayout() const { return *pipelineLayout_; }

	private:
		const SwapChain& swapChain_;
		VULKAN_HANDLE(VkPipeline, pipeline_)
		std::unique_ptr<DescriptorSetManager> descriptorSetManager_;
		std::unique_ptr<Vulkan::PipelineLayout> pipelineLayout_;
	};

	class VisualDebuggerPipeline final
	{
	public:
		VULKAN_NON_COPIABLE(VisualDebuggerPipeline)

		VisualDebuggerPipeline(
			const SwapChain& swapChain,
			const ImageView& debugImage1View,
			const ImageView& debugImage2View,
			const ImageView& debugImage3View,
			const ImageView& debugImage4View,
			const std::vector<Assets::UniformBuffer>& uniformBuffers);
		~VisualDebuggerPipeline();

		VkDescriptorSet DescriptorSet(uint32_t index) const;
		const PipelineLayout& PipelineLayout() const { return *pipelineLayout_; }

	private:
		const SwapChain& swapChain_;
		VULKAN_HANDLE(VkPipeline, pipeline_)

		std::unique_ptr<DescriptorSetManager> descriptorSetManager_;
		std::unique_ptr<Vulkan::PipelineLayout> pipelineLayout_;
	};
}
