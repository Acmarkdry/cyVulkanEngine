#include "VulkanBaseRenderer.hpp"
#include "Buffer.hpp"
#include "CommandPool.hpp"
#include "CommandBuffers.hpp"
#include "DebugUtilsMessenger.hpp"
#include "DepthBuffer.hpp"
#include "Device.hpp"
#include "Fence.hpp"
#include "FrameBuffer.hpp"
#include "GraphicsPipeline.hpp"
#include "Instance.hpp"
#include "PipelineLayout.hpp"
#include "RenderPass.hpp"
#include "Semaphore.hpp"
#include "Surface.hpp"
#include "SwapChain.hpp"
#include "Window.hpp"
#include "Assets/Model.hpp"
#include "Assets/Scene.hpp"
#include "Assets/UniformBuffer.hpp"
#include "Assets/Texture.hpp"
#include "Utilities/Exception.hpp"
#include "Utilities/Console.hpp"
#include <array>
#include <fmt/format.h>

#include "Enumerate.hpp"
#include "ImageMemoryBarrier.hpp"
#include "Options.hpp"
#include "RenderImage.hpp"
#include "SingleTimeCommands.hpp"
#include "Strings.hpp"
#include "Version.hpp"
// #include "HybridDeferred/HybridDeferredRenderer.hpp"
// #include "LegacyDeferred/LegacyDeferredRenderer.hpp"
// #include "ModernDeferred/ModernDeferredRenderer.hpp"
// #include "RayQuery/RayQueryRenderer.hpp"
#include "Vulkan/PipelineCommon/CommonComputePipeline.hpp"

namespace 
{
    void PrintVulkanSdkInformation()
    {
        fmt::print("Vulkan SDK Header Version: {}\n\n", VK_HEADER_VERSION);
    }

    void PrintVulkanInstanceInformation(const Vulkan::VulkanBaseRenderer& application, const bool benchmark)
    {
        if (benchmark)
        {
            return;
        }

        puts("Vulkan Instance Extensions:");

        for (const auto& extension : application.Extensions())
        {
            fmt::print("- {} ({})\n", extension.extensionName, to_string(Vulkan::Version(extension.specVersion)));
        }

        puts("");
    }

    void PrintVulkanLayersInformation(const Vulkan::VulkanBaseRenderer& application, const bool benchmark)
    {
        if (benchmark)
        {
            return;
        }

        puts("Vulkan Instance Layers:");

        for (const auto& layer : application.Layers())
        {
            fmt::print("- {} ({}) : {}\n", layer.layerName, to_string(Vulkan::Version(layer.specVersion)), layer.description);
        }

        puts("");
    }

    void PrintVulkanDevices(const Vulkan::VulkanBaseRenderer& application)
    {
        puts("Vulkan Devices:");

        for (const auto& device : application.PhysicalDevices())
        {
            VkPhysicalDeviceDriverProperties driverProp{};
            driverProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;

            VkPhysicalDeviceProperties2 deviceProp{};
            deviceProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
            deviceProp.pNext = &driverProp;
            vkGetPhysicalDeviceProperties2(device, &deviceProp);
            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceFeatures(device, &features);

            const auto& prop = deviceProp.properties;

            const Vulkan::Version vulkanVersion(prop.apiVersion);
            const Vulkan::Version driverVersion(prop.driverVersion, prop.vendorID);

            fmt::print("- [{}] {} '{}' ({}: vulkan {} driver {} {} - {})\n",
                       prop.deviceID, Vulkan::Strings::VendorId(prop.vendorID), prop.deviceName, Vulkan::Strings::DeviceType(prop.deviceType),
                       to_string(vulkanVersion), driverProp.driverName, driverProp.driverInfo, to_string(driverVersion));

            const auto extensions = Vulkan::GetEnumerateVector(device, static_cast<const char*>(nullptr), vkEnumerateDeviceExtensionProperties);
            const auto hasRayTracing = std::any_of(extensions.begin(), extensions.end(), [](const VkExtensionProperties& extension)
            {
                return strcmp(extension.extensionName, VK_KHR_RAY_QUERY_EXTENSION_NAME) == 0;
            });
        }

        puts("");
    }

    bool SupportRayQuery(const Vulkan::VulkanBaseRenderer& application)
    {
        bool SupportRayQuery = false;
        for (const auto& device : application.PhysicalDevices())
        {
            const auto extensions = Vulkan::GetEnumerateVector(device, static_cast<const char*>(nullptr), vkEnumerateDeviceExtensionProperties);
            const auto hasRayTracing = std::any_of(extensions.begin(), extensions.end(), [](const VkExtensionProperties& extension)
            {
                return strcmp(extension.extensionName, VK_KHR_RAY_QUERY_EXTENSION_NAME) == 0;
            });

            SupportRayQuery = SupportRayQuery | hasRayTracing;
        }
        return SupportRayQuery;
    }

