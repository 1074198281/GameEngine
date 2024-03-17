#pragma once

#include "ISceneManager.hpp"
#include "ISceneParser.hpp"

namespace My {
	class SceneManager : __implements ISceneManager {
	public:
		~SceneManager() override = default;;

		int Initialize() override;
		void Finalize() override;
		void Tick() override;

	public:
		void LoadScene(const char* scene_file_name) override;

	public:
		void LoadglTFScene(const char* glTF_scene_file_name);
		void LoadOgexScene(const char* ogex_scene_file_name);

	public:
		bool IsSceneChanged() override;
		void NotifySceneIsRenderingQueued() override;

		const Scene& GetSceneForRendering() override;
		uint64_t GetSceneRevision() override;

		void ResetScene() override;
	protected:
		std::unique_ptr<Scene> m_pScene;
		uint64_t m_nSceneRevision = 0;
	};
}