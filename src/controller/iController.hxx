#ifndef I_CONTROLLER_HXX_
#define I_CONTROLLER_HXX_

#include <memory>

class iController {
public:
  virtual ~iController() = 0;
};

using iControllerPtr = std::unique_ptr<iController>;

#endif // I_CONTROLLER_HXX_
