#pragma once
#include <exception>

class Options final
{
public:
	class Help:public std::exception
	{
	public:
		Help() = default;
		~Help() = default;
	};

	Options(int argc,const char *argv[]);
	~Options() = default;

	// window options
	uint32_t Width{};
	uint32_t Height{};
	uint32_t PresentMode{};
	bool FullScreen{};
	
};

inline const Options* GOptions = nullptr;