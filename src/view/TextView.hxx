#ifndef TEXT_VIEW_HXX
#define TEXT_VIEW_HXX

#include "iViewElement.hxx"
#include "../util/Color.hxx"
#include "../renderer/iRenderer.hxx"
#include "../model/SettingsModel.hxx"

class TextView : public iViewElement, public SettingsModelListener
{
  const std::string m_TextKey;
  std::string m_Text;
  RGBAColor m_Color;

public:
  TextView(const std::string &, const std::string &, RGBAColor = Palette::White);
  virtual void draw(iRenderer &) const noexcept override;
  virtual ~TextView() override;
  void setup(class GameService &context) noexcept final;
  void update(SettingsModelListener::Notification) noexcept final;
};

using TextViewPtr = std::shared_ptr<TextView>;

#endif // TEXT_VIEW_HXX
