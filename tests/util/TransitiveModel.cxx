#include <catch2/catch.hpp>
#include <bitset>
using Catch::Matchers

/* This is our data model for a Button */
struct ButtonModel
{
  enum Operations {
    CLICK, HOVER, DISABLE, DISPLAY,
    VIEW, ANIMATE, FOCUS, PRESS,
    ACTIVATE
  };
  /* This is our data representation
   * Bit 0: Is the button active 
   * Bit 1: Is the button hovered
   * Bit 2: Is the button disabled
   * Bit 3: Is the button displayed
   * Bit 4: Is the button visible
   * Bit 5: Is the button being animated
   * Bit 6: Is the button being focused
   * Bit 7: Is the button being pressed
   * Bit 8: Is the button 
   * m_text: The display text
   * Why do we even bother with all these unused bits?
   * Well it's better to use a multiple of 8 bits
   * because the smallest possible chunk of data
   * in memory is 1 byte. Even a bool is 1 byte.
   * */
  std::bitset<8> m_flags;
  std::string m_text;
};

/* This is our state for a Button
 * One of the advantages of TransitiveModels
 * is that we could implement many different states
 * which all use the same ButtonModel, but triggers
 * events differently. Moreover, our Observers
 * would work for all of these different states. */
class ButtonState : TransitiveModel<ButtonModel>
{
  public:
    /* Here, we implement the operations */
    void click(void) noexcept
    { 
      updateObservers(Transition{ ButtonModel::CLICK });
      /* When we click, we mark our button as "active" */
      if(not m_flags[0])
      {
        m_flags |= 0x1 << 0;
        notifyObservers(Transition{ ButtonModel::ACTIVATE });
      }
      /* When we click, we mark our button as "hovered" */
      if(not m_flags[1])
      {
        m_flags |= 0x1 << 1;
        notifyObservers(Transition{ ButtonModel::HOVER });
      }
      /* When we click, we mark our button as "focused" */
      if(not m_flags[6])
      {
        m_flags |= 0x1 << 6;
        notifyObservers(Transition{ ButtonModel::FOCUS });
      }
    }
    /* Typically add all other possible operations here */
}

/* This is another button that
 * can be either toggled or not. It can also be
 * disabled
 */
class ToggleButtonState : TransitiveModel<ButtonModel>
{
  public:
    void click(void) noexcept
    {
      /* We can only toggle if it's not disabled */
      if(not m_flags[2])
      {
        m_flags[2] ^= 1;
      }
      notifyObservers(Transition{ ButtonModel::PRESS });
    }

    void disable(bool isDisabled) noexcept
    {
      if(isDisabled != m_flags[2])
      {
        m_flags[2] = isDisabled;
        notifyObservers(Transition{ ButtonModel::DISABLE });
      }
    }
};

/* If we wanted to reunite all of these buttons into
 * a common interface, we could create iClickable
 * struct iClickable { void click(void) noexcept = 0; };
 * and make all these models implement iClickable
 */

/* This is a Button view that would implement the model completely */
class CompleteButtonView : vector<Transition<ButtonModel> >
{
private:
  struct Listener : Observer< Transition<ButtonModel> >
  {
    CompleteButtonView& m_Parent;
    Listener(CompleteButtonView& Parent) : m_Parent(Parent) { }
    final void update(Transition<ButtonModel> event)
    {
      switch(event.type)
      {
        case ButtonModel::CLICK: [[fallthrough]];
        case ButtonModel::HOVER: [[fallthrough]];
        case ButtonModel::DISABLE: [[fallthrough]];
        case ButtonModel::DISPLAY: [[fallthrough]];
        case ButtonModel::VIEW: [[fallthrough]];
        case ButtonModel::ANIMATE: [[fallthrough]];
        case ButtonModel::FOCUS: [[fallthrough]];
        case ButtonModel::PRESS: [[fallthrough]];
        case ButtonModel::ACTIVATE:
        {
          /* Here we would repaint our View for each different
           * event. If that's not possible without repainting
           * everything, we can keep a local copy of
           * parameters in m_Parent and repaint everything */
          m_Parent.push_back(event);
          break;
        }
        default:
        {
          throw RuntimeError("Received unknown event");
          break;
        }
      }
    }
  };

public:
  ButtonView(TransitiveModel<ButtonModel>& model) 
    : m_Listener(std::make_shared<Listener>(*this))
  {
    /* this button view subscribes to everything */
    model.subscribe(m_Listener);
  }
  /* Here there would be other methods
   * specific to the way this Button should be displayed */
};

/* This is a Button view that would implement only specific functionality */
class SimpleButtonView : vector<Transition<ButtonModel> >
{
private:
  struct Listener : Observer< Transition<ButtonModel> >
  {
    SimpleButtonView& m_Parent;
    Listener(SimpleButtonView& Parent) : m_Parent(Parent) { }
    final void update(Transition<ButtonModel> event)
    {
      switch(event.type)
      {
        case ButtonModel::PRESS: [[fallthrough]];
        {
          m_Parent.push_back(event);
          break;
        }
        default:
        {
          throw RuntimeError("Received unknown event");
          break;
        }
      }
    }
  };

public:
  ButtonView(TransitiveModel<ButtonModel>& model) 
    : m_Listener(std::make_shared<Listener>(*this))
  {
    /* this button view subscribes only to PRESS events */
    model.subscribe(m_Listener, ButtonModel::PRESS);
  }
};

/* We could have more types of ButtonViews here... Like
 * a RadioButtonView for example.
 * This means we can have different Views, being updated
 * on the events they care about, synchronized from the
 * same data */

SECTION("I cannot call TransitiveModel::addObserver", "[util]")
{
  REQUIRE(!std::is_invokable<TransitiveModel::addObserver, ObserverSPtr>::value);
}

SCENARIO("I can subscribe to all events", "[util]")
{
  GIVEN("I'm an Observer subscribed to a model for all events")
  {
    ButtonState model;
    CompleteButtonView view(model);
    REQUIRE(view.size() == 0);
    WHEN("The model sends any event")
    {
      model.subscribe(view);
      model.click();
      THEN("I receive all events")
      {
        vector expectedEvents = {
          Transition{ ButtonModel::CLICK },
          Transition{ ButtonModel::ACTIVATE },
          Transition{ ButtonModel::HOVER },
          Transition{ ButtonModel::FOCUS }
        };
        REQUIRE_THAT(view, Contains(expectedEvents));
        REQUIRE(view.size() == 4);
      }
    }
  }
}

SCENARIO("I can subscribe to specific events", "[util]")
{
  GIVEN("I'm an Observer subscribed to a model for specific events")
  {
    ToggleButtonState model;
    SimpleButtonView view(model);
    REQUIRE(view.size() == 0);
    WHEN("The model sends an event I'm subscribed to")
    {
      model.click();
      THEN("I receive the event")
      {
        REQUIRE(view.size() == 1);
        REQUIRE_THAT(view, Contains(
              vector{ Transition{ ButtonModel::PRESS } }));
      }
    }
    WHEN("The model sends an event I'm not subscribed to")
    {
      model.disable();
      THEN("I don't receive any event")
      {
        REQUIRE(view.size() == 0);
        REQUIRE_THAT(view, !Contains(
              vector{ Transition{ ButtonModel::DISABLE } }));
      }
    }
  }
}
