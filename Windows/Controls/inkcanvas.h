#ifndef INKCANVAS_H
#define INKCANVAS_H

#include "InkCanvas_global.h"

#include "Windows/uielement.h"
#include "Windows/Ink/events.h"
#include "Windows/Controls/editingmode.h"
#include "Windows/Controls/inkcanvasselectionhitresult.h"
#include "Internal/Ink/inkcanvasclipboardformat.h"
#include "Windows/Ink/applicationgesture.h"

class InkCanvasSelection;
class InkCanvasSelectionAdorner;
class InkCanvasFeedbackAdorner;
class InkCanvasInnerCanvas;
class EditingCoordinator;
class InkPresenter;
class ClipboardProcessor;
class GestureRecognizer;
class DynamicRenderer;
class StylusPointCollection;
class Visual;
class HitTestResult;
class PointHitTestParameters;
class QMimeData;
class QBrush;

typedef QMimeData DataObject;

class InkCanvasSelectionEditingEventArgs;
class InkCanvasStrokeCollectedEventArgs;
class InkCanvasStrokeErasingEventArgs;
class InkCanvasGestureEventArgs;
class InkCanvasStrokesReplacedEventArgs;
class RoutedEventArgs;
class EventArgs;
class InkCanvasSelectionChangingEventArgs;
class QueryCursorEventArgs;
class DependencyPropertyChangedEventArgs;
class RoutedEvent;
class ExecutedRoutedEventArgs;

class RoutedCommand {};

class AdornerDecorator;

class QPolygonF;

// namespace System.Windows.Controls

class INKCANVAS_EXPORT InkCanvas : public FrameworkElement
{
    Q_OBJECT
public:
    /// <summary>
    /// The static constructor
    /// </summary>
    /*
    static InkCanvas()
    {
        Type ownerType = typeof(InkCanvas);

        // NTRAID-WINDOWS#1423922-2005/12/15-WAYNEZEN,
        // We should add the following listener as the class handler which will be guarantied to receive the
        // notification before the handler on the instances. So we won't be trapped in the bad state due to the
        // event routing.
        // Listen to stylus events which will be redirected to the current StylusEditingBehavior

        //Down
        EventManager.RegisterClassHandler(ownerType, Stylus.StylusDownEvent,
            new StylusDownEventHandler(_OnDeviceDown<StylusDownEventArgs>));
        EventManager.RegisterClassHandler(ownerType, Mouse.MouseDownEvent,
            new MouseButtonEventHandler(_OnDeviceDown<MouseButtonEventArgs>));

        //Up
        EventManager.RegisterClassHandler(ownerType, Stylus.StylusUpEvent,
            new StylusEventHandler(_OnDeviceUp<StylusEventArgs>));
        EventManager.RegisterClassHandler(ownerType, Mouse.MouseUpEvent,
            new MouseButtonEventHandler(_OnDeviceUp<MouseButtonEventArgs>));



        EventManager.RegisterClassHandler(ownerType, Mouse.QueryCursorEvent,
            new QueryCursorEventHandler(_OnQueryCursor), true);

        // Set up the commanding handlers
        _RegisterClipboardHandlers();
        CommandHelpers.RegisterCommandHandler(ownerType, ApplicationCommands::Delete,
            new ExecutedRoutedEventHandler(_OnCommandExecuted), new CanExecuteRoutedEventHandler(_OnQueryCommandEnabled));

        CommandHelpers.RegisterCommandHandler(ownerType, ApplicationCommands::SelectAll,
            Key.A, ModifierKeys.Control, new ExecutedRoutedEventHandler(_OnCommandExecuted), new CanExecuteRoutedEventHandler(_OnQueryCommandEnabled));

        CommandHelpers.RegisterCommandHandler(ownerType, InkCanvas.DeselectCommand,
            new ExecutedRoutedEventHandler(_OnCommandExecuted), new CanExecuteRoutedEventHandler(_OnQueryCommandEnabled),
            SRID.InkCanvasDeselectKey, SRID.InkCanvasDeselectKeyDisplayString);

        //
        //set our clipping
        //
        ClipToBoundsProperty.OverrideMetadata(ownerType, new FrameworkPropertyMetadata(BooleanBoxes.TrueBox));

        //
        //enable input focus
        //
        FocusableProperty.OverrideMetadata(ownerType, new FrameworkPropertyMetadata(BooleanBoxes.TrueBox));

        // The default InkCanvas style
        Style defaultStyle = new Style(ownerType);
        // The background - Window Color
        defaultStyle.Setters.Add(new Setter(InkCanvas.BackgroundProperty,
                        new DynamicResourceExtension(SystemColors.WindowBrushKey)));
        // Default InkCanvas to having flicks disabled by default.
        defaultStyle.Setters.Add(new Setter(Stylus.IsFlicksEnabledProperty, false));
        // Default InkCanvas to having tap feedback disabled by default.
        defaultStyle.Setters.Add(new Setter(Stylus.IsTapFeedbackEnabledProperty, false));
        // Default InkCanvas to having touch feedback disabled by default.
        defaultStyle.Setters.Add(new Setter(Stylus.IsTouchFeedbackEnabledProperty, false));

        // Set MinWidth to 350d if Width is set to Auto
        Trigger trigger = new Trigger();
        trigger.Property = WidthProperty;
        trigger.Value = double.NaN;
        Setter setter = new Setter();
        setter.Property = MinWidthProperty;
        setter.Value = 350d;
        trigger.Setters.Add(setter);
        defaultStyle.Triggers.Add(trigger);

        // Set MinHeight to 250d if Height is set to Auto
        trigger = new Trigger();
        trigger.Property = HeightProperty;
        trigger.Value = double.NaN;
        setter = new Setter();
        setter.Property = MinHeightProperty;
        setter.Value = 250d;
        trigger.Setters.Add(setter);
        defaultStyle.Triggers.Add(trigger);

        // Seal the default style
        defaultStyle.Seal();

        StyleProperty.OverrideMetadata(ownerType, new FrameworkPropertyMetadata(defaultStyle));
        DefaultStyleKeyProperty.OverrideMetadata(ownerType, new FrameworkPropertyMetadata(typeof(InkCanvas)));

        FocusVisualStyleProperty.OverrideMetadata(ownerType, new FrameworkPropertyMetadata((object)null /* default value /));
    }*/

private:
    Q_DISABLE_COPY(InkCanvas)

public:
    /// <summary>
    /// Public constructor.
    /// </summary>
    InkCanvas(QGraphicsItem* parent = nullptr);

    virtual ~InkCanvas() override;

private:
    /// <summary>
    /// Private initialization method used by the constructors
    /// </summary>
    void Initialize();

    /// <summary>
    /// Private helper used to change the Ink objects.  Used in the constructor
    /// and the Ink property.
    ///
    /// NOTE -- Caller is responsible for clearing any selection!  (We can't clear it
    ///         here because the Constructor calls this method and it would end up calling
    ///         looking like it could call a virtual method and FxCop doesn't like that!)
    ///
    /// </summary>
    void InitializeInkObject();


protected:
    /// <summary>
    /// MeasureOverride
    /// </summary>
    /// <param name="availableSize"></param>
    /// <returns></returns>
    virtual QSizeF MeasureOverride(QSizeF availableSize) override;

