#include "GuiSubPass.hpp"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

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
		
	}

	m_pGraphicsManager->DrawGui(frame);
}