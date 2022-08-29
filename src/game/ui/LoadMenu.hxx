#ifndef LOADMENU_HXX_
#define LOADMENU_HXX_

#include "UIManager.hxx"

class LoadMenu : public GameMenu
{
public:
  enum Result
  {
    e_none,
    e_close,
    e_load_file,
    e_delete_file
  };

  LoadMenu();
  ~LoadMenu();
  void draw() const override;

  const std::string &filename() const { return m_filename; }
  Result result() const { return m_result; }

private:
  mutable std::string m_filename;
  mutable Result m_result;
};

#endif // LOADMENU_HXX_