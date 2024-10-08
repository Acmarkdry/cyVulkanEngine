#pragma once
#include <unordered_map>
#include<functional>

#include "Assets/Model.hpp"

namespace Assets
{
	class Node;
	class Model;
	class Texture;
	class Material;
	class LightObject;
	class CameraInitialSate;

	typedef std::pmr::unordered_map<std::string,std::string> uo_string_string_t;
	const uo_string_string_t sceneNames =
	{
		{"qx50.glb",            "Qx50"},
		{"track.glb",           "LowpolyTrack"},
		{"simple.glb",          "Simple"},
		{"complex.glb",         "Complex"},
		{"livingroom.glb",      "LivingRoom"},
		{"kitchen.glb",         "Kitchen"},
		{"luxball.glb",         "LuxBall"},
		{"moderndepart.glb",    "ModernHouse1"},
	};
}

typedef std::pair<std::string,std::function<void (Assets::CameraInitialSate&,
	std::vector<Assets::Node>& nodes,
	std::vector<Assets::Model>& models,
	std::vector<Assets::Material>& materials,
	std::vector<Assets::LightObject>& 
	)>> scene_pairs;

class SceneList final
{
public:
	static void ScanScenes();
	static int32_t AddExternalScene(std::string absPath);

	static std::vector<scene_pairs> AllScenes;
};

