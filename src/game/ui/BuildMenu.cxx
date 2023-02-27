#include "BuildMenu.hxx"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#include <UIManager.hxx>
#include <Settings.hxx>
#include <GameStates.hxx>
#include <MapLayers.hxx>
#include <mapEdit.hxx>
#include <tileData.hxx>
#include "../engine/ResourcesManager.hxx"

namespace ui = ImGui;

BuildMenu::BuildMenu()
{
  // function create new big category button
  auto main_button = [this](const char *tx, const char *id)
  {
    m_buttons.emplace_back(std::make_shared<BuildMenuButton>(tx, id));
    return m_buttons.back();
  };

  // special case for construction button
  auto constrution = main_button("Button_ConstructionMenu", "Construction");
  constrution->addActionButton("Button_ConstructionMenu_Demolish", "Demolish", "Demolish");
  constrution->addActionButton("Button_ConstructionMenu_RaiseTerrain", "RaiseTerrain", "Raise Terrain");
  constrution->addActionButton("Button_ConstructionMenu_LowerTerrain", "LowerTerrain", "Lower Terrain");
  constrution->addActionButton("Button_ConstructionMenu_LevelTerrain", "LevelTerrain", "Level Terrain");
  constrution->addActionButton("Button_ConstructionMenu_DeZone", "DeZone", "De-zone");
  constrution->addActionButton("Button_ConstructionMenu_Water", "Water", "Water");

  // add big caterogries buttons
  main_button("Button_NatureMenu", "Nature");
  main_button("Button_RoadsMenu", "Roads");
  main_button("Button_ZonesMenu", "Zones");
  main_button("Button_RecreationMenu", "Recreation");
  main_button("Button_PowerMenu", "Power");
  main_button("Button_WaterMenu", "Waterworks");
  main_button("Button_EmergencyMenu", "Emergency");
  main_button("Button_DebugMenu", "Debug");

  // save texture size
  ImSpan2i size;
  SDL_QueryTexture(m_buttons.front()->m_tex, nullptr, nullptr, &size.w, &size.h);
  m_btnSize = ImVec2(size.w, size.h);

  createSubMenus();
}

namespace detail
{
float getItemSpan(int deep) { return deep == 0 ? 16.f : 6.f; }
} // namespace detail

