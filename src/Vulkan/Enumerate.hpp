#pragma once

#include "Vulkan.hpp"
#include <vector>

namespace  Vulkan {

template <class TValue>
inline void GetEnumerateVector(VkResult(enumerate) (uint32_t*, TValue*), std::vector<TValue>& vector)
{
	/*
	 * 永远将vulkan的枚举函数的结果保存到一个std::vector里面
	 * vkResult(enumerate)(uint32 Tvalue)代表一个函数指针
	 */
	uint32_t count = 0;
	Check(enumerate(&count, nullptr),
		"enumerate");

	vector.resize(count);
	Check(enumerate(&count, vector.data()),
		"enumerate");
}

template <class THandle, class TValue>
inline void GetEnumerateVector(THandle handle, void(enumerate) (THandle, uint32_t*, TValue*), std::vector<TValue>& vector)
{
	uint32_t count = 0;
	enumerate(handle, &count, nullptr);

	vector.resize(count);
	enumerate(handle, &count, vector.data());
}

template <class THandle, class TValue>
inline void GetEnumerateVector(THandle handle, VkResult(enumerate) (THandle, uint32_t*, TValue*), std::vector<TValue>& vector)
{
	uint32_t count = 0;
	Check(enumerate(handle, &count, nullptr),
		"enumerate");

	vector.resize(count);
	Check(enumerate(handle, &count, vector.data()),
		"enumerate");
}

template <class THandle1, class THandle2, class TValue>
inline void GetEnumerateVector(THandle1 handle1, THandle2 handle2, VkResult(enumerate) (THandle1, THandle2, uint32_t*, TValue*), std::vector<TValue>& vector)
{
	uint32_t count = 0;
	Check(enumerate(handle1, handle2, &count, nullptr),
		"enumerate");

	vector.resize(count);
	Check(enumerate(handle1, handle2, &count, vector.data()),
		"enumerate");
}

template <class TValue>
inline std::vector<TValue> GetEnumerateVector(VkResult(enumerate) (uint32_t*, TValue*))
{
	std::vector<TValue> initial;
	GetEnumerateVector(enumerate, initial);
	return initial;
}

template <class THandle, class TValue>
inline std::vector<TValue> GetEnumerateVector(THandle handle, void(enumerate) (THandle, uint32_t*, TValue*))
{
	std::vector<TValue> initial;
	GetEnumerateVector(handle, enumerate, initial);
	return initial;
}

template <class THandle, class TValue>
inline std::vector<TValue> GetEnumerateVector(THandle handle, VkResult(enumerate) (THandle, uint32_t*, TValue*))
{
	std::vector<TValue> initial;
	GetEnumerateVector(handle, enumerate, initial);
	return initial;
}

template <class THandle1, class THandle2, class TValue>
inline std::vector<TValue> GetEnumerateVector(THandle1 handle1, THandle2 handle2, VkResult(enumerate) (THandle1, THandle2, uint32_t*, TValue*))
{
	std::vector<TValue> initial;
	GetEnumerateVector(handle1, handle2, enumerate, initial);
	return initial;
}

}
