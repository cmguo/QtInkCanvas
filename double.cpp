#include "double.h"

INKCANVAS_BEGIN_NAMESPACE

const double Double::NegativeInfinity = (double)-1.0 / (double)(0.0);
const double Double::PositiveInfinity = (double)1.0 / (double)(0.0);
const double Double::NaN = (double)0.0 / (double)0.0;

INKCANVAS_END_NAMESPACE
