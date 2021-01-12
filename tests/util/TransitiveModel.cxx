#include <catch.hpp>
#include <bitset>

#include "TransitiveModel.hxx"
#include "IEquatable.hxx"

using namespace Catch::Matchers;
template <class T> using vector = std::vector<T>;
using RuntimeError = std::runtime_error;

/* This is our data model for a Button */
struct ButtonModel
{
  enum Operations
  {
    CLICK,
    HOVER,
    DISABLE,
    DISPLAY,
    VIEW,
    ANIMATE,
    FOCUS,
    PRESS,
    ACTIVATE
  };

  template <typename T> struct BaseButtonData : public IEquatable<T>
  {
    using MyModelType = ButtonModel;
    BaseButtonData() { this->onHashChanged(); }

  private:
    std::size_t Hash() const noexcept { return 0; }
    friend IEquatable<T>;
  };

  struct ClickData : public BaseButtonData<ClickData>
  {
  };
  struct HoverData : public BaseButtonData<HoverData>
  {
  };
  struct DisableData : public BaseButtonData<DisableData>
  {
  };
  struct DisplayData : public BaseButtonData<DisplayData>
  {
  };
  struct ViewData : public BaseButtonData<ViewData>
  {
  };
  struct AnimateData : public BaseButtonData<AnimateData>
  {
  };
  struct FocusData : public BaseButtonData<FocusData>
  {
  };
  struct PressData : public BaseButtonData<PressData>
  {
  };
  struct ActivateData : public BaseButtonData<ActivateData>
  {
  };

  using OperationTypes =
      TypeList<ClickData, HoverData, DisableData, DisplayData, ViewData, AnimateData, FocusData, PressData, ActivateData>;
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
class TestButton : public TransitiveModel<ButtonModel>
{
public:
  /* Here, we implement the operations */
  void click(void) noexcept
  {
    notifyObservers(Transition{ClickData{}});
    /* When we click, we mark our button as "active" */
    if (not m_flags[0])
    {
      m_flags |= 0x1 << 0;
      notifyObservers(Transition{ActivateData{}});
    }
    /* When we click, we mark our button as "hovered" */
    if (not m_flags[1])
    {
      m_flags |= 0x1 << 1;
      notifyObservers(Transition{HoverData{}});
    }
    /* When we click, we mark our button as "focused" */
    if (not m_flags[6])
    {
      m_flags |= 0x1 << 6;
      notifyObservers(Transition{FocusData{}});
    }
  }
  /* Typically add all other possible operations here */
};

std::ostream &operator<<(std::ostream &os, const Transition<ButtonModel> &transition)
{
  std::vector<std::string> names = {"CLICK", "HOVER", "DISABLE", "DISPLAY", "VIEW", "ANIMATE", "FOCUS", "PRESS", "ACTIVATE"};
  os << names.at(transition.getType());
  return os;
}

/* This is another button that
 * can be either toggled or not. It can also be
 * disabled
 */
class ToggleTestButton : public TransitiveModel<ButtonModel>
{
public:
  void click(void) noexcept
  {
    /* We can only toggle if it's not disabled */
    if (not m_flags[2])
    {
      m_flags[2].flip();
    }
    notifyObservers(Transition{PressData{}});
  }

