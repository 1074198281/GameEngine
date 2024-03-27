#pragma once
#include "Interface.hpp"

namespace My {
	__Interface ISubPass {
	public:
		virtual ~ISubPass() = default;
		virtual void BeginSubPass() = 0;
		virtual void EndSubPass() = 0;
	};
}