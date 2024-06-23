#pragma once

#include "Interface.hpp"
#include "GfxConfiguration.h"

namespace My {
	
	__Interface IApplication {
	public:
		virtual int Initialize() = 0;
		virtual void Finalize() = 0;
		
		//one cycle of the main loop
		virtual void Tick() = 0;

		virtual bool IsQuit() = 0;

		virtual void SetCommandLineParameters(int argc, char** argv) = 0;

		virtual int GetCommandLineParametersCount() = 0;

		virtual const char* GetCommandLineParameters(int index) = 0;

		virtual int CreateMainWindow() = 0;

		virtual GfxConfiguration& GetConfiguration() = 0;

		virtual IApplication* GetApplication() = 0;
	};
}