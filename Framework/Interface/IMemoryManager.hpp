#pragma once

#include "IRuntimeModule.hpp"

namespace My {
	__Interface IMemoryManager : public IRuntimeModule{
	public:
		IMemoryManager() = default;
		virtual ~IMemoryManager() = default;
	};
}