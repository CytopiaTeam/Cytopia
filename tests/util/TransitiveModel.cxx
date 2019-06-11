#include <bitset>

/* This is our data model for ButtonState */
struct __ButtonState
{
  enum Operations {
    CLICKING, HOVERING, DISABLING, APPEARING,
    MASKING, ANIMATING, FOCUSING, ACTIVATING
  };
  std::bitset<8> m_flags;
  std::string m_text;
};

/* This is ButtonState */
class ButtonState;

/* This is our Button view */
class ButtonView
{

};
