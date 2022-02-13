#include "SettingsActivity.hxx"
#include "../view/TextButton.hxx"
#include "../view/IconButton.hxx"
#include "../view/ImageView.hxx"
#include "../util/BoxSizing.hxx"
#include "../view/Window.hxx"
#include "../layout/SelectionLayout.hxx"
#include "../controller/SelectionLayoutController.hxx"
#include "../view/TextView.hxx"
#include "../view/CheckBoxButton.hxx"
#include "../controller/ButtonHandler.hxx"
#include "../util/LOG.hxx"
#include "../util/Exception.hxx"
#include "../util/ZipRange.hxx"
#include "../services/MouseController.hxx"
#include "../view/LanguageSelectionItem.hxx"

SettingsActivity::SettingsActivity(GameService::ServiceTuple &context, Window &w)
    : iActivity(context, w), AbsoluteLayout(w.getBounds()), m_Languages{}
{
  m_Languages.emplace_back(LanguageSelection{"Chinese", "zh", false});
  m_Languages.emplace_back(LanguageSelection{"Dutch", "nl", true});
  m_Languages.emplace_back(LanguageSelection{"English", "en", true});
  m_Languages.emplace_back(LanguageSelection{"Finnish", "fi", false});
  m_Languages.emplace_back(LanguageSelection{"French", "fr", false});
  m_Languages.emplace_back(LanguageSelection{"Hungarian", "hu", false});
  m_Languages.emplace_back(LanguageSelection{"Indonesian", "id", false});
  m_Languages.emplace_back(LanguageSelection{"Norwegian Bokmal", "nb_NO", false});
  m_Languages.emplace_back(LanguageSelection{"Portuguese", "pt", false});
  m_Languages.emplace_back(LanguageSelection{"Russian", "ru", false});
  m_Languages.emplace_back(LanguageSelection{"Spanish", "es", false});
  m_Languages.emplace_back(LanguageSelection{"Turkish", "tr", true});

  auto &localization = w.getSettingsModel().getLocalization();

  ///* Create background */
  //{
  //  ImageViewPtr img = std::make_shared<ImageView>("/resources/images/ui/general/background", DisplayMode::RepeatXY);
  //  addElement(img, BoxSizing{100_lw, 100_lw}, AbsolutePosition::Origin());
  //}
  /* Create title */
  {
    std::string title = "Settings";
    TextViewPtr text = std::make_shared<TextView>(title, localization.translateText(title));
    addElement(text, BoxSizing{50_lw, 10_lh}, AbsolutePosition{50_lw - 50_ew, 5_lh});
  }
  /* Create all text buttons */
  {
    std::string fullScreenText = "Full Screen";
    TextViewPtr text = std::make_shared<TextView>(fullScreenText, localization.translateText(fullScreenText));
    addElement(text, BoxSizing{50_lw, 10_lh}, AbsolutePosition{50_lw - 80_ew, 20_lh});
  }

  /* checkbox */
  {
    // std::string fullScreenText = "Full Screen";
    CheckBoxButtonPtr checkbox = std::make_shared<CheckBoxButton>(true);
    // TextViewPtr text = std::make_shared<TextView>(fullScreenText, localization.translateText(fullScreenText));
    addElement(checkbox, BoxSizing{50_lw, 10_lh}, AbsolutePosition{50_lw - 70_ew, 20_lh});
  }

    /* Create all text buttons */
  {
    std::string buttonText{"Back"};
    TextButtonPtr button = std::make_shared<TextButton>(buttonText, localization.translateText(buttonText));
    ButtonModel &state = createModel<ButtonModel>();
    addElement(button, BoxSizing{30_lw, 10_lh}, AbsolutePosition{50_lw - 50_ew, 87_lh});
    createController<ButtonHandler>(std::bind(&SettingsActivity::onBack, this), state, getWindow().getMouseState(),
                                    *button);
    state.addObserver(button);
  }

  /* Create all text buttons */
  {
    std::string buttonText{"Back"};
    TextButtonPtr button = std::make_shared<TextButton>(buttonText, localization.translateText(buttonText));
    ButtonModel &state = createModel<ButtonModel>();
    addElement(button, BoxSizing{30_lw, 10_lh}, AbsolutePosition{50_lw - 50_ew, 87_lh});
    createController<ButtonHandler>(std::bind(&SettingsActivity::onBack, this), state, getWindow().getMouseState(),
                                    *button);
    state.addObserver(button);
  }

  
}

void SettingsActivity::onBack() { activitySwitch(ActivityType::MainMenu); }

void SettingsActivity::onLanguageSelected(int index)
{
  getWindow().getSettingsModel().setLanguage(m_Languages[index].shortName);
}

SettingsActivity::~SettingsActivity() = default;

void SettingsActivity::setup(class GameService &context) noexcept
{
  iLayout::setup(context);
  iActivity::setup(context);
}

int SettingsActivity::getSelectedLanguageIndex()
{
  std::string selectedLanguage = getWindow().getSettingsModel().getLanguage();
  auto selectedLangugeIter = std::find_if(m_Languages.begin(), m_Languages.end(),
                                          [&selectedLanguage](auto &language) { return language.shortName == selectedLanguage; });
  int selectedLanguageIndex = SelectionLayoutModel::NOT_SELECTED_INDEX;
  if (selectedLangugeIter != m_Languages.end())
  {
    selectedLanguageIndex = selectedLangugeIter - m_Languages.begin();
  }

  return selectedLanguageIndex;
}