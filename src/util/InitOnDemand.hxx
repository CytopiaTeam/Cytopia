#ifndef INITONDEMAND_HXX_
#define INITONDEMAND_HXX_

template<class T>
struct InitOnDemand
{
public:
  ~InitOnDemand() { demandDestroy(); }

  // Note: access before initialization or after deinitialization is not alloved because of type-safety
  // If you need explicit check use get() for that
  operator T *()    const { return getObj(); }
  T &operator *()   const { return *getObj(); }
  T *operator ->()  const { return getObj(); }
  T *get()          const { return inited ? getObj() : nullptr; }
  explicit operator bool() const { return inited; }

  /// special inplace new/delete operators (used in dyntab)
  enum { _NEW_INPLACE = 1 };
  template <typename... Args>
  T *demandInit(const Args & ... args)
  {
    if (!inited)
    {
      new (objBuf, _NEW_INPLACE) T(args...);
      inited = true;
    }
    return getObj();
  }
  void demandDestroy()
  {
    if (inited)
    {
      getObj()->~T();
      inited = false;
    }
  }

private:

  T *getObj() const { return (T *)objBuf; }
private:

  alignas(T) char objBuf[sizeof(T)];
  bool inited = false;
};

#endif // INITONDEMAND_HXX_
