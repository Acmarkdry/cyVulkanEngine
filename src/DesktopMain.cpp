#include "Utilities/Console.hpp"
#include "Utilities/Exception.hpp"
#include "Options.hpp"
#include "Runtime/Engine.hpp"

#include <fmt/format.h>
#include <iostream>
#include <filesystem>

#include "Runtime/Platform/PlatformCommon.h"

int main(int argc, const char** argv) noexcept
{
	{
		Options options(argc, argv);
		GOption = &options;

		if (options.RenderDoc)
		{
		
		}
		
		NextRenderer::PlatformInit();
		
		std::unique_ptr<NextEngine> GApplication;
		GApplication.reset(new NextEngine(options));
		GApplication->Start();
		while (true)
		{
			if (GApplication->Tick())
				break;
		}
		GApplication->End();
		GApplication.reset();
		
		return EXIT_SUCCESS;
	}
}