#include "GameTimeMenu.hxx"

#include "SettingsMenu.hxx"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "../services/GameClock.hxx"

namespace ui = ImGui;

void GameTimeMenu::draw() const
{
  ImVec2 screenSize = ui::GetIO().DisplaySize;

  auto &uiManager = UIManager::instance();

  const auto &layout = uiManager.getLayouts()["BuildMenuButtons"];
  const ImVec2 buttonSize(32, 32);
  const int spacing = 8;
  ImVec2 windowSize((buttonSize.x + spacing) * 4 + spacing, buttonSize.y + spacing * 2);
  ImVec2 pos(screenSize.x - windowSize.x, 0);
  const ImRect bb(ImVec2{0, 0}, ImVec2{0, 0} + windowSize);

  struct
  {
    uint8_t clr, frame, frameShade, bottomFrame, bottomFrameShade;
  } bg;
  bg = {128, 150, 200, 106, 84};

  const ImVec2 timeFramePos = bb.Min + ImVec2(spacing, spacing);

  ui::PushFont(layout.font);
  ui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
  ui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});

  ui::SetNextWindowPos(pos);
  ui::SetNextWindowSize(windowSize);

  bool open = true;
  ui::Begin("##gametimemenu", &open,
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration);

  ImGuiWindow *window = ImGui::GetCurrentWindow();

  /*window->DrawList->PathLineTo(window->Pos);
  window->DrawList->PathLineTo(window->Pos + ImVec2(64, 0));
  window->DrawList->PathLineTo(window->Pos + ImVec2(windowSize.x / 2, windowSize.y - 64));
  window->DrawList->PathLineTo(window->Pos + ImVec2(windowSize.x / 2, windowSize.y));
  window->DrawList->PathLineTo(window->Pos + ImVec2(0, windowSize.y));
  window->DrawList->PathFillConvex(ImColor{ bg.clr, bg.clr, bg.clr });

  window->DrawList->PathLineTo(window->Pos + ImVec2(windowSize.x / 2, windowSize.y - 64));
  window->DrawList->PathLineTo(window->Pos + ImVec2(windowSize.x, windowSize.y - 64));
  window->DrawList->PathLineTo(window->Pos + ImVec2(windowSize.x, windowSize.y));
  window->DrawList->PathLineTo(window->Pos + ImVec2(windowSize.x / 2, windowSize.y));
  window->DrawList->PathFillConvex(ImColor{ bg.clr, bg.clr, bg.clr });

  const ImVec2 &wpos = window->Pos + ImVec2(window->WindowBorderSize, window->WindowBorderSize);
  const ImVec2 upOffset{0, 2};
  const ImVec2 sideOffset{2, 0};
  ImColor bgShade{bg.bottomFrameShade, bg.bottomFrameShade, bg.bottomFrameShade};
  ImColor bgFrame{bg.bottomFrame, bg.bottomFrame, bg.bottomFrame};

  ImVec2 last = wpos;
  auto borderLine = [&] (auto p, auto c) { window->DrawList->AddLine(last, p, c, 2); last = p; };
  borderLine(wpos + ImVec2(64, 0), bgShade); // 0-> top left to middle
  borderLine(wpos + ImVec2(windowSize.x * 2 / 4, windowSize.y - 64), bgFrame); // slop top midle to bottom middle 
  borderLine(wpos + ImVec2(windowSize.x, windowSize.y - 64) - sideOffset * 2, bgFrame); // -> line bottom middle to rigth side
  borderLine(wpos + ImVec2(windowSize.x, windowSize.y) - (sideOffset + upOffset) * 2, bgFrame); // | rigth side to rbottom corner
  borderLine(wpos + ImVec2(0, windowSize.y) - upOffset * 2, bgShade); // <- bottom line to left side
  borderLine(wpos, bgShade); // |0  left line to left top corner */

  ui::SetCursorPos(timeFramePos);
  const float speedFactor = GameClock::instance().getGameClockSpeed();
  auto is_interval = [&](float val) { return fabs(speedFactor - val) < 0.1f ? ImGuiButtonFlags_ForcePressed : 0; };
  if (ui::ButtonCtEx("||", buttonSize, is_interval(0.f)))
  {
    GameClock::instance().setGameClockSpeed(0.0f);
  }

  ui::SameLine(0, spacing);
  if (ui::ButtonCtEx(">", buttonSize, is_interval(1.f)))
  {
    GameClock::instance().setGameClockSpeed(1.0f);
  }

  ui::SameLine(0, spacing);
  if (ui::ButtonCtEx(">>", buttonSize, is_interval(2.f)))
  {
    GameClock::instance().setGameClockSpeed(2.0f);
  }

  ui::SameLine(0, spacing);
  if (ui::ButtonCtEx(">>>", buttonSize, is_interval(8.f)))
  {
    GameClock::instance().setGameClockSpeed(8.0f);
  }
  ui::PopFont();
  ui::PopStyleVar(3);

  ui::End();
}