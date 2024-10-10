#include "CommonComputePipline.hpp"

#include "Assets/UniformBuffer.hpp"
#include "Utils/Console.hpp"
#include "Utils/FileHelper.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/DescriptorBinding.hpp"
#include "Vulkan/DescriptorSetManager.hpp"
#include "Vulkan/DescriptorSets.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/PipelineLayout.hpp"
#include "Vulkan/ShaderModule.hpp"
#include "Vulkan/SwapChain.hpp"

namespace Vulkan::PipelineCommon
{

	FinalComposePipline::FinalComposePipline(const SwapChain& swapChain, const ImageView& sourceImageView, const std::vector<Assets::UniformBuffer>& uniformBuffers):
	swapChain_(swapChain)
	{
		// todo 这里的代码需要理解一下
		const auto& device = swapChain_.Device();
		const std::vector<DescriptorBinding> descriptorBindings = {
			{0,1,VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,VK_SHADER_STAGE_COMPUTE_BIT},
			{1,1,VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,VK_SHADER_STAGE_COMPUTE_BIT},
			{2,1,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_COMPUTE_BIT}
		};

        descriptorSetManager_.reset(new DescriptorSetManager(device, descriptorBindings, swapChain.ImageViews().size()));
		auto & descriptorSets = descriptorSetManager_->DescriptorSets();

		for(uint32_t i = 0; i < swapChain_.Images().size(); i++)
		{
			VkDescriptorImageInfo info0 = {NULL,sourceImageView.Handle(),VK_IMAGE_LAYOUT_GENERAL};
			VkDescriptorImageInfo info1 = {NULL,swapChain_.ImageViews()[i]->Handle(),VK_IMAGE_LAYOUT_GENERAL};
			VkDescriptorBufferInfo uniformBufferInfo = {};
			uniformBufferInfo.buffer = uniformBuffers[i].Buffer().Handle();
			uniformBufferInfo.range = VK_WHOLE_SIZE;

			std::vector<VkWriteDescriptorSet> descriptorWrites = {
				descriptorSets.Bind(i,0,info0),
				descriptorSets.Bind(i,1,info1),
				descriptorSets.Bind(i,2,uniformBufferInfo),
			};
			descriptorSets.UpdateDescriptors(i,descriptorWrites);
		}
		pipelineLayout_.reset(new class PipelineLayout(device,descriptorSetManager_->DescriptorSetLayout()));
		const ShaderModule denoiseShader(device,Utilities::FileHelper::GetPlatformFilePath("assets/shaders/FinalCompose.comp.spv"));

		VkComputePipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stage = denoiseShader.CreateShaderStage(VK_SHADER_STAGE_COMPUTE_BIT);
		pipelineCreateInfo.layout = pipelineLayout_->Handle();

		Check(vkCreateComputePipelines(device.Handle(),VK_NULL_HANDLE,1,&pipelineCreateInfo,NULL,&pipeline_),
			"create deffered shading pipeline");
		
	}

	FinalComposePipline::~FinalComposePipline()
	{
		if(pipeline_ != nullptr)
		{
			vkDestroyPipeline(swapChain_.Device().Handle(),pipeline_,nullptr);
			pipeline_ = nullptr;
		}

		pipelineLayout_.reset();
		descriptorSetManager_.reset();
	}
    
    VkDescriptorSet FinalComposePipline::DescriptorSet(uint32_t index) const
    {
          return descriptorSetManager_->DescriptorSets().Handle(index);
    }
    
    BufferClearPipeline::BufferClearPipeline(const SwapChain& swapChain):swapChain_(swapChain)
    {
	    const auto& device = swapChain.Device();
		const std::vector<DescriptorBinding> descriptorBindings = {
			{0,1,VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,VK_SHADER_STAGE_COMPUTE_BIT}
		};

		descriptorSetManager_.reset(new DescriptorSetManager(device,descriptorBindings,swapChain.ImageViews().size()));
		auto& descriptorSets = descriptorSetManager_->DescriptorSets();

		for(uint32_t i = 0;i != swapChain_.Images().size();i++)
		{
			VkDescriptorImageInfo Info0 = {NULL,swapChain.ImageViews()[i]->Handle(),VK_IMAGE_LAYOUT_GENERAL};
			std::vector<VkWriteDescriptorSet> descriptorWrites = {
			descriptorSets.Bind(i,0,Info0),};
			descriptorSets.UpdateDescriptors(i,descriptorWrites);
		}

		pipelineLayout_.reset(new class PipelineLayout(device,descriptorSetManager_->DescriptorSetLayout()));
		const ShaderModule denoiseShader(device, Utilities::FileHelper::GetPlatformFilePath("assets/shaders/BufferClear.comp.spv"));

		VkComputePipelineCreateInfo pipelineCreatInfo = {};
		pipelineCreatInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineCreatInfo.stage = denoiseShader.CreateShaderStage(VK_SHADER_STAGE_COMPUTE_BIT);
		pipelineCreatInfo.layout = pipelineLayout_->Handle();

		Check(vkCreateComputePipelines(device.Handle(),VK_NULL_HANDLE,1,&pipelineCreatInfo,NULL,&pipeline_),"creat buffer clear pipeline");
    }

