#include "Options.hpp"

#include <iostream>
#include<boost/program_options.hpp>

using namespace boost::program_options;

Options::Options(int argc, const char* argv[])
{
	const int lineLength = 120;

	options_description window("Window options",lineLength);
	window.add_options()
		("width",value<uint32_t>(&Width)->default_value(1280),"the frame buffer width")
		("height",value<uint32_t>(&Height)->default_value(720),"the frame buffer height")
		("perdent-mode",value<uint32_t>(&PresentMode)->default_value(3),"the present mode")
		("fullscreen",bool_switch(&FullScreen)->default_value(false))
	;

	options_description desc("application options",lineLength);

	desc.add(window);

	const positional_options_description positional;
	variables_map vm;
	
	store(command_line_parser(argc, argv).options(desc).positional(positional).run(), vm);
	notify(vm);

	if(vm.count("help"))
	{
		std::cout << desc << std::endl;
		throw(Help());
	}

	if (PresentMode > 3)
	{
		throw(std::out_of_range("invalid present mode"));
	}
	
}