    /// <summary>
    /// ArrangeOverride
    /// </summary>
    /// <param name="arrangeSize"></param>
    /// <returns></returns>
    virtual QSizeF ArrangeOverride(QSizeF arrangeSize) override;


    /// <summary>
    /// HitTestCore implements precise hit testing against render contents
    /// </summary>
    virtual HitTestResult HitTestCore(PointHitTestParameters hitTestParams) override;

    /// <summary>
    /// OnPropertyChanged
    /// </summary>
    virtual void OnPropertyChanged(DependencyPropertyChangedEventArgs& e) override;

    /// <summary>
    /// Called when the Template's tree is about to be generated
    /// </summary>
    virtual void OnPreApplyTemplate() override;

    /*
    /// <summary>
    /// Returns the Visual children count.
    /// </summary>
    protected override int VisualChildrenCount
    {
        get { return (_localAdornerDecorator == nullptr) ? 0 : 1; }
    }

    /// <summary>
    /// Returns the child at the specified index.
    /// </summary>
    protected override Visual GetVisualChild(int index)
    {
        if (    (_localAdornerDecorator == nullptr)
            ||  (index != 0))
        {
            throw new ArgumentOutOfRangeException("index", index, SR.Get(SRID.Visual_ArgumentOutOfRange));
        }

        return _localAdornerDecorator;
    }

    /// <summary>
    /// UIAutomation support
    /// </summary>
    protected override AutomationPeer OnCreateAutomationPeer()
    {
        return new InkCanvasAutomationPeer(this);
    }*/

    /// <summary>
    ///     The DependencyProperty for the Background property.
    /// </summary>
    static DependencyProperty const * const BackgroundProperty;

    /// <summary>
    ///     An object that describes the background.
    /// </summary>
    //[Bindable(true), Category("Appearance")]
    QBrush Background();
    void SetBackground(QBrush value);


    /// <summary>
    /// Top DependencyProperty
    /// </summary>
    static DependencyProperty const * const TopProperty;

    /// <summary>
    /// Reads the attached property Top from the given element.
    /// </summary>
    /// <param name="element">The element from which to read the Top attached property.</param>
    /// <returns>The property's value.</returns>
    /// <seealso cref="InkCanvas.TopProperty" />
    //[TypeConverter("System.Windows.LengthConverter, PresentationFramework, Version=" +BuildInfo.WCP_VERSION + ", Culture=neutral, PublicKeyToken=" +BuildInfo.WCP_PUBLIC_KEY_TOKEN + ", Custom=null")]
    //[AttachedPropertyBrowsableForChildren()]
    static double GetTop(UIElement* element)
    {
        if (element == nullptr) { throw new std::exception("element"); }
        return element->GetValue<double>(TopProperty);
    }

    /// <summary>
    /// Writes the attached property Top to the given element.
    /// </summary>
    /// <param name="element">The element to which to write the Top attached property.</param>
    /// <param name="length">The length to set</param>
    /// <seealso cref="InkCanvas.TopProperty" />
    static void SetTop(UIElement* element, double length)
    {
        if (element == nullptr) { throw new std::exception("element"); }
        element->SetValue(TopProperty, length);
    }

    /// <summary>
    /// The Bottom DependencyProperty
    /// </summary>
    static DependencyProperty const * const BottomProperty;

    /// <summary>
    /// Reads the attached property Bottom from the given element.
    /// </summary>
    /// <param name="element">The element from which to read the Bottom attached property.</param>
    /// <returns>The property's Length value.</returns>
    /// <seealso cref="InkCanvas.BottomProperty" />
    //[TypeConverter("System.Windows.LengthConverter, PresentationFramework, Version=" +BuildInfo.WCP_VERSION + ", Culture=neutral, PublicKeyToken=" +BuildInfo.WCP_PUBLIC_KEY_TOKEN + ", Custom=null")]
    //[AttachedPropertyBrowsableForChildren()]
    static double GetBottom(UIElement* element)
    {
        if (element == nullptr) { throw new std::exception("element"); }
        return element->GetValue<double>(BottomProperty);
    }

    /// <summary>
    /// Writes the attached property Bottom to the given element.
    /// </summary>
    /// <param name="element">The element to which to write the Bottom attached property.</param>
    /// <param name="length">The Length to set</param>
    /// <seealso cref="InkCanvas.BottomProperty" />
    static void SetBottom(UIElement* element, double length)
    {
        if (element == nullptr) { throw new std::exception("element"); }
        element->SetValue(BottomProperty, length);
    }

    /// <summary>
    /// The Left DependencyProperty
    /// </summary>
    static DependencyProperty const * const LeftProperty;

    /// <summary>
    /// Reads the attached property Left from the given element.
    /// </summary>
    /// <param name="element">The element from which to read the Left attached property.</param>
    /// <returns>The property's value.</returns>
    /// <seealso cref="InkCanvas.LeftProperty" />
    //[TypeConverter("System.Windows.LengthConverter, PresentationFramework, Version=" +BuildInfo.WCP_VERSION + ", Culture=neutral, PublicKeyToken=" +BuildInfo.WCP_PUBLIC_KEY_TOKEN + ", Custom=null")]
    //[AttachedPropertyBrowsableForChildren()]
    static double GetLeft(UIElement* element)
    {
        if (element == nullptr) { throw new std::exception("element"); }
        return element->GetValue<double>(LeftProperty);
    }

    /// <summary>
    /// Writes the attached property Left to the given element.
    /// </summary>
    /// <param name="element">The element to which to write the Left attached property.</param>
    /// <param name="length">The length to set</param>
    /// <seealso cref="InkCanvas.LeftProperty" />
    static void SetLeft(UIElement* element, double length)
    {
        if (element == nullptr) { throw new std::exception("element"); }
        element->SetValue(LeftProperty, length);
    }

    /// <summary>
    /// The Right DependencyProperty
    /// </summary>
    static DependencyProperty const * const RightProperty;

    /// <summary>
    /// Reads the attached property Right from the given element.
    /// </summary>
    /// <param name="element">The element from which to read the Right attached property.</param>
    /// <returns>The property's Length value.</returns>
    /// <seealso cref="InkCanvas.RightProperty" />
    //[TypeConverter("System.Windows.LengthConverter, PresentationFramework, Version=" +BuildInfo.WCP_VERSION + ", Culture=neutral, PublicKeyToken=" +BuildInfo.WCP_PUBLIC_KEY_TOKEN + ", Custom=null")]
    //[AttachedPropertyBrowsableForChildren()]
    static double GetRight(UIElement* element)
    {
        if (element == nullptr) { throw new std::exception("element"); }
        return element->GetValue<double>(RightProperty);
    }

    /// <summary>
    /// Writes the attached property Right to the given element.
    /// </summary>
    /// <param name="element">The element to which to write the Right attached property.</param>
    /// <param name="length">The Length to set</param>
    /// <seealso cref="InkCanvas.RightProperty" />
    static void SetRight(UIElement* element, double length)
    {
        if (element == nullptr) { throw new std::exception("element"); }
        element->SetValue(RightProperty, length);
    }

