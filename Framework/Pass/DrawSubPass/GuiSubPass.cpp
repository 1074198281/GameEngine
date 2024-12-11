#include "GuiSubPass.hpp"
#include "BaseApplication.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#include <deque>


namespace My {
	static const std::string debug_texture_name_array[5] = {
		"BaseColor Texture",
		"Metallic Texture",
		"Roughness Texture",
		"Emissive Texture",
		"Normal Texture"
	};
}

void My::GuiSubPass::BeginSubPass()
{
	m_pGraphicsManager->BeginSubPass("RenderGui");
}

void My::GuiSubPass::EndSubPass()
{
	m_pGraphicsManager->EndSubPass();
}

void My::GuiSubPass::Draw(Frame& frame)
{
	if (ImGui::GetCurrentContext()) {
		static bool show_app_debug_panel = false;
		static bool show_app_metrics = false;
		static bool show_app_about = false;
		static bool show_app_scene_status = false;
		static bool show_app_debug_texture = false;

		ImVec2 next_window_pos(ImGui::GetWindowPos().x + ImGui::GetWindowWidth(), ImGui::GetWindowPos().y);
		ImGui::SetNextWindowPos(next_window_pos, ImGuiCond_FirstUseEver);
		ImGui::Begin("MyViewer", NULL, ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu((const char*)u8"Debug Menu"))
			{
				ImGui::MenuItem((const char*)u8"Debug Window", NULL, &show_app_debug_panel);
				ImGui::MenuItem((const char*)u8"ImGui Status And Debug Window", NULL, &show_app_metrics);
				ImGui::MenuItem((const char*)u8"Scene Status", NULL, &show_app_scene_status);
				ImGui::MenuItem((const char*)u8"Texture Status", NULL, &show_app_debug_texture);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu((const char*)u8"Helper"))
			{
				ImGui::MenuItem((const char*)u8"About", NULL, &show_app_about);
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		if (show_app_metrics)
		{
			ImGui::ShowMetricsWindow(&show_app_metrics);
		}

		if (show_app_debug_panel) {
			static std::deque<float> fps_data;
			ImGui::Begin((const char*)u8"Debug Window");  // Create a debug window

			ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
			if (ImGui::CollapsingHeader((const char*)u8"Running Status And Basic Param")) {
				ImGui::ColorEdit3(
					(const char*)u8"ClearColor",
					(float*)frame.ClearColor);  // Edit 3 floats representing a color

				float fps = ImGui::GetIO().Framerate;
				ImGui::Text((const char*)u8"Average Render Time %.3f ms/Frame (%.1f FPS)",
					1000.0f / fps,
					fps);

				fps_data.push_back(fps);
				if (fps_data.size() > 100) fps_data.pop_front();

				auto getData = [](void* data, int index)->float {
					return ((decltype(fps_data)*)data)->at(index);
					};

				ImGui::PlotLines((const char*)u8"FrameRate", getData, (void*)&fps_data, fps_data.size(), 0, "FPS", 0.0f);
			}

			ImGui::End();
		}

		if (show_app_scene_status) {
			auto pScene = dynamic_cast<BaseApplication*>(m_pApp)->GetSceneManager()->GetSceneForGui();
			auto pPhysicsManager = dynamic_cast<BaseApplication*>(m_pApp)->GetPhysicsManager();
			ImGui::Begin((const char*)u8"Scene Status");
			ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
			
			//Base Scene Settings
			if (ImGui::TreeNode("BaseSceneSettings"))
			{
				if (ImGui::TreeNode("Skybox")) {
					bool* pIsDrawSkybox = m_pGraphicsManager->GetDrawSkyboxStatus();
					ImGui::Checkbox("SkyboxVisible", pIsDrawSkybox);
					int* skyboxIndex = m_pGraphicsManager->GetSkyboxIndex();
					std::vector<std::string> skyboxInfo = m_pGraphicsManager->GetSkyboxInfo();
					ImGui::SliderInt("SkyboxIndex", skyboxIndex, 0, skyboxInfo.size() - 1);

					if (ImGui::TreeNode("Texture Debug")) {
						//size_t handle_ptr = m_pGraphicsManager->GetSkyboxTextureGpuPtr(skyboxInfo[*skyboxIndex]);
						//ImGui::Image((ImTextureID)handle_ptr, ImVec2((float)1024, (float)1024));
						ImGui::TreePop();
					}
					ImGui::TreePop();
				}

				bool* pIsCastShadow = m_pGraphicsManager->GetCastShadowStatus();
				ImGui::Checkbox("CastShadow", pIsCastShadow);

				bool* pIsGuassBlur = m_pGraphicsManager->GetGuassBlurStatus();
				ImGui::Checkbox("GuassBlur", pIsGuassBlur);

				bool* pIsOverlay = m_pGraphicsManager->GetOverlayStatus();
				ImGui::Checkbox("Overlay", pIsOverlay);

				ImGui::TreePop();
			}


			// SceneNode Settings
			for (auto& batch_Context : frame.BatchContexts) {
				auto& GeoNode = batch_Context->Node;
				std::string GeoName = GeoNode->GetSceneObjectRef();
				if (ImGui::TreeNode(GeoName.c_str()))
				{
					bool bVisible = GeoNode->Visible();
					ImGui::Checkbox("Visible", &bVisible);

					ImGui::SeparatorTextEx(0, "Model Matrix", NULL, 0);
					Matrix4X4f model;
					if (GeoNode->GetRigidBody()) {
						model = pPhysicsManager->GetRigidBodyTransform(GeoNode->GetRigidBody());
					} else {
						model = *GeoNode->GetCalculatedTransform().get();
					}
					 
					if (ImGui::TreeNode("Model Matrix")) {
						ImGui::InputFloat4("", model[0]);
						ImGui::InputFloat4("", model[1]);
						ImGui::InputFloat4("", model[2]);
						ImGui::InputFloat4("", model[3]);
						ImGui::TreePop();
					}

					std::string MaterialName = GeoName + "Material";
					ImGui::SeparatorTextEx(0, MaterialName.c_str(), NULL, 0);
					std::string GeoMaterialName = GeoNode->GetMaterialCount() > 0 ? GeoNode->GetMaterialRef(0) : "default";
					if (!pScene->Materials[GeoMaterialName]) {
						std::cout << "Node " << GeoName << " Has No Material!" << std::endl;
						//assert(false, "Material Not Exist!");
						GeoNode->SetVisibility(bVisible);
						ImGui::TreePop();
						continue;
					}
					if (ImGui::TreeNode("PBR Properties")) {
						ImGui::SliderFloat4("BaseColor", frame.BatchContexts[batch_Context->BatchIndex]->BaseColorFactor, 0, 1);
						ImGui::SliderFloat("Metallic", &frame.BatchContexts[batch_Context->BatchIndex]->MetallicRoughnessFactor[0], 0, 1);
						ImGui::SliderFloat("Roughness", &frame.BatchContexts[batch_Context->BatchIndex]->MetallicRoughnessFactor[1], 0, 1);
						ImGui::SliderFloat3("Emissive", frame.BatchContexts[batch_Context->BatchIndex]->EmissiveFactor, 0, 1);
						ImGui::SliderFloat("NormalScale", &frame.BatchContexts[batch_Context->BatchIndex]->NormalTextureScale, 0, 1);
						ImGui::TreePop();
					}
					
					ImGui::SeparatorTextEx(0, "Debug_Texture_Show", NULL, 0);
					uint32_t debug_texture_width, debug_texture_height, debug_texture_size;
					size_t debug_texture_gpu_handle = 0;

					for (int i = 0; i < _countof(debug_texture_name_array); i++) {
						if (ImGui::TreeNode(debug_texture_name_array[i].c_str())) {
							debug_texture_gpu_handle = m_pGraphicsManager->GetTextureGpuPtr(batch_Context->BatchIndex, i, debug_texture_width, debug_texture_height, debug_texture_size);
							if (debug_texture_gpu_handle != 0) {
								ImGui::Image((ImTextureID)debug_texture_gpu_handle, ImVec2((float)256, (float)256));
							}
							ImGui::TreePop();
						}
					}

					GeoNode->SetVisibility(bVisible);
					ImGui::TreePop();
				}
			}

			if (pScene->LightNodes.size())
			{
				LightInfo* pLightInfo = reinterpret_cast<LightInfo*>(m_pGraphicsManager->GetLightInfo());
				int lightNum = pScene->LightNodes.size();
				for (int i = 0; i < lightNum; i++)
				{
					Light* l = &pLightInfo->Lights[i];
					std::string lightName = m_pGraphicsManager->GetLightName(l->padding0);
					if (ImGui::TreeNode(lightName.c_str()))
					{
						ImGui::SliderFloat("LightInsensity", &l->Insensity, 0, 100);
						ImGui::SliderFloat4("LightColor", l->LightColor, 0, 1);
						ImGui::SliderFloat4("LightPosition", l->LightPosition, -100, 100);

						ImGui::TreePop();
					}
				}
			}

			ImGui::End();
		}

		if (show_app_debug_texture) {
			ImGui::Begin((const char*)u8"Texture Status");
			if (ImGui::TreeNode("Skybox Texture")) {
				// get skybox index and its gpu handle
				uint32_t width, height;
				int* skyboxIndex = m_pGraphicsManager->GetSkyboxIndex();
				std::vector<std::string> skyboxInfo = m_pGraphicsManager->GetSkyboxInfo();
				size_t handle_ptr = m_pGraphicsManager->GetSkyboxTextureGpuPtr(skyboxInfo[*skyboxIndex], width, height);

				ImGui::Image((ImTextureID)handle_ptr, ImVec2((float)width, (float)height));
				ImGui::TreePop();
			}
			ImGui::End();
		}

		ImGui::End();
	}

	m_pGraphicsManager->DrawGui(frame);
}