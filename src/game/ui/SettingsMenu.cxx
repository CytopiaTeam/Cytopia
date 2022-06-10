#include "SettingsMenu.hxx"

SettingsMenu::SettingsMenu() {
  auto &uiManager = UIManager::instance();
  uiManager.setGroupVisibility("SettingsMenu", true);
}

void SettingsMenu::draw() const {

}

SettingsMenu::~SettingsMenu() {
  auto &uiManager = UIManager::instance();
  uiManager.setGroupVisibility("SettingsMenu", false);
}