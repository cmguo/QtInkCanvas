#ifndef INKCANVAS_H
#define INKCANVAS_H

#include "InkCanvas_global.h"
#include "styluspointcollection.h"
#include "strokecollection.h"
#include "drawingattributes.h"
#include "dependencyobject.h"
#include "events.h"
#include "inkcanvaseditingmode.h"

#include <QWidget>


class InkCanvasSelection;
class InkCanvasSelectionAdorner;
class InkCanvasFeedbackAdorner;
class InkCanvasInnerCanvas;
class AdornerDecorator;
class EditingCoordinator;
class InkPresenter;
class ClipboardProcessor;
class GestureRecognizer;
class DynamicRenderer;
class ApplicationGesture;
class StylusPointCollection;
class InkCanvasClipboardFormat;
class Visual;

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

class InkCanvas : public QWidget, public DependencyObject
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
        CommandHelpers.RegisterCommandHandler(ownerType, ApplicationCommands.Delete,
            new ExecutedRoutedEventHandler(_OnCommandExecuted), new CanExecuteRoutedEventHandler(_OnQueryCommandEnabled));

        CommandHelpers.RegisterCommandHandler(ownerType, ApplicationCommands.SelectAll,
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
    InkCanvas(QWidget * parent = nullptr);

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
    /*
    override Size MeasureOverride(Size availableSize)
    {
        // No need to invoke VerifyAccess since _localAdornerDecorator.Measure should check it.
        if ( _localAdornerDecorator == null )
        {
            ApplyTemplate();
        }

        _localAdornerDecorator.Measure(availableSize);

        return  _localAdornerDecorator.DesiredSize;
    }

    /// <summary>
    /// ArrangeOverride
    /// </summary>
    /// <param name="arrangeSize"></param>
    /// <returns></returns>
    protected override Size ArrangeOverride(Size arrangeSize)
    {
        // No need to invoke VerifyAccess since _localAdornerDecorator.Arrange should check it.

        if ( _localAdornerDecorator == null )
        {
            ApplyTemplate();
        }

        _localAdornerDecorator.Arrange(new Rect(arrangeSize));

        return arrangeSize;
    }*/


    /// <summary>
    /// HitTestCore implements precise hit testing against render contents
    /// </summary>
    HitTestResult HitTestCore(PointHitTestParameters hitTestParams);

    /// <summary>
    /// OnPropertyChanged
    /// </summary>
    /*
    void OnPropertyChanged(DependencyPropertyChangedEventArgs& e)
    {
        base.OnPropertyChanged(e);

        if (e.IsAValueChange || e.IsASubPropertyChange)
        {
            if (e.Property == QWidget*.RenderTransformProperty ||
                e.Property == FrameworkElement.LayoutTransformProperty)
            {
                EditingCoordinator.InvalidateTransform();

                Transform transform = e.NewValue as Transform;
                if (transform != null && !transform.HasAnimatedProperties)
                {
                    TransformGroup transformGroup = transform as TransformGroup;
                    if ( transformGroup != null )
                    {
                        //walk down the tree looking for animated transforms
                        Stack<Transform> transforms = new Stack<Transform>();
                        transforms.Push(transform);
                        while ( transforms.Count > 0 )
                        {
                            transform = transforms.Pop();
                            if ( transform.HasAnimatedProperties )
                            {
                                return;
                            }
                            transformGroup = transform as TransformGroup;
                            if ( transformGroup != null )
                            {
                                for ( int i = 0; i < transformGroup.Children.Count; i++ )
                                {
                                    transforms.Push(transformGroup.Children[i]);
                                }
                            }
                        }
                    }

                    //
                    // only invalidate when there is not an animation on the xf,
                    // or we could wind up creating thousands of new cursors.  That's bad.
                    //
                    _editingCoordinator.InvalidateBehaviorCursor(_editingCoordinator.InkCollectionBehavior);
                    EditingCoordinator.UpdatePointEraserCursor();
                }
            }
            if (e.Property == FrameworkElement.FlowDirectionProperty)
            {
                //flow direction only affects the inking cursor.
                _editingCoordinator.InvalidateBehaviorCursor(_editingCoordinator.InkCollectionBehavior);
            }
        }
    }*/

    /// <summary>
    /// Called when the Template's tree is about to be generated
    /// </summary>
    /*
    internal override void OnPreApplyTemplate()
    {
        // No need for calling VerifyAccess since we call the method on the base here.

        base.OnPreApplyTemplate();

        // Build our visual tree here.
        // <InkCanvas>
        //     <AdornerDecorator>
        //         <InkPresenter>
        //             <InnerCanvas />
        //             <ContainerVisual />              <!-- Ink Renderer static visual -->
        //             <HostVisual />                   <!-- Ink Dynamic Renderer -->
        //         </InkPresenter>
        //         <AdornerLayer>                       <!-- Create by AdornerDecorator automatically -->
        //             <InkCanvasSelectionAdorner />
        //             <InkCanvasFeedbackAdorner* />     <!-- Dynamically hooked up when moving/sizing the selection -->
        //         </AdornerLayer>
        //     </AdornerDecorator>
        //  </InkCanvas>

        if ( _localAdornerDecorator == null )
        {
            //
            _localAdornerDecorator = new AdornerDecorator();
            InkPresenter* inkPresenter = InkPresenter;

            // Build the visual tree top-down
            AddVisualChild(_localAdornerDecorator);
            _localAdornerDecorator.Child = inkPresenter;
            InkPresenter().Child = InnerCanvas;

            // Add the SelectionAdorner after Canvas is added.
            _localAdornerDecorator.AdornerLayer.Add(SelectionAdorner);
        }
    }

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

    /*
    /// <summary>
    ///     The DependencyProperty for the Background property.
    /// </summary>
    public static readonly DependencyProperty BackgroundProperty =
            Panel.BackgroundProperty.AddOwner(
                    typeof(InkCanvas),
                    new FrameworkPropertyMetadata(
                            null,
                            FrameworkPropertyMetadataOptions.AffectsRender));

    /// <summary>
    ///     An object that describes the background.
    /// </summary>
    [Bindable(true), Category("Appearance")]
    public Brush Background
    {
        get { return (Brush) GetValue(BackgroundProperty); }
        set { SetValue(BackgroundProperty, value); }
    }

    /// <summary>
    /// Top DependencyProperty
    /// </summary>
    public static readonly DependencyProperty TopProperty =
            DependencyProperty.RegisterAttached("Top", typeof(double), typeof(InkCanvas),
                new FrameworkPropertyMetadata(Double.NaN, new PropertyChangedCallback(OnPositioningChanged)),
                new ValidateValueCallback(System.Windows.Shapes.Shape.IsDoubleFiniteOrNaN));

    /// <summary>
    /// Reads the attached property Top from the given element.
    /// </summary>
    /// <param name="element">The element from which to read the Top attached property.</param>
    /// <returns>The property's value.</returns>
    /// <seealso cref="InkCanvas.TopProperty" />
    [TypeConverter("System.Windows.LengthConverter, PresentationFramework, Version=" +BuildInfo.WCP_VERSION + ", Culture=neutral, PublicKeyToken=" +BuildInfo.WCP_PUBLIC_KEY_TOKEN + ", Custom=null")]
    [AttachedPropertyBrowsableForChildren()]
    public static double GetTop(QWidget* element)
    {
        if (element == nullptr) { throw new std::exception("element"); }
        return (double)element.GetValue(TopProperty);
    }

    /// <summary>
    /// Writes the attached property Top to the given element.
    /// </summary>
    /// <param name="element">The element to which to write the Top attached property.</param>
    /// <param name="length">The length to set</param>
    /// <seealso cref="InkCanvas.TopProperty" />
    public static void SetTop(QWidget* element, double length)
    {
        if (element == nullptr) { throw new std::exception("element"); }
        element.SetValue(TopProperty, length);
    }

    /// <summary>
    /// The Bottom DependencyProperty
    /// </summary>
    public static readonly DependencyProperty BottomProperty =
            DependencyProperty.RegisterAttached("Bottom", typeof(double), typeof(InkCanvas),
                new FrameworkPropertyMetadata(Double.NaN, new PropertyChangedCallback(OnPositioningChanged)),
                new ValidateValueCallback(System.Windows.Shapes.Shape.IsDoubleFiniteOrNaN));

    /// <summary>
    /// Reads the attached property Bottom from the given element.
    /// </summary>
    /// <param name="element">The element from which to read the Bottom attached property.</param>
    /// <returns>The property's Length value.</returns>
    /// <seealso cref="InkCanvas.BottomProperty" />
    [TypeConverter("System.Windows.LengthConverter, PresentationFramework, Version=" +BuildInfo.WCP_VERSION + ", Culture=neutral, PublicKeyToken=" +BuildInfo.WCP_PUBLIC_KEY_TOKEN + ", Custom=null")]
    [AttachedPropertyBrowsableForChildren()]
    public static double GetBottom(QWidget* element)
    {
        if (element == nullptr) { throw new std::exception("element"); }
        return (double)element.GetValue(BottomProperty);
    }

    /// <summary>
    /// Writes the attached property Bottom to the given element.
    /// </summary>
    /// <param name="element">The element to which to write the Bottom attached property.</param>
    /// <param name="length">The Length to set</param>
    /// <seealso cref="InkCanvas.BottomProperty" />
    public static void SetBottom(QWidget* element, double length)
    {
        if (element == nullptr) { throw new std::exception("element"); }
        element.SetValue(BottomProperty, length);
    }

    /// <summary>
    /// The Left DependencyProperty
    /// </summary>
    public static readonly DependencyProperty LeftProperty =
            DependencyProperty.RegisterAttached("Left", typeof(double), typeof(InkCanvas),
                new FrameworkPropertyMetadata(Double.NaN, new PropertyChangedCallback(OnPositioningChanged)),
                new ValidateValueCallback(System.Windows.Shapes.Shape.IsDoubleFiniteOrNaN));

    /// <summary>
    /// Reads the attached property Left from the given element.
    /// </summary>
    /// <param name="element">The element from which to read the Left attached property.</param>
    /// <returns>The property's value.</returns>
    /// <seealso cref="InkCanvas.LeftProperty" />
    [TypeConverter("System.Windows.LengthConverter, PresentationFramework, Version=" +BuildInfo.WCP_VERSION + ", Culture=neutral, PublicKeyToken=" +BuildInfo.WCP_PUBLIC_KEY_TOKEN + ", Custom=null")]
    [AttachedPropertyBrowsableForChildren()]
    public static double GetLeft(QWidget* element)
    {
        if (element == nullptr) { throw new std::exception("element"); }
        return (double)element.GetValue(LeftProperty);
    }

    /// <summary>
    /// Writes the attached property Left to the given element.
    /// </summary>
    /// <param name="element">The element to which to write the Left attached property.</param>
    /// <param name="length">The length to set</param>
    /// <seealso cref="InkCanvas.LeftProperty" />
    static void SetLeft(QWidget* element, double length)
    {
        if (element == nullptr) { throw new std::exception("element"); }
        element.SetValue(LeftProperty, length);
    }

    /// <summary>
    /// The Right DependencyProperty
    /// </summary>
    //public static readonly DependencyProperty RightProperty =
    //        DependencyProperty.RegisterAttached("Right", typeof(double), typeof(InkCanvas),
    //            new FrameworkPropertyMetadata(Double.NaN, new PropertyChangedCallback(OnPositioningChanged)),
    //            new ValidateValueCallback(System.Windows.Shapes.Shape.IsDoubleFiniteOrNaN));

    /// <summary>
    /// Reads the attached property Right from the given element.
    /// </summary>
    /// <param name="element">The element from which to read the Right attached property.</param>
    /// <returns>The property's Length value.</returns>
    /// <seealso cref="InkCanvas.RightProperty" />
    //[TypeConverter("System.Windows.LengthConverter, PresentationFramework, Version=" +BuildInfo.WCP_VERSION + ", Culture=neutral, PublicKeyToken=" +BuildInfo.WCP_PUBLIC_KEY_TOKEN + ", Custom=null")]
    //[AttachedPropertyBrowsableForChildren()]
    static double GetRight(QWidget* element)
    {
        if (element == nullptr) { throw new std::exception("element"); }
        return (double)element.GetValue(RightProperty);
    }

    /// <summary>
    /// Writes the attached property Right to the given element.
    /// </summary>
    /// <param name="element">The element to which to write the Right attached property.</param>
    /// <param name="length">The Length to set</param>
    /// <seealso cref="InkCanvas.RightProperty" />
    static void SetRight(QWidget* element, double length)
    {
        if (element == nullptr) { throw new std::exception("element"); }
        element.SetValue(RightProperty, length);
    }

    /// <summary>
    /// OnPositioningChanged
    /// </summary>
    /// <param name="d"></param>
    /// <param name="e"></param>
    static void OnPositioningChanged(DependencyObject d, DependencyPropertyChangedEventArgs& e)
    {
        QWidget* uie = d as QWidget*;
        if ( uie != null )
        {
            // Make sure the QWidget* is a child of InkCanvasInnerCanvas.
            InkCanvasInnerCanvas p = VisualTreeHelper.GetParent(uie) as InkCanvasInnerCanvas;
            if ( p != null )
            {
                if ( e.Property == InkCanvas.LeftProperty
                    || e.Property == InkCanvas.TopProperty )
                {
                    // Invalidate measure for Left and/or Top.
                    p.InvalidateMeasure();
                }
                else
                {
                    //Debug.Assert(e.Property == InkCanvas.RightProperty || e.Property == InkCanvas.BottomProperty,
                        string.Format(System.Globalization.CultureInfo.InvariantCulture, "Unknown dependency property detected - {0}.", e.Property));

                    // Invalidate arrange for Right and/or Bottom.
                    p.InvalidateArrange();
                }
            }
        }
    }*/

public:
    /// <summary>
    ///     The DependencyProperty for the Strokes property.
    /// </summary>
    //public static readonly DependencyProperty StrokesProperty =
    //        InkPresenter().StrokesProperty.AddOwner(
    //                typeof(InkCanvas),
    //                new FrameworkPropertyMetadata(
    //                        new StrokeCollectionDefaultValueFactory(),
    //                        new PropertyChangedCallback(OnStrokesChanged)));

    static DependencyProperty * const StrokesProperty;

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
    //        return GestureRecognizer.IsRecognizerAvailable;
     //   }
    //}


    /// <summary>
    /// Emulate Panel's Children property.
    /// </summary>
    //[DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
    //public QWidget*Collection Children
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
    //public static readonly DependencyProperty DefaultDrawingAttributesProperty =
    //        DependencyProperty.Register(
    //                "DefaultDrawingAttributes",
    //                typeof(DrawingAttributes),
    //                typeof(InkCanvas),
    //                new FrameworkPropertyMetadata(
    //                        new DrawingAttributesDefaultValueFactory(),
    //                        new PropertyChangedCallback(OnDefaultDrawingAttributesChanged)),
    //                (ValidateValueCallback)delegate(object value)
    //                    { return value != null; });

    static DependencyProperty * const DefaultDrawingAttributesProperty;

    /// <summary>
    /// Gets/Sets the DefaultDrawingAttributes property.
    /// </summary>
    QSharedPointer<DrawingAttributes> DefaultDrawingAttributes()
    {
        return GetValue<QSharedPointer<DrawingAttributes>>(DefaultDrawingAttributesProperty);
    }
    void SetDefaultDrawingAttributes(QSharedPointer<DrawingAttributes> value)
    {
        SetValue(DefaultDrawingAttributesProperty, value);
    }

    static void OnDefaultDrawingAttributesChanged(DependencyObject& d, DependencyPropertyChangedEventArgs& e);

    /// <summary>
    /// Read/Write access to the EraserShape property.
    /// </summary>
    //[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
    StylusShape* EraserShape();
    void SetEraserShape(StylusShape * value);


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
    //public static readonly DependencyProperty ActiveEditingModeProperty = ActiveEditingModePropertyKey.DependencyProperty;
    static DependencyProperty * const ActiveEditingModeProperty;

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
    //public static readonly DependencyProperty EditingModeProperty =
   //         DependencyProperty.Register(
     //               "EditingMode",
    //                typeof(InkCanvasEditingMode),
    //                typeof(InkCanvas),
    //                new FrameworkPropertyMetadata(
    //                        InkCanvasEditingMode.Ink,
     //                       new PropertyChangedCallback(OnEditingModeChanged)),
    //                new ValidateValueCallback(ValidateEditingMode));
    static DependencyProperty * const EditingModeProperty;


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
    //public static readonly DependencyProperty EditingModeInvertedProperty =
    //        DependencyProperty.Register(
    //                "EditingModeInverted",
    //                typeof(InkCanvasEditingMode),
    //                typeof(InkCanvas),
    //                new FrameworkPropertyMetadata(
    //                        InkCanvasEditingMode.EraseByStroke,
    //                        new PropertyChangedCallback(OnEditingModeInvertedChanged)),
    //                new ValidateValueCallback(ValidateEditingMode));
    static DependencyProperty * const EditingModeInvertedProperty;

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
    bool UseCustomCursor()
    {
        VerifyAccess();
        return _useCustomCursor;
    }
    void SetUseCustomCursor(bool value)
    {
        VerifyAccess();

        if ( _useCustomCursor != value )
        {
            _useCustomCursor = value;
            UpdateCursor();
        }
    }

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
    QSharedPointer<StylusPointDescription> DefaultStylusPointDescription()
    {
        VerifyAccess();

        return _defaultStylusPointDescription;
    }
    void SetDefaultStylusPointDescription(QSharedPointer<StylusPointDescription> value)
    {
        VerifyAccess();

        //
        // no nulls allowed
        //
        if ( value == nullptr )
        {
            throw new std::exception("value");
        }

        _defaultStylusPointDescription = value;
    }

    /// <summary>
    /// Read/Write the enabled ClipboardFormats
    /// </summary>
    //[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
    QList<InkCanvasClipboardFormat> PreferredPasteFormats();
    void SetPreferredPasteFormats();


    /// <summary>
    ///     The StrokeErased Routed Event
    /// </summary>
    //public static readonly RoutedEvent StrokeCollectedEvent =
    //    EventManager.RegisterRoutedEvent("StrokeCollected", RoutingStrategy.Bubble, typeof(InkCanvasStrokeCollectedEventHandler), typeof(InkCanvas));

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

    /// <summary>
    /// Protected virtual version for developers deriving from InkCanvas.
    /// This method is what actually throws the event.
    /// </summary>
    /// <param name="e">InkCanvasStrokeCollectedEventArgs& to raise the event with</param>
    virtual void OnStrokeCollected(InkCanvasStrokeCollectedEventArgs& e)
    {
        // No need to invoke VerifyAccess since this method is thread free.

        //if ( e == null )
        //{
        //    throw new std::exception("e");
        //}

        RaiseEvent(e);
    }

    /// <summary>
    /// Allows the InkCollectionBehavior to raise the StrokeCollected event via the protected virtual
    /// </summary>
    /// <param name="e">InkCanvasStrokeCollectedEventArgs& to raise the event with</param>
    /// <param name="userInitiated">true only if 100% of the stylusPoints that makes up the stroke
    /// came from EventArgs& with the UserInitiated flag set to true</param>
    /// <SecurityNote>
    ///     Critical: Calls critical method GestureRecognizer.CriticalRecognize.  It is important
    ///         that this is only called if userInitiated is true.
    /// </SecurityNote>
    //[SecurityCritical]
    void RaiseGestureOrStrokeCollected(InkCanvasStrokeCollectedEventArgs& e, bool userInitiated);
    /// <summary>
    ///     The Gesture Routed Event
    /// </summary>
    //public static readonly RoutedEvent GestureEvent =
     //   EventManager.RegisterRoutedEvent("Gesture", RoutingStrategy.Bubble, typeof(InkCanvasGestureEventHandler), typeof(InkCanvas));

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

    /// <summary>
    /// Protected virtual version for developers deriving from InkCanvas.
    /// This method is what actually throws the event.
    /// </summary>
    /// <param name="e">InkCanvasGestureEventArgs& to raise the event with</param>
    virtual void OnGesture(InkCanvasGestureEventArgs& e)
    {
        // No need to invoke VerifyAccess since this method is thread free.

        //if ( e == null )
        //{
        //    throw new std::exception("e");
        //}

        RaiseEvent(e);
    }

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
    virtual void OnStrokesReplaced(InkCanvasStrokesReplacedEventArgs& e)
    {
        // No need to invoke VerifyAccess since this method is thread free.

        //if ( e == null )
        //{
        //    throw new std::exception("e");
        //}
        //if (nullptr != StrokesReplaced)
        //{
            StrokesReplaced(e);
        //}
    }

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
    virtual void OnDefaultDrawingAttributesReplaced(DrawingAttributesReplacedEventArgs& e)
    {
        // No need to invoke VerifyAccess since this method is thread free.

        //if (e == nullptr)
        //{
        //    throw new std::exception("e");
        //}
        //if (nullptr != DefaultDrawingAttributesReplaced)
        //{
            DefaultDrawingAttributesReplaced(e);
        //}
    }

    /// <summary>
    /// Private helper for raising DDAReplaced.  Invalidates the inking cursor
    /// </summary>
    /// <param name="e"></param>
    void RaiseDefaultDrawingAttributeReplaced(DrawingAttributesReplacedEventArgs& e);

    /// <summary>
    ///     Event corresponds to ActiveEditingModeChanged
    /// </summary>
    //public static readonly RoutedEvent ActiveEditingModeChangedEvent =
    //    EventManager.RegisterRoutedEvent("ActiveEditingModeChanged", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(InkCanvas));

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

    /// <summary>
    /// Protected virtual version for developers deriving from InkCanvas.
    /// This method is what actually throws the event.
    /// </summary>
    /// <param name="e">EventArgs& to raise the event with</param>
    virtual void OnActiveEditingModeChanged(RoutedEventArgs& e)
    {
        // No need to invoke VerifyAccess since this method is thread free.

        //if (e == nullptr)
        //{
        //    throw new std::exception("e");
        //}

        RaiseEvent(e);
    }

    /// <summary>
    /// Private helper that raises ActiveEditingModeChanged
    /// </summary>
    /// <param name="e">EventArgs& to raise the event with</param>
    void RaiseActiveEditingModeChanged(RoutedEventArgs& e);



    /// <summary>
    ///     Event corresponds to EditingModeChanged
    /// </summary>
    //public static readonly RoutedEvent EditingModeChangedEvent =
    //    EventManager.RegisterRoutedEvent("EditingModeChanged", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(InkCanvas));

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
    virtual void OnEditingModeChanged(RoutedEventArgs& e)
    {
        // No need to invoke VerifyAccess since this method is thread free.

        //if ( e == null )
        //{
        //    throw new std::exception("e");
        //}

        RaiseEvent(e);
    }
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
    //public static readonly RoutedEvent EditingModeInvertedChangedEvent =
    //    EventManager.RegisterRoutedEvent("EditingModeInvertedChanged", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(InkCanvas));

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
    virtual void OnEditingModeInvertedChanged(RoutedEventArgs& e)
    {
        // No need to invoke VerifyAccess since this method is thread free.

        //if ( e == null )
        //{
        //    throw new std::exception("e");
        //}

        RaiseEvent(e);
    }
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
    virtual void OnSelectionMoving( InkCanvasSelectionEditingEventArgs& e)
    {
        // No need to invoke VerifyAccess since this method is thread free.

        //if ( e == null )
        //{
        //    throw new std::exception("e");
        //}
        //if (nullptr != SelectionMoving)
        //{
            SelectionMoving(e);
        //}
    }

    /// <summary>
    /// Allows the EditingBehaviors to raise the SelectionMoving event via the protected virtual
    /// </summary>
    /// <param name="e"> InkCanvasSelectionEditingEventArgs& to raise the event with</param>
    void RaiseSelectionMoving( InkCanvasSelectionEditingEventArgs& e)
    {
        //Debug.Assert(e != null, "EventArg can not be null");
        OnSelectionMoving(e);
    }

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
    virtual void OnSelectionMoved(EventArgs& e)
    {
        // No need to invoke VerifyAccess since this method is thread free.

        //if ( e == null )
        //{
        //    throw new std::exception("e");
        //}
        //if (nullptr != SelectionMoved)
        //{
            SelectionMoved(e);
        //}
    }

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
    virtual void OnStrokeErasing(InkCanvasStrokeErasingEventArgs& e)
    {
        // No need to invoke VerifyAccess since this method is thread free.

        //if ( e == null )
        //{
        //    throw new std::exception("e");
        //}
        //if (nullptr != StrokeErasing)
        //{
            StrokeErasing(e);
        //}
    }

    /// <summary>
    /// Allows the EditingBehaviors to raise the InkErasing event via the protected virtual
    /// </summary>
    /// <param name="e">InkCanvasStrokeErasingEventArgs& to raise the event with</param>
    void RaiseStrokeErasing(InkCanvasStrokeErasingEventArgs& e)
    {
        //Debug.Assert(e != null, "EventArg can not be null");
        OnStrokeErasing(e);
    }

    /// <summary>
    ///     The StrokeErased Routed Event
    /// </summary>
    //public static readonly RoutedEvent StrokeErasedEvent =
    //    EventManager.RegisterRoutedEvent("StrokeErased", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(InkCanvas));

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

    /// <summary>
    /// Protected virtual version for developers deriving from InkCanvas.
    /// This method is what actually throws the event.
    /// </summary>
    /// <param name="e">EventArgs& to raise the event with</param>
    virtual void OnStrokeErased(RoutedEventArgs& e)
    {
        // No need to invoke VerifyAccess since this method is thread free.

        //if ( e == null )
        //{
        //    throw new std::exception("e");
        //}
        RaiseEvent(e);
    }

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
    virtual void OnSelectionResizing( InkCanvasSelectionEditingEventArgs& e)
    {
        // No need to invoke VerifyAccess since this method is thread free.

        //if ( e == null )
        //{
        //    throw new std::exception("e");
        //}
        //if (nullptr != SelectionResizing)
        //{
            SelectionResizing(e);
        //}
    }

    /// <summary>
    /// Allows the EditingBehaviors to raise the SelectionResizing event via the protected virtual
    /// </summary>
    /// <param name="e"> InkCanvasSelectionEditingEventArgs& to raise the event with</param>
    void RaiseSelectionResizing( InkCanvasSelectionEditingEventArgs& e)
    {
        //Debug.Assert(e != null, "EventArg can not be null");
        OnSelectionResizing(e);
    }

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
    virtual void OnSelectionResized(EventArgs& e)
    {
        // No need to invoke VerifyAccess since this method is thread free.

        //if ( e == null )
        //{
        //    throw new std::exception("e");
        //}
        //if (nullptr != SelectionResized)
        //{
            SelectionResized(e);
        //}
    }

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
    virtual void OnSelectionChanging(InkCanvasSelectionChangingEventArgs& e)
    {
        // No need to invoke VerifyAccess since this method is thread free.

        //if ( e == null )
        //{
        //    throw new std::exception("e");
        //}
        //if (nullptr != SelectionChanging)
        //{
            SelectionChanging(e);
        //}
    }

    /// <summary>
    /// Allows the EditingBehaviors to raise the SelectionChanging event via the protected virtual
    /// </summary>
    /// <param name="e">InkCanvasSelectionChangingEventArgs& to raise the event with</param>
    void RaiseSelectionChanging(InkCanvasSelectionChangingEventArgs& e)
    {
        //Debug.Assert(e != null, "EventArg can not be null");
        OnSelectionChanging(e);
    }

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
    virtual void OnSelectionChanged(EventArgs& e)
    {
        // No need to invoke VerifyAccess since this method is thread free.

        //if ( e == null )
        //{
        //    throw new std::exception("e");
        //}
        //if (nullptr != SelectionChanged)
        //{
            SelectionChanged(e);
        //}
    }

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
    //void RaiseOnVisualChildrenChanged(DependencyObject visualAdded, DependencyObject visualRemoved)
    //{
    //    OnVisualChildrenChanged(visualAdded, visualRemoved);
    //}


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
    //QList<QWidget*> GetSelectedElements()
    //{
    //    VerifyAccess();
    //    return InkCanvasSelection.SelectedElements;
    //}

    /// <summary>
    /// provides read access to the currently selected strokes
    /// </summary>
    QSharedPointer<StrokeCollection> GetSelectedStrokes();

    /// <summary>
    /// Overload which calls the more complex version, passing null for selectedElements
    /// </summary>
    /// <param name="selectedStrokes">The strokes to select</param>
    void Select(QSharedPointer<StrokeCollection> selectedStrokes);

    /// <summary>
    /// Overload which calls the more complex version, passing null for selectedStrokes
    /// </summary>
    /// <param name="selectedElements">The elements to select</param>
    void Select(QList<QWidget*> selectedElements);

    /// <summary>
    /// Overload which calls the more complex version, passing null for selectedStrokes
    /// </summary>
    /// <param name="selectedStrokes">The strokes to select</param>
    /// <param name="selectedElements">The elements to select</param>
    void Select(QSharedPointer<StrokeCollection> selectedStrokes, QList<QWidget*> selectedElements);

    /// <summary>
    /// Hit test on the selection
    /// </summary>
    /// <param name="point"></param>
    /// <returns></returns>
    InkCanvasSelectionHitResult HitTestSelection(QPointF const &point);

    /// <summary>
    /// Copy the current selection in the InkCanvas to the clipboard
    /// </summary>
    void CopySelection()
    {
        VerifyAccess();
        PrivateCopySelection();
    }

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

   //     if ( value == null )
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
    DynamicRenderer* GetDynamicRenderer()
    {
        VerifyAccess();
        return &InternalDynamicRenderer();
    }
    void SetDynamicRenderer(DynamicRenderer* value);

    /// <summary>
    /// Protected read only access to the InkPresenter this InkCanvas uses
    /// </summary>
    InkPresenter& GetInkPresenter();



    /// <summary>
    /// Deselect the current selection
    /// </summary>
    //internal static readonly RoutedCommand DeselectCommand = new RoutedCommand("Deselect", typeof(InkCanvas));

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
    /// This method pastes data from an IDataObject object
    /// </summary>
    //void PasteFromDataObject(IDataObject dataObj, Point point);

    /// <summary>
    /// Copies the InkCanvas contents to a DataObject and returns it to the caller.
    ///  Can return NULL for DataObject.
    /// </summary>
    /// <SecurityNote>
    ///     Critical: Clipboard.SetDataObject will invoke DataObject.DataStore.GetFormats.
    ///                 The methods demands SerializationPermission. We perform the elevation before
    ///                 calling SetDataObject.
    ///     TreatAsSafe: There is no input here. The ISF data are safe to being put in the clipboard.
    /// </SecurityNote>
    //[SecurityCritical, SecurityTreatAsSafe]
    //InkCanvasClipboardDataFormats CopyToDataObject();

    /// <summary>
    /// Read-only property of the associated EditingCoordinator.
    /// </summary>
    EditingCoordinator& GetEditingCoordinator()
    {
        return *_editingCoordinator;
    }

    /// <summary>
    /// Internal access to the protected DynamicRenderer.  Can be null.
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
    //void BeginDynamicSelection(Visual visual);

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
    void ChangeInkCanvasSelection(QSharedPointer<StrokeCollection> strokes, QVector<QWidget*> elements);


    /// <summary>
    /// Helper method used by ChangeInkCanvasSelection and directly by ClearSelectionWithoutSelectionChanging
    /// </summary>
    /// <param name="validStrokes">validStrokes</param>
    /// <param name="validElements">validElements</param>
    /// <param name="raiseSelectionChanged">raiseSelectionChanged</param>
    void CoreChangeSelection(QSharedPointer<StrokeCollection> validStrokes, QList<QWidget*> validElements, bool raiseSelectionChanged);

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
    static void _RegisterClipboardHandlers();

    /// <summary>
    /// Private helper used to ensure that any stroke collection
    /// passed to the InkCanvas is valid.  Throws exceptions if the argument is invalid
    /// </summary>
    QSharedPointer<StrokeCollection> ValidateSelectedStrokes(QSharedPointer<StrokeCollection> strokes);

    /// <summary>
    /// Private helper used to ensure that a QWidget* argument passed in
    /// is valid.
    /// </summary>
    QVector<QWidget*> ValidateSelectedElements(QList<QWidget*> selectedElements);

    /// <summary>
    /// Helper method used by DesignActivation to see if an element
    /// has this InkCanvas as an Ancestor
    /// </summary>
    bool InkCanvasIsAncestorOf(QWidget* element);

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
    void UpdateDynamicRenderer()
    {
        UpdateDynamicRenderer(DefaultDrawingAttributes());
    }

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
    /*
    static void _OnCommandExecuted(object sender, ExecutedRoutedEventArgs& args)
    {
        ICommand command = args.Command;
        InkCanvas inkCanvas = sender as InkCanvas;

        //Debug.Assert(inkCanvas != nullptr);

        if ( inkCanvas.IsEnabled && !inkCanvas.EditingCoordinator.UserIsEditing )
        {
            if ( command == ApplicationCommands.Delete )
            {
                inkCanvas.DeleteCurrentSelection(true, true);
            }
            else if ( command == ApplicationCommands.Cut )
            {
                inkCanvas.CutSelection();
            }
            else if ( command == ApplicationCommands.Copy )
            {
                inkCanvas.CopySelection();
            }
            else if ( command == ApplicationCommands.SelectAll )
            {
                if ( inkCanvas.ActiveEditingMode == InkCanvasEditingMode.Select )
                {
                    QList<QWidget*> children = nullptr;
                    QWidget*Collection QWidget*Collection = inkCanvas.Children;
                    if ( QWidget*Collection.Count > 0 )
                    {
                        //QWidget*Collection doesn't implement QList<QWidget*>
                        //for some reason
                        QWidget*[] QWidget*Array = new QWidget*[QWidget*Collection.Count];
                        for ( int i = 0; i < QWidget*Collection.Count; i++ )
                        {
                            QWidget*Array[i] = QWidget*Collection[i];
                        }
                        children = QWidget*Array;
                    }
                    inkCanvas.Select(inkCanvas.Strokes, children);
                }
            }
            else if ( command == ApplicationCommands.Paste )
            {
                try
                {
                    inkCanvas.Paste();
                }
                // Eat it and do nothing if one of the following exceptions is caught.
                catch ( System.Runtime.InteropServices.COMException )
                {
                    // The window may be destroyed which could cause the opening failed..
                }
                catch ( XamlParseException )
                {
                    // The Xaml parser fails
                }
                catch ( ArgumentException )
                {
                    // The ISF decoder fails
                }
            }
            else if ( command == InkCanvas.DeselectCommand )
            {
                inkCanvas.ClearSelectionRaiseSelectionChanging();
            }
        }
    }*/

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
            if ( command == ApplicationCommands.Delete
                || command == ApplicationCommands.Cut
                || command == ApplicationCommands.Copy
                || command == InkCanvas.DeselectCommand )
            {
                args.CanExecute = inkCanvas.InkCanvasSelection.HasSelection;
            }
            else if ( command == ApplicationCommands.Paste )
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
            else if ( command == ApplicationCommands.SelectAll )
            {
                //anything to select?
                args.CanExecute = ( inkCanvas.ActiveEditingMode == InkCanvasEditingMode.Select
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
        if ( command == ApplicationCommands.Cut || command == ApplicationCommands.Copy
            || command == ApplicationCommands.Paste )
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
    static void _OnDeviceDown(EventArgs& e);

    /// <summary>
    /// _OnDeviceUp
    /// </summary>
    /// <typeparam name="TEventArgs"></typeparam>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    static void _OnDeviceUp(EventArgs& e);

    /// <summary>
    /// _OnQueryCursor
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    static void _OnQueryCursor(QueryCursorEventArgs& e);

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
    /// This is a binding converter which translates the InkCanvas.ActiveEditingMode to QWidget*.Visibility.
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
            return null;
        }
    };
    */


    /// <summary>
    /// The element that represents the selected ink strokes, if any exist.  Will frequently be null.
    /// </summary>
private:
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
    EditingCoordinator*          _editingCoordinator;

    /// <summary>
    /// Defines the default StylusPointDescription
    /// </summary>
    QSharedPointer<StylusPointDescription>     _defaultStylusPointDescription;


    /// <summary>
    /// Defines the shape of the eraser tip
    /// </summary>
    StylusShape*                 _eraserShape;

    /// <summary>
    /// Determines if EditingBehaviors should use their own cursor or a custom one specified.
    /// </summary>
    bool                        _useCustomCursor = false;


    //
    // Rendering support.
    //
    InkPresenter*                _inkPresenter;

    //
    // The RealTimeInking PlugIn that handles our off UIContext rendering.
    //
    DynamicRenderer*             _dynamicRenderer;

    //
    // Clipboard Helper
    //
    ClipboardProcessor*          _clipboardProcessor;

    //
    // Gesture support
    //
    GestureRecognizer*           _gestureRecognizer;

    //
    // HighContrast support
    //
    RTIHighContrastCallback*     _rtiHighContrastCallback;

    static constexpr double                    c_pasteDefaultLocation = 0.0;
};

#endif // INKCANVAS_H
