#include "double.h"

INKCANVAS_BEGIN_NAMESPACE

//const double Double::NegativeInfinity = -1.0 / 0.0;
const double Double::PositiveInfinity = 1.0 / 0.0;
const double Double::NegativeInfinity = -PositiveInfinity;
const double Double::NaN = 0.0 / 0.0;

INKCANVAS_END_NAMESPACE