    /// <summary>
    /// OnPositioningChanged
    /// </summary>
    /// <param name="d"></param>
    /// <param name="e"></param>
    static void OnPositioningChanged(DependencyObject& d, DependencyPropertyChangedEventArgs& e);

public:
    /// <summary>
    ///     The DependencyProperty for the Strokes property.
    /// </summary>
    //static DependencyProperty StrokesProperty =
    //        InkPresenter().StrokesProperty.AddOwner(
    //                typeof(InkCanvas),
    //                new FrameworkPropertyMetadata(
    //                        new StrokeCollectionDefaultValueFactory(),
    //                        new PropertyChangedCallback(OnStrokesChanged)));

    static DependencyProperty const * const StrokesProperty;

    /// <summary>
    /// Gets/Sets the Strokes property.
    /// </summary>
    QSharedPointer<StrokeCollection> Strokes()
    {
        return GetValue<QSharedPointer<StrokeCollection>>(StrokesProperty);
    }
    void SetStrokes(QSharedPointer<StrokeCollection> value)
    {
        SetValue(StrokesProperty, value);
    }

    static void OnStrokesChanged(DependencyObject& d, DependencyPropertyChangedEventArgs& e);

public:
    /// <summary>
    /// Returns the SelectionAdorner
    /// </summary>
    InkCanvasSelectionAdorner& SelectionAdorner();

    /// <summary>
    /// Returns the FeedbackAdorner
    /// </summary>
    InkCanvasFeedbackAdorner& FeedbackAdorner();

    /// <summary>
    /// Read/Write access to the EraserShape property.
    /// </summary>
    //[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
    //public bool IsGestureRecognizerAvailable
    //{
    //    get
    //    {
            //this property will verify access
    //        return GetGestureRecognizer().IsRecognizerAvailable;
     //   }
    //}


    /// <summary>
    /// Emulate Panel's Children property.
    /// </summary>
    //[DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
    //public UIElement*Collection Children
    //{
    //    get
   //     {
            // No need to invoke VerifyAccess since the call is forwarded.

    //        return InnerCanvas.Children;
   //     }
    //}

    /// <summary>
    /// The DependencyProperty for the DefaultDrawingAttributes property.
    /// </summary>
    //static DependencyProperty DefaultDrawingAttributesProperty =
    //        DependencyProperty.Register(
    //                "DefaultDrawingAttributes",
    //                typeof(DrawingAttributes),
    //                typeof(InkCanvas),
    //                new FrameworkPropertyMetadata(
    //                        new DrawingAttributesDefaultValueFactory(),
    //                        new PropertyChangedCallback(OnDefaultDrawingAttributesChanged)),
    //                (ValidateValueCallback)delegate(object value)
    //                    { return value != nullptr; });

    static DependencyProperty const * const DefaultDrawingAttributesProperty;

    /// <summary>
    /// Gets/Sets the DefaultDrawingAttributes property.
    /// </summary>
    QSharedPointer<DrawingAttributes> DefaultDrawingAttributes();
    void SetDefaultDrawingAttributes(QSharedPointer<DrawingAttributes> value);

    static void OnDefaultDrawingAttributesChanged(DependencyObject& d, DependencyPropertyChangedEventArgs& e);

    /// <summary>
    /// Read/Write access to the EraserShape property.
    /// </summary>
    //[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
    StylusShape* EraserShape();
    void SetEraserShape(StylusShape * value);

    void SetEraseClip(QPolygonF const & shape);

    /// <summary>
    /// ActiveEditingMode
    /// </summary>
    //internal static readonly DependencyPropertyKey ActiveEditingModePropertyKey =
    //        DependencyProperty.RegisterReadOnly(
    //                "ActiveEditingMode",
    //                typeof(InkCanvasEditingMode),
    //                typeof(InkCanvas),
    //                new FrameworkPropertyMetadata(InkCanvasEditingMode.Ink));

    /// <summary>
    /// ActiveEditingModeProperty Dependency Property
    /// </summary>
    //static DependencyProperty ActiveEditingModeProperty = ActiveEditingModePropertyKey.DependencyProperty;
    static DependencyProperty const * const ActiveEditingModeProperty;

    /// <summary>
    /// Gets the ActiveEditingMode
    /// </summary>
    InkCanvasEditingMode ActiveEditingMode()
    {
        return GetValue<InkCanvasEditingMode>(ActiveEditingModeProperty);
    }

    /// <summary>
    /// The DependencyProperty for the EditingMode property.
    /// </summary>
    //static DependencyProperty EditingModeProperty =
    //        DependencyProperty.Register(
    //                "EditingMode",
    //                typeof(InkCanvasEditingMode),
    //                typeof(InkCanvas),
    //                new FrameworkPropertyMetadata(
    //                        InkCanvasEditingMode.Ink,
    //                      new PropertyChangedCallback(OnEditingModeChanged)),
    //                  new ValidateValueCallback(ValidateEditingMode));
    static DependencyProperty const * const EditingModeProperty;


    /// <summary>
    /// Gets/Sets EditingMode
    /// </summary>
    InkCanvasEditingMode EditingMode()
    {
        return GetValue<InkCanvasEditingMode>(EditingModeProperty);
    }
    void SetEditingMode(InkCanvasEditingMode value)
    {
        SetValue(EditingModeProperty, value);
    }


    static void OnEditingModeChanged(DependencyObject& d, DependencyPropertyChangedEventArgs& e);

    /// <summary>
    /// The DependencyProperty for the EditingModeInverted property.
    /// </summary>
    //static DependencyProperty EditingModeInvertedProperty =
    //        DependencyProperty.Register(
    //                "EditingModeInverted",
    //                typeof(InkCanvasEditingMode),
    //                typeof(InkCanvas),
    //                new FrameworkPropertyMetadata(
    //                        InkCanvasEditingMode.EraseByStroke,
    //                        new PropertyChangedCallback(OnEditingModeInvertedChanged)),
    //                new ValidateValueCallback(ValidateEditingMode));
    static DependencyProperty const * const EditingModeInvertedProperty;

    /// <summary>
    /// Gets/Sets EditingMode
    /// </summary>
    InkCanvasEditingMode EditingModeInverted()
    {
        return GetValue<InkCanvasEditingMode>(EditingModeInvertedProperty);
    }
    void SetEditingModeInverted(InkCanvasEditingMode value)
    {
        SetValue(EditingModeInvertedProperty, value);
    }

    static void OnEditingModeInvertedChanged(DependencyObject& d, DependencyPropertyChangedEventArgs& e);

    static bool ValidateEditingMode(QVariant value);

    /// <summary>
    /// This flag indicates whether the developer is using a custom mouse cursor.
    ///
    /// If this flag is true, we will never change the current cursor on them. Not
    /// on edit mode change.
    /// </summary>
    //[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
    bool UseCustomCursor();
    void SetUseCustomCursor(bool value);

