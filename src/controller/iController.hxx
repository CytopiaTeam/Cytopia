#ifndef I_CONTROLLER_HXX
#define I_CONTROLLER_HXX

#include <memory>

class iController
{
public:
  iController();
  iController(iController&&);
  iController & operator=(iController&&);
  virtual ~iController() = 0;
  
  iController(const iController &) = delete;
  iController & operator=(const iController &) = delete;
};

using iControllerPtr = std::unique_ptr<iController>;

#endif // I_CONTROLLER_HXX
