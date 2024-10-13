#include "Application.hpp"

#include <fmt/format.h>

#include "Options.hpp"
#include "Utils/Console.hpp"
#include "Utils/Localization.hpp"

namespace
{
	const bool EnableValidationLayers =
#if defined(NDEBUG) || defined(ANDROID)
		false;
#else
			true;
#endif

	struct SceneTaskContext
	{
		float elapsed;
		std::array<float, 256> outputInfo;
	};
	
}

template <typename Renderer>
cyVulkanEngineApplication<Renderer>::cyVulkanEngineApplication(const UserSettings& userSettings, const Vulkan::WindowConfig& windowConfig, VkPresentModeKHR presentMode):
Renderer(Renderer::StaticClass(), windowConfig, presentMode,EnableValidationLayers),userSettings_(userSettings)
{
	CheckFramebufferSize();
	status_ = cyVulkanEngine::EApplicationStatus::Starting;

#if !ANDROID
	Utilities::Localization::ReadLocTexts(fmt::format("assets/locale/{}.txt",GOption->locale).c_str());

	//todo 编辑器指令

#endif
	
}


template <typename Renderer>
cyVulkanEngineApplication<Renderer>::~cyVulkanEngineApplication()
{
#if !ANDROID
	Utilities::Localization::SaveLocTexts(fmt::format("assets/locale/{}.txt",GOption->locale).c_str());
#endif
	scene_.reset();
}

template <typename Renderer>
Assets::UniformBufferObject cyVulkanEngineApplication<Renderer>::GetUniformBufferObject(const VkOffset2D offset, const VkExtent2D extent) const
{
	glm::mat4 pre_rotate_mat = glm::mat4(1.0f);
	glm::vec3 rotation_axis = glm::vec3(0.0f,0.0f,1.0f);

	pre_rotate_mat = glm::rotate(pre_rotate_mat,glm::radians(90.0f),rotation_axis);

	if(userSettings_.CameraIdx >= 0 && previousSettings_.CameraIdx != userSettings_.CameraIdx)
	{
		modelViewController_.Reset((userSettings_.cameras[userSettings_.CameraIdx]).ModelView);
	}

	const auto& init = cameraInitialSate_;
	Assets::UniformBufferObject ubo = {};

	ubo.ModelView = modelViewController_.ModelView();
	ubo.Projection = glm::perspective(glm::radians(userSettings_.FieldOfView),
		extent.width/static_cast<float>(extent.height),0.1f,10000.0f);
	ubo.Projection[1][1] *= -1; // todo 没有看懂

#if ANDROID
#endif

	ubo.ModelViewInverse = glm::inverse(ubo.ModelView);
	ubo.ProjectionInverse = glm::inverse(ubo.Projection);
	ubo.ViewProjection = ubo.Projection*ubo.ModelView;
	ubo.PrevViewProjection = prevUBO_.TotalFrames != 0 ? prevUBO_.ViewProjection : ubo.ViewProjection;

	ubo.ViewportRect = glm::vec4(offset.x,offset.y,extent.width,extent.height);
	glm::vec2 pixel = mousePos_ - glm::vec2(offset.x,offset.y);
	glm::vec2 uv = pixel / glm::vec2(extent.width,extent.height)*glm::vec2(2.0f,2.0f) - glm::vec2(1.0f,1.0f);
	glm::vec4 origin = ubo.ModelViewInverse*glm::vec4(0.0,0.0,0.0,1.0f);
	glm::vec4 target = ubo.ProjectionInverse*(glm::vec4(uv.x,uv.y,1,1));

	glm::vec3 raydir = ubo.ModelViewInverse*glm::vec4(normalize(glm::vec3(target) - glm::vec3(0.f,0.f,0.f)),0.0);
	glm::vec3 rayorg = glm::vec3(origin);

	
	Renderer::SetRaycastRay(rayorg, raydir);
	Assets::RayCastResult rayResult;
	Renderer::GetLastRayCastResult(rayResult);

	if(userSettings_.AutoFocus)
	{
		if(rayResult.Hitted)
		{
			userSettings_.FocusDistance = rayResult.T;
			scene_->SetSelectedId(rayResult.InstanceId);
		}
		else
		{
			scene_->SetSelectedId(-1);
		}
	}
	ubo.SelectedId = scene_->GetSelectedId();
	userSettings_.HitResult = rayResult;

	ubo.SkyRotation = userSettings_.SkyRotation;
	ubo.MaxNumberOfBounces = userSettings_.MaxNumberOfBounces;
	ubo.TotalFrames = totalFrames_;
	ubo.NumberOfSamples = userSettings_.NumberOfSamples;
	ubo.NumberOfBounces = userSettings_.NumberOfBounces;
	ubo.RR_MIN_DEPTH = userSettings_.RR_MIN_DEPTH;
	ubo.AdaptiveSample = userSettings_.AdaptiveSample;
	ubo.AdaptiveVariance = userSettings_.AdaptiveVariance;
	ubo.AdaptiveSteps = userSettings_.AdaptiveSteps;
	ubo.TAA = userSettings_.TAA;
	ubo.RandomSeed = rand();
	ubo.SunDirection = glm::vec4( glm::normalize(glm::vec3( sinf(float( userSettings_.SunRotation * M_PI )), 0.75f, cosf(float( userSettings_.SunRotation * M_PI )) )), 0.0f );
	ubo.SunColor = glm::vec4(1,1,1, 0) * userSettings_.SunLuminance;
	ubo.SkyIntensity = userSettings_.SkyIntensity;
	ubo.SkyIdx = userSettings_.SkyIdx;
	ubo.BackGroundColor = glm::vec4(0.4, 0.6, 1.0, 0.0) * 4.0f * userSettings_.SkyIntensity;
	ubo.HasSky = init.HasSky;
	ubo.HasSun = init.HasSun && userSettings_.SunLuminance > 0;
	ubo.ShowHeatmap = false;
	ubo.HeatmapScale = userSettings_.HeatmapScale;
	ubo.UseCheckerBoard = userSettings_.UseCheckerBoardRendering;
	ubo.TemporalFrames = userSettings_.TemporalFrames;
	ubo.HDR = Renderer::SwapChain().IsHDR();
	ubo.PaperWhiteNit = userSettings_.PaperWhiteNit;
	ubo.LightCount = scene_->GetLightCount();
	prevUBO_ = ubo;

	return ubo;
}

template <typename Renderer>
void cyVulkanEngineApplication<Renderer>::SetPhysicalDeviceImpl(VkPhysicalDevice physicalDevice, std::vector<const char*> requiredExtensions, VkPhysicalDeviceFeatures& deviceFeatures, void* nextDeviceFeatures)
{
	// 获取extensions，只有windows需要
#if WIN32
	requiredExtensions.insert(requiredExtensions.end(), VK_KHR_SWAPCHAIN_EXTENSION_NAME);
#endif

	// todo 写到了这里
	
}

