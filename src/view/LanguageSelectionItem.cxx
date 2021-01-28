#include "LanguageSelectionItem.hxx"
#include "../renderer/iRenderer.hxx"
#include "../util/PixelBuffer.hxx"
#include "../util/Filesystem.hxx"

LanguageSelectionItem::LanguageSelectionItem(std::string language, bool isCompleteTranslated, RGBAColor textColor)
    : m_Language{language}, m_TextColor{textColor}, m_IsCompleteTranslated{isCompleteTranslated}
{
}

void LanguageSelectionItem::draw(iRenderer &renderer) const noexcept
{
  Rectangle target = getBounds();
  const int pictureSize = target.height();

  if (m_IsSelected || !m_IsCompleteTranslated)
  {
    std::string picturePath;
    if (m_IsSelected)
      picturePath = "resources/images/ui/general/check.png";
    else
      picturePath = "resources/images/ui/general/warning.png";

    PixelBuffer pb = fs::readPNGFile(fs::getBasePath() + picturePath);
    pb.scale(((float)pictureSize - 2 * PADDING) / pb.bounds().height());
    Rectangle rect{pb.bounds().getBounds()};
    rect.translateY(target.y1() + m_YOffset + PADDING);
    rect.translateX(PADDING);
    renderer.drawPicture(std::move(rect), pb.data());
  }

  PixelBuffer pb = fs::readPNGFile(fs::getBasePath() + "resources/images/ui/selectionLayout/selection_item.png");
  Rectangle o_bounds = pb.bounds();
  Rectangle selectionPart =
      Rectangle{target.x1() + pictureSize, target.y1() + m_YOffset, target.x2() - PADDING, target.y2() + m_YOffset};
  const int backgroundCnt = 2;
  const bool highlighted = m_IsSelected || m_IsHovered;
  const int width = pb.bounds().width() / backgroundCnt;
  Rectangle crop_filter{highlighted ? 0 : width, 0, highlighted ? width : width * backgroundCnt, pb.bounds().height()};
  pb.crop(std::move(crop_filter));
  pb.expandCenter(Rectangle{selectionPart});
  renderer.drawPicture(selectionPart, pb.data());

  renderer.drawText(
      m_Language.c_str(), RGBAColor(0xFFFFFFFF),
      Rectangle{target.x1() + pictureSize + PADDING, target.y1() + m_YOffset, target.x2() - PADDING, target.y2() + m_YOffset},
      PositionType::Normal);
}