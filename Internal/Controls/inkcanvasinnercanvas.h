#ifndef INKCANVASINNERCANVAS_H
#define INKCANVASINNERCANVAS_H

#include "Windows/uielement.h"

class InkCanvas;

// namespace MS.Internal.Controls

/// <summary>
/// A subclass of Panel which does layout for GetInkCanvas().
/// </summary>
class InkCanvasInnerCanvas : public FrameworkElement
{
    Q_OBJECT
public:
    //------------------------------------------------------
    //
    //  Cnostructors
    //
    //------------------------------------------------------

    //#region Constructors

    InkCanvasInnerCanvas(InkCanvas& inkCanvas);

private:
    // No default constructor
    InkCanvasInnerCanvas();

    //#endregion Constructors

    //------------------------------------------------------
    //
    //  Protected Methods
    //
    //------------------------------------------------------

    //#region Protected Methods
protected:
    /// <summary>
    /// Override OnVisualChildrenChanged
    /// </summary>
    /// <param name="visualAdded"></param>
    /// <param name="visualRemoved"></param>
    virtual void OnVisualChildrenChanged(DependencyObject* visualAdded, DependencyObject* visualRemoved);

    /// <summary>
    /// Override of <seealso cref="FrameworkElement.MeasureOverride" />
    /// The code is similar to Canvas.MeasureOverride. The only difference we have is that
    /// InkCanvasInnerCanvas does report the size based on its children's sizes.
    /// </summary>
    /// <param name="constraint">Constraint size.</param>
    /// <returns>Computed desired size.</returns>
    virtual QSizeF MeasureOverride(QSizeF constraint);

    /// <summary>
    /// Canvas computes a position for each of its children taking into account their margin and
    /// attached Canvas properties: Top, Left.
    ///
    /// Canvas will also arrange each of its children.
    /// This code is same as the Canvas'.
    /// </summary>
    /// <param name="arrangeSize">Size that Canvas will assume to position children.</param>
    virtual QSizeF ArrangeOverride(QSizeF arrangeSize);

    /// <summary>
    /// OnChildDesiredSizeChanged
    /// </summary>
    /// <param name="child"></param>
    virtual void OnChildDesiredSizeChanged(UIElement* child);

    /// <summary>
    /// Override CreateUIElementCollection method.
    /// The logical parent of InnerCanvas will be set to InkCanvas instead.
    /// </summary>
    /// <param name="logicalParent"></param>
    /// <returns></returns>
    virtual QList<UIElement*> CreateUIElementCollection(UIElement* logicalParent);

    /// <summary>
    /// Returns LogicalChildren
    /// </summary>
    virtual QList<UIElement*> LogicalChildren();

    /// <summary>
    /// The overridden GetLayoutClip method
    /// </summary>
    /// <returns>Geometry to use as additional clip if ClipToBounds=true</returns>
    virtual Geometry* GetLayoutClip(QSizeF layoutSlotSize);
    //#endregion Protected Methods

    //------------------------------------------------------
    //
    //  Internal Methods
    //
    //------------------------------------------------------

    //#region Internal Methods

public:
    /// <summary>
    /// Hit test on the children
    /// </summary>
    /// <param name="point"></param>
    /// <returns></returns>
    UIElement* HitTestOnElements(QPointF point);

    /// <summary>
    /// Returns the private logical children
    /// </summary>
    QList<UIElement*> PrivateLogicalChildren();

    /// <summary>
    /// Returns the associated InkCanvas
    /// </summary>
    InkCanvas& GetInkCanvas()
    {
        return _inkCanvas;
    }

    //#endregion Internal Methods

    //------------------------------------------------------
    //
    //  Private Methods
    //
    //------------------------------------------------------

    //#region Private Methods


    //#endregion Private Methods

    //------------------------------------------------------
    //
    //  Private Fields
    //
    //------------------------------------------------------

    //#region Private Fields

    // The host InkCanvas
private:
    InkCanvas&   _inkCanvas;

    //#endregion Private Fields
};

#endif // INKCANVASINNERCANVAS_H
