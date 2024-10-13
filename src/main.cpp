#include<algorithm>

#define BUILDVER(X) std::string buildver(#X)
#include <iostream>
#include <string>
#include <boost/program_options/option.hpp>

#include "Application.hpp"
#include "build.version" // todo 这个也要研究一下
#include "Options.hpp"
#include "SceneList.hpp"
#include "UserSettings.hpp"
#include "Assets/Scene.hpp"
#include "Utils/Console.hpp"
#include "Vulkan/Strings.hpp"
#include "Vulkan/SwapChain.hpp"
#include "Vulkan/Version.hpp"
#include "Vulkan/VulkanBaseRenderer.hpp"

class Options;

namespace cyNextRender
{
	std::string GetBuildVersion()
	{
		return buildver;
	}
}

namespace
{
	UserSettings CreateUserSettings(const Options& options);
	void PrintVulkanSdkInformation()
	{
		fmt::print("Vulkan SDK Header Version:{}\n\n",VK_HEADER_VERSION);
	}
	void PrintVulkanInstanceInformation(const Vulkan::VulkanBaseRenderer& application,bool benchmark)
	{
		if(benchmark)
		{
			return;
		}
		puts("Vulkan Instance Extensions:");

		for(const auto& extension:application.Extensions())
		{
			fmt::print("- {}({})\n",extension.extensionName,to_string(Vulkan::Version(extension.specVersion)));
		}

		puts("");
	}
	
	void PrintVulkanLayersInformation(const Vulkan::VulkanBaseRenderer& application,bool benchmark)
	{
		if(benchmark)
			return;

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

		for(const auto& device: application.PhysicalDevices())
		{
			VkPhysicalDeviceDriverProperties driverProp{};
			driverProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;

			VkPhysicalDeviceProperties2 deviceProp{};
			deviceProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
			deviceProp.pNext = &driverProp;
			vkGetPhysicalDeviceProperties2(device, &deviceProp);
			VkPhysicalDeviceFeatures features{};
			vkGetPhysicalDeviceFeatures(device, &features);

			const auto&prop = deviceProp.properties;

			const Vulkan::Version vulkanVersion(prop.apiVersion);
			const Vulkan::Version driverVersion(prop.driverVersion,prop.vendorID);

			fmt::print("- [{}] {} '{}' ({}: vulkan {} driver {} {} - {})\n",
						prop.deviceID, Vulkan::Strings::VendorId(prop.vendorID), prop.deviceName, Vulkan::Strings::DeviceType(prop.deviceType),
						to_string(vulkanVersion), driverProp.driverName, driverProp.driverInfo, to_string(driverVersion));
		}
		puts("");
	}
	
	void PrintVulkanSwapChainInformation(const Vulkan::VulkanBaseRenderer& application,bool benchmark)
	{
		const auto& swapChain = application.SwapChain();
		fmt::print("Swap Chain:\n- image count:{}\n-persent mode:{}\n\n",swapChain.Images().size(),static_cast<int>(swapChain.PresentMode()));
	}
	
	void SetVulkanDevice(Vulkan::VulkanBaseRenderer& application,uint32_t gpuIdx)
	{
		const auto& physicalDevices = application.PhysicalDevices();
		VkPhysicalDevice pDevice = physicalDevices[gpuIdx <= physicalDevices.size()-1 ? gpuIdx : 0];
		VkPhysicalDeviceProperties2 deviceProp{};
		deviceProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		vkGetPhysicalDeviceProperties2(pDevice,&deviceProp);

		fmt::print("Setting Device[{}]\n",deviceProp.properties.deviceName);
		application.SetPhysicalDevice(pDevice);

		puts("Vulkan Device Properties:");
	}
}

std::unique_ptr<Vulkan::VulkanBaseRenderer> GApplication = nullptr;

void StartApplication(uint32_t renderType,const Vulkan::WindowConfig& windowConfig,const UserSettings& userSettings,const Options& options)
{
	switch(renderType)
	{
	case 0:
		printf("set render type 0");
	case 1:
		printf("set render type 0");
	case 2:
		printf("set render type 0");
	case 3:
		printf("set render type 0");
	case 4:
		printf("set render type 0");
	case 5:
		printf("set render type 0");
	default:
		GApplication.reset(new cyNextRender<Vulkan::VulkanBaseRenderer>(
			userSettings, windowConfig, static_cast<VkPresentModeKHR>(options.Benchmark ? 0 : options.PresentMode)));
	}

	fmt::print("Render: {}, Builder {}\n",GApplication->GetRendererType(),cyNextRender::GetBuildVersion());

	PrintVulkanSdkInformation();
	PrintVulkanDevices(*GApplication);
	SetVulkanDevice(*GApplication,options.GpuIdx);
	PrintVulkanSwapChainInformation(*GApplication,options.Benchmark);
}