// recusively draw categories
template <class Holder> void drawSubmenu(ImVec2 pos, float categoryOffset, const Holder &holder, BuildMenu *menu, int deep)
{
  if (holder.getButtons().empty())
    return;

  const ImVec2 &frameSize = holder.getButtons().front()->getBtnSize();
  const float frameFullWidth = frameSize.x + detail::getItemSpan(deep);
  ImVec2 screenSize = ui::GetIO().DisplaySize;

  auto &uiManager = UIManager::instance();

  ImVec2 windowSize;
  ImVec2 itemSpacing;
  ImVec2 nextPos;
  ImVec2 nextOffset{0, 0};
  bool verticalMenu =
      (uiManager.buildMenuLayout() == BUILDMENU_LAYOUT::LEFT || uiManager.buildMenuLayout() == BUILDMENU_LAYOUT::RIGHT);
  itemSpacing = verticalMenu ? itemSpacing = ImVec2(0, detail::getItemSpan(deep)) : ImVec2(detail::getItemSpan(deep), 0);
  windowSize = verticalMenu ? ImVec2(frameSize.x, frameFullWidth * (float)holder.getButtons().size())
                            : ImVec2(frameFullWidth * (float)holder.getButtons().size(), frameSize.y);

  switch (uiManager.buildMenuLayout())
  {
  case BUILDMENU_LAYOUT::BOTTOM:
    nextOffset.y = -categoryOffset;
    break;
  case BUILDMENU_LAYOUT::TOP:
    nextOffset.y = +categoryOffset;
    break;
  case BUILDMENU_LAYOUT::LEFT:
    nextOffset.x = categoryOffset;
    break;
  default:
    nextOffset.x = -categoryOffset;
  }

  const auto &layout = uiManager.getLayouts()["BuildMenuButtons"];

  ui::SetNextWindowPos(pos);
  ui::SetNextWindowSize(windowSize);

  // Set frame style, font size, noborder, transparent
  ui::PushFont(layout.font);
  ui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{0, 0});
  ui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
  ui::PushStyleVar(ImGuiStyleVar_ItemSpacing, itemSpacing);

  // every subcaterory is different widget, it need unique id
  bool open = true;
  std::string wId = std::string("##BuildMenu_") + holder.getId() + std::to_string(deep);
  // begin buttons
  ui::Begin(wId.c_str(), &open,
            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoScrollWithMouse);

  // setup screen size cliip rect that handle mouse event outside
  ui::PushClipRect(ImVec2{0, 0}, screenSize, false);
  BuildMenuButton::Ptr nextMenuLevel = nullptr;
  int idx = 0;

  // draw buttons, each button need unique id
  std::array<char, 128> id_str = {0};
  for (auto btn : holder.getButtons())
  {
    snprintf(id_str.data(), 128, "%s_%d", holder.getId().c_str(), ++idx);

    ImVec2 imgPos{btn->m_destRect.x, btn->m_destRect.y};
    ImVec2 imgSize{btn->m_destRect.z, btn->m_destRect.w};

    // draw bg | pressed state
    ImGuiButtonFlags flags = btn->m_open ? ImGuiButtonFlags_ForcePressed : 0;
    flags |= btn->m_background ? 0 : ImGuiButtonFlags_NoBackground;

    if (ui::ImageButtonCt(btn->m_tex, flags, frameSize, imgPos, imgSize, btn->m_uv0, btn->m_uv1, -1, ImVec4(0, 0, 0, 0)))
    {
      btn->m_open = !btn->m_open;
      menu->setItemVisible(btn, btn->m_open); // close all other buttons and open only me
      menu->onClick(btn);
    }

    if (ui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
    {
      if (ui::GetHoveredTimer() > menu->getTooltipDelay())
      {
        ui::SetTooltip("%s", btn->getId().c_str());
      }

      if (deep == 0)
      {
        static const std::string textureHoveringTypeStr = "Texture_Hovering";
        btn->m_tex = ResourcesManager::instance().getUITexture(btn->m_texstr, textureHoveringTypeStr);
      }
    }
    else
    {
      if (deep == 0)
      {
        btn->m_tex = ResourcesManager::instance().getUITexture(btn->m_texstr);
      }
    }

    // calc next subcategory position center
    if (btn->m_open && !btn->getButtons().empty())
    {
      nextMenuLevel = btn;
      const ImVec2 &nextFrameSize = btn->getButtons().front()->getBtnSize();
      const float nextFrameFullWidth = (verticalMenu ? nextFrameSize.y : nextFrameSize.x) + detail::getItemSpan(deep + 1);
      const float nextCategoryOffset = (nextFrameFullWidth * (float)btn->getButtons().size()) / 2;

      switch (uiManager.buildMenuLayout())
      {
      case BUILDMENU_LAYOUT::BOTTOM:
        nextPos.x = ui::GetCursorScreenPos().x + ((float)(idx - 1) * frameFullWidth) - nextCategoryOffset + frameFullWidth / 2;
        nextPos.y = pos.y - nextFrameFullWidth;
        break;
      case BUILDMENU_LAYOUT::TOP:
        nextPos.x = ui::GetCursorScreenPos().x + ((float)(idx - 1) * frameFullWidth) - nextCategoryOffset + frameFullWidth / 2;
        nextPos.y = pos.y + frameFullWidth + detail::getItemSpan(1);
        break;
      case BUILDMENU_LAYOUT::LEFT:
        nextPos.x = pos.x + frameFullWidth + detail::getItemSpan(1);
        nextPos.y = ui::GetCursorScreenPos().y - nextCategoryOffset - frameFullWidth / 2;
        break;
      default:
        nextPos.x = pos.x - nextFrameFullWidth;
        nextPos.y = ui::GetCursorScreenPos().y - nextCategoryOffset - frameFullWidth / 2;
        break;
      }
    }

    if (!verticalMenu)
    {
      ui::SameLine();
    }
  }

  ui::PopClipRect(); // back common clip rect

  ui::End(); // end buttons

  ui::PopStyleVar(4);
  ui::PopFont();

  if (nextMenuLevel)
  {
    drawSubmenu(nextPos, frameSize.y + 10.f, *nextMenuLevel, menu, deep + 1);
  }
}

