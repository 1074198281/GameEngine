#pragma once
#include <ctype.h>
#include <string>
#include <assert.h>
#include <vector>
#include <intrin.h>
#include <memory>

namespace My {
	template<class T>
	inline void SafeRelease(T **ppInterfaceToRelease)
	{
		if (*ppInterfaceToRelease != nullptr)
		{
			(*ppInterfaceToRelease)->Release();

			(*ppInterfaceToRelease) = nullptr;
		}
	}

#define BIT_MASK(b)      (1 << (b))
#define SET_BIT(word, bit)    ((word) |= BIT_MASK(bit))
#define CLEAR_BIT(word, bit)    ((word) &= ~BIT_MASK(bit))
#define TEST_BIT(word, bit)   (((word) & BIT_MASK(bit)) != 0)
#define ALL_BITS(type)      (~(type)0))


#define BIG_LITTLE_SWAP32(x)        ( (((*(long int *)&x) & 0xff000000) >> 24) | \
                                      (((*(long int *)&x) & 0x00ff0000) >> 8) | \
                                      (((*(long int *)&x) & 0x0000ff00) << 8) | \
                                      (((*(long int *)&x) & 0x000000ff) << 24) )

	inline std::string ToLower(const std::string& str)
	{
		std::string lower_case = str;
		for (char& s : lower_case)
			s = tolower(s);
		return lower_case;
	}

	inline std::string RemoveBasePath(const std::string& filePath)
	{
		size_t lastSlash;
		if ((lastSlash = filePath.rfind('/')) != std::string::npos)
			return filePath.substr(lastSlash + 1, std::string::npos);
		else if ((lastSlash = filePath.rfind('\\')) != std::string::npos)
			return filePath.substr(lastSlash + 1, std::string::npos);
		else
			return filePath;
	}

	inline std::string GetBasePath(const std::string& filePath)
	{
		size_t lastSlash;
		if ((lastSlash = filePath.rfind('/')) != std::string::npos)
			return filePath.substr(0, lastSlash + 1);
		else if ((lastSlash = filePath.rfind('\\')) != std::string::npos)
			return filePath.substr(0, lastSlash + 1);
		else
			return "";
	}

	inline std::string GetFileExtension(const std::string& filePath)
	{
		std::string fileName = RemoveBasePath(filePath);
		size_t extOffset = fileName.rfind('.');
		if (extOffset == std::string::npos)
			return "";

		return fileName.substr(extOffset + 1);
	}

	inline std::string RemoveExtension(const std::string& filePath)
	{
		return filePath.substr(0, filePath.rfind("."));
	}

	inline size_t GetSize(void* fp)
	{
		FILE* _fp = static_cast<FILE*>(fp);
		assert(_fp);
		if (!_fp) {
			return 0;
		}

		long pos = ftell(_fp);
		fseek(_fp, 0, SEEK_END);
		size_t length = ftell(_fp);
		fseek(_fp, pos, SEEK_SET);

		return length;
	}

	inline std::shared_ptr<std::vector<uint8_t>> ReadFileSyncDirectly(const char* file_name, bool isBinary = false)
	{
		FILE* file;
		std::vector<uint8_t> data;
		if (!isBinary) {
			file = fopen(file_name, "r");
		}
		else {
			file = fopen(file_name, "rb");
		}
		
		assert(file);
		size_t data_size = GetSize(file);
		data.resize(data_size);
		size_t size = fread(data.data(), data_size, 1, static_cast<FILE*>(file));
		std::shared_ptr<std::vector<uint8_t>> ret = std::make_shared<std::vector<uint8_t>>(data);
		return ret;
	}

	template <typename T> __forceinline T AlignUpWithMask(T value, size_t mask)
	{
		return (T)(((size_t)value + mask) & ~mask);
	}

	template <typename T> __forceinline T AlignDownWithMask(T value, size_t mask)
	{
		return (T)((size_t)value & ~mask);
	}

	template <typename T> __forceinline T AlignUp(T value, size_t alignment)
	{
		return AlignUpWithMask(value, alignment - 1);
	}

	template <typename T> __forceinline T AlignDown(T value, size_t alignment)
	{
		return AlignDownWithMask(value, alignment - 1);
	}

	template <typename T> __forceinline bool IsAligned(T value, size_t alignment)
	{
		return 0 == ((size_t)value & (alignment - 1));
	}

	template <typename T> __forceinline T DivideByMultiple(T value, size_t alignment)
	{
		return (T)((value + alignment - 1) / alignment);
	}

	template <typename T> __forceinline bool IsPowerOfTwo(T value)
	{
		return 0 == (value & (value - 1));
	}

	template <typename T> __forceinline bool IsDivisible(T value, T divisor)
	{
		return (value / divisor) * divisor == value;
	}

	__forceinline uint8_t Log2(uint64_t value)
	{
		unsigned long mssb; // most significant set bit
		unsigned long lssb; // least significant set bit

		// If perfect power of two (only one set bit), return index of bit.  Otherwise round up
		// fractional log by adding 1 to most signicant set bit's index.
		if (_BitScanReverse64(&mssb, value) > 0 && _BitScanForward64(&lssb, value) > 0)
			return uint8_t(mssb + (mssb == lssb ? 0 : 1));
		else
			return 0;
	}

	template <typename T> __forceinline T AlignPowerOfTwo(T value)
	{
		return value == 0 ? 0 : 1 << Log2(value);
	}
}
