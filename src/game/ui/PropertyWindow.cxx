#include "PropertyWindow.hxx"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#include <Point.hxx>
#include <MapFunctions.hxx>

namespace ui = ImGui;

bool PropertyWindow::m_show = false;

void PropertyWindow::drawProperty(const char *uname, uint32_t l, const char *title, const std::string &data)
{
  ui::TableNextRow();
  ui::TableSetColumnIndex(0);
  ui::AlignTextToFramePadding();
  char wid[128]; sprintf(wid, "##layer_%s_%d", uname, l);
  ui::TreeNodeEx(wid, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, title);

  ui::TableSetColumnIndex(1);
  ImGui::Text(data.c_str());
  ImGui::NextColumn();
}

void PropertyWindow::drawProperty(const char *uname, uint32_t l, const char *title, int value)
{
  ui::TableNextRow();
  ui::TableSetColumnIndex(0);
  ui::AlignTextToFramePadding();
  char wid[128]; sprintf(wid, "##layer_%s_%d", uname, l);
  ui::TreeNodeEx(wid, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, title);

  ui::TableSetColumnIndex(1);
  ImGui::InputScalar("", ImGuiDataType_S32, (void *)&value, NULL, NULL, "%d", ImGuiInputTextFlags_ReadOnly);
  ImGui::NextColumn();
}

void PropertyWindow::showTileInfo(const Point &isoCoord)
{
  // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
  ui::PushID("##propobj");

  const MapNode &mapNode = MapFunctions::instance().getMapNode(isoCoord);

  ImGui::TableNextRow();
  ImGui::TableSetColumnIndex(0);
  ImGui::AlignTextToFramePadding();
  bool objopen = ImGui::TreeNodeEx("Object", ImGuiTreeNodeFlags_DefaultOpen, "Object_%u", 1);
  ImGui::TableSetColumnIndex(1);
  ImGui::Text("");

  if (objopen)
  {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();
    ui::TreeNodeEx("IsoCoord", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "IsoCoord");

    ui::TableSetColumnIndex(1);
    int col_width = ui::GetColumnWidth();
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "X:");
    ui::SameLine(); ui::SetNextItemWidth(col_width * 0.3); ImGui::InputScalar("", ImGuiDataType_S32, (void *)&isoCoord.x, NULL, NULL, "%d", ImGuiInputTextFlags_ReadOnly);
    ui::SameLine(); ui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), "Y:");
    ui::SameLine(); ui::SetNextItemWidth(col_width * 0.3); ImGui::InputScalar("", ImGuiDataType_S32, (void *)&isoCoord.y, NULL, NULL, "%d", ImGuiInputTextFlags_ReadOnly);

#define $(a) {a, #a}
    struct
    {
      uint32_t type;
      const char *name;
    } layers[10] = {$(UNDERGROUND), $(TERRAIN), $(ZONE), $(ROAD), $(WATER), $(MOVABLE_OBJECTS), $(BUILDINGS), $(POWERLINES), $(FLORA), $(GROUND_DECORATION)};
#undef $

    auto haveData = [] (auto *tiled) { return tiled && (!tiled->author.empty() || !tiled->category.empty() || !tiled->subCategory.empty() || !tiled->title.empty()); };

    for (auto [l, name] : layers)
    {
      ImGui::TableNextRow();
      ui::TableSetColumnIndex(0);
      auto &layerd = mapNode.getMapNodeDataForLayer((Layer)l);
      const TileData *tiled = layerd.tileData;

      char wid[128]; sprintf(wid, "##layer_%d", l);
      if (haveData(tiled) && ImGui::TreeNodeEx(wid, ImGuiTreeNodeFlags_DefaultOpen, "[L]%s", name))
      {
        ImGui::PushID(Layer::TERRAIN); // Use field index as identifier.
        drawProperty("layer_tile", l, "TileID", layerd.tileID);
        drawProperty("layer_author", l, "Author", tiled ? tiled->author : "");
        drawProperty("layer_category", l, "Category", tiled ? tiled->category : "");
        drawProperty("layer_subcategory", l, "Subcategory", tiled ? tiled->subCategory : "");
        drawProperty("layer_title", l, "Title", tiled ? tiled->title : "");

        int def = 0;
        drawProperty("tile_pollution", l, "Pollution", tiled ? tiled->pollutionLevel : def);
        drawProperty("tile_crimeLevel", l, "Crime Level", tiled ? tiled->crimeLevel : def);
        drawProperty("tile_fireHazardLevel", l, "Fire/HazardLevel", tiled ? tiled->fireHazardLevel : def);
        drawProperty("tile_inhabitants", l, "Inhabitants", tiled ? tiled->inhabitants : def);
        drawProperty("tile_happiness", l, "Happiness", tiled ? tiled->happiness : def);
        drawProperty("tile_educationLevel", l, "Education Level", tiled ? tiled->educationLevel : def);

        ui::PopID();

        ui::TreePop();
      }
    }
    ui::TreePop();
  }
  ui::PopID();
}

void PropertyWindow::showInfo(const Point &isoCoord)
{
  ui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
  if (!ui::Begin("Property editor", &m_show))
  {
    ui::End();
    return;
  }


  if (isoCoord.x < 0 && isoCoord.y < 0) {
    ui::End();
    return;
  }

  ui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
  if (ui::BeginTable("split", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
  {
    showTileInfo(isoCoord);
    ui::EndTable();
  }

  ui::PopStyleVar();
  ui::End();
}