#include <catch2/catch.hpp>
#include <string>

#include "Meta.hxx"
#include "Observer.hxx"

using string = std::string;

/* This is the basic way to use Observers */

struct MyTestSubject : public Subject<int>
{
  /* This could be any call that ultimately
   * triggers an event */
  void modifySubject(void) { notifyObservers(0); }
};

struct MyTestObserver : public MyTestSubject::Listener
{
  int notifyCount = 0;

private:
  void update(Notification) noexcept override { notifyCount++; }
};

SCENARIO("Observers listen to their Subject", "[util]")
{
  GIVEN("I have an observer that listens to a subject")
  {
    MyTestSubject subject;
    auto observer = std::make_shared<MyTestObserver>();
    subject.addObserver(observer);
    REQUIRE(observer->notifyCount == 0);
    WHEN("My subject notifies observers")
    {
      subject.modifySubject();
      THEN("My observer gets updated") { REQUIRE(observer->notifyCount == 1); }
    }
  }
}
