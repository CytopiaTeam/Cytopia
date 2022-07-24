#include "PopupWindow.hxx"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include <UIManager.hxx>

namespace ui = ImGui;

void PopupWindow::draw() const
{
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

  // We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
  // Based on your use case you may want one of the other.
  const ImGuiViewport* viewport = ui::GetMainViewport();
  auto &uiManager = UIManager::instance();
  const ImVec2 scrSize = viewport->WorkSize;

  ui::SetNextWindowPos(viewport->WorkPos);
  ui::SetNextWindowSize(scrSize);

  const float itemSpacing = 8.f;
  const ImVec2 buttonSize(200, 40);
  const float frameFullWidth = buttonSize.x + itemSpacing;
  const ImVec2 buttonsFrameSize(frameFullWidth * (float)m_buttons.size(), buttonSize.y);

  const auto &layout = uiManager.getAllLayoutGroups()["BuildMenuButtons"].layout;

  bool p_open = true;
  if (ui::Begin("##fullscreenpopupbody", &p_open, flags))
  {
    ui::PushFont(layout.font);
    ui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{0, 0});
    ui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{itemSpacing, 0});

    const ImVec2 textSize = ui::CalcTextSize(m_text.c_str(), nullptr, false, scrSize.x * 0.33f);
    const ImVec2 startTextPos((scrSize.x - textSize.x) / 2.f, (scrSize.y - textSize.y) / 2.f);
    ui::SetCursorScreenPos(startTextPos);

    const ImVec2 borderOffset(8, 8);
    ImVec2 startBgPosX(ImMin(startTextPos.x, (scrSize.x - buttonsFrameSize.x) / 2.f), startTextPos.y);
    startBgPosX -= borderOffset;
    const ImVec2 bgSize((scrSize.x / 2.f - startBgPosX.x) * 2.f, textSize.y + buttonSize.y + borderOffset.y * 2);
    ui::RenderWindowCtBackground(ui::GetCurrentWindow()->DrawList, startBgPosX, bgSize);

    ui::PushTextWrapPos(ImGui::GetCursorPos().x + scrSize.x * 0.33f);
    ui::TextWrapped(m_text.c_str());
    ui::PopTextWrapPos();

    ui::SetCursorScreenPos(ImVec2((scrSize.x - buttonsFrameSize.x) / 2.f, (scrSize.y + textSize.y) / 2.f));
    for (const auto &b : m_buttons)
    {
      if (ui::ButtonCt(b.text.c_str(), buttonSize))
      {
        m_result = b.id;
        
      }

      if (m_buttons.size() > 1)
        ui::SameLine();
    }

    ui::PopStyleVar(4);
    ui::PopFont();
  }
  ui::End();
}

void PopupWindow::close() const
{
  UIManager::instance().closeMenu();
}