void BuildMenu::draw() const
{
  ImVec2 screenSize = ui::GetIO().DisplaySize;
  auto _self = const_cast<BuildMenu *>(this); // const_cast here because button open state can be changed

  ImVec2 pos{0, 0};
  const auto &uiManager = UIManager::instance();
  const float frameSize = m_btnSize.x + detail::getItemSpan(0);
  const float buttonsSize = (float)m_buttons.size();

  switch (uiManager.buildMenuLayout())
  {
  case BUILDMENU_LAYOUT::BOTTOM:
    pos = {(screenSize.x - buttonsSize * frameSize) / 2, screenSize.y - m_btnSize.y};
    break;
  case BUILDMENU_LAYOUT::TOP:
    pos = ImVec2{(screenSize.x - buttonsSize * frameSize) / 2, 0};
    break;
  case BUILDMENU_LAYOUT::LEFT:
    pos = ImVec2{0, (screenSize.y - buttonsSize * frameSize) / 2};
    break;
  default:
    pos = {screenSize.x - m_btnSize.x, (screenSize.y - buttonsSize * frameSize) / 2};
  }

  drawSubmenu(pos, frameSize, *this, _self, 0);
}

void BuildMenu::createSubMenus()
{
  auto debugBtnIt = std::find_if(m_buttons.begin(), m_buttons.end(), [](auto &btn) { return btn->getId() == "Debug"; });
  if (debugBtnIt == m_buttons.end())
  {
    return;
  }

  std::map<std::string, BuildMenuButton::Ptr> categories;
  for (auto btn : m_buttons)
  {
    categories[btn->getId()] = btn;
  }

  for (const auto &[tx, tileData] : TileManager::instance().getAllTileData())
  {
    const std::string &category = tileData.category;
    const std::string subCategory =
        (tileData.subCategory == tileData.category) ? (tileData.category + "_" + tileData.subCategory) : tileData.subCategory;
    const std::string &title = tileData.title;
    const std::string &tooltip = !title.empty() ? title : subCategory.empty() ? category : subCategory;

    // Skip all items that have no button group
    if (category == "Water" || category == "Terrain")
    {
      continue;
    }

    if (!subCategory.empty())
    {
      auto it = categories.find(subCategory);
      if (it == categories.end())
      {
        auto category_it = categories.find(category);
        BuildMenuButton::Ptr btn;
        if (category_it == categories.end())
        {
          btn = (*debugBtnIt)->addCategoryButton(tx, tooltip, tileData);
          categories[category] = btn;
        }
        else
        {
          btn = category_it->second;
        }

        auto subBtn = btn->addCategoryButton(tx, tooltip, tileData);
        categories[subCategory] = subBtn;

        subBtn->addTileButton(tx, tooltip, tileData);
      }
      else
      {
        it->second->addTileButton(tx, tooltip, tileData);
      }
    }
    else
    {
      auto it = categories.find(category);
      if (it == categories.end())
      {
        auto btn = (*debugBtnIt)->addTileButton(tx, tooltip, tileData);
        categories[category] = btn;
      }
      else
      {
        it->second->addTileButton(tx, tooltip, tileData);
      }
    }
  }
}