    /// <summary>
    /// Gets or set if moving of selection is enabled
    /// </summary>
    /// <value>bool</value>
    bool MoveEnabled();
    void SetMoveEnabled(bool value);

    /// <summary>
    /// Gets or set if resizing selection is enabled
    /// </summary>
    /// <value>bool</value>
    bool ResizeEnabled();
    void SetResizeEnabled(bool value);


    /// <summary>
    /// Read/Write access to the DefaultPacketDescription property.
    /// </summary>
    //[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
    QSharedPointer<StylusPointDescription> DefaultStylusPointDescription();
    void SetDefaultStylusPointDescription(QSharedPointer<StylusPointDescription> value);

    /// <summary>
    /// Read/Write the enabled ClipboardFormats
    /// </summary>
    //[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
    QList<InkCanvasClipboardFormat> PreferredPasteFormats();
    void SetPreferredPasteFormats(QList<InkCanvasClipboardFormat> value);


    /// <summary>
    ///     The StrokeErased Routed Event
    /// </summary>
    static RoutedEvent StrokeCollectedEvent;

    /// <summary>
    ///     Add / Remove StrokeCollected handler
    /// </summary>
    //[Category("Behavior")]
    //public event InkCanvasStrokeCollectedEventHandler StrokeCollected
   // {
   //     add
   //     {
   //         AddHandler(InkCanvas.StrokeCollectedEvent, value);
   //     }

   //     remove
   //     {
    //        RemoveHandler(InkCanvas.StrokeCollectedEvent, value);
   //     }
    //}
signals:
    void StrokeCollected(InkCanvasStrokeCollectedEventArgs& e);

protected:
    /// <summary>
    /// Protected virtual version for developers deriving from InkCanvas.
    /// This method is what actually throws the event.
    /// </summary>
    /// <param name="e">InkCanvasStrokeCollectedEventArgs& to raise the event with</param>
    virtual void OnStrokeCollected(InkCanvasStrokeCollectedEventArgs& e);

    /// <summary>
    /// Allows the InkCollectionBehavior to raise the StrokeCollected event via the protected virtual
    /// </summary>
    /// <param name="e">InkCanvasStrokeCollectedEventArgs& to raise the event with</param>
    /// <param name="userInitiated">true only if 100% of the stylusPoints that makes up the stroke
    /// came from EventArgs& with the UserInitiated flag set to true</param>
    /// <SecurityNote>
    ///     Critical: Calls critical method GetGestureRecognizer().CriticalRecognize.  It is important
    ///         that this is only called if userInitiated is true.
    /// </SecurityNote>
    //[SecurityCritical]
    void RaiseGestureOrStrokeCollected(InkCanvasStrokeCollectedEventArgs& e, bool userInitiated);

public:
    /// <summary>
    ///     The Gesture Routed Event
    /// </summary>
    static RoutedEvent GestureEvent;

    /// <summary>
    ///     Add / Remove Gesture handler
    /// </summary>
    //[Category("Behavior")]
    //public event InkCanvasGestureEventHandler Gesture
    //{
    //    add
    //    {
    //        AddHandler(InkCanvas.GestureEvent, value);
    //    }

    //    remove
    //    {
    //        RemoveHandler(InkCanvas.GestureEvent, value);
    //    }
    //}
signals:
    void Gesture(InkCanvasGestureEventArgs& e);

protected:

    /// <summary>
    /// Protected virtual version for developers deriving from InkCanvas.
    /// This method is what actually throws the event.
    /// </summary>
    /// <param name="e">InkCanvasGestureEventArgs& to raise the event with</param>
    virtual void OnGesture(InkCanvasGestureEventArgs& e);

    /// <summary>
    /// Raised when the InkCanvas.Strokes StrokeCollection has been replaced with another one
    /// </summary>
    //public event InkCanvasStrokesReplacedEventHandler StrokesReplaced;

signals:
    void StrokesReplaced(InkCanvasStrokesReplacedEventArgs& e);

protected:
    /// <summary>
    /// Protected virtual version for developers deriving from InkCanvas.
    /// This method is what actually throws the event.
    /// </summary>
    /// <param name="e">InkCanvasStrokesChangedEventArgs& to raise the event with</param>
    virtual void OnStrokesReplaced(InkCanvasStrokesReplacedEventArgs& e);

    /// <summary>
    /// Raised when the InkCanvas.DefaultDrawingAttributes has been replaced with another one
    /// </summary>
    //public event DrawingAttributesReplacedEventHandler DefaultDrawingAttributesReplaced;

signals:
    void DefaultDrawingAttributesReplaced(DrawingAttributesReplacedEventArgs& e);

protected:
    /// <summary>
    /// Protected virtual version for developers deriving from InkCanvas.
    /// This method is what actually throws the event.
    /// </summary>
    /// <param name="e">DrawingAttributesReplacedEventArgs& to raise the event with</param>
    virtual void OnDefaultDrawingAttributesReplaced(DrawingAttributesReplacedEventArgs& e);

    /// <summary>
    /// Private helper for raising DDAReplaced.  Invalidates the inking cursor
    /// </summary>
    /// <param name="e"></param>
    void RaiseDefaultDrawingAttributeReplaced(DrawingAttributesReplacedEventArgs& e);

    /// <summary>
    ///     Event corresponds to ActiveEditingModeChanged
    /// </summary>
    static RoutedEvent ActiveEditingModeChangedEvent;

    /// <summary>
    ///     Add / Remove ActiveEditingModeChanged handler
    /// </summary>
    //[Category("Behavior")]
    //public event RoutedEventHandler ActiveEditingModeChanged
    //{
    //    add
    //    {
    //        AddHandler(InkCanvas.ActiveEditingModeChangedEvent, value);
    //    }
    //    remove
    //    {
    //        RemoveHandler(InkCanvas.ActiveEditingModeChangedEvent, value);
    //    }
    //}
signals:
    void ActiveEditingModeChanged(RoutedEventArgs & e);

protected:
    /// <summary>
    /// Protected virtual version for developers deriving from InkCanvas.
    /// This method is what actually throws the event.
    /// </summary>
    /// <param name="e">EventArgs& to raise the event with</param>
    virtual void OnActiveEditingModeChanged(RoutedEventArgs& e);

    /// <summary>
    /// Private helper that raises ActiveEditingModeChanged
    /// </summary>
    /// <param name="e">EventArgs& to raise the event with</param>
    void RaiseActiveEditingModeChanged(RoutedEventArgs& e);



    /// <summary>
    ///     Event corresponds to EditingModeChanged
    /// </summary>
    static RoutedEvent EditingModeChangedEvent;

    /// <summary>
    ///     Add / Remove EditingModeChanged handler
    /// </summary>
    //[Category("Behavior")]
    //public event RoutedEventHandler EditingModeChanged
    //{
    //    add
    //    {
    //        AddHandler(InkCanvas.EditingModeChangedEvent, value);
    //    }

    //    remove
    //    {
    //        RemoveHandler(InkCanvas.EditingModeChangedEvent, value);
    //    }
    //}

