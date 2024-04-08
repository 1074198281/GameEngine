#include "GuiSubPass.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#include <deque>

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

		ImVec2 next_window_pos(ImGui::GetWindowPos().x + ImGui::GetWindowWidth(), ImGui::GetWindowPos().y);
		ImGui::SetNextWindowPos(next_window_pos, ImGuiCond_FirstUseEver);
		ImGui::Begin("MyViewer", NULL, ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu((const char*)u8"Debug Menu"))
			{
				ImGui::MenuItem((const char*)u8"Debug Window", NULL, &show_app_debug_panel);
				ImGui::MenuItem((const char*)u8"ImGui Status And Debug Window", NULL, &show_app_metrics);
				ImGui::MenuItem((const char*)u8"Scene Status", NULL, &show_app_scene_status);
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
			ImGui::Begin((const char*)u8"Scene Status");
			ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
			for (auto& GeoNode : m_pScene->GeometryNodes) {
				std::string GeoName = GeoNode.first;
				if (ImGui::TreeNode(GeoName.c_str()))
				{
					std::string MaterialName = GeoName + "Material";
					std::string GeoMaterialName = GeoNode.second->GetMaterialCount() > 0 ? GeoNode.second->GetMaterialRef(0) : "default";
					if (!m_pScene->Materials[GeoMaterialName]) {
						assert(false, "Material Not Exist!");
						ImGui::TreePop();
						continue;
					}

					ImGui::SeparatorTextEx(0, MaterialName.c_str(), NULL, 0);
					ImGui::SliderFloat4("BaseColor", m_pScene->Materials[GeoName]->GetBaseColorFactorData(), 0, 1);
					ImGui::SliderFloat("Metallic", m_pScene->Materials[GeoName]->GetMetallicFactorData(), 0, 1);
					ImGui::SliderFloat("Roughness", m_pScene->Materials[GeoName]->GetRoughnessFactorData(), 0, 1);
					ImGui::SliderFloat3("Emissive", m_pScene->Materials[GeoName]->GetEmissiveFactorData(), 0, 1);
					ImGui::SliderFloat("NormalScale", m_pScene->Materials[GeoName]->GetNornalScaleFactorData(), 0, 1);

					ImGui::TreePop();
				}
			}
			ImGui::End();
		}

		ImGui::End();
	}

	m_pGraphicsManager->DrawGui(frame);
}