void BuildMenu::setItemVisible(BuildMenuButton::Ptr btn, bool visible)
{
  if (!visible)
  {
    for (auto it : btn->getButtons())
    {
      it->hideItems();
    }
  }
  else
  {
    for (auto it : m_buttons)
    {
      it->hideItems();
    }

    BuildMenuButton *parent = btn.get();
    while (parent)
    {
      parent->m_open = true;
      parent = parent->m_parent;
    }
  }
}

void BuildMenu::onClick(BuildMenuButton::Ptr btn)
{
  clearState();

  if (!btn->m_action.empty())
  {
    onAction(btn->m_action, btn->m_open);
  }

  const bool hasAction = !btn->m_tiletype.empty();
  const bool isCategoryButton = btn->getButtons().size() > 0;
  if (hasAction && !isCategoryButton)
  {
    onChangeTileType(btn->m_tiletype, btn->m_open);
  }
}

void BuildMenu::onAction(const std::string &action, bool checked)
{
  if (action == "Demolish")
  {
    demolishMode = checked;
    highlightSelection = checked;

    if (demolishMode)
    {
      GameStates::instance().placementMode = PlacementMode::RECTANGLE;
      GameStates::instance().demolishMode = DemolishMode::DEFAULT;
    }
  }
  else if (action == "LowerTerrain")
  {
    terrainEditMode = checked ? TerrainEdit::LOWER : TerrainEdit::NONE;
    highlightSelection = checked;
  }
  else if (action == "RaiseTerrain")
  {
    terrainEditMode = checked ? TerrainEdit::RAISE : TerrainEdit::NONE;
    highlightSelection = checked;
  }
  else if (action == "LevelTerrain")
  {
    terrainEditMode = checked ? TerrainEdit::LEVEL : TerrainEdit::NONE;
    highlightSelection = checked;
  }
  else if (action == "DeZone")
  {
    demolishMode = checked;
    highlightSelection = checked;
    if (demolishMode)
    {
      GameStates::instance().placementMode = PlacementMode::RECTANGLE;
      GameStates::instance().demolishMode = DemolishMode::DE_ZONE;
    }
  }
  else if (action == "underground_pipes")
  {
    GameStates::instance().layerEditMode = checked ? LayerEditMode::BLUEPRINT : LayerEditMode::TERRAIN;
  }
  else if (action == "Water")
  {
    tileToPlace = checked ? "water" : "";
    highlightSelection = checked;
    GameStates::instance().placementMode = PlacementMode::RECTANGLE;
  }
  else
  {
    terrainEditMode = TerrainEdit::NONE;
    highlightSelection = false;
  }
}

void BuildMenu::onChangeTileType(const std::string &actionParameter, bool checked)
{
  if (actionParameter.empty())
  {
    tileToPlace = "";
    highlightSelection = false;
  }

  tileToPlace = checked ? actionParameter : "";
  highlightSelection = checked;
  if (GameStates::instance().layerEditMode == LayerEditMode::BLUEPRINT)
  {
    GameStates::instance().layerEditMode = LayerEditMode::TERRAIN;
    MapLayers::setLayerEditMode(GameStates::instance().layerEditMode);
  }

  if (!tileToPlace.empty())
  {
    if (TileManager::instance().getTileData(tileToPlace))
    {
      switch (TileManager::instance().getTileData(tileToPlace)->tileType)
      {
      case +TileType::DEFAULT:
        GameStates::instance().placementMode = PlacementMode::SINGLE;
        break;

      case +TileType::ROAD:
      case +TileType::AUTOTILE:
      case +TileType::POWERLINE:
        GameStates::instance().placementMode = PlacementMode::LINE;
        break;

      case +TileType::GROUNDDECORATION:
      case +TileType::WATER:
      case +TileType::ZONE:
        GameStates::instance().placementMode = PlacementMode::RECTANGLE;
        break;

      case +TileType::UNDERGROUND:
        GameStates::instance().placementMode = PlacementMode::LINE;
        GameStates::instance().layerEditMode = LayerEditMode::BLUEPRINT;
        MapLayers::setLayerEditMode(GameStates::instance().layerEditMode);
        break;

      default:
        break;
      }
    }
  }
}

