#include "Internal/Controls/inkcanvasinnercanvas.h"
#include "Windows/Controls/inkcanvas.h"
#include "Internal/Ink/inkcanvasselection.h"
#include "Windows/Media/hittestresult.h"
#include "Internal/debug.h"

INKCANVAS_BEGIN_NAMESPACE

//------------------------------------------------------
//
//  Cnostructors
//
//------------------------------------------------------

//#region Constructors

InkCanvasInnerCanvas::InkCanvasInnerCanvas(InkCanvas& inkCanvas)
    : _inkCanvas(inkCanvas)
{
    //Debug::Assert(inkCanvas != nullptr);
    //_inkCanvas = inkCanvas;
}


// No default constructor
InkCanvasInnerCanvas::InkCanvasInnerCanvas()
    : _inkCanvas(*static_cast<InkCanvas*>(nullptr))
{
}

//#endregion Constructors

//------------------------------------------------------
//
//  Protected Methods
//
//------------------------------------------------------

//#region Protected Methods
/// <summary>
/// Override OnVisualChildrenChanged
/// </summary>
/// <param name="visualAdded"></param>
/// <param name="visualRemoved"></param>
void InkCanvasInnerCanvas::OnVisualChildrenChanged(DependencyObject* visualAdded, DependencyObject* visualRemoved)
{
    UIElement::OnVisualChildrenChanged(visualAdded, visualRemoved);

    UIElement* removedElement = static_cast<UIElement*>(visualRemoved);

    // If there is an element being removed, we should make sure to update our selected elements list..
    if ( removedElement != nullptr )
    {
        GetInkCanvas().GetInkCanvasSelection().RemoveElement(removedElement);
    }

    //resurface this on the containing InkCanvas
    GetInkCanvas().RaiseOnVisualChildrenChanged(visualAdded, visualRemoved);
}

/// <summary>
/// Override of <seealso cref="FrameworkElement.MeasureOverride" />
/// The code is similar to Canvas.MeasureOverride. The only difference we have is that
/// InkCanvasInnerCanvas does report the size based on its children's sizes.
/// </summary>
/// <param name="constraint">Constraint size.</param>
/// <returns>Computed desired size.</returns>
Size InkCanvasInnerCanvas::MeasureOverride(Size constraint)
{
    Size childConstraint = Size(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());

    Size newSize;
    for ( UIElement* child : InternalChildren() )
    {
        if ( child == nullptr ) { continue; }
        child->Measure(childConstraint);

        // NOTICE-2006/02/03-WAYNEZEN,
        // We only honor Left and/or Top property for the measure.
        // For Right/Bottom, only the child->width()/Height will be used. Those properties will be used by the arrange
        // but not the measure.
        double left = (double)GetInkCanvas().GetLeft(child);
        if ( !Double::IsNaN(left) )
        {
            newSize.setWidth(qMax(newSize.Width(), left + child->DesiredSize().Width()));
        }
        else
        {
            newSize.setWidth(qMax(newSize.Width(), child->DesiredSize().Width()));
        }

        double top = (double)GetInkCanvas().GetTop(child);
        if ( !Double::IsNaN(top) )
        {
            newSize.setHeight(Math::Max(newSize.Height(), top + child->DesiredSize().Height()));
        }
        else
        {
            newSize.setHeight(Math::Max(newSize.Height(), child->DesiredSize().Height()));
        }
    }

    return newSize;
}

/// <summary>
/// Canvas computes a position for each of its children taking into account their margin and
/// attached Canvas properties: Top, Left.
///
/// Canvas will also arrange each of its children.
/// This code is same as the Canvas'.
/// </summary>
/// <param name="arrangeSize">Size that Canvas will assume to position children.</param>
Size InkCanvasInnerCanvas::ArrangeOverride(Size arrangeSize)
{
    //Canvas arranges children at their DesiredSize.
    //This means that Margin on children is actually respected and added
    //to the size of layout partition for a child->
    //Therefore, is Margin is 10 and Left is 20, the child's ink will start at 30.

    for ( UIElement* child : InternalChildren() )
    {
        if ( child == nullptr ) { continue; }

        double x = 0;
        double y = 0;


        //Compute offset of the child:
        //If Left is specified, then Right is ignored
        //If Left is not specified, then Right is used
        //If both are not there, then 0
        double left = GetInkCanvas().GetLeft(child);
        if ( !Double::IsNaN(left) )
        {
            x = left;
        }
        else
        {
            double right = GetInkCanvas().GetRight(child);

            if ( !Double::IsNaN(right) )
            {
                x = arrangeSize.Width() - child->DesiredSize().Width() - right;
            }
        }

        double top = GetInkCanvas().GetTop(child);
        if ( !Double::IsNaN(top) )
        {
            y = top;
        }
        else
        {
            double bottom = GetInkCanvas().GetBottom(child);

            if ( !Double::IsNaN(bottom) )
            {
                y = arrangeSize.Height() - child->DesiredSize().Height() - bottom;
            }
        }

        child->Arrange(Rect(Point(x, y), child->DesiredSize()));
    }

    return arrangeSize;
}

