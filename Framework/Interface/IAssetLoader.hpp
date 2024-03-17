#pragma once
#include <string>
#include "IRuntimeModule.hpp"
#include "Buffer.hpp"

namespace My {

	typedef void* AssetFilePtr;

	enum AssetOpenMode {
		MY_OPEN_TEXT = 0, /// Open In Text Mode
		MY_OPEN_BINARY = 1, /// Open In Binary Mode 
	};

	enum AssetSeekBase {
		MY_SEEK_SET = 0, /// SEEK_SET
		MY_SEEK_CUR = 1, /// SEEK_CUR
		MY_SEEK_END = 2  /// SEEK_END
	};

	__Interface IAssetLoader : public IRuntimeModule{
	public:
		IAssetLoader() = default;
		virtual ~IAssetLoader() = default;

		virtual std::string SyncOpenAndReadTextFileToString(const char* file_name) = 0;

		virtual AssetFilePtr OpenFile(const char* name, AssetOpenMode mode) = 0;

		virtual Buffer SyncOpenAndReadText(const char* filePath) = 0;

		virtual Buffer SyncOpenAndReadBinary(const char* filePath) = 0;

	};
}