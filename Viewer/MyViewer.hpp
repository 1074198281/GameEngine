#pragma once

#include "IGameLogic.hpp"

namespace My {
	class MyViewerLogic : public IGameLogic {
	public:
		MyViewerLogic() {}
		~MyViewerLogic() {}

		int Initialize() override;
		void Finalize() override;
		void Tick() override;
	public:
		void GetInputKey() override {}
		int ExecuteCommandLine() override;
	};
}