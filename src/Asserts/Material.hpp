#pragma once
#include <glm/vec3.hpp>

namespace Assets
{
	struct alignas(16) Material final
	{
		public:
		// todo 梳理这里的五种材质都是什么
		static Material Lambertian(const glm::vec3& diffuse, const int32_t textureId = -1)
		{
			return Material{glm::vec4(diffuse, 1), textureId, -1, -1, 1.0f, 1.45f, Enum::Lambertian, 0};
		}

		static Material Metallic(const glm::vec3& diffuse, const float fuzziness, const int32_t textureId = -1)
		{
			return Material{glm::vec4(diffuse, 1), textureId, -1, -1, fuzziness, 1.45f, Enum::Metallic, 1};
		}

		static Material Dielectric(const float refractionIndex, const float fuzziness, const int32_t textureId = -1)
		{
			return Material{glm::vec4(1.0f, 1.0f, 1.0f, 1), textureId, -1, -1, fuzziness, refractionIndex, Enum::Dielectric, 0};
		}

		static Material Isotropic(const glm::vec3& diffuse, const float refractionIndex, const float fuzziness, const int32_t textureId = -1)
		{
			return Material{glm::vec4(diffuse, 1), textureId, -1, -1, fuzziness, refractionIndex, Enum::Isotropic};
		}

		static Material DiffuseLight(const glm::vec3& diffuse, const int32_t textureId = -1)
		{
			return Material{glm::vec4(diffuse, 1), textureId, -1, -1, 0.0f, 0.0f, Enum::DiffuseLight};
		}

		enum class Enum:uint32_t
		{
			Lambertian = 0,
			Metallic = 1,
			Dielectric = 2,
			Isotropic = 3,
			DiffuseLight = 4,
			Mixture = 5,
		};

		glm::vec4 Diffuse;
		int32_t DiffuseTextureId;
		int32_t MRATextureId;
		int32_t NormalTextureId;

		float Fuzziness;
		Enum MaterialModel;
		float Metalness;
		float RefractionIndex2;
		float Reserverd1;
		float Reserverd2
	};
}