/// <summary>
/// OnChildDesiredSizeChanged
/// </summary>
/// <param name="child"></param>
void InkCanvasInnerCanvas::OnChildDesiredSizeChanged(UIElement* child)
{
    UIElement::OnChildDesiredSizeChanged(child);

    // Invalid InkCanvasInnerCanvas' measure.
    InvalidateMeasure();
}

/// <summary>
/// Override CreateUIElementCollection method.
/// The logical parent of InnerCanvas will be set to InkCanvas instead.
/// </summary>
/// <param name="logicalParent"></param>
/// <returns></returns>
List<UIElement*> InkCanvasInnerCanvas::CreateUIElementCollection(UIElement* logicalParent)
{
    (void) logicalParent;
    // Replace the logical parent of the InnerCanvas children with our GetInkCanvas().
    return UIElement::CreateUIElementCollection(&_inkCanvas);
}

/// <summary>
/// Returns LogicalChildren
/// </summary>
List<UIElement*> InkCanvasInnerCanvas::LogicalChildren()
{
    // InnerCanvas won't have any logical children publicly.
    return List<UIElement*>();
}

/// <summary>
/// The overridden GetLayoutClip method
/// </summary>
/// <returns>Geometry to use as additional clip if ClipToBounds=true</returns>
Geometry* InkCanvasInnerCanvas::GetLayoutClip(Size layoutSlotSize)
{
    // NTRAID:WINDOWSOS#1516798-2006/02/17-WAYNEZEN
    // By default an FE will clip its content if the ink size exceeds the layout size (the final arrange size).
    // Since we are auto growing, the ink size is same as the desired size. So it ends up the strokes will be clipped
    // regardless ClipToBounds is set or not.
    // We override the GetLayoutClip method so that we can bypass the default layout clip if ClipToBounds is set to false.
    if ( ClipToBounds() )
    {
        return UIElement::GetLayoutClip(layoutSlotSize);
    }
    else
        return nullptr;
}

//#endregion Protected Methods

//------------------------------------------------------
//
//  Internal Methods
//
//------------------------------------------------------

//#region Internal Methods

/// <summary>
/// Hit test on the children
/// </summary>
/// <param name="point"></param>
/// <returns></returns>
UIElement* InkCanvasInnerCanvas::HitTestOnElements(Point point)
{
    UIElement* hitElement = nullptr;

    // Do hittest.
    HitTestResult hitTestResult = HitTestCore(point);

    // Now find out which element is hit if there is a result.
    if ( hitTestResult != HitTestResult(nullptr) )
    {
        UIElement*visual = static_cast<UIElement*>(hitTestResult.VisualHit());
        //System.Windows.Media.Media3D.Visual3D visual3D = hitTestResult.VisualHit as System.Windows.Media.Media3D.Visual3D;

        UIElement* currentObject = nullptr;
        if ( visual != nullptr )
        {
            currentObject = visual;
        }
        //else if ( visual3D != nullptr )
        //{
        //    currentObject = visual3D;
        //}

        while ( currentObject != nullptr )
        {
            UIElement* parent = currentObject->Parent();
            if ( parent == &GetInkCanvas().InnerCanvas() )
            {
                // Break when we hit the inner canvas in the visual tree.
                hitElement = currentObject;
                Debug::Assert(Children().Contains(hitElement), "The hit element should be a child of InnerCanvas.");
                break;
            }
            else
            {
                currentObject = parent;
            }
        }
    }

    return hitElement;
}

/// <summary>
/// Returns the private logical children
/// </summary>
List<UIElement*> InkCanvasInnerCanvas::PrivateLogicalChildren()
{
    // Return the logical children of the base - Canvas
    return UIElement::LogicalChildren();
}

INKCANVAS_END_NAMESPACE
