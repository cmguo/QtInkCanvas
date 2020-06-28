#ifndef UIELEMENT_H
#define UIELEMENT_H

#include "InkCanvas_global.h"

#include "Windows/Media/visual.h"
#include "Windows/Media/matrix.h"
#include "Windows/routedeventargs.h"
#include "Windows/rect.h"
#include "Collections/Generic/list.h"

#include <QFlags>
#include <QObject>

#include <functional>

INKCANVAS_BEGIN_NAMESPACE

class RoutedEvent;
class RoutedEventArgs;
class Geometry;

class StylusPlugInCollection;
class PenContexts;

// namespace System.Windows

class INKCANVAS_EXPORT UIElement : public QObject, public Visual
{
    Q_OBJECT
public:
    static RoutedEvent LostMouseCaptureEvent;

    static RoutedEvent LostStylusCaptureEvent;

    static DependencyProperty const * const RenderTransformProperty;

    static constexpr int ITEM_DATA = 6000;

    static UIElement* fromItem(QGraphicsItem* item);

signals:
    void LayoutUpdated(EventArgs&);

    void IsVisibleChanged();

    void IsHitTestVisibleChanged();

    void IsEnabledChanged();

public:
    UIElement();

    virtual ~UIElement() override;

    void ApplyTemplate();

    List<UIElement*> Children();

    UIElement* Parent();

    void InvalidateVisual();

    template<typename T>
    bool InstanceOf()
    {
        return metaObject()->inherits(&T::staticMetaObject);
    }

    GeneralTransform LayoutTransform();

    GeneralTransform RenderTransform();

    GeneralTransform TransformToVisual(UIElement* visual);

    Size DesiredSize();

    Size RenderSize();

    void SetRenderSize(Size const & size);

    void Arrange(Rect const & rect);

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

    void Measure(Size);

    void SetWidth(double);

    void SetHeight(double);

    StylusPlugInCollection& StylusPlugIns();

    PenContexts* GetPenContexts();

    void SetLimitInputPosition(bool active);

    bool GetLimitInputPosition() const;

public:
    void RaiseEvent(RoutedEventArgs & e);

protected:
    virtual void OnChildDesiredSizeChanged(UIElement* child);

    virtual List<UIElement*> CreateUIElementCollection(UIElement* logicalParent);

    virtual List<UIElement*> LogicalChildren();

    virtual List<UIElement*> InternalChildren();

    virtual Geometry* GetLayoutClip(Size layoutSlotSize);

    virtual Size ArrangeOverride(Size arrangeSize);

    virtual Size MeasureOverride(Size availableSize);

    virtual void OnPreApplyTemplate();

public:
    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

    virtual bool sceneEvent(QEvent *event) override;

    virtual QRectF boundingRect() const override;

private:
    enum PrivateFlag {
        HasRoutedEventStore = 1,
        HasPenContexts = 2,
        LimitInputPosition = 4,
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
    static DependencyProperty const * const LayoutTransformProperty;

public:
    Rect Margin();

    double ActualWidth();

    double ActualHeight();

    void BringIntoView(Rect);

    FlowDirection GetFlowDirection();

private:
    virtual int VisualChildrenCount();

    virtual Visual* GetVisualChild(int index);
};

INKCANVAS_END_NAMESPACE

#endif // UIELEMENT_H
