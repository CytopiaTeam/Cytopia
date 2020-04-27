#ifndef I_VIEW_HXX
#define I_VIEW_HXX

#include <memory>

class iRenderer;
class Rectangle;
class GameService;

class iView
{
public:
  virtual void draw(iRenderer &) const noexcept = 0;
  virtual const Rectangle & getBounds() const noexcept = 0;
  virtual void setup() noexcept = 0;
  virtual void bindHandlers(GameService & context) noexcept = 0;
  virtual ~iView() = 0;
private:
  virtual void setBounds(Rectangle &&) noexcept = 0;
};

using iViewPtr = std::shared_ptr<iView>;

#endif // I_VIEW_HXX
