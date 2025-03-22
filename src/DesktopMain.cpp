#include "Utils/Console.hpp"
#include "utils/Exception.hpp"
#include "Options.hpp"
#include "Runtime/Engine.hpp"

#include "iostream"
#include "Runtime/platform/PlatformCommon.h"

int main(int argc, const char** argv) noexcept
{
	{
		Options options(argc, argv);
		GOption = &options;
		
		NextRenderer::PlatformInit();
		
		std::unique_ptr<NextEngine> GApplication;
	}
}