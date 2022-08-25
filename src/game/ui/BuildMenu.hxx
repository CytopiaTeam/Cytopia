#ifndef BUILDMENU_HXX_
#define BUILDMENU_HXX_

#include "imgui.h"
#include "UIManager.hxx"

struct TileData;

struct BuildMenuButton
{
  using Ptr = std::shared_ptr<BuildMenuButton>;
  using Items = std::vector<Ptr>;

  bool m_open = false;       // draw subbuttons
  bool m_background = false; // draw background

  std::string m_texstr;   // texture name
  std::string m_id;       // unique id
  std::string m_action;   // additional action
  std::string m_tiletype; // which tiletype place on map

  SDL_Texture *m_tex; // sdl texture
  ImVec2 m_uv0 = ImVec2{0, 0};
  ImVec2 m_uv1 = ImVec2{1, 1};         // texture sub coords
  ImSpan2i m_texSize;                  // texutre size
  BuildMenuButton *m_parent = nullptr; // parent button, need for recursive logic

  ImVec4 m_destRect; // texture additional offsets
  ImVec2 m_btnSize;  // button frame rect

  Items m_buttons; // sub items

  // ctor simple button with sub items
  BuildMenuButton(const std::string &tx, const std::string &category);

  // ctor button which can change tile type
  BuildMenuButton(const std::string &tx, const std::string &category, const TileData &tile);

  // add new subbutton
  Ptr addCategoryButton(const std::string &tx, const std::string &id, const TileData &tile)
  {
    return addButtonImpl(tx, id, tile);
  }

  // add button which change tile type
  Ptr addTileButton(const std::string &tx, const std::string &id, const TileData &tile) { return addButtonImpl(tx, id, tile); }

  // add button with special action, like water or demolish
  Ptr addActionButton(const std::string &tx, const std::string &action, const std::string &tooltip);

  // create new button with params
  Ptr addButtonImpl(const std::string &tx, const std::string &id, const TileData &tile);

  /**
  * @brief takes an SDL_Rect, default button width and height, and image width and height
  *        and scales the image to fit on a button of the default button size (maintaining the 
  *        aspect ration of the original image).
  * @param ret the address of a rect that will contain the size of the scaled image
  * @param btnW the default button width
  * @param btnH the default button height
  * @param imgW the width of the image to scale
  * @param imgH the height of the image to scale
  */
  void scaleCenterImage(SDL_Rect &ret, int btnW, int btnH, int imgW, int imgH) const;

  const ImVec2 &getBtnSize() const { return m_btnSize; }
  const std::string &getId() const { return m_id; }
  const Items &getButtons() const { return m_buttons; }

  // hide items recursively
  void hideItems();
};

class BuildMenu final : public GameMenu
{
public:
  BuildMenu();

  void draw() const override;

  // read config and crete build menu
  void createSubMenus();

  // show/hide button with items, hide another buttons
  void setItemVisible(BuildMenuButton::Ptr btn, bool visible);

  // on click button
  void onClick(BuildMenuButton::Ptr btn);

  // click on button with action param
  void onAction(const std::string &action, bool checked);

  // click on button with tile type
  void onChangeTileType(const std::string &id, bool checked);

  // hide all buttons
  void closeSubmenus() override;
  void clearState();

  const std::string &getId() const { return m_id; }
  const BuildMenuButton::Items &getButtons() const { return m_buttons; }
  float getTooltipDelay() const { return m_tooltipDelay; }

private:
  BuildMenuButton::Items m_buttons;
  std::string m_id = "Main";
  ImVec2 m_btnSize;
  float m_tooltipDelay = 3.f;
};

#endif // BUILDMENU_HXX_
