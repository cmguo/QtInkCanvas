#ifndef UIELEMENT_H
#define UIELEMENT_H

#include "InkCanvas_global.h"

#include "Windows/Media/visual.h"
#include "Windows/routedeventargs.h"

#include <QFlags>

#include <functional>

class RoutedEvent;
class RoutedEventArgs;
class Geometry;

class StylusPlugInCollection;
class PenContexts;

// namespace System.Windows

class INKCANVAS_EXPORT UIElement : public Visual
{
    Q_OBJECT
public:
    static RoutedEvent LostMouseCaptureEvent;

    static RoutedEvent LostStylusCaptureEvent;

signals:
    void LayoutUpdated(EventArgs&);

    void IsVisibleChanged();

    void IsHitTestVisibleChanged();

    void IsEnabledChanged();

public:
    UIElement();

    void ApplyTemplate();

    QList<UIElement*> Children();

    UIElement* Parent();

    void InvalidateVisual();

    template<typename T>
    bool InstanceOf()
    {
        return metaObject()->inherits(&T::staticMetaObject);
    }

    QTransform LayoutTransform();

    QTransform RenderTransform();

    QTransform TransformToVisual(UIElement* visual);

    QSizeF DesiredSize();

    QSizeF RenderSize();

    void SetRenderSize(QSizeF);

    void Arrange(QRectF rect);

    void InvalidateMeasure();

    void InvalidateArrange();

    bool IsVisible();

    bool IsHitTestVisible();

    bool IsMeasureValid();

    bool IsArrangeValid();

    void UpdateLayout();

    bool IsMouseOver();

    bool IsStylusOver();

    bool Focus();

    void CaptureMouse();

    bool IsMouseCaptured();

    bool IsEnabled();

    bool ForceCursor();

    bool ClipToBounds();

    void ReleaseMouseCapture();

    void CaptureStylus();

    bool IsStylusCaptured();

    bool IsAncestorOf(UIElement*);

    void ReleaseStylusCapture();

public:
    void AddHandler(RoutedEvent & event, RoutedEventHandler const & handler);

    void RemoveHandler(RoutedEvent & event, RoutedEventHandler const & handler);

    void Measure(QSizeF);

    void SetWidth(double);

    void SetHeight(double);

    StylusPlugInCollection& StylusPlugIns();

    PenContexts* GetPenContexts();

protected:
    void RaiseEvent(RoutedEventArgs & e);

protected:
    virtual void OnChildDesiredSizeChanged(UIElement* child);

    virtual QList<UIElement*> CreateUIElementCollection(UIElement* logicalParent);

    virtual QList<UIElement*> LogicalChildren();

    virtual QList<UIElement*> InternalChildren();

    virtual Geometry* GetLayoutClip(QSizeF layoutSlotSize);

    virtual QSizeF ArrangeOverride(QSizeF arrangeSize);

    virtual QSizeF MeasureOverride(QSizeF availableSize);

    virtual void OnPreApplyTemplate();

protected:

private:
    virtual bool event(QEvent *event) override;

private:
    enum PrivateFlag {
        HasRoutedEventStore = 1,
        HasPenContexts = 2,
    };

    Q_DECLARE_FLAGS(PrivateFlags, PrivateFlag)

    PrivateFlags privateFlags_;
};

enum class FlowDirection
{
    /// <internalonly>
    /// Sets the primary text advance direction to left-to-right, and the line
    /// progression direction to top-to-bottom as is common in most Roman-based
    /// documents. For most characters, the current text position is advanced
    /// from left to right after each glyph is rendered. The 'direction' property
    /// is set to 'ltr'.
    /// </internalonly>
    LeftToRight,

    /// <internalonly>
    /// Sets the primary text advance direction to right-to-left, and the line
    /// progression direction to top-to-bottom as is common in Arabic or Hebrew
    /// scripts. The direction property is set to 'rtl'.
    /// </internalonly>
    RightToLeft,
};

class INKCANVAS_EXPORT FrameworkElement : public UIElement
{
    Q_OBJECT
public:
    QRectF Margin();

    double ActualWidth();

    double ActualHeight();

    void BringIntoView(QRectF);

    FlowDirection GetFlowDirection();

private:
    virtual int VisualChildrenCount();

    virtual Visual* GetVisualChild(int index);
};

#endif // UIELEMENT_H