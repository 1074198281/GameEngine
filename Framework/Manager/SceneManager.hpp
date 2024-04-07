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
		int LoadScene(const char* scene_file_name) override;

	public:
		void LoadglTFScene(const char* glTF_scene_file_name);
		void LoadOgexScene(const char* ogex_scene_file_name);

	public:
		const Scene& GetSceneForRendering() override;
		Scene* GetSceneForGui() override;
		uint64_t GetSceneRevision() override;

		void ResetScene() override;
	protected:
		std::unique_ptr<Scene> m_pScene;
		uint64_t m_nSceneRevision = 0;
	};
}