#ifndef LAYOUT_HXX_
#define LAYOUT_HXX_

#include <string>
#include <vector>

struct LayoutData
{
  std::string alignment;             ///< <mandatory> where the element should be placed. e.g. CENTER
  std::string layoutType;            ///< <mandatory> how to layout, default = HORIZONTAL
  std::string layoutParentElementID; ///< elementID of the parent UiElement this group should be aligned to.
  float alignmentOffset = 0.0F;      ///< Offset in percent to the screen point. can be negative
  int padding = 0;                   ///< padding between elements in pixels
  int paddingToParent = 0;           ///< padding between this group and the parent in pixels
  int groupHeight = 0;               ///< <internal> height of all elements in group
  int groupWidth = 0;                ///< <internal> Width of all elements in group
  uint32_t fontSize = 20;            ///< <internal> default font size of all elements in group
  struct ImFont *font = nullptr;     ///< <internal> font pointer for elements
};

#endif
