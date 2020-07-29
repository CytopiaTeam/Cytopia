#ifndef I_MODEL_HXX
#define I_MODEL_HXX

#include <memory>

class iModel
{
public:
  virtual ~iModel() = 0;
};

using iModelPtr = std::unique_ptr<iModel>;

#endif // I_MODEL_HXX