    /// <summary>
    /// Protected virtual version for developers deriving from InkCanvas.
    /// This method is what actually throws the event.
    /// </summary>
    /// <param name="e">EventArgs& to raise the event with</param>
    virtual void OnEditingModeChanged2(RoutedEventArgs& e);

    /// <summary>
    /// Private helper that raises EditingModeChanged but first
    /// talks to the InkEditor about it
    /// </summary>
    /// <param name="e">EventArgs& to raise the event with</param>
    void RaiseEditingModeChanged(RoutedEventArgs& e);

    //note: there is no need for an internal RaiseEditingModeInvertedChanging
    //since this isn't a dynamic property and therefore can not be set
    //outside of this class

    /// <summary>
    ///     Event corresponds to EditingModeInvertedChanged
    /// </summary>
    static RoutedEvent EditingModeInvertedChangedEvent;

    /// <summary>
    ///     Add / Remove EditingModeChanged handler
    /// </summary>
    //[Category("Behavior")]
    //public event RoutedEventHandler EditingModeInvertedChanged
    //{
    //    add
    //    {
    //        AddHandler(InkCanvas.EditingModeInvertedChangedEvent, value);
    //    }

    //    remove
    //    {
    //        RemoveHandler(InkCanvas.EditingModeInvertedChangedEvent, value);
    //    }
    //}

    /// <summary>
    /// Protected virtual version for developers deriving from InkCanvas.
    /// This method is what actually throws the event.
    /// </summary>
    /// <param name="e">EventArgs& to raise the event with</param>
    virtual void OnEditingModeInvertedChanged2(RoutedEventArgs& e);

    /// <summary>
    /// Private helper that raises EditingModeInvertedChanged but first
    /// talks to the InkEditor about it
    /// </summary>
    /// <param name="e">EventArgs& to raise the event with</param>
    void RaiseEditingModeInvertedChanged(RoutedEventArgs& e);

    /// <summary>
    /// Occurs when the user has moved the selection, after they lift their stylus to commit the change.
    /// This event allows the developer to cancel the move.
    /// </summary>
    //public event  InkCanvasSelectionEditingEventHandler SelectionMoving;
signals:
    void SelectionMoving(InkCanvasSelectionEditingEventArgs& e);

protected:
    /// <summary>
    /// Protected virtual version for developers deriving from InkCanvas.
    /// This method is what actually throws the event.
    /// </summary>
    /// <param name="e"> InkCanvasSelectionEditingEventArgs& to raise the event with</param>
    virtual void OnSelectionMoving( InkCanvasSelectionEditingEventArgs& e);

    /// <summary>
    /// Allows the EditingBehaviors to raise the SelectionMoving event via the protected virtual
    /// </summary>
    /// <param name="e"> InkCanvasSelectionEditingEventArgs& to raise the event with</param>
    void RaiseSelectionMoving( InkCanvasSelectionEditingEventArgs& e);

    /// <summary>
    /// Occurs when the user has moved the selection, after they lift their stylus to commit the change.
    /// This event allows the developer to cancel the move.
    /// </summary>
    //public event EventHandler SelectionMoved;
signals:
    void SelectionMoved(EventArgs& e);

protected:
    /// <summary>
    /// Protected virtual version for developers deriving from InkCanvas.
    /// This method is what actually throws the event.
    /// </summary>
    /// <param name="e">EventArgs& to raise the event with</param>
    virtual void OnSelectionMoved(EventArgs& e);

    /// <summary>
    /// Allows the EditingBehaviors to raise the SelectionMoved event via the protected virtual
    /// </summary>
    /// <param name="e">EventArgs& to raise the event with</param>
    void RaiseSelectionMoved(EventArgs& e);

    /// <summary>
    /// Occurs when the user has erased Strokes using the erase behavior
    ///
    /// This event allows the developer to cancel the erase -- therefore, the Stroke should not disappear until
    /// this event has finished.
    /// </summary>
    //public event InkCanvasStrokeErasingEventHandler StrokeErasing;
signals:
    void StrokeErasing(InkCanvasStrokeErasingEventArgs& e);

protected:
    /// <summary>
    /// Protected virtual version for developers deriving from InkCanvas.
    /// This method is what actually throws the event.
    /// </summary>
    /// <param name="e">InkCanvasStrokeErasingEventArgs& to raise the event with</param>
    virtual void OnStrokeErasing(InkCanvasStrokeErasingEventArgs& e);

    /// <summary>
    /// Allows the EditingBehaviors to raise the InkErasing event via the protected virtual
    /// </summary>
    /// <param name="e">InkCanvasStrokeErasingEventArgs& to raise the event with</param>
    void RaiseStrokeErasing(InkCanvasStrokeErasingEventArgs& e);

    /// <summary>
    ///     The StrokeErased Routed Event
    /// </summary>
    static RoutedEvent StrokeErasedEvent;

    /// <summary>
    ///     Add / Remove EditingModeChanged handler
    /// </summary>
    //[Category("Behavior")]
    //public event RoutedEventHandler StrokeErased
    //{
    //    add
    //    {
    //        AddHandler(InkCanvas.StrokeErasedEvent, value);
    //    }

    //    remove
    //    {
    //        RemoveHandler(InkCanvas.StrokeErasedEvent, value);
    //    }
    //}

signals:
    void StrokeErased(RoutedEventArgs& e);

protected:
    /// <summary>
    /// Protected virtual version for developers deriving from InkCanvas.
    /// This method is what actually throws the event.
    /// </summary>
    /// <param name="e">EventArgs& to raise the event with</param>
    virtual void OnStrokeErased(RoutedEventArgs& e);

    /// <summary>
    /// Allows the EditingBehaviors to raise the InkErasing event via the protected virtual
    /// </summary>
    void RaiseInkErased();

    /// <summary>
    /// Occurs when the user has resized the selection, after they lift their stylus to commit the change.
    /// This event allows the developer to cancel the resize.
    /// </summary>
    //public event  InkCanvasSelectionEditingEventHandler SelectionResizing;
signals:
    void SelectionResizing(InkCanvasSelectionEditingEventArgs& e);

protected:
    /// <summary>
    /// Protected virtual version for developers deriving from InkCanvas.
    /// This method is what actually throws the event.
    /// </summary>
    /// <param name="e"> InkCanvasSelectionEditingEventArgs& to raise the event with</param>
    virtual void OnSelectionResizing( InkCanvasSelectionEditingEventArgs& e);

    /// <summary>
    /// Allows the EditingBehaviors to raise the SelectionResizing event via the protected virtual
    /// </summary>
    /// <param name="e"> InkCanvasSelectionEditingEventArgs& to raise the event with</param>
    void RaiseSelectionResizing( InkCanvasSelectionEditingEventArgs& e);

    /// <summary>
    /// Occurs when the selection has been resized via UI interaction.
    /// </summary>
    //public event EventHandler SelectionResized;
signals:
    void SelectionResized(EventArgs& e);

protected:
    /// <summary>
    /// Protected virtual version for developers deriving from InkCanvas.
    /// This method is what actually throws the event.
    /// </summary>
    /// <param name="e">EventArgs& to raise the event with</param>
    virtual void OnSelectionResized(EventArgs& e);

