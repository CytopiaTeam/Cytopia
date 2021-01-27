#ifndef LANGUAGE_SELECTION_ACTIVITY_HXX
#define LANGUAGE_SELECTION_ACTIVITY_HXX

#include "../layout/AbsoluteLayout.hxx"
#include "../activity/iActivity.hxx"


class LanguageSelectionActivity : public AbsoluteLayout, public iActivity
{
private:
  void onBack();
public:
  LanguageSelectionActivity(GameService::ServiceTuple &, class Window &);
  virtual ~LanguageSelectionActivity() final;
  void setup(class GameService &context) noexcept final;
};

#endif // LANGUAGE_SELECTION_ACTIVITY_HXX
