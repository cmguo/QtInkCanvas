#include "visual.h"
#include "hittestresult.h"

Visual::Visual()
{

}


void Visual::SetOpacity(double opacity)
{

}


void Visual::AddVisualChild(Visual *)
{

}

void Visual::OnVisualChildrenChanged(DependencyObject* visualAdded, DependencyObject* visualRemoved)
{

}

HitTestResult Visual::HitTestCore(PointHitTestParameters hitTestParams)
{
    return HitTestResult(nullptr);
}
