#program once

#include "Common/CoreMinimal.hpp"
#include "SceneList.hpp"
#include  "UserSettings.hpp"
#include "Assets/UniformBuffer.hpp"
#include "Assets/Model.hpp"
#include "Vulkan/VulkanBaseRenderer.hpp"
#include "Vulkan/FrameBuffer.hpp"
#include "Vulkan/Window.hpp"
#include "Options.hpp"


namespace qjs
{
	class Context;
	class Runtime;
};

Class NextEngine;

class NextGameInstanceBase
{
public:
	NextGameInstanceBase(Vulkan::WindowConfig&config, Options& options, NextEngine* engine) {}
	virtual ~NextGameInstanceBase() {}
	virtual void OnInit() = 0;
	virtual void OnTick(double deltaSeconds) = 0;
	virtual void OnDestroy() = 0;
	virtual void OnRenderUI() = 0;
	virtual void OnPreConfigUI() {}
	virtual void OnInitUI() {}
	virtual void OnRayHitResponse(Assets::RayCastResult& result) {}

	// camera
	virtual bool OverrideRenderCamera()
};