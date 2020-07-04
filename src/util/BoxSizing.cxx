#include "BoxSizing.hxx"

int ScreenMeasurement::measure(const ScreenMeasurementValuation &mv)
{
  return px + (ew * mv.elementWidth) / 100 + (eh * mv.elementHeight) / 100 + (lw * mv.layoutWidth) / 100 +
         (lh * mv.layoutHeight) / 100;
}
