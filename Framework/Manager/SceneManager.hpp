#pragma once

#include "ISceneManager.hpp"
#include "ISceneParser.hpp"

#include "../PCG/PCGCommonObject.h"

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

		bool IsSceneEmpty() override;

	public:
		void AddWaves(float x, float y, float centerX, float centerY, float centerZ, 
			float stepX, float stepY, float length, float amplitude, float speed) override;

	private:
		bool m_bEmptyScene = false;
	protected:
		std::unique_ptr<Scene> m_pScene;
		uint64_t m_nSceneRevision = 0;

	private:
		std::unordered_map<uint32_t, std::unique_ptr<PCGCommonObject>> m_ScenePCGObjects;
		uint32_t m_iObjectId;
	};
}