#ifndef TOOLTIP_HXX_
#define TOOLTIP_HXX_

#include "../basics/UIElement.hxx"
#include "Text.hxx"
#include "../../../services/GameClock.hxx"

/** @brief Tooltip class
  * @details Tooltip is derived from Text, and adds some extensions to it. It is set to visible after 
  * the timer times out.
  * @note There should only be one tooltip object, managed by the UIManager
  * @see UIManager#startTooltip
*/
class Tooltip : public Text
{
public:
  Tooltip();
  ~Tooltip() override = default;

  void draw() override;

  /** @brief starts the countdown to when the Tooltip appears
   * @details starts the timer for the countdown to when the Tooltip becomes visible
   */
  void startTimer();

  /** @brief makes the Tooltip visible
   */
  void showTooltip();

  /** @brief resets the Tooltip
   * @details Hides and deactivates the Tooltip and stops the timer
   */
  void reset();

private:
  /// Handler to the started clock (timer) task
  GameClock::ClockTaskHndl clockHndl;
  /// if the Tooltip is active or not
  bool m_active = false;
};

#endif