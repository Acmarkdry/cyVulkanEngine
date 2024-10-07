#pragma once
#include <cstdint>
#include <vector>

class UserSettings final
{
public:
	// application
	bool Benchmark;

	// benchmark
	bool BenchmarkNextScenes{};
	uint32_t BenchmarkMaxTime{};
	uint32_t BenchmarkMaxFrame{};

	// scene
	int SceneIndex;

	// renderer
	bool IsRayTraced;
	bool AccumulateRays;
	uint32_t NumberOfSamples;
	uint32_t NumberOfBounces;
	uint32_t MaxNumberOfBounces;
	bool AdaptiveSample;
	float AdaptiveVariance;
	int AdaptiveSteps;
	bool TAA;

	// camera
	float FieldOfView;
	float Aperture;
	float FocusDistance;
	bool AutoFocus;

	float SkyRotation;
	float SunRotation;
	float SunLuminance;
	float SkyIntensity;
	int SkyIdx,CameraIdx;

	// profiler
	bool ShowVisualDebug;
	float HeatmapScale;

	// UI
	bool ShowSettings;
	bool ShowOverlay;

	// performance
	bool UseCheckerBoardRendering;
	int TemporalFrames;

	// Denoise
	bool Denoiser;

	float PaperWhiteNit;
	inline const static float FieldOfViewMinValue = 10.f;
	inline const static float FieldOfViewMaxValue = 90.f;

	std::vector<Asset::Camera> cameras;
	//hitresult
	Assets::RayCastResult HitResult;
	
};
