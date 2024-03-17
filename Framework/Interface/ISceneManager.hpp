#pragma once

#include "IRuntimeModule.hpp"
#include "Scene.hpp"

namespace My {
	__Interface ISceneManager : public IRuntimeModule{	
	public:
		ISceneManager() = default;
		virtual ~ISceneManager() = default;

		virtual int Initialize() { return 0; }
		virtual void Finalize() {} 
		virtual void Tick() {} 

	public:
		virtual void LoadScene(const char* scene_file_name) = 0;

		virtual bool IsSceneChanged() = 0;

		virtual void NotifySceneIsRenderingQueued() = 0;

		virtual const Scene& GetSceneForRendering() = 0;

		virtual void ResetScene() = 0;

		virtual uint64_t GetSceneRevision() = 0;
	};
}