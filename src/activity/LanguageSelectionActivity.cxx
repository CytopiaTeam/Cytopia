#include "LanguageSelectionActivity.hxx"
#include "../view/TextButton.hxx"
#include "../view/IconButton.hxx"
#include "../view/ImageView.hxx"
#include "../util/BoxSizing.hxx"
#include "../view/Window.hxx"
//#include "../layout/SelectionLayout.hxx"
//#include "../SelectionLayoutController.hxx"
//#include "../view/TextView.hxx"
#include "../controller/ButtonHandler.hxx"
#include "../util/LOG.hxx"
#include "../util/Exception.hxx"
#include "../util/ZipRange.hxx"
#include "../services/MouseController.hxx"
//#include "../LanguageSelectionItem.hxx"

LanguageSelectionActivity::LanguageSelectionActivity(GameService::ServiceTuple &context, Window &w)
    : iActivity(context, w), AbsoluteLayout(w.getBounds())
{
  auto &localization = w.getSettingsModel().getLocalization();

  ///* Create background */
  //{
  //  ImageViewPtr img = std::make_shared<ImageView>("/resources/images/ui/general/background", DisplayMode::RepeatXY);
  //  addElement(img, BoxSizing{100_lw, 100_lw}, AbsolutePosition::Origin());
  //}
  /* Create title */
  /*{
    TextViewPtr text = std::make_shared<TextView>("Select a language");
    addElement(text, BoxSizing{50_lw, 10_lh}, AbsolutePosition{50_lw - 50_ew, 5_lh});
  }*/
  /* Create all text buttons */
  {
    auto texts = std::array{"Back"};
    auto topPositions = std::array{87_lh};
    auto callbacks = std::array{
        std::bind(&LanguageSelectionActivity::onBack, this),
    };
    for (auto [text, tp, cb] : ZipRange{texts, topPositions, callbacks})
    {
      TextButtonPtr button = std::make_shared<TextButton>(text, localization.translateText(text));
      ButtonModel &state = createModel<ButtonModel>();
      addElement(button, BoxSizing{30_lw, 10_lh}, AbsolutePosition{50_lw - 50_ew, tp});
      createController<ButtonHandler>(cb, state, getWindow().getMouseState(), *button);
      state.addObserver(button);
    }
  }
  /* Create Scrolling layout with language elements */
  //{
  //  auto languagesView = std::make_shared<SelectionLayout>(Rectangle::EMPTY(), 40_px);

  //  for (auto lang : {"English", "French",  "German",  "VERYLONG______________________________VERYLONG",
  //                    "English", "English", "English", "English",
  //                    "English", "English", "English", "English",
  //                    "English", "English", "English", "English",
  //                    "English", "English", "English", "English",
  //                    "English", "English", "English", "English",
  //                    "English", "English", "English", "English",
  //                    "English", "English", "English", "English",
  //                    "English", "English", "English", "English",
  //                    "LAST"})
  //  {
  //    auto pItem = std::make_shared<LanguageSelectionItem>(lang, std::strcmp(lang, "English"));
  //    languagesView->addElement(pItem);
  //  }

  //  addElement(languagesView, BoxSizing{70_lw, 70_lh}, AbsolutePosition{15_lw, 15_lh});
  //  SelectionLayoutModel &model = createModel<SelectionLayoutModel>();
  //  createController<SelectionLayoutController>(model, getWindow().getMouseState(), *languagesView);
  //  model.addObserver(languagesView);
  //}
}

void LanguageSelectionActivity::onBack() { activitySwitch(ActivityType::MainMenu); }

LanguageSelectionActivity::~LanguageSelectionActivity() = default;

void LanguageSelectionActivity::setup(class GameService &context) noexcept
{
  iLayout::setup(context);
  iActivity::setup(context);
}