int main(int argc,const char *argv[]) noexcept
{
	try
	{
		const Options options(argc,argv);
		GOption = &options;
		UserSettings userSettings = CreateUserSettings(options);
		const Vulkan::WindowConfig windowConfig{
			"cyVulkanEngine " + cyNextRender::GetBuildVersion(),
			options.Width,
			options.Height,
			options.Benchmark && options.Fullscreen,
			options.Fullscreen,
			!options.Fullscreen,
			options.SaveFile,
			nullptr,
			options.ForceSDR
		};

		uint32_t renderType = options.RendererType;

#if WIN32 && !defined(__MINGW32__)
		HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD dwMode;

		GetConsoleMode(hOutput,&dwMode);
		dwMode |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hOutput,dwMode);
#endif

		StartApplication(renderType,windowConfig,userSettings,options);

		GApplication->Start();
		while(true)
		{
			if(GApplication->Tick())
			{
				break;
			}
			GApplication->End();
		}

		GApplication.reset();
		return EXIT_SUCCESS;
	}
	catch (const Options::Help&)
	{
		return EXIT_SUCCESS;
	}
	catch(const std::exception& exception)
	{
		Utilities::Console::Write(Utilities::Severity::Fatal,[&exception]()
		{
			const auto stacktrace = boost::get_error_info<traced>(exception);

			std::cerr << "Fatal:" << exception.what() << std::endl;
			if(stacktrace)
			{
				std::cerr << "\n" << *stacktrace << "\n";
			}
		});
	}
	catch(...)
	{
		Utilities::Console::Write(Utilities::Severity::Fatal,[]()
		{
			fmt::print(stderr,"FATAL:caught unhandled exception\n");
		});
	}

	return EXIT_FAILURE;
}

namespace
{
	UserSettings CreateUserSettings(const Options& options)
	{
		SceneList::ScanScenes();

		UserSettings userSettings{};

		userSettings.Benchmark = options.Benchmark;
		userSettings.BenchmarkNextScenes = options.BenchmarkNextScenes;
		userSettings.BenchmarkMaxFrame = options.BenchmarkMaxFrame;
		userSettings.BenchmarkMaxTime = options.BenchmarkMaxTime;
		userSettings.SceneIndex = options.SceneIndex;

		if(options.SceneName != "")
		{
			std::string mappedSceneName = "";
			bool foundInAssets = false;

			Assets::uo_string_string_t::const_iterator got = Assets::sceneNames.find(options.SceneName);
			if(got != Assets::sceneNames.end()) mappedSceneName = got->second;

			for(uint32_t i = 0;i < SceneList::AllScenes.size();i++)
			{
				if(SceneList::AllScenes[i].first == options.SceneName || SceneList::AllScenes[i].first == mappedSceneName)
				{
					userSettings.SceneIndex = i;
					foundInAssets = true;
					break;
				}
			}

			if(!foundInAssets)
			{
				userSettings.SceneIndex = SceneList::AddExternalScene(options.SceneName);
			}
		}
		userSettings.IsRayTraced = true;
		userSettings.AccumulateRays = false;
		userSettings.NumberOfSamples = options.Benchmark ? 1 : options.Samples;
		userSettings.NumberOfBounces = options.Benchmark ? 4 : options.Bounces;
		userSettings.MaxNumberOfBounces = options.MaxBounces;
		userSettings.RR_MIN_DEPTH = options.RR_MIN_DEPTH;
		userSettings.AdaptiveSample = options.AdaptiveSample;
		userSettings.AdaptiveVariance = 6.f;
		userSettings.AdaptiveSteps = 8;
		userSettings.TAA = true;

		userSettings.ShowSettings = !options.Benchmark;
		userSettings.ShowOverlay = true;

		userSettings.ShowVisualDebug = false;
		userSettings.HeatmapScale = 1.5f;

		userSettings.UseCheckerBoardRendering = false;
		userSettings.TemporalFrames = options.Benchmark ? 256 : options.Temporal;

		userSettings.Denoiser = options.Denoiser;

		userSettings.PaperWhiteNit = 600.f;

		userSettings.SunRotation = 0.5f;
		userSettings.SunLuminance = 500.f;
		userSettings.SkyIntensity = 50.f;
        
		userSettings.AutoFocus = false;

		return userSettings;
	}
	
	
}
