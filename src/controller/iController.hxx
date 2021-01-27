#ifndef I_CONTROLLER_HXX_
#define I_CONTROLLER_HXX_

#include <memory>
#include "../GameService.hxx"

class iController
{
public:
  virtual ~iController() = 0;

  /**
   * @brief   Called when a Layout changed the bounding box
   */
  virtual void setup(GameService &) noexcept = 0;
};

using iControllerPtr = std::unique_ptr<iController>;

#endif // I_CONTROLLER_HXX_