    /// <summary>
    /// Allows the EditingBehaviors to raise the SelectionResized event via the protected virtual
    /// </summary>
    /// <param name="e">EventArgs& to raise the event with</param>
    void RaiseSelectionResized(EventArgs& e);

    /// <summary>
    /// Occurs when the selection has been changed, either using the lasso or programmatically.
    /// This event allows the developer to cancel the change.
    /// </summary>
    //public event InkCanvasSelectionChangingEventHandler SelectionChanging;
signals:
    void SelectionChanging(InkCanvasSelectionChangingEventArgs& e);

protected:
    /// <summary>
    /// Protected virtual version for developers deriving from InkCanvas.
    /// This method is what actually throws the event.
    /// </summary>
    /// <param name="e">InkCanvasSelectionChangingEventArgs& to raise the event with</param>
    virtual void OnSelectionChanging(InkCanvasSelectionChangingEventArgs& e);

    /// <summary>
    /// Allows the EditingBehaviors to raise the SelectionChanging event via the protected virtual
    /// </summary>
    /// <param name="e">InkCanvasSelectionChangingEventArgs& to raise the event with</param>
    void RaiseSelectionChanging(InkCanvasSelectionChangingEventArgs& e);

    /// <summary>
    /// Occurs when the selection has been changed
    /// </summary>
    //public event EventHandler SelectionChanged;
signals:
    void SelectionChanged(EventArgs& e);

protected:
    /// <summary>
    /// Protected virtual version for developers deriving from InkCanvas.
    /// This method is what actually throws the event.
    /// </summary>
    /// <param name="e">EventArgs& to raise the event with</param>
    virtual void OnSelectionChanged(EventArgs& e);

    /// <summary>
    /// Allows the EditingBehaviors to raise the SelectionChanged event via the protected virtual
    /// </summary>
    /// <param name="e">EventArgs& to raise the event with</param>
    void RaiseSelectionChanged(EventArgs& e);

    /// <summary>
    /// The InkCanvas uses an inner Canvas to host children.  When the inner Canvas's children
    /// are changed, we need to call the protected virtual OnVisualChildrenChanged on the InkCanvas
    /// so that subclasses can be notified
    /// </summary>
    void RaiseOnVisualChildrenChanged(DependencyObject* visualAdded, DependencyObject* visualRemoved)
    {
        OnVisualChildrenChanged(visualAdded, visualRemoved);
    }


public:
    /// <summary>
    /// Returns the enabled gestures.  This method throws an exception if GestureRecognizerAvailable
    /// is false
    /// </summary>
    /// <returns></returns>
    QList<ApplicationGesture> GetEnabledGestures();

    /// <summary>
    /// Sets the enabled gestures.  This method throws an exception if GestureRecognizerAvailable
    /// is false
    /// </summary>
    /// <returns></returns>
    void SetEnabledGestures(QList<ApplicationGesture> applicationGestures);

    /// <summary>
    /// Get the selection bounds.
    /// </summary>
    /// <returns></returns>
    QRectF GetSelectionBounds();

    /// <summary>
    /// provides access to the currently selected elements which are children of this InkCanvas
    /// </summary>
    QList<UIElement*> GetSelectedElements();
    //{
    //    VerifyAccess();
    //    return InkCanvasSelection.SelectedElements;
    //}

    /// <summary>
    /// provides read access to the currently selected strokes
    /// </summary>
    QSharedPointer<StrokeCollection> GetSelectedStrokes();

    /// <summary>
    /// Overload which calls the more complex version, passing nullptr for selectedElements
    /// </summary>
    /// <param name="selectedStrokes">The strokes to select</param>
    void Select(QSharedPointer<StrokeCollection> selectedStrokes);

    /// <summary>
    /// Overload which calls the more complex version, passing nullptr for selectedStrokes
    /// </summary>
    /// <param name="selectedElements">The elements to select</param>
    void Select(QList<UIElement*> selectedElements);

    /// <summary>
    /// Overload which calls the more complex version, passing nullptr for selectedStrokes
    /// </summary>
    /// <param name="selectedStrokes">The strokes to select</param>
    /// <param name="selectedElements">The elements to select</param>
    void Select(QSharedPointer<StrokeCollection> selectedStrokes, QList<UIElement*> selectedElements);

    /// <summary>
    /// Hit test on the selection
    /// </summary>
    /// <param name="point"></param>
    /// <returns></returns>
    InkCanvasSelectionHitResult HitTestSelection(QPointF const &point);

    /// <summary>
    /// Copy the current selection in the InkCanvas to the clipboard
    /// </summary>
    void CopySelection();

    /// <summary>
    /// Copy the current selection in the InkCanvas to the clipboard and then delete it
    /// </summary>
    void CutSelection();

    /// <summary>
    /// Paste the contents of the clipboard into the InkCanvas
    /// </summary>
    void Paste();

    /// <summary>
    /// Paste the contents of the clipboard to the specified location in the InkCanvas
    /// </summary>
    void Paste(QPointF const &point);

    /// <summary>
    /// Return true if clipboard contents can be pasted into the InkCanvas.
    /// </summary>
    bool CanPaste();


    //------------------------------------------------------
    //
    //  IAddChild Interface
    //
    //------------------------------------------------------


    ///<summary>
    /// Called to Add the object as a Child.
    ///</summary>
    ///<param name="value">
    /// Object to add as a child
    ///</param>
    //void IAddChild.AddChild(Object value)
    //{
        //             VerifyAccess();

   //     if ( value == nullptr )
   //     {
    //        throw new std::exception("value");
    //    }

    //    ( (IAddChild)InnerCanvas ).AddChild(value);
    //}

    ///<summary>
    /// Called when text appears under the tag in markup.
    ///</summary>
    ///<param name="textData">
    /// Text to Add to the Canvas
    ///</param>
    //void IAddChild.AddText(string textData)
    //{
        //             VerifyAccess();

    //    ( (IAddChild)InnerCanvas ).AddText(textData);
    //}


    //------------------------------------------------------
    //
    //  Protected Properties
    //
    //------------------------------------------------------


    /// <summary>
    /// Returns enumerator to logical children.
    /// </summary>
    //protected internal override IEnumerator LogicalChildren
    //{
    //    get
    //    {
            //        VerifyAccess( );

            // Return the private logical children of the InnerCanvas
    //        return ( (InkCanvasInnerCanvas)InnerCanvas).PrivateLogicalChildren;
    //    }
    //}

    /// <summary>
    /// Protected DynamicRenderer* property.
    /// </summary>
    DynamicRenderer* GetDynamicRenderer();
    void SetDynamicRenderer(DynamicRenderer* value);

    /// <summary>
    /// Protected read only access to the InkPresenter this InkCanvas uses
    /// </summary>
    InkPresenter& GetInkPresenter();



    /// <summary>
    /// Deselect the current selection
    /// </summary>
    static RoutedCommand DeselectCommand;

private:
    /// <summary>
    /// UserInitiatedCanPaste
    /// </summary>
    /// <returns></returns>
    /// <SecurityNote>
    ///     Critical -      Elevates the AllClipboard permission for checking the supported data in InkCanvas.
    /// </SecurityNote>
    //[SecurityCritical]
    bool UserInitiatedCanPaste();

