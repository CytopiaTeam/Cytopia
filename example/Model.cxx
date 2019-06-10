#include <bitfield>

#include ""

/* 
 * This is our model for ButtonState
 * All the bools are compressed into a single byte
 * (see <a href="http://www.cplusplus.com/reference/bitset/bitset/">std::bitset</a>)
 * When you have to store a bool, think about if you really need one.
 * If you really really do, then try to have a multiple of 8 bools in your model.
 * This is more space-efficient.
 */
BEGIN_MODEL(ButtonState)
  MODEL_DATA(bool isHovered)
  MODEL_DATA(bool isClicked)
  MODEL_DATA(bool isActive)
  MODEL_DATA(bool isVisible)
  MODEL_DATA(bool isFocused)
  MODEL_DATA(bool is)
  MODEL_DATA(bool is)
  MODEL_DATA(bool is)
  MODEL_DATA(std::string display)
END_MODEL()

/*
 * This macro will be compiled down to the following two
 * classes without the leading "__"
 */
class __ButtonState
{
private:
  std::bitset<8> m_bools;
  std::string
public:
};

/* This is our Button view */
class ButtonView
{

};