  void disable(bool isDisabled) noexcept
  {
    if (isDisabled != m_flags[2])
    {
      m_flags[2] = isDisabled;
      notifyObservers(Transition{DisableData{}});
    }
  }
};

/* If we wanted to reunite all of these buttons into
 * a common interface, we could create iClickable
 * struct iClickable { void click(void) noexcept = 0; };
 * and make all these models implement iClickable
 */

/* This is a Button view that would implement the model completely */
class CompleteButtonView : public vector<Transition<ButtonModel>>
{
private:
  struct Listener : Observer<Transition<ButtonModel>>
  {
    CompleteButtonView &m_Parent;
    explicit Listener(CompleteButtonView &Parent) : m_Parent(Parent) {}
    void update(Transition<ButtonModel> event) noexcept final
    {
      switch (event.getType())
      {
      case ButtonModel::CLICK:
        [[fallthrough]];
      case ButtonModel::HOVER:
        [[fallthrough]];
      case ButtonModel::DISABLE:
        [[fallthrough]];
      case ButtonModel::DISPLAY:
        [[fallthrough]];
      case ButtonModel::VIEW:
        [[fallthrough]];
      case ButtonModel::ANIMATE:
        [[fallthrough]];
      case ButtonModel::FOCUS:
        [[fallthrough]];
      case ButtonModel::PRESS:
        [[fallthrough]];
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
        break;
      }
      }
    }
  };

  using ListenerPtr = std::shared_ptr<Listener>;
  ListenerPtr m_Listener;

public:
  explicit CompleteButtonView(TransitiveModel<ButtonModel> &model) : m_Listener(std::make_shared<Listener>(*this))
  {
    /* this button view subscribes to everything */
    model.subscribe(m_Listener);
  }
  /* Here there would be other methods
   * specific to the way this Button should be displayed */
};

/* This is a Button view that would implement only specific functionality */
class SimpleButtonView : public vector<Transition<ButtonModel>>
{
private:
  struct Listener : Observer<Transition<ButtonModel>>
  {
    SimpleButtonView &m_Parent;
    explicit Listener(SimpleButtonView &Parent) : m_Parent(Parent) {}
    void update(Transition<ButtonModel> event) noexcept final
    {
      switch (event.getType())
      {
      case ButtonModel::PRESS:
      {
        m_Parent.push_back(event);
        break;
      }
      default:
      {
        break;
      }
      }
    }
  };

  using ListenerPtr = std::shared_ptr<Listener>;
  ListenerPtr m_Listener;

public:
  explicit SimpleButtonView(TransitiveModel<ButtonModel> &model) : m_Listener(std::make_shared<Listener>(*this))
  {
    /* this button view subscribes only to PRESS events */
    model.subscribe(m_Listener, {ButtonModel::PRESS});
  }
};

/* We could have more types of ButtonViews here... Like
 * a RadioButtonView for example.
 * This means we can have different Views, being updated
 * on the events they care about, synchronized from the
 * same data */

TEST_CASE("I cannot call TransitiveModel::addObserver", "[util]")
{
  REQUIRE(!std::is_invocable<TestButton, ObserverSPtr<Transition<ButtonModel>>>::value);
}

SCENARIO("I can subscribe to all events", "[util]")
{
  GIVEN("I'm an Observer subscribed to a model for all events")
  {
    TestButton model;
    CompleteButtonView view(model);
    REQUIRE(view.size() == 0);
    WHEN("The model sends any event")
    {
      model.click();
      THEN("I receive all events")
      {
        vector<TestButton::Notification> expectedEvents{
            Transition{ButtonModel::ClickData{}}, Transition{ButtonModel::ActivateData{}}, Transition{ButtonModel::HoverData{}},
            Transition{ButtonModel::FocusData{}}};
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
    ToggleTestButton model;
    SimpleButtonView view(model);
    REQUIRE(view.size() == 0);
    WHEN("The model sends an event I'm subscribed to")
    {
      model.click();
      THEN("I receive the event")
      {
        REQUIRE(view.size() == 1);
        REQUIRE_THAT(view, Contains(vector<TestButton::Notification>{Transition{ButtonModel::PressData{}}}));
      }
    }
    WHEN("The model sends an event I'm not subscribed to")
    {
      model.disable(true);
      model.disable(false);
      THEN("I don't receive any event")
      {
        REQUIRE(view.size() == 0);
        REQUIRE_THAT(view, !Contains(vector<TestButton::Notification>{Transition{ButtonModel::DisableData{}}}));
      }
    }
  }
}