    /// <summary>
    /// PrivateCanPaste
    /// </summary>
    /// <returns></returns>
    bool PrivateCanPaste();

public:
    /// <summary>
    /// This method pastes data from an DataObject object
    /// </summary>
    void PasteFromDataObject(DataObject const * dataObj, QPointF const & point);

    /// <summary>
    /// Copies the InkCanvas contents to a DataObject and returns it to the caller.
    ///  Can return nullptr for DataObject.
    /// </summary>
    /// <SecurityNote>
    ///     Critical: Clipboard.SetDataObject will invoke DataObject.DataStore.GetFormats.
    ///                 The methods demands SerializationPermission. We perform the elevation before
    ///                 calling SetDataObject.
    ///     TreatAsSafe: There is no input here. The ISF data are safe to being put in the clipboard.
    /// </SecurityNote>
    //[SecurityCritical, SecurityTreatAsSafe]
    InkCanvasClipboardDataFormats CopyToDataObject();

    /// <summary>
    /// Read-only property of the associated EditingCoordinator.
    /// </summary>
    EditingCoordinator& GetEditingCoordinator()
    {
        return *_editingCoordinator;
    }

    /// <summary>
    /// Internal access to the protected DynamicRenderer.  Can be nullptr.
    /// </summary>
    DynamicRenderer* InternalDynamicRenderer()
    {
        return _dynamicRenderer;
    }

    /// <summary>
    /// Return the inner Canvas.
    /// </summary>
    InkCanvasInnerCanvas& InnerCanvas();

    /// <summary>
    /// Internal access to the current selection
    /// </summary>
    InkCanvasSelection& GetInkCanvasSelection();

    /// <summary>
    /// Internal helper called by the LassoSelectionBehavior
    /// </summary>
    void BeginDynamicSelection(Visual* visual);

    /// <summary>
    /// Internal helper called by LassoSelectionBehavior to update the display
    /// of dynamically added strokes
    /// </summary>
    void UpdateDynamicSelection(   QSharedPointer<StrokeCollection> strokesToDynamicallySelect,
                                            QSharedPointer<StrokeCollection> strokesToDynamicallyUnselect);

    /// <summary>
    /// Internal helper used by LassoSelectionBehavior
    /// </summary>
    QSharedPointer<StrokeCollection> EndDynamicSelection(Visual* visual);

    /// <summary>
    /// Clears the selection in the ink canvas
    /// and returns a bool indicating if the selection was actually cleared
    /// (developers can cancel selectionchanging)
    ///
    /// If the InkCanvas has no selection, selectionchanging is not raised
    /// and this method returns true
    ///
    /// used by InkEditor during editing operations
    /// </summary>
    /// <returns>true if selection was cleared even after raising selectionchanging</returns>
    bool ClearSelectionRaiseSelectionChanging();

    /// <summary>
    /// ClearSelection
    ///     Called by:
    ///         PasteFromDataObject
    ///         EditingCoordinator.UpdateEditingState
    /// </summary>
    void ClearSelection(bool raiseSelectionChangedEvent);


    /// <summary>
    /// Helper that creates selection for an InkCanvas.  Used by the SelectedStrokes and
    /// SelectedElements properties
    /// </summary>
    void ChangeInkCanvasSelection(QSharedPointer<StrokeCollection> strokes, QList<UIElement*> elements);


    /// <summary>
    /// Helper method used by ChangeInkCanvasSelection and directly by ClearSelectionWithoutSelectionChanging
    /// </summary>
    /// <param name="validStrokes">validStrokes</param>
    /// <param name="validElements">validElements</param>
    /// <param name="raiseSelectionChanged">raiseSelectionChanged</param>
    void CoreChangeSelection(QSharedPointer<StrokeCollection> validStrokes, QList<UIElement*> validElements, bool raiseSelectionChanged);

#if DEBUG_LASSO_FEEDBACK
    ContainerVisual RendererRootContainer()
    {
        return _inkContainerVisual;
    }
#endif

public:
    /// <summary>
    /// Private helper method used to retrieve a StrokeCollection which does AND operation on two input collections.
    /// </summary>
    /// <param name="subset">The possible subset</param>
    /// <param name="superset">The container set</param>
    /// <returns>True if subset is a subset of superset, false otherwise</returns>
    static QSharedPointer<StrokeCollection> GetValidStrokes(QSharedPointer<StrokeCollection> subset, QSharedPointer<StrokeCollection> superset);

private:
    /// <summary>
    /// Register the commanding handlers for the clipboard operations
    /// </summary>
    /// <SecurityNote>
    ///     Critical: Elevates to associate a protected command (paste) with keyboard
    ///     TreatAsSafe: We don't take user input here. Shift+Insert is the correct key binding,
    ///                  and therefore is expected by the user.
    /// </SecurityNote>
    //[SecurityCritical, SecurityTreatAsSafe]
    void _RegisterClipboardHandlers();

    /// <summary>
    /// Private helper used to ensure that any stroke collection
    /// passed to the InkCanvas is valid.  Throws exceptions if the argument is invalid
    /// </summary>
    QSharedPointer<StrokeCollection> ValidateSelectedStrokes(QSharedPointer<StrokeCollection> strokes);

    /// <summary>
    /// Private helper used to ensure that a UIElement* argument passed in
    /// is valid.
    /// </summary>
    QList<UIElement*> ValidateSelectedElements(QList<UIElement*> selectedElements);

    /// <summary>
    /// Helper method used by DesignActivation to see if an element
    /// has this InkCanvas as an Ancestor
    /// </summary>
    bool InkCanvasIsAncestorOf(UIElement* element);

    /// <summary>
    /// Handler called whenever changes are made to properties of the DefaultDrawingAttributes
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    /// <remarks>For example, when a developer changes the Color property on the DefaultDrawingAttributes,
    /// this event will fire - allowing the InkCanvas a chance to notify the BasicRTI Service.
    /// Also - we do not currently call through the DefaultDrawingAttributes setter since
    /// parameter validation in the setter may detect if the reference isn't changing, and ignore
    /// the call. Also - there is no need for extra parameter validation.</remarks>
    void DefaultDrawingAttributes_Changed(PropertyDataChangedEventArgs& args);

public:
    /// <summary>
    /// Helper method used to set up the DynamicRenderer.
    /// </summary>
    void UpdateDynamicRenderer();

private:
    /// <summary>
    /// Helper method used to set up the DynamicRenderer.
    /// </summary>
    void UpdateDynamicRenderer(QSharedPointer<DrawingAttributes> newDrawingAttributes);

    bool EnsureActiveEditingMode(InkCanvasEditingMode newEditingMode);

    // The ClipboardProcessor instance which deals with the operations relevant to the clipboard.
    ClipboardProcessor& GetClipboardProcessor();

    //lazy instance the gesture recognizer
    GestureRecognizer& GetGestureRecognizer();

    /// <summary>
    /// Delete the current selection
    /// </summary>
    void DeleteCurrentSelection(bool removeSelectedStrokes, bool removeSelectedElements);

