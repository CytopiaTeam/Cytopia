#ifndef LANGUAGE_SELECTION_ITEM_HXX
#define LANGUAGE_SELECTION_ITEM_HXX

#include "SelectionItem.hxx"
#include "../util/Color.hxx"
#include <string>

class LanguageSelectionItem : public SelectionItem
{
private:
  std::string m_Language;
  RGBAColor m_TextColor;
  bool m_IsCompleteTranslated;
  static constexpr int PADDING = 8;

public:
  LanguageSelectionItem(std::string language, bool isCompleteTranslated, RGBAColor textColor = Palette::White);
  void draw(iRenderer &) const noexcept override;
  ~LanguageSelectionItem() = default;
};

#endif // LANGUAGE_SELECTION_ITEM_HXX
