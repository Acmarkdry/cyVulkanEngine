#include "Texture.hpp"

namespace Assets
{
	struct TextureTaskContext
	{
		int32_t textureId;
		TextureImage* transferPtr;
		float elapsed;
		std::array<char,256> outputInfo;
	};
	
	
}