    void PrintVulkanSwapChainInformation(const Vulkan::VulkanBaseRenderer& application)
    {
        const auto& swapChain = application.SwapChain();

        fmt::print("Swap Chain:\n- image count: {}\n- present mode: {}\n\n", swapChain.Images().size(), static_cast<int>(swapChain.PresentMode()));
    }

    void SetVulkanDevice(Vulkan::VulkanBaseRenderer& application, uint32_t gpuIdx)
    {
        const auto& physicalDevices = application.PhysicalDevices();
        VkPhysicalDevice pDevice = physicalDevices[gpuIdx <= physicalDevices.size() ? gpuIdx : 0];
        VkPhysicalDeviceProperties2 deviceProp{};
        deviceProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        vkGetPhysicalDeviceProperties2(pDevice, &deviceProp);

        fmt::print("Setting Device [{}]\n", deviceProp.properties.deviceName);
        application.SetPhysicalDevice(pDevice);

        puts("");
    }
}

namespace Vulkan
{
    VulkanBaseRenderer::VulkanBaseRenderer(Vulkan::Window* window, VkPresentModeKHR presentMode, bool enableValidationLayers)
        :presentMode_(presentMode)
    {
        const auto validationLayers = enableValidationLayers ? std::vector<const char*>{"VK_LAYER_KHRONOS_validation"} : std::vector<const char*> {};

        window_ = window;
        instance_.reset(new Instance(*window_, validationLayers, VK_API_VERSION_1_2));
        debugUtilsMessenger_.reset(enableValidationLayers ? new DebugUtilsMessenger(*instance_, VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) : nullptr);
        surface_.reset(new Surface(*instance_));
        supportDenoiser_ = false;
        forceSDR_ = GOption->ForceSDR;

        uptime = std::chrono::high_resolution_clock::now().time_since_epoch().count();

        supportRayTracing_ = SupportRayQuery(*this);
    }

    VulkanGpuTimer::VulkanGpuTimer(VkDevice device, uint32_t totalCount, const VkPhysicalDeviceProperties& prop)
    {
        device_ = device;
        time_stamps.resize(totalCount);
        timeStampPeriod_ = prop.limits.timestampPeriod;
        // Create the query pool object used to get the GPU time tamps
        VkQueryPoolCreateInfo query_pool_info{};
        query_pool_info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        // We need to specify the query type for this pool, which in our case is for time stamps
        query_pool_info.queryType = VK_QUERY_TYPE_TIMESTAMP;
        // Set the no. of queries in this pool
        query_pool_info.queryCount = static_cast<uint32_t>(time_stamps.size());
        Check(vkCreateQueryPool(device, &query_pool_info, nullptr, &query_pool_timestamps), "create timestamp pool");
    }

    VulkanGpuTimer::~VulkanGpuTimer()
    {
        vkDestroyQueryPool(device_, query_pool_timestamps, nullptr);
    }

    VulkanBaseRenderer::~VulkanBaseRenderer()
    {
        VulkanBaseRenderer::DeleteSwapChain();

        rtEditorViewport_.reset();
        gpuTimer_.reset();
        globalTexturePool_.reset();
        commandPool_.reset();
        commandPool2_.reset();
        device_.reset();
        surface_.reset();
        debugUtilsMessenger_.reset();
        instance_.reset();
        window_ = nullptr;
    }

    const std::vector<VkExtensionProperties>& VulkanBaseRenderer::Extensions() const
    {
        return instance_->Extensions();
    }

    const std::vector<VkLayerProperties>& VulkanBaseRenderer::Layers() const
    {
        return instance_->Layers();
    }

    const std::vector<VkPhysicalDevice>& VulkanBaseRenderer::PhysicalDevices() const
    {
        return instance_->PhysicalDevices();
    }

    void VulkanBaseRenderer::SetPhysicalDevice(VkPhysicalDevice physicalDevice)
    {
        if (device_)
        {
            Throw(std::logic_error("physical device has already been set"));
        }

        std::vector<const char*> requiredExtensions =
        {
            // VK_KHR_swapchain
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#if __APPLE__
        "VK_KHR_portability_subset",
#endif
        };

        VkPhysicalDeviceFeatures deviceFeatures = {};

        deviceFeatures.multiDrawIndirect = true;
        deviceFeatures.drawIndirectFirstInstance = true;

        SetPhysicalDeviceImpl(physicalDevice, requiredExtensions, deviceFeatures, nullptr);
        globalTexturePool_.reset(new Assets::GlobalTexturePool())
    }
    


}
