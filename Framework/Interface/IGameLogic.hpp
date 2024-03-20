#pragma once
#include "IRuntimeModule.hpp"

namespace My {
	__Interface IGameLogic : public IRuntimeModule{
	public:
		IGameLogic() = default;
		virtual ~IGameLogic() = default;

	public:
		virtual void GetInputKey() = 0;
		virtual int ExecuteCommandLine() = 0;
	};
}