	BufferClearPipeline::~BufferClearPipeline()
	{
		if(pipeline_ != nullptr)
		{
			vkDestroyPipeline(swapChain_.Device().Handle(),pipeline_,nullptr);
			pipeline_ = nullptr;
		}

		pipelineLayout_.reset();
		descriptorSetManager_.reset();
	}

	VkDescriptorSet BufferClearPipeline::DescriptorSet(uint32_t index) const
	{
		return descriptorSetManager_->DescriptorSets().Handle(index);
	}

	VisualDebuggerPipeline::VisualDebuggerPipeline(const SwapChain& swapChain, const ImageView& debugImage1View, const ImageView& debugImage2View, const ImageView& debugImage3View, const ImageView& debugImage4View, const std::vector<Assets::UniformBuffer>& uniformBuffers):swapChain_(swapChain)
	{
        const auto& device = swapChain.Device();
        const std::vector<DescriptorBinding> descriptorBindings =
        {
            {0, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT},
            {1, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT},
            {2, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT},
            {3, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT},
            {4, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT},
        };

        descriptorSetManager_.reset(new DescriptorSetManager(device, descriptorBindings, swapChain.ImageViews().size()));

        auto& descriptorSets = descriptorSetManager_->DescriptorSets();

        for (uint32_t i = 0; i != swapChain.Images().size(); ++i)
        {
            
            VkDescriptorImageInfo Info0 = {NULL, swapChain.ImageViews()[i]->Handle(), VK_IMAGE_LAYOUT_GENERAL};
            VkDescriptorImageInfo Info1 = {NULL, debugImage1View.Handle(), VK_IMAGE_LAYOUT_GENERAL};
            VkDescriptorImageInfo Info2 = {NULL, debugImage2View.Handle(), VK_IMAGE_LAYOUT_GENERAL};
            VkDescriptorImageInfo Info3 = {NULL, debugImage3View.Handle(), VK_IMAGE_LAYOUT_GENERAL};
            VkDescriptorImageInfo Info4 = {NULL, debugImage4View.Handle(), VK_IMAGE_LAYOUT_GENERAL};
            
            VkDescriptorBufferInfo uniformBufferInfo = {};
            uniformBufferInfo.buffer = uniformBuffers[i].Buffer().Handle();
            uniformBufferInfo.range = VK_WHOLE_SIZE;

            std::vector<VkWriteDescriptorSet> descriptorWrites =
            {
                descriptorSets.Bind(i, 0, Info0),
                descriptorSets.Bind(i, 1, Info1),
                descriptorSets.Bind(i, 2, Info2),
                descriptorSets.Bind(i, 3, Info3),
                descriptorSets.Bind(i, 4, Info4),
            };

            descriptorSets.UpdateDescriptors(i, descriptorWrites);
        }

        pipelineLayout_.reset(new class PipelineLayout(device, descriptorSetManager_->DescriptorSetLayout()));
        const ShaderModule denoiseShader(device, Utilities::FileHelper::GetPlatformFilePath("assets/shaders/VisualDebugger.comp.spv"));

        VkComputePipelineCreateInfo pipelineCreateInfo = {};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.stage = denoiseShader.CreateShaderStage(VK_SHADER_STAGE_COMPUTE_BIT);
        pipelineCreateInfo.layout = pipelineLayout_->Handle();

        Check(vkCreateComputePipelines(device.Handle(), VK_NULL_HANDLE,
                                       1, &pipelineCreateInfo,
                                       NULL, &pipeline_),
              "create deferred shading pipeline");
	}

	VisualDebuggerPipeline::~VisualDebuggerPipeline()
	{
		if (pipeline_ != nullptr)
		{
			vkDestroyPipeline(swapChain_.Device().Handle(), pipeline_, nullptr);
			pipeline_ = nullptr;
		}

		pipelineLayout_.reset();
		descriptorSetManager_.reset();
	}

	VkDescriptorSet VisualDebuggerPipeline::DescriptorSet(uint32_t index) const
	{
		return descriptorSetManager_->DescriptorSets().Handle(index);
	}



	
}
