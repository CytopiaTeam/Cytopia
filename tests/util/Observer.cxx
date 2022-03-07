#include <catch.hpp>
#include <string>

#include "../../src/util/Observer.hxx"

using string = std::string;

/* This is the simple way to use Observers */

struct MyTestSubject : public Subject<>
{
  /* This could be any call that ultimately
   * triggers an event */
  void modifySubject(void) { notifyObservers(); }
};

struct MyTestObserver : public Observer<>
{
  int notifyCount = 0;

private:
  void update(void) noexcept override { notifyCount++; }
};

SCENARIO("Observers listen to their Subject", "[util]")
{
  GIVEN("I have an observer that listens to a subject")
  {
    MyTestSubject subject;
    ObserverSPtr<> observer = std::make_shared<MyTestObserver>();
    subject.addObserver(observer);
    /* As you can see, it's ugly and bad to downcast.
       Observers should not keep any state, or have any
       other methods than update(...)
       We present another way to deal with Observers below */
    REQUIRE(std::dynamic_pointer_cast<MyTestObserver>(observer)->notifyCount == 0);
    WHEN("My subject notifies observers")
    {
      subject.modifySubject();
      THEN("My observer gets updated") { REQUIRE(std::dynamic_pointer_cast<MyTestObserver>(observer)->notifyCount == 1); }
    }
  }
}

/* This is a better way to use Observers */

class MyModel : public Subject<int, bool, string>
{
private:
  int m_x = 0;
  bool m_y = false;
  string m_z = "BEFORE";

public:
  void setX(int x)
  {
    m_x = x;
    notifyObservers(m_x, m_y, m_z);
  }
  void setY(bool y)
  {
    m_y = y;
    notifyObservers(m_x, m_y, m_z);
  }
  void setZ(string z)
  {
    /* This is one of the advantages of Observers:
     * We can optimize when they should be notified
     * and keep that logic in the Subject close to the data */
    if (m_z != z)
    {
      m_z = std::move(z);
      notifyObservers(m_x, m_y, m_z);
    }
  }
  virtual inline void onObserverExpired() noexcept final {}
};

class MyListener
{

private:
  /* Observer as inner class */
  struct FirstObserver : public Observer<int, bool, string>
  {
    /* We keep a reference to parent class */
    MyListener &m_listener;

    explicit FirstObserver(MyListener &listener) : m_listener(listener) {}

    void update(int x, bool y, string z) noexcept
    {
      /* All inner classes are friends of their outer
       * classes in C++ */
      m_listener.m_x = x;
      m_listener.m_y = y;
      m_listener.m_z = z;
      m_listener.m_notifyCount++;
      /* In a real listener, this is where we would control
         something (for example: a UIElement, or trigger an event) */
    }
  };

  /* You could add more Observer declarations and types here ... */
  ObserverSPtr<int, bool, string> m_observer1;

  /* In a real listener, we would not store
     the data. We are only doing this for testing. */
  int m_x = -1;
  bool m_y = true;
  string m_z = "DEFAULT";
  int m_notifyCount = 0;
  /* However, notice that the Listener needs
   * to assume a "default" state for Subject.
   * This can be useful or unwanted. A way to change this
   * is to overload addObserver(...) in
   * Subject to immediately call update on the
   * new Observer, so that it is always synchronised. */

public:
  explicit MyListener(MyModel &model)
  {
    /* Here we initialize all observers and add them
       to the model */
    m_observer1 = std::make_shared<FirstObserver>(*this);
    model.addObserver(m_observer1);
  }
  int getNotifiedX() { return m_x; }
  bool getNotifiedY() { return m_y; }
  string getNotifiedZ() { return m_z; }
  int getNotifiedCount() { return m_notifyCount; }
  /* This is also for testing */
  void expire() noexcept { m_observer1.reset(); }
};

SCENARIO("Observers can become expired", "[util]")
{
  GIVEN("I have a model and two listeners")
  {
    MyModel model;
    MyListener listener1(model);
    MyListener listener2(model);
    /* Much cleaner */
    REQUIRE(listener2.getNotifiedX() == -1);
    REQUIRE(listener2.getNotifiedY() == true);
    REQUIRE(listener2.getNotifiedZ() == "DEFAULT");
    WHEN("A listener becomes expired")
    {
      listener1.expire();
      WHEN("I notify all observers")
      {
        REQUIRE_NOTHROW(model.setX(123));
        THEN("My active listener gets updated")
        {
          REQUIRE(listener2.getNotifiedX() == 123);
          REQUIRE(listener2.getNotifiedY() == false);
          REQUIRE(listener2.getNotifiedZ() == "BEFORE");
        }
        THEN("My expired listener does not get updated")
        {
          REQUIRE(listener1.getNotifiedX() == -1);
          REQUIRE(listener1.getNotifiedY() == true);
          REQUIRE(listener1.getNotifiedZ() == "DEFAULT");
        }
      }
    }
  }
}
