#ifndef SINGLETON_HXX_
#define SINGLETON_HXX_

/**
 * @class Singleton
 * @brief Abstract Singleton implementation
 */
template <typename T> class Singleton
{

public:
  /**
   * @brief Get an instance of the singleton
   */
  static T &instance(void)
  {
    static T singleton;
    return singleton;
  }

protected:
  Singleton() noexcept = default;
  ~Singleton() noexcept = default;
};

#endif