    /// <summary>
    /// A class handler of the Commands
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    void _OnCommandExecuted();

    /// <summary>
    /// A class handler for querying the enabled status of the commands.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    /// <SecurityNote>
    ///     Critical -      Call into UserInitiatedCanPaste which is SecurityCritical.
    ///     TreatAsSafe -   We check whether QueryCanPaste is initiated by user or not
    ///                     before invoking the critical method.
    /// </SecurityNote>
    //[SecurityCritical, SecurityTreatAsSafe]
    /*
    static void _OnQueryCommandEnabled(object sender, CanExecuteRoutedEventArgs& args)
    {
        RoutedCommand command = (RoutedCommand)(args.Command);
        InkCanvas inkCanvas = sender as InkCanvas;

        //Debug.Assert(inkCanvas != nullptr);

        if ( inkCanvas.IsEnabled
            // NTRAID-WINDOWSOS#1578484-2006/04/14-WAYNEZEN,
            // If user is editing, we should disable all commands.
            && !inkCanvas.EditingCoordinator.UserIsEditing )
        {
            if ( command == ApplicationCommands::Delete
                || command == ApplicationCommands::Cut
                || command == ApplicationCommands::Copy
                || command == InkCanvas.DeselectCommand )
            {
                args.CanExecute = inkCanvas.InkCanvasSelection.HasSelection;
            }
            else if ( command == ApplicationCommands::Paste )
            {
                try
                {
                    args.CanExecute = args.UserInitiated
                                        ? inkCanvas.UserInitiatedCanPaste() /* Call UserInitiatedCanPaste when the query is initiated by user /
                                        : inkCanvas.CanPaste() /* Call the public CanPaste if not /;
                }
                catch ( System.Runtime.InteropServices.COMException )
                {
                    // The window may be destroyed which could cause the opening failed..
                    // Eat the exception and do nothing.
                    args.CanExecute = false;
                }
            }
            else if ( command == ApplicationCommands::SelectAll )
            {
                //anything to select?
                args.CanExecute = ( inkCanvas.ActiveEditingMode() == InkCanvasEditingMode.Select
                                        && (inkCanvas.Strokes.Count > 0 || inkCanvas.Children.Count > 0));
            }
        }
        else
        {
            // NTRAID:WINDOWSOS#1564508-2006/03/20-WAYNEZEN,
            // Return false for CanExecute if InkCanvas is disabled.
            args.CanExecute = false;
        }

        // NTRAID#WINDOWS-1371659-2005/11/08-waynezen,
        // Mark Handled as true so that the clipboard commands stops routing to InkCanvas' ancestors.
        if ( command == ApplicationCommands::Cut || command == ApplicationCommands::Copy
            || command == ApplicationCommands::Paste )
        {
            args.Handled = true;
        }

    }*/

    InkCanvasClipboardDataFormats PrivateCopySelection();


    /// <summary>
    /// _OnDeviceDown
    /// </summary>
    /// <typeparam name="TEventArgs"></typeparam>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void _OnDeviceDown(EventArgs& e);

    /// <summary>
    /// _OnDeviceUp
    /// </summary>
    /// <typeparam name="TEventArgs"></typeparam>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void _OnDeviceUp(EventArgs& e);

    /// <summary>
    /// _OnQueryCursor
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void _OnQueryCursor(QueryCursorEventArgs& e);

public:
    /// <summary>
    /// Update the current cursor if mouse is over InkCanvas. Called by
    ///     EditingCoordinator.InvalidateBehaviorCursor
    ///     EditingCoordinator.UpdateEditingState
    ///     InkCanvas.set_UseCustomCursor
    /// </summary>
    void UpdateCursor();


    //------------------------------------------------------
    //
    //  Private Classes
    //
    //------------------------------------------------------



    /// <summary>
    /// A helper class for RTI high contrast support
    /// </summary>
    class RTIHighContrastCallback;

    /*
    /// <summary>
    /// This is a binding converter which translates the InkCanvas.ActiveEditingMode() to UIElement*.Visibility.
    /// </summary>
    class ActiveEditingMode2VisibilityConverter : IValueConverter
    {
        public object Convert(object o, Type type, object parameter, System.Globalization.CultureInfo culture)
        {
            InkCanvasEditingMode activeMode = (InkCanvasEditingMode)o;

            // If the current EditingMode is the mode which menuitem is expecting, return true for IsChecked.
            if ( activeMode != InkCanvasEditingMode.None )
            {
                return Visibility.Visible;
            }
            else
            {
                return Visibility.Collapsed;
            }
        }

        public object ConvertBack(object o, Type type, object parameter, System.Globalization.CultureInfo culture)
        {
            // Non-reversed convertion
            return nullptr;
        }
    };
    */


    /// <summary>
    /// The element that represents the selected ink strokes, if any exist.  Will frequently be nullptr.
    /// </summary>
private:
    friend class EditingCoordinator;
    friend class InkCollectionBehavior;
    friend class EraserBehavior;
    friend class InkCanvasSelection;
    friend class InkCanvasInnerCanvas;

    InkCanvasSelection*          _selection = nullptr;
    InkCanvasSelectionAdorner*   _selectionAdorner = nullptr;
    InkCanvasFeedbackAdorner*    _feedbackAdorner = nullptr;

    /// <summary>
    /// The internal Canvas used to hold elements
    /// </summary>
    InkCanvasInnerCanvas*        _innerCanvas = nullptr;

    /// <summary>
    /// The internal private AdornerDecorator
    /// </summary>
    AdornerDecorator*            _localAdornerDecorator = nullptr;

    /// <summary>
    /// Runtime Selection StrokeCollection
    /// </summary>
    QSharedPointer<StrokeCollection>            _dynamicallySelectedStrokes;

    /// <summary>
    /// Our editing logic
    /// </summary>
    EditingCoordinator*          _editingCoordinator = nullptr;

    /// <summary>
    /// Defines the default StylusPointDescription
    /// </summary>
    QSharedPointer<StylusPointDescription>     _defaultStylusPointDescription;


    /// <summary>
    /// Defines the shape of the eraser tip
    /// </summary>
    std::unique_ptr<StylusShape>                 _eraserShape = nullptr;

    /// <summary>
    /// Determines if EditingBehaviors should use their own cursor or a custom one specified.
    /// </summary>
    bool                        _useCustomCursor = false;


    //
    // Rendering support.
    //
    InkPresenter*                _inkPresenter = nullptr;

    //
    // The RealTimeInking PlugIn that handles our off UIContext rendering.
    //
    DynamicRenderer*             _dynamicRenderer = nullptr;

    //
    // Clipboard Helper
    //
    ClipboardProcessor*          _clipboardProcessor = nullptr;

    //
    // Gesture support
    //
    GestureRecognizer*           _gestureRecognizer = nullptr;

    //
    // HighContrast support
    //
    RTIHighContrastCallback*     _rtiHighContrastCallback = nullptr;

    static constexpr double                    c_pasteDefaultLocation = 0.0;
};

#endif // INKCANVAS_H
