#pragma once

#include "IRuntimeModule.hpp"

namespace My {
	__Interface IPass : public IRuntimeModule{
	public:
		IPass() = default;
		virtual ~IPass() = 0;
	};
}
