#pragma once
#include <string>
#include <glm/vec2.hpp>

#include "UserSettings.hpp"

namespace cyVulkanEngine
{
	enum class EApplicationStatus
	{
		Starting,
		Running,
		Loading,
		AsyncPreparing,
	};

	std::string GetBuildVersion{};
}


template <typename Renderer>
class cyVulkanEngineApplication final: public Renderer
{
public:

private:
	void LoadScene(uint32_t sceneIndex);
	void CheckAndUpdateBenchmarkState(double prevTime);
	void CheckFramebufferSize() const;

	void Report(int fps,const std::string& sceneName,double upload_screen,bool save_screen);

	uint32_t sceneIndex_{((uint32_t)~((uint32_t)0))}; // 取反拿最大值
	mutable UserSettings userSettings_{};
	UserSettings previousSettings_{};
	Assets::CameraInitialSate cameraInitialSate_{};

	
	
	cyVulkanEngine::EApplicationStatus status_{};
	uint32_t totalFrames_{};
	// benchmark stats
	double sceneInitialTime_{};
	double periodInitialTime_{};
	uint32_t periodTotalFrames_{};
	uint32_t benchmarkTotalFrames_{};
	uint32_t benchmarkNumbers_{0};
	std::ofstream benchmarkCsvReportFile;

	glm::vec2 mousePos_{};
};
