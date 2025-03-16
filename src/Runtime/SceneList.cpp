#include "SceneList.hpp"

#include "Utils/FileHelper.hpp"

namespace Vulkan
{
	class VulkanBaseRenderer;
}

using namespace glm;

namespace Assets
{
	class Material;
	class Model;
	class Texture;
}

namespace
{
	int CreateMaterial(std::vector<Assets::Material>& materials,Assets::Material mat)
	{
		materials.emplace_back(mat);
		return static_cast<int>(materials.size() - 1); // 序号
	}
	
}


void CornellBox(Assets::CameraInitialSate& camera,std::vector<Assets::Node> nodes,std::vector<Assets::Model> models,
	std::vector<Assets::Material> materials,std::vector<Assets::LightObject> lights)
{
	camera.ModelView = lookAt(vec3(0, 278, 1078), vec3(0, 278, 0), vec3(0, 1, 0));
	camera.FieldOfView = 40;
	camera.Aperture = 0.0f;
	camera.FocusDistance = 778.0f;
	camera.ControlSpeed = 200.0f;
	camera.GammaCorrection = true;
	camera.HasSky = false;
	camera.HasSun = false;


	int cbox_model = Assets::Model::CreateCornellBox(555, models, materials, lights);
	nodes.push_back(Assets::Node::CreateNode(Utilities::NameHelper::RandomName(6),mat4(1), cbox_model, false));

    
	materials.push_back(Assets::Material::Lambertian(vec3(0.73f, 0.73f, 0.73f)) );
	auto box0 = Assets::Model::CreateBox(vec3(0, 0, -165), vec3(165, 165, 0), static_cast<int>(materials.size() - 1) );
	models.push_back(box0);
    
	glm::mat4 ts0 = (rotate(translate(mat4(1), vec3(278 - 130 - 165, 0, 213)), radians(-18.0f), vec3(0, 1, 0)));
	glm::mat4 ts1 = (rotate(scale(translate(mat4(1), vec3(278 - 265 - 165, 0, 17)), vec3(1, 2, 1)),
										  radians(15.0f), vec3(0, 1, 0)));
   
	nodes.push_back(Assets::Node::CreateNode(Utilities::NameHelper::RandomName(6),ts0, 1, false));
	nodes.push_back(Assets::Node::CreateNode(Utilities::NameHelper::RandomName(6),ts1, 1, false));
}

std::vector<scene_pairs> SceneList::AllScenes = {
	{"Cornell Box",CornellBox},
};


bool compareSceneNames(scene_pairs a,scene_pairs b)
{
	return a.first < b.first;
}

void SceneList::ScanScenes()
{
	std::string path = Utilities::FileHelper::GetPlatformFilePath("assets/models/");
	for (const auto & entry : std::filesystem::directory_iterator(path))
	{
		std::filesystem::path filename = entry.path().filename();

		//looking for beauty scene name by file name
		std::string fn = filename.string();
		Assets::uo_string_string_t::const_iterator got = Assets::sceneNames.find(fn);

		//if found - change fn. if not - just filename
		if (got != Assets::sceneNames.end()) fn = got->second;
		std::string ext = entry.path().extension().string();
		if(ext != ".glb" && ext != ".obj") continue;

		AllScenes.push_back({fn, [=](Assets::CameraInitialSate& camera, std::vector<Assets::Node>& nodes, std::vector<Assets::Model>& models,
					std::vector<Assets::Material>& materials,
					std::vector<Assets::LightObject>& lights)
		{
			if(ext == ".glb")
			{
				Assets::Model::LoadGLTFScene(std::filesystem::absolute(entry.path()).string(),camera,nodes,models,materials,lights);
			}
			else if(ext == ".obj")
			{
				Assets::Model::LoadObjModel(absolute(entry.path()).string(), nodes, models, materials, lights);

				camera.FieldOfView = 38;
				camera.Aperture = 0.0f;
				camera.FocusDistance = 100.0f;
				camera.ControlSpeed = 1.0f;
				camera.GammaCorrection = true;
				camera.HasSky = true;

				Assets::Model::AutoFocusCamera(camera, models);
			}
		}
		});
	}

	std::sort(AllScenes.begin(), AllScenes.end(), compareSceneNames);
}

int32_t SceneList::AddExternalScene(std::string absPath)
{
	std::filesystem::path filename = absPath;
	std::string ext = filename.extension().string();
	if(std::filesystem::exists(filename) && (ext == ".glb" || ext == ".obj"))
	{
		AllScenes.push_back({filename.filename().string(), [=](Assets::CameraInitialSate& camera, std::vector<Assets::Node>& nodes, std::vector<Assets::Model>& models,
					std::vector<Assets::Material>& materials,
					std::vector<Assets::LightObject>& lights)
		{
			if(ext == ".glb")
			{
				Assets::Model::LoadGLTFScene(absolute(filename).string(), camera, nodes, models, materials, lights);
			}
			else if(ext == ".obj")
			{
				Assets::Model::LoadObjModel(absolute(filename).string(), nodes, models, materials, lights);

				camera.FieldOfView = 38;
				camera.Aperture = 0.0f;
				camera.FocusDistance = 100.0f;
				camera.ControlSpeed = 1.0f;
				camera.GammaCorrection = true;
				camera.HasSky = true;

				Assets::Model::AutoFocusCamera(camera, models);
			}
		}
		});
	}

	return static_cast<int32_t>(AllScenes.size() - 1);
}