void BuildMenu::closeSubmenus()
{
  clearState();

  for (auto &btn : m_buttons)
  {
    btn->hideItems();
  }
}

void BuildMenu::clearState()
{
  tileToPlace = "";
  highlightSelection = false;
  terrainEditMode = TerrainEdit::NONE;
}

BuildMenuButton::BuildMenuButton(const std::string &tx, const std::string &id) : m_texstr(tx), m_id(id)
{
  m_tex = ResourcesManager::instance().getUITexture(tx);
  SDL_QueryTexture(m_tex, nullptr, nullptr, &m_texSize.w, &m_texSize.h);
  m_btnSize = ImVec2(m_texSize.w, m_texSize.h);
  m_destRect = ImVec4(0, 0, (float)m_texSize.w, (float)m_texSize.h);
}

BuildMenuButton::BuildMenuButton(const std::string &tx, const std::string &id, const TileData &tile)
    : m_background(true), m_texstr(tx), m_id(id), m_tiletype(tx)
{
  int bWid = Settings::instance().subMenuButtonWidth;  //UI button width for sub menues
  int bHei = Settings::instance().subMenuButtonHeight; //UI button height for sub menues

  SDL_Rect destRect;
  scaleCenterImage(destRect, bWid, bHei, tile.tiles.clippingWidth, tile.tiles.clippingHeight);

  m_tex = TileManager::instance().getTexture(tx);
  SDL_QueryTexture(m_tex, nullptr, nullptr, &m_texSize.w, &m_texSize.h);
  m_uv0 = ImVec2((float)(tile.tiles.clippingWidth * tile.tiles.offset) / (float)m_texSize.w, 0.f);
  m_uv1 = ImVec2(m_uv0.x + (float)(tile.tiles.clippingWidth) / (float)m_texSize.w,
                 m_uv0.y + ((float)tile.tiles.clippingHeight) / (float)m_texSize.h);
  m_btnSize = ImVec2(bWid, bHei);
  m_destRect = ImVec4((float)destRect.x, (float)destRect.y, (float)destRect.w, (float)destRect.h);
}

BuildMenuButton::Ptr BuildMenuButton::addActionButton(const std::string &tx, const std::string &action,
                                                      const std::string &tooltip)
{
  auto btn = std::make_shared<BuildMenuButton>(tx, action);
  btn->m_id = tooltip;
  btn->m_background = true;
  btn->m_parent = this;
  btn->m_action = action;
  m_buttons.push_back(btn);
  return btn;
}

BuildMenuButton::Ptr BuildMenuButton::addButtonImpl(const std::string &tx, const std::string &id, const TileData &tile)
{
  auto btn = std::make_shared<BuildMenuButton>(tx, id, tile);
  btn->m_parent = this;
  m_buttons.push_back(btn);
  return btn;
}

void BuildMenuButton::scaleCenterImage(SDL_Rect &ret, int btnW, int btnH, int imgW, int imgH) const
{
  if (imgW > imgH)
  {
    float ratio = (float)imgH / (float)imgW;
    ret.w = btnW;
    ret.h = static_cast<int>(ceil((float)btnH * ratio));
    ret.x = 0;
    ret.y = btnH - ret.h;
  }
  else
  {
    float ratio = (float)imgW / (float)imgH;
    ret.w = static_cast<int>(ceil((float)btnW * ratio));
    ret.h = btnH;
    ret.x = static_cast<int>(ceil(((float)(btnW - ret.w)) / 2));
    ret.y = 0;
  }
}

void BuildMenuButton::hideItems()
{
  m_open = false;
  for (auto btn : m_buttons)
  {
    btn->hideItems();
  }
}
