#include "strokeintersection.h"

StrokeIntersection StrokeIntersection::s_empty(AfterLast, AfterLast, BeforeFirst, BeforeFirst);
StrokeIntersection StrokeIntersection::s_full(BeforeFirst, BeforeFirst, AfterLast, AfterLast);

