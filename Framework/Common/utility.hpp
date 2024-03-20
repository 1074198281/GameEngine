#pragma once
#include <ctype.h>
#include <string>
#include <assert.h>


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

	inline std::shared_ptr<std::vector<uint8_t>> ReadFileSyncDirectly(const char* file_name)
	{
		FILE* file;
		std::vector<uint8_t> data;
		file = fopen(file_name, "r");
		if (file != nullptr) {
			// file_name is Text file
		} else if (!file) {
			// file_name may be Binary file
			file = fopen(file_name, "rb");
		}
		size_t data_size = GetSize(file);
		data.resize(data_size);
		size_t size = fread(data.data(), data_size, 1, static_cast<FILE*>(file));
		std::shared_ptr<std::vector<uint8_t>> ret = std::make_shared<std::vector<uint8_t>>(data);
		return ret;
	}
}
