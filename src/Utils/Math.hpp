#pragma once

#include<string>
#include<fmt/printf.h>

namespace Utilities
{
	namespace Math
	{
		static uint32_t GetSafeDispatchCount(uint32_t size,uint32_t divider)
		{
			return size%divider == 0 ? size/divider : size/divider + 1;
		}

		static int32_t floorToInt(float value)
		{
			return static_cast<int32_t>(std::floor(value));
		}

		static int32_t ceilToInt(float value)
		{
			return static_cast<int32_t>(std::ceil(value));
		}
	}

	static std::string merticFormatter(double value,std::string unit,int kilo = 1000)
	// 度量衡规范化
	{
		static double s_value[] = {static_cast<double>(kilo*kilo*kilo),static_cast<double>(kilo*kilo),static_cast<double>(kilo),
		1.f,1.0f/static_cast<double>(kilo),1.0f/static_cast<double>(kilo*kilo),1.0f/static_cast<double>(kilo*kilo*kilo)};
		static const char* s_prefix[] = {"G","M","K","","m","u","n"};
		constexpr int s_valueSZ = sizeof(s_value)/sizeof(double);
		
		if(value < s_value[s_valueSZ - 1])
		{
			return fmt::sprintf(" ");
		}
		if(value < 10001.f)
		{
			return fmt::sprintf("%.0f",value);
		}

		for(int i = 0;i < s_valueSZ;i++)
		{
			if(fabs(value) >= s_value[i])
			{
				return fmt::sprintf("%.2f%s%s",value/s_value[i],s_prefix[i],unit);
			}
		}
		return fmt::sprintf("%.2f%s%s", value / s_value[6], s_prefix[6], unit);
	}
	
	
}
