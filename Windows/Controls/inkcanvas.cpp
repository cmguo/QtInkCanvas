#include "Windows/Controls/inkcanvas.h"
#include "Windows/Input/styluspointdescription.h"
#include "Windows/Ink/drawingattributes.h"
#include "Windows/Input/StylusPlugIns/dynamicrenderer.h"
#include "Windows/Input/StylusPlugIns/stylusplugincollection.h"
#include "Internal/Ink/editingcoordinator.h"
#include "Windows/Ink/stylusshape.h"
#include "Internal/Ink/selectioneditor.h"
#include "Windows/dependencypropertychangedeventargs.h"
#include "Windows/Controls/inkevents.h"
#include "Internal/Ink/inkcollectionbehavior.h"
#include "Internal/Ink/lassoselectionbehavior.h"
#include "Internal/Ink/eraserbehavior.h"
#include "Windows/Ink/stroke.h"
#include "Windows/Input/styluspointcollection.h"
#include "Windows/Ink/strokecollection.h"
#include "Windows/Ink/drawingattributes.h"
#include "Windows/Media/pointhittestresult.h"
#include "Windows/Media/pointhittestparameters.h"
#include "Internal/Ink/clipboardprocessor.h"
#include "Internal/Ink/inkcanvasselection.h"
#include "Windows/Input/inputeventargs.h"
#include "Windows/routedeventargs.h"
#include "Windows/Input/stylusdevice.h"
#include "Internal/Controls/inkcanvasfeedbackadorner.h"
#include "Internal/Controls/inkcanvasinnercanvas.h"
#include "Internal/Controls/inkcanvasselectionadorner.h"
#include "Windows/Input/querycursoreventargs.h"
#include "Windows/Input/mousedevice.h"
#include "Internal/Ink/clipboardprocessor.h"
#include "Windows/Controls/inkpresenter.h"
#include "Windows/Controls/decorator.h"
#include "Windows/Ink/gesturerecognizer.h"
#include "Internal/doubleutil.h"
#include "Internal/finallyhelper.h"
#include "Internal/debug.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QShortcut>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDebug>

/// <summary>
/// Public constructor.
/// </summary>
InkCanvas::InkCanvas(QGraphicsItem* parent)
{
    if (Mouse::PrimaryDevice == nullptr)
        Mouse::PrimaryDevice = new MouseDevice;
    Initialize();
    setParentItem(parent);
    QObject::connect(this, &UIElement::IsVisibleChanged, [this]() {
        UpdateCursor();
        _RegisterClipboardHandlers();
    });
}

InkCanvas::~InkCanvas()
{
    if (_feedbackAdorner && !_feedbackAdorner->VisualParent())
        delete _feedbackAdorner;
    SetDynamicRenderer(nullptr);
}

/// <summary>
/// Private initialization method used by the constructors
/// </summary>
void InkCanvas::Initialize()
{
    //
    // instance the DynamicRenderer* and add it to the StylusPlugIns
    //
    _dynamicRenderer = new DynamicRenderer();
    _dynamicRenderer->SetEnabled(false);
    StylusPlugIns().InsertItem(StylusPlugIns().size(), _dynamicRenderer);

    //
    // create and initialize an editing coordinator
    //
    _editingCoordinator = new EditingCoordinator(*this);
    _editingCoordinator->UpdateActiveEditingState();


    // connect the attributes event handler after setting the stylus shape to avoid unnecessary
    //      calls into the RTI service
    //DefaultDrawingAttributes.AttributeChanged += new PropertyDataChangedEventHandler(DefaultDrawingAttributes_Changed);
    QObject::connect(DefaultDrawingAttributes().get(), &DrawingAttributes::AttributeChanged, this, &InkCanvas::DefaultDrawingAttributes_Changed);
    //
    //
    // We must initialize this here (after adding DynamicRenderer* to Sytlus).
    //
    InitializeInkObject();

    //_rtiHighContrastCallback = new RTIHighContrastCallback(this);

    // Register rti high contrast callback. Then check whether we are under the high contrast already.
    //HighContrastHelper.RegisterHighContrastCallback(_rtiHighContrastCallback);
    //if ( SystemParameters.HighContrast )
    //{
    //    _rtiHighContrastCallback.TurnHighContrastOn(SystemColors.WindowTextColor);
    //}

    AddHandler(Stylus::StylusDownEvent,
               RoutedEventHandlerT<InkCanvas, EventArgs, &InkCanvas::_OnDeviceDown>(this));
    AddHandler(Mouse::MouseDownEvent,
               RoutedEventHandlerT<InkCanvas, EventArgs, &InkCanvas::_OnDeviceDown>(this));
    AddHandler(Stylus::StylusUpEvent,
               RoutedEventHandlerT<InkCanvas, EventArgs, &InkCanvas::_OnDeviceUp>(this));
    AddHandler(Mouse::MouseUpEvent,
               RoutedEventHandlerT<InkCanvas, EventArgs, &InkCanvas::_OnDeviceUp>(this));
    AddHandler(Mouse::QueryCursorEvent,
               RoutedEventHandlerT<InkCanvas, QueryCursorEventArgs, &InkCanvas::_OnQueryCursor>(this));
}

/// <summary>
/// Private helper used to change the Ink objects.  Used in the constructor
/// and the Ink property.
///
/// NOTE -- Caller is responsible for clearing any selection!  (We can't clear it
///         here because the Constructor calls this method and it would end up calling
///         looking like it could call a virtual method and FxCop doesn't like that!)
///
/// </summary>
void InkCanvas::InitializeInkObject()
{
    // Update the RealTimeInking PlugIn for the Renderer changes.
    UpdateDynamicRenderer();

    // Initialize DefaultPacketDescription
    _defaultStylusPointDescription.reset(new StylusPointDescription);

}


QSizeF InkCanvas::MeasureOverride(QSizeF availableSize)
{
    // No need to invoke VerifyAccess since _localAdornerDecorator.Measure should check it.
    if ( _localAdornerDecorator == nullptr )
    {
        ApplyTemplate();
    }

    _localAdornerDecorator->Measure(availableSize);

    return  _localAdornerDecorator->DesiredSize();
}

/// <summary>
/// ArrangeOverride
/// </summary>
/// <param name="arrangeSize"></param>
/// <returns></returns>
QSizeF InkCanvas::ArrangeOverride(QSizeF arrangeSize)
{
    // No need to invoke VerifyAccess since _localAdornerDecorator.Arrange should check it.

    if ( _localAdornerDecorator == nullptr )
    {
        ApplyTemplate();
    }

    _localAdornerDecorator->Arrange(QRectF(QPointF(0, 0), arrangeSize));

    return arrangeSize;
}



/// <summary>
///     The DependencyProperty for the Background property.
/// </summary>
DependencyProperty const * const InkCanvas::BackgroundProperty =
        new DependencyProperty(nullptr);


/// <summary>
/// Top DependencyProperty
/// </summary>
DependencyProperty const * const InkCanvas::TopProperty =
        new DependencyProperty(nan(""),
                               &InkCanvas::OnPositioningChanged);

/// <summary>
/// The Bottom DependencyProperty
/// </summary>
DependencyProperty const * const InkCanvas::BottomProperty =
        new DependencyProperty(nan(""),
                               &InkCanvas::OnPositioningChanged);


/// <summary>
/// The Left DependencyProperty
/// </summary>
DependencyProperty const * const InkCanvas::LeftProperty =
        new DependencyProperty(nan(""),
                               &InkCanvas::OnPositioningChanged);

/// <summary>
/// The Right DependencyProperty
/// </summary>
DependencyProperty const * const InkCanvas::RightProperty =
        new DependencyProperty(nan(""),
                               &InkCanvas::OnPositioningChanged);



/// <summary>
/// OnPositioningChanged
/// </summary>
/// <param name="d"></param>
/// <param name="e"></param>
void InkCanvas::OnPositioningChanged(DependencyObject& d, DependencyPropertyChangedEventArgs& e)
{
    UIElement& uie = static_cast<UIElement&>(d);
    //if ( uie != nullptr )
    {
        // Make sure the UIElement* is a child of InkCanvasInnerCanvas.
        InkCanvasInnerCanvas* p = qobject_cast<InkCanvasInnerCanvas*>(uie.Parent());
        if ( p != nullptr )
        {
            if ( e.Property() == InkCanvas::LeftProperty
                || e.Property() == InkCanvas::TopProperty )
            {
                // Invalidate measure for Left and/or Top.
                p->InvalidateMeasure();
            }
            else
            {
                Debug::Assert(e.Property() == InkCanvas::RightProperty || e.Property() == InkCanvas::BottomProperty,
                    "Unknown dependency property detected - {0}.");

                // Invalidate arrange for Right and/or Bottom.
                p->InvalidateArrange();
            }
        }
    }
}


/// <summary>
/// HitTestCore implements precise hit testing against render contents
/// </summary>
HitTestResult InkCanvas::HitTestCore(PointHitTestParameters hitTestParams)
{
    VerifyAccess();

    QRectF r(QPointF(), RenderSize());
    if (r.contains(hitTestParams.HitPoint()))
    {
        return PointHitTestResult(this, hitTestParams.HitPoint());
    }

    return PointHitTestResult();
}

/// <summary>
/// OnPropertyChanged
/// </summary>
void InkCanvas::OnPropertyChanged(DependencyPropertyChangedEventArgs& e)
{
    UIElement::OnPropertyChanged(e);

    //if (e.IsAValueChange || e.IsASubPropertyChange)
    {
        if (e.Property() == UIElement::RenderTransformProperty ||
            e.Property() == FrameworkElement::LayoutTransformProperty)
        {
            GetEditingCoordinator().InvalidateTransform();

            //Transform transform = e.NewValue as Transform;
            //if (transform != nullptr && !transform.HasAnimatedProperties)
            {
                /*
                TransformGroup transformGroup = transform as TransformGroup;
                if ( transformGroup != nullptr )
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
                        if ( transformGroup != nullptr )
                        {
                            for ( int i = 0; i < transformGroup.Children.Count; i++ )
                            {
                                transforms.Push(transformGroup.Children[i]);
                            }
                        }
                    }
                }
                */
                //
                // only invalidate when there is not an animation on the xf,
                // or we could wind up creating thousands of new cursors.  That's bad.
                //
                _editingCoordinator->InvalidateBehaviorCursor(_editingCoordinator->GetInkCollectionBehavior());
                GetEditingCoordinator().UpdatePointEraserCursor();
            }
        }
        //if (e.Property == FrameworkElement.FlowDirectionProperty)
        //{
            //flow direction only affects the inking cursor.
        //    _editingCoordinator.InvalidateBehaviorCursor(_editingCoordinator.InkCollectionBehavior);
        //}
    }
}

/// <summary>
/// Called when the Template's tree is about to be generated
/// </summary>
void InkCanvas::OnPreApplyTemplate()
{
    // No need for calling VerifyAccess since we call the method on the base here.

    //base.OnPreApplyTemplate();

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

    if ( _localAdornerDecorator == nullptr )
    {
        //
        _localAdornerDecorator = new AdornerDecorator();
        _localAdornerDecorator->setObjectName("InkCanvas::LocalAdornerDecorator");
        InkPresenter& inkPresenter = GetInkPresenter();

        // Build the visual tree top-down
        AddVisualChild(_localAdornerDecorator);
        _localAdornerDecorator->SetChild(&inkPresenter);
        inkPresenter.SetChild(&InnerCanvas());

        // Add the SelectionAdorner after Canvas is added.
        _localAdornerDecorator->GetAdornerLayer()->Add(&SelectionAdorner());
    }
}

QBrush InkCanvas::Background()
{
    return GetValue<QBrush>(BackgroundProperty);
}

void InkCanvas::SetBackground(QBrush value)
{
    SetValue(BackgroundProperty, value);
}

/// <summary>
///     The DependencyProperty for the Strokes property.
/// </summary>
DependencyProperty const * const InkCanvas::StrokesProperty =
        new DependencyProperty(new StrokeCollectionDefaultValueFactory,
                               &InkCanvas::OnStrokesChanged);
//        GetInkPresenter().StrokesProperty.AddOwner(
//                typeof(InkCanvas),
//                new FrameworkPropertyMetadata(
//                        new StrokeCollectionDefaultValueFactory(),
//                        new PropertyChangedCallback(OnStrokesChanged)));


void InkCanvas::OnStrokesChanged(DependencyObject& d, DependencyPropertyChangedEventArgs& e)
{
    InkCanvas& inkCanvas = static_cast<InkCanvas&>(d);
    QSharedPointer<StrokeCollection> oldValue = e.OldValue().value<QSharedPointer<StrokeCollection>>();
    QSharedPointer<StrokeCollection> newValue = e.NewValue().value<QSharedPointer<StrokeCollection>>();

    // Bind the InkPresenter.Strokes to InkCanvas.Strokes
    inkCanvas.GetInkPresenter().SetStrokes(newValue);

    //
    // only change the prop if it's a different object.  We don't
    // want to be doing this for no reason
    //
    if ( oldValue != newValue )
    {
        // Clear the selected strokes without raising event.
        inkCanvas.CoreChangeSelection(QSharedPointer<StrokeCollection>(new StrokeCollection()), inkCanvas.GetInkCanvasSelection().SelectedElements(), false);

        inkCanvas.InitializeInkObject();

        InkCanvasStrokesReplacedEventArgs args(newValue, oldValue); //new, previous

        //raise the StrokesChanged event through our protected virtual
        inkCanvas.OnStrokesReplaced(args);
    }

}

static void OnActiveEditingModePropertyChanged(DependencyObject& d, DependencyPropertyChangedEventArgs& args)
{
    // Bind the InkCanvas.ActiveEditingModeProperty
    // to SelectionAdorner.VisibilityProperty.

    InkCanvasEditingMode activeMode = args.NewValue().value<InkCanvasEditingMode>();
    InkCanvas& inkCanvas = static_cast<InkCanvas&>(d);

    // If the current EditingMode is the mode which menuitem is expecting, return true for IsChecked.
    inkCanvas.SelectionAdorner().setVisible(activeMode != InkCanvasEditingMode::None);
}

/// <summary>
/// Returns the SelectionAdorner
/// </summary>
InkCanvasSelectionAdorner& InkCanvas::SelectionAdorner()
{
    // We have to create our visual at this point.
    if ( _selectionAdorner == nullptr )
    {
        // Create the selection Adorner.
        _selectionAdorner = new InkCanvasSelectionAdorner(&InnerCanvas());
        _selectionAdorner->setObjectName("InkCanvas::SelectionAdorner");

        // Bind the InkCanvas.ActiveEditingModeProperty
        // to SelectionAdorner.VisibilityProperty.
        //Binding activeEditingModeBinding = new Binding();
        //activeEditingModeBinding.Path = new PropertyPath(InkCanvas.ActiveEditingModeProperty);
        //activeEditingModeBinding.Mode = BindingMode.OneWay;
        //activeEditingModeBinding.Source = this;
        //activeEditingModeBinding.Converter = new ActiveEditingMode2VisibilityConverter();
        //_selectionAdorner.SetBinding(UIElement*.VisibilityProperty, activeEditingModeBinding);
        _selectionAdorner->setVisible(ActiveEditingMode() != InkCanvasEditingMode::None);
    }

    return *_selectionAdorner;
}

/// <summary>
/// Returns the FeedbackAdorner
/// </summary>
InkCanvasFeedbackAdorner& InkCanvas::FeedbackAdorner()
{
    VerifyAccess();

    if ( _feedbackAdorner == nullptr )
    {
        _feedbackAdorner = new InkCanvasFeedbackAdorner(*this);
        _feedbackAdorner->setObjectName("InkCanvas::FeedbackAdorner");
    }

    return *_feedbackAdorner;
}

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
DependencyProperty const * const InkCanvas::DefaultDrawingAttributesProperty =
        new DependencyProperty(new DrawingAttributesDefaultValueFactory,
                               &InkCanvas::OnDefaultDrawingAttributesChanged,
                               ([](auto v) { return !v.isNull(); }));
//        DependencyProperty.Register(
//                "DefaultDrawingAttributes",
//                typeof(DrawingAttributes),
//                typeof(InkCanvas),
//                new FrameworkPropertyMetadata(
//                        new DrawingAttributesDefaultValueFactory(),
//                        new PropertyChangedCallback(OnDefaultDrawingAttributesChanged)),
//                (ValidateValueCallback)delegate(object value)
//                    { return value != nullptr; });


/// <summary>
/// Gets/Sets the DefaultDrawingAttributes property.
/// </summary>
QSharedPointer<DrawingAttributes> InkCanvas::DefaultDrawingAttributes()
{
    return GetValue<QSharedPointer<DrawingAttributes>>(DefaultDrawingAttributesProperty);
}
void InkCanvas::SetDefaultDrawingAttributes(QSharedPointer<DrawingAttributes> value)
{
    SetValue(DefaultDrawingAttributesProperty, value);
}

void InkCanvas::OnDefaultDrawingAttributesChanged(DependencyObject& d, DependencyPropertyChangedEventArgs& e)
{
    InkCanvas& inkCanvas = static_cast<InkCanvas&>(d);

    QSharedPointer<DrawingAttributes> oldValue = e.OldValue().value<QSharedPointer<DrawingAttributes>>();
    QSharedPointer<DrawingAttributes> newValue = e.NewValue().value<QSharedPointer<DrawingAttributes>>();

    // This can throw, so call it first
    inkCanvas.UpdateDynamicRenderer(newValue);

    // We only fire Changed event when there is an instance change.
    if ( oldValue != newValue )
    {
        //we didn't throw, change our backing value
        //oldValue.AttributeChanged -= new PropertyDataChangedEventHandler(inkCanvas.DefaultDrawingAttributes_Changed);
        QObject::disconnect(oldValue.get(), &DrawingAttributes::AttributeChanged, &inkCanvas, &InkCanvas::DefaultDrawingAttributes_Changed);
        DrawingAttributesReplacedEventArgs args(newValue, oldValue);

        //newValue.AttributeChanged += new PropertyDataChangedEventHandler(inkCanvas.DefaultDrawingAttributes_Changed);
        QObject::connect(newValue.get(), &DrawingAttributes::AttributeChanged, &inkCanvas, &InkCanvas::DefaultDrawingAttributes_Changed);
        inkCanvas.RaiseDefaultDrawingAttributeReplaced(args);
    }
}

/// <summary>
/// Read/Write access to the EraserShape property.
/// </summary>
//[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
StylusShape* InkCanvas::EraserShape()
{
    VerifyAccess();
    if (_eraserShape == nullptr)
    {
        _eraserShape.reset(new RectangleStylusShape(8, 8));
    }
    return _eraserShape.get();

}
void InkCanvas::SetEraserShape(StylusShape * value)
{
    VerifyAccess();
    if (value == nullptr)
    {
        throw new std::runtime_error("value");
    }
    else
    {
        // Invoke getter since this property is lazily created.
        EraserShape();
        std::unique_ptr<StylusShape> oldShape(std::move(_eraserShape));

        _eraserShape.reset(value);

        if ( oldShape->Width() != _eraserShape->Width() || oldShape->Height() != _eraserShape->Height()
            || oldShape->Rotation() != _eraserShape->Rotation() || oldShape->metaObject() != _eraserShape->metaObject())
        {
            GetEditingCoordinator().UpdatePointEraserCursor();
        }
    }
}

#if STROKE_COLLECTION_EDIT_MASK
void InkCanvas::SetEditMask(QPolygonF const & shape)
{
    Strokes()->SetEditMask(shape);
}
#endif

DependencyProperty const * const InkCanvas::ActiveEditingModeProperty =
        new DependencyProperty(QVariant::fromValue(InkCanvasEditingMode::Ink), &OnActiveEditingModePropertyChanged);

DependencyProperty const * const InkCanvas::EditingModeProperty =
        new DependencyProperty(QVariant::fromValue(InkCanvasEditingMode::Ink),
                               &InkCanvas::OnEditingModeChanged,
                               &InkCanvas::ValidateEditingMode);

void InkCanvas::OnEditingModeChanged(DependencyObject& d, DependencyPropertyChangedEventArgs& e)
{
    (void) e;
    RoutedEventArgs e1(InkCanvas::EditingModeChangedEvent, &d);
    static_cast<InkCanvas&>( d ).RaiseEditingModeChanged(e1);
}

DependencyProperty const * const InkCanvas::EditingModeInvertedProperty =
        new DependencyProperty(QVariant::fromValue(InkCanvasEditingMode::EraseByStroke),
                               &InkCanvas::OnEditingModeInvertedChanged,
                               &InkCanvas::ValidateEditingMode);

void InkCanvas::OnEditingModeInvertedChanged(DependencyObject& d, DependencyPropertyChangedEventArgs& e)
{
    (void) e;
    RoutedEventArgs e1(InkCanvas::EditingModeInvertedChangedEvent, &d);
    static_cast<InkCanvas&>( d ).RaiseEditingModeInvertedChanged(e1);
}

bool InkCanvas::ValidateEditingMode(QVariant value)
{
    return EditingModeHelper::IsDefined(static_cast<InkCanvasEditingMode>(value.toInt()));
}

/// <summary>
/// This flag indicates whether the developer is using a custom mouse cursor.
///
/// If this flag is true, we will never change the current cursor on them. Not
/// on edit mode change.
/// </summary>
//[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
bool InkCanvas::UseCustomCursor()
{
    VerifyAccess();
    return _useCustomCursor;
}
void InkCanvas::SetUseCustomCursor(bool value)
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
bool InkCanvas::MoveEnabled()
{
    VerifyAccess();
    return _editingCoordinator->MoveEnabled();
}
void InkCanvas::SetMoveEnabled(bool value)
{
    VerifyAccess();
    bool oldValue = _editingCoordinator->MoveEnabled();

    if (oldValue != value)
    {
        _editingCoordinator->SetMoveEnabled(value);
    }
}

/// <summary>
/// Gets or set if resizing selection is enabled
/// </summary>
/// <value>bool</value>
bool InkCanvas::ResizeEnabled()
{
    VerifyAccess();
    return _editingCoordinator->ResizeEnabled();
}
void InkCanvas::SetResizeEnabled(bool value)
{
    VerifyAccess();
    bool oldValue = _editingCoordinator->ResizeEnabled();

    if (oldValue != value)
    {
        _editingCoordinator->SetResizeEnabled(value);
    }
}


/// <summary>
/// Read/Write access to the DefaultPacketDescription property.
/// </summary>
//[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
QSharedPointer<StylusPointDescription> InkCanvas::DefaultStylusPointDescription()
{
    VerifyAccess();

    return _defaultStylusPointDescription;
}
void InkCanvas::SetDefaultStylusPointDescription(QSharedPointer<StylusPointDescription> value)
{
    VerifyAccess();

    //
    // no nulls allowed
    //
    if ( value == nullptr )
    {
        throw new std::runtime_error("value");
    }

    _defaultStylusPointDescription = value;
}

/// <summary>
/// Read/Write the enabled ClipboardFormats
/// </summary>
//[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
QList<InkCanvasClipboardFormat> InkCanvas::PreferredPasteFormats()
{
    VerifyAccess();

    return GetClipboardProcessor().PreferredFormats();
}
void InkCanvas::SetPreferredPasteFormats(QList<InkCanvasClipboardFormat> value)
{
    VerifyAccess();

    // Cannot be null
    //if ( value == nullptr )
    //{
        // Null is not allowed as the argument value
    //    throw new std::runtime_error("value");
    //}

    GetClipboardProcessor().SetPreferredFormats(value);
}


/// <summary>
///     The StrokeErased Routed Event
/// </summary>
RoutedEvent InkCanvas::StrokeCollectedEvent(0);

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
void InkCanvas::OnStrokeCollected(InkCanvasStrokeCollectedEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
    //{
    //    throw new std::runtime_error("e");
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
void InkCanvas::RaiseGestureOrStrokeCollected(InkCanvasStrokeCollectedEventArgs& e, bool userInitiated)
{
    //Debug::Assert(e != null, "EventArg can not be null");
    bool addStrokeToInkCanvas = true; // Initialize our flag.

    // The follow code raises Gesture event
    // The out-side code could throw exception in the their handlers. We use try/finally block to protect our status.
    //try
    {
        FinallyHelper final([this, &addStrokeToInkCanvas, &e](){
            if ( addStrokeToInkCanvas )
            {
                Strokes()->AddItem(e.GetStroke());
            }
        });

#if WIN32
        //
        // perform gesture reco before raising this event
        // if we're in the right mode
        //
        //IMPORTANT: only call gesture recognition if userInitiated.  See SecurityNote.
        if (userInitiated)
        {
            if ((ActiveEditingMode() == InkCanvasEditingMode::InkAndGesture ||
                  ActiveEditingMode() == InkCanvasEditingMode::GestureOnly) &&
                  GetGestureRecognizer().IsRecognizerAvailable())
            {
                QSharedPointer<StrokeCollection> strokes(new StrokeCollection());
                strokes->AddItem(e.GetStroke());

                //
                // GestureRecognizer.Recognize demands unmanaged code, we assert it here
                // as this codepath is only called in response to user input
                //
                QList<GestureRecognitionResult> results =
                    GetGestureRecognizer().CriticalRecognize(strokes);

                if (results.size() > 0)
                {
                    InkCanvasGestureEventArgs args(strokes, results);

                    if (results[0].GetApplicationGesture() == ApplicationGesture::NoGesture)
                    {
                        //
                        // we set Cancel=true if we didn't detect a gesture
                        //
                        args.SetCancel(true);
                    }
                    else
                    {
                        args.SetCancel(false);
                    }

                    OnGesture(args);

                    //
                    // now that we've raised the Gesture event and the developer
                    // has had a chance to change args.Cancel, see what their intent is.
                    //
                    if (args.Cancel() == false)
                    {
                        //bail out and don't add
                        //the stroke to InkCanvas.Strokes
                        addStrokeToInkCanvas = false; // Reset the flag.
                        return;
                    }
                }
            }
        }
#endif

        // Reset the flag.
        addStrokeToInkCanvas = false;

        //
        // only raise StrokeCollected if we're in InkCanvasEditingMode::Ink or InkCanvasEditingMode::InkAndGesture
        //
        if ( ActiveEditingMode() == InkCanvasEditingMode::Ink ||
            ActiveEditingMode() == InkCanvasEditingMode::InkAndGesture )
        {
            //add the stroke to the StrokeCollection and raise this event
            Strokes()->AddItem(e.GetStroke());
            OnStrokeCollected(e);
        }
    }
    //finally
    //{
        // If the gesture events are failed, we should still add Stroke to the InkCanvas so that the data won't be lost.
    //    if ( addStrokeToInkCanvas )
    //    {
    //        Strokes.Add(e.Stroke);
    //    }
    //}
}

/// <summary>
///     The Gesture Routed Event
/// </summary>
RoutedEvent InkCanvas::GestureEvent(QEvent::Gesture);

/// <summary>
/// Protected virtual version for developers deriving from InkCanvas.
/// This method is what actually throws the event.
/// </summary>
/// <param name="e">InkCanvasGestureEventArgs& to raise the event with</param>
void InkCanvas::OnGesture(InkCanvasGestureEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
    //{
    //    throw new std::runtime_error("e");
    //}

    RaiseEvent(e);
}


/// <summary>
/// Protected virtual version for developers deriving from InkCanvas.
/// This method is what actually throws the event.
/// </summary>
/// <param name="e">InkCanvasStrokesChangedEventArgs& to raise the event with</param>
void InkCanvas::OnStrokesReplaced(InkCanvasStrokesReplacedEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
    //{
    //    throw new std::runtime_error("e");
    //}
    //if (nullptr != StrokesReplaced)
    //{
        StrokesReplaced(e);
    //}
}


/// <summary>
/// Protected virtual version for developers deriving from InkCanvas.
/// This method is what actually throws the event.
/// </summary>
/// <param name="e">DrawingAttributesReplacedEventArgs& to raise the event with</param>
void InkCanvas::OnDefaultDrawingAttributesReplaced(DrawingAttributesReplacedEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if (e == nullptr)
    //{
    //    throw new std::runtime_error("e");
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
void InkCanvas::RaiseDefaultDrawingAttributeReplaced(DrawingAttributesReplacedEventArgs& e)
{
    OnDefaultDrawingAttributesReplaced(e);

    // Invalidate the inking cursor
    _editingCoordinator->InvalidateBehaviorCursor(_editingCoordinator->GetInkCollectionBehavior());
}

/// <summary>
///     Event corresponds to ActiveEditingModeChanged
/// </summary>
RoutedEvent InkCanvas::ActiveEditingModeChangedEvent(0);

/// <summary>
/// Protected virtual version for developers deriving from InkCanvas.
/// This method is what actually throws the event.
/// </summary>
/// <param name="e">EventArgs& to raise the event with</param>
void InkCanvas::OnActiveEditingModeChanged(RoutedEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if (e == nullptr)
    //{
    //    throw new std::runtime_error("e");
    //}

    RaiseEvent(e);
}

/// <summary>
/// Private helper that raises ActiveEditingModeChanged
/// </summary>
/// <param name="e">EventArgs& to raise the event with</param>
void InkCanvas::RaiseActiveEditingModeChanged(RoutedEventArgs& e)
{
    //Debug::Assert(e != null, "EventArg can not be null");

    InkCanvasEditingMode mode = ActiveEditingMode();
    if (mode != _editingCoordinator->ActiveEditingMode())
    {
        //change our DP, then raise the event via our protected override
        SetValue(ActiveEditingModeProperty, _editingCoordinator->ActiveEditingMode());

        OnActiveEditingModeChanged(e);
    }
}



/// <summary>
///     Event corresponds to EditingModeChanged
/// </summary>
RoutedEvent InkCanvas::EditingModeChangedEvent(0);

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
void InkCanvas::OnEditingModeChanged2(RoutedEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
    //{
    //    throw new std::runtime_error("e");
    //}

    RaiseEvent(e);
}
/// <summary>
/// Private helper that raises EditingModeChanged but first
/// talks to the InkEditor about it
/// </summary>
/// <param name="e">EventArgs& to raise the event with</param>
void InkCanvas::RaiseEditingModeChanged(RoutedEventArgs& e)
{
    //Debug::Assert(e != null, "EventArg can not be null");

    _editingCoordinator->UpdateEditingState(false /* EditingMode */);

    OnEditingModeChanged2(e);
}

//note: there is no need for an internal RaiseEditingModeInvertedChanging
//since this isn't a dynamic property and therefore can not be set
//outside of this class

/// <summary>
///     Event corresponds to EditingModeInvertedChanged
/// </summary>
RoutedEvent InkCanvas::EditingModeInvertedChangedEvent(0);

/// <summary>
/// Protected virtual version for developers deriving from InkCanvas.
/// This method is what actually throws the event.
/// </summary>
/// <param name="e">EventArgs& to raise the event with</param>
void InkCanvas::OnEditingModeInvertedChanged2(RoutedEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
    //{
    //    throw new std::runtime_error("e");
    //}

    RaiseEvent(e);
}
/// <summary>
/// Private helper that raises EditingModeInvertedChanged but first
/// talks to the InkEditor about it
/// </summary>
/// <param name="e">EventArgs& to raise the event with</param>
void InkCanvas::RaiseEditingModeInvertedChanged(RoutedEventArgs& e)
{
    //Debug::Assert(e != null, "EventArg can not be null");

    _editingCoordinator->UpdateEditingState(true /* EditingModeInverted */);

    OnEditingModeInvertedChanged2(e);
}


/// <summary>
/// Protected virtual version for developers deriving from InkCanvas.
/// This method is what actually throws the event.
/// </summary>
/// <param name="e"> InkCanvasSelectionEditingEventArgs& to raise the event with</param>
void InkCanvas::OnSelectionMoving( InkCanvasSelectionEditingEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
    //{
    //    throw new std::runtime_error("e");
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
void InkCanvas::RaiseSelectionMoving( InkCanvasSelectionEditingEventArgs& e)
{
    //Debug::Assert(e != null, "EventArg can not be null");
    OnSelectionMoving(e);
}


/// <summary>
/// Protected virtual version for developers deriving from InkCanvas.
/// This method is what actually throws the event.
/// </summary>
/// <param name="e">EventArgs& to raise the event with</param>
void InkCanvas::OnSelectionMoved(EventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
    //{
    //    throw new std::runtime_error("e");
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
void InkCanvas::RaiseSelectionMoved(EventArgs& e)
{
    //Debug::Assert(e != null, "EventArg can not be null");

    OnSelectionMoved(e);
    // Update the cursor of SelectionEditor behavior.
    GetEditingCoordinator().GetSelectionEditor()->OnInkCanvasSelectionChanged();
}

/// <summary>
/// Occurs when the user has erased Strokes using the erase behavior

/// <summary>
/// Protected virtual version for developers deriving from InkCanvas.
/// This method is what actually throws the event.
/// </summary>
/// <param name="e">InkCanvasStrokeErasingEventArgs& to raise the event with</param>
void InkCanvas::OnStrokeErasing(InkCanvasStrokeErasingEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
    //{
    //    throw new std::runtime_error("e");
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
void InkCanvas::RaiseStrokeErasing(InkCanvasStrokeErasingEventArgs& e)
{
    //Debug::Assert(e != null, "EventArg can not be null");
    OnStrokeErasing(e);
}

/// <summary>
///     The StrokeErased Routed Event
/// </summary>
RoutedEvent InkCanvas::StrokeErasedEvent(0);

/// <summary>
/// Protected virtual version for developers deriving from InkCanvas.
/// This method is what actually throws the event.
/// </summary>
/// <param name="e">EventArgs& to raise the event with</param>
void InkCanvas::OnStrokeErased(RoutedEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
    //{
    //    throw new std::runtime_error("e");
    //}
    RaiseEvent(e);
}

/// <summary>
/// Allows the EditingBehaviors to raise the InkErasing event via the protected virtual
/// </summary>
void InkCanvas::RaiseInkErased()
{
    RoutedEventArgs e(InkCanvas::StrokeErasedEvent, this);
    OnStrokeErased(e);
}

/// <summary>
/// Protected virtual version for developers deriving from InkCanvas.
/// This method is what actually throws the event.
/// </summary>
/// <param name="e"> InkCanvasSelectionEditingEventArgs& to raise the event with</param>
void InkCanvas::OnSelectionResizing( InkCanvasSelectionEditingEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
    //{
    //    throw new std::runtime_error("e");
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
void InkCanvas::RaiseSelectionResizing( InkCanvasSelectionEditingEventArgs& e)
{
    //Debug::Assert(e != null, "EventArg can not be null");
    OnSelectionResizing(e);
}


/// <summary>
/// Protected virtual version for developers deriving from InkCanvas.
/// This method is what actually throws the event.
/// </summary>
/// <param name="e">EventArgs& to raise the event with</param>
void InkCanvas::OnSelectionResized(EventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
    //{
    //    throw new std::runtime_error("e");
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
void InkCanvas::RaiseSelectionResized(EventArgs& e)
{
    //Debug::Assert(e != null, "EventArg can not be null");

    OnSelectionResized(e);
    // Update the cursor of SelectionEditor behavior.
    GetEditingCoordinator().GetSelectionEditor()->OnInkCanvasSelectionChanged();
}

/// <summary>
/// Protected virtual version for developers deriving from InkCanvas.
/// This method is what actually throws the event.
/// </summary>
/// <param name="e">InkCanvasSelectionChangingEventArgs& to raise the event with</param>
void InkCanvas::OnSelectionChanging(InkCanvasSelectionChangingEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
    //{
    //    throw new std::runtime_error("e");
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
void InkCanvas::RaiseSelectionChanging(InkCanvasSelectionChangingEventArgs& e)
{
    //Debug::Assert(e != null, "EventArg can not be null");
    OnSelectionChanging(e);
}

/// <summary>
/// Protected virtual version for developers deriving from InkCanvas.
/// This method is what actually throws the event.
/// </summary>
/// <param name="e">EventArgs& to raise the event with</param>
void InkCanvas::OnSelectionChanged(EventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
    //{
    //    throw new std::runtime_error("e");
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
void InkCanvas::RaiseSelectionChanged(EventArgs& e)
{
    //Debug::Assert(e != null, "EventArg can not be null");

    OnSelectionChanged(e);
    // Update the cursor of SelectionEditor behavior.
    GetEditingCoordinator().GetSelectionEditor()->OnInkCanvasSelectionChanged();
}

/// <summary>
/// The InkCanvas uses an inner Canvas to host children.  When the inner Canvas's children
/// are changed, we need to call the protected virtual OnVisualChildrenChanged on the InkCanvas
/// so that subclasses can be notified
/// </summary>
//void RaiseOnVisualChildrenChanged(DependencyObject visualAdded, DependencyObject visualRemoved)
//{
//    OnVisualChildrenChanged(visualAdded, visualRemoved);
//}


/// <summary>
/// Returns the enabled gestures.  This method throws an exception if GestureRecognizerAvailable
/// is false
/// </summary>
/// <returns></returns>
QList<ApplicationGesture> InkCanvas::GetEnabledGestures()
{
    // No need to invoke VerifyAccess since it's checked in GestureRecognizer.GetEnabledGestures.

    //gestureRecognizer throws appropriately if there is no gesture recognizer available
#if WIN32
    return QList<ApplicationGesture>(GetGestureRecognizer().GetEnabledGestures());
#else
    return {};
#endif
}

/// <summary>
/// Sets the enabled gestures.  This method throws an exception if GestureRecognizerAvailable
/// is false
/// </summary>
/// <returns></returns>
void InkCanvas::SetEnabledGestures(QList<ApplicationGesture> applicationGestures)
{
    // No need to invoke VerifyAccess since it's checked in GestureRecognizer.GetEnabledGestures.

    //gestureRecognizer throws appropriately if there is no gesture recognizer available
#if WIN32
    GetGestureRecognizer().SetEnabledGestures(applicationGestures);
#endif
}

/// <summary>
/// Get the selection bounds.
/// </summary>
/// <returns></returns>
QRectF InkCanvas::GetSelectionBounds()
{
    VerifyAccess();

    return GetInkCanvasSelection().SelectionBounds();
}

/// <summary>
/// provides access to the currently selected elements which are children of this InkCanvas
/// </summary>
QList<UIElement*> InkCanvas::GetSelectedElements()
{
    VerifyAccess();
    return GetInkCanvasSelection().SelectedElements();
}

/// <summary>
/// provides read access to the currently selected strokes
/// </summary>
QSharedPointer<StrokeCollection> InkCanvas::GetSelectedStrokes()
{
    VerifyAccess();

    QSharedPointer<StrokeCollection> sc(new StrokeCollection());
    sc->Add(GetInkCanvasSelection().SelectedStrokes());
    return sc;
}

/// <summary>
/// Overload which calls the more complex version, passing null for selectedElements
/// </summary>
/// <param name="selectedStrokes">The strokes to select</param>
void InkCanvas::Select(QSharedPointer<StrokeCollection> selectedStrokes)
{
    // No need to invoke VerifyAccess since this call is forwarded.
    Select(selectedStrokes, {});
}

/// <summary>
/// Overload which calls the more complex version, passing null for selectedStrokes
/// </summary>
/// <param name="selectedElements">The elements to select</param>
void InkCanvas::Select(QList<UIElement*> selectedElements)
{
    // No need to invoke VerifyAccess since this call is forwarded.
    Select(nullptr, selectedElements);
}

/// <summary>
/// Overload which calls the more complex version, passing null for selectedStrokes
/// </summary>
/// <param name="selectedStrokes">The strokes to select</param>
/// <param name="selectedElements">The elements to select</param>
void InkCanvas::Select(QSharedPointer<StrokeCollection> selectedStrokes, QList<UIElement*> selectedElements)
{
    VerifyAccess();

    // NTRAID-WINDOWS#1134932-2005/12/01-WAYNEZEN
    // Try to switch to Select mode first. If we fail to change the mode, then just simply no-op.
    if ( EnsureActiveEditingMode(InkCanvasEditingMode::Select) )
    {
        //
        // validate
        //
        QList<UIElement*> validElements = ValidateSelectedElements(selectedElements);
        QSharedPointer<StrokeCollection> validStrokes = ValidateSelectedStrokes(selectedStrokes);

        //
        // this will raise the 'SelectionChanging' event ONLY if the selection
        // is actually different
        //
        ChangeInkCanvasSelection(validStrokes, validElements);
    }
}

/// <summary>
/// Hit test on the selection
/// </summary>
/// <param name="point"></param>
/// <returns></returns>
InkCanvasSelectionHitResult InkCanvas::HitTestSelection(QPointF const &point)
{
    VerifyAccess();

    // Ensure the visual tree.
    if ( _localAdornerDecorator == nullptr )
    {
        ApplyTemplate();
    }

    return GetInkCanvasSelection().HitTestSelection(point);
}

/// <summary>
/// Copy the current selection in the InkCanvas to the clipboard
/// </summary>
void InkCanvas::CopySelection()
{
    VerifyAccess();
    PrivateCopySelection();
}

/// <summary>
/// Copy the current selection in the InkCanvas to the clipboard and then delete it
/// </summary>
void InkCanvas::CutSelection()
{
    VerifyAccess();

    // Copy first
    InkCanvasClipboardDataFormats copiedDataFormats = PrivateCopySelection();

    // Don't even bother if we don't have a selection.
    if ( copiedDataFormats != InkCanvasClipboardDataFormats() )
    {
        // Then delete the current selection. Note the XAML format won't be avaliable under Partial
        // Trust. So, the selected element shouldn't be copied or removed.
        DeleteCurrentSelection(
            /* We want to delete the selected Strokes if there is ISF and/or XAML data being copied */
            (copiedDataFormats & InkCanvasClipboardDataFormats(
                {InkCanvasClipboardDataFormat::ISF, InkCanvasClipboardDataFormat::XAML})) != 0,
            /* We only want to delete the selected elements if there is XAML data being copied */
            (copiedDataFormats & InkCanvasClipboardDataFormat::XAML) != 0);
    }
}

/// <summary>
/// Paste the contents of the clipboard into the InkCanvas
/// </summary>
void InkCanvas::Paste()
{
    // No need to call VerifyAccess since this call is forwarded.

    // We always paste the data to the default location which is (0,0).
    Paste(QPointF(c_pasteDefaultLocation, c_pasteDefaultLocation));
}

/// <summary>
/// Paste the contents of the clipboard to the specified location in the InkCanvas
/// </summary>
void InkCanvas::Paste(QPointF const &point)
{
    VerifyAccess();

    if (DoubleUtil::IsNaN(point.x()) ||
        DoubleUtil::IsNaN(point.y()) ||
        qIsInf(point.x())||
        qIsInf(point.y()) )
    {
            throw std::runtime_error("point");
    }


    //
    // only do this if the user is not editing (input active)
    // or we will violate a dispatcher lock
    //
    if (!_editingCoordinator->UserIsEditing())
    {
        DataObject const * dataObj = nullptr;
        try
        {
            dataObj = QApplication::clipboard()->mimeData();
        }
        catch (...)
        {
            //harden against ExternalException
            return;
        }
        if (dataObj != nullptr)
        {
            PasteFromDataObject(dataObj, point);
        }
    }
}

/// <summary>
/// Return true if clipboard contents can be pasted into the InkCanvas.
/// </summary>
bool InkCanvas::CanPaste()
{
    VerifyAccess();

    bool ret = false;
    //
    // can't paste if the user is editing (input active)
    // or we will violate a dispatcher lock
    //
    if (_editingCoordinator->UserIsEditing())
    {
        return false;
    }

    // Check whether the caller has the clipboard permission.
    //if ( !SecurityHelper.CallerHasAllClipboardPermission() )
    //{
    //    return false;
    //}

    ret = PrivateCanPaste();

    return ret;
}


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
//        throw new std::runtime_error("value");
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
DynamicRenderer* InkCanvas::GetDynamicRenderer()
{
    VerifyAccess();
    return InternalDynamicRenderer();
}
void InkCanvas::SetDynamicRenderer(DynamicRenderer* value)
{
    VerifyAccess();
    if (value != _dynamicRenderer)
    {
        int previousIndex = -1;
        //remove the existing plugin
        if (_dynamicRenderer != nullptr)
        {
            //remove the plugin from the collection
            previousIndex = StylusPlugIns().indexOf(_dynamicRenderer);
            if (-1 != previousIndex)
            {
                StylusPlugIns().RemoveItem(previousIndex);
            }

            //remove the plugin's visual from the InkPresenter
            if (GetInkPresenter().ContainsAttachedVisual(_dynamicRenderer->RootVisual()))
            {
                GetInkPresenter().DetachVisuals(_dynamicRenderer->RootVisual());
            }
            delete _dynamicRenderer;
        }

        _dynamicRenderer = value;

        if (_dynamicRenderer != nullptr) //null is acceptable
        {
            //remove the plugin from the collection
            if (!StylusPlugIns().contains(_dynamicRenderer))
            {
                if (-1 != previousIndex)
                {
                    //insert the new DR in the same location as the old one
                    StylusPlugIns().InsertItem(previousIndex, _dynamicRenderer);
                }
                else
                {
                    StylusPlugIns().InsertItem(StylusPlugIns().size(), _dynamicRenderer);
                }
            }

            //refer to the same DrawingAttributes as the InkCanvas
            _dynamicRenderer->SetDrawingAttributes(DefaultDrawingAttributes());

            //attach the DynamicRenderer* if it is not already
            if (!(GetInkPresenter().ContainsAttachedVisual(_dynamicRenderer->RootVisual())) &&
                _dynamicRenderer->Enabled() &&
                _dynamicRenderer->RootVisual() != nullptr)
            {
                GetInkPresenter().AttachVisuals(_dynamicRenderer->RootVisual(), DefaultDrawingAttributes());
            }
        }
    }
}

/// <summary>
/// Protected read only access to the InkPresenter this InkCanvas uses
/// </summary>
InkPresenter& InkCanvas::GetInkPresenter()
{
    VerifyAccess();
    if ( _inkPresenter == nullptr )
    {
        _inkPresenter = new InkPresenter();

        // Bind the GetInkPresenter().Strokes to InkCanvas.Strokes
        //Binding strokes = new Binding();
        //strokes.Path = new PropertyPath(InkCanvas.StrokesProperty);
        //strokes.Mode = BindingMode.OneWay;
        //strokes.Source = this;
        //GetInkPresenter().SetBinding(GetInkPresenter().StrokesProperty, strokes);
        GetInkPresenter().SetStrokes(Strokes());

    }
    return *_inkPresenter;
}

/// <summary>
/// Deselect the current selection
/// </summary>
RoutedCommand InkCanvas::DeselectCommand;

/// <summary>
/// UserInitiatedCanPaste
/// </summary>
/// <returns></returns>
/// <SecurityNote>
///     Critical -      Elevates the AllClipboard permission for checking the supported data in InkCanvas.
/// </SecurityNote>
//[SecurityCritical]
bool InkCanvas::UserInitiatedCanPaste()
{
    //( new UIPermission(UIPermissionClipboard.AllClipboard) ).Assert();//BlessedAssert
    //try
    {
        return PrivateCanPaste();
    }
    //finally
    //{
    //    UIPermission.RevertAssert();
    //}

}

/// <summary>
/// PrivateCanPaste
/// </summary>
/// <returns></returns>
bool InkCanvas::PrivateCanPaste()
{
    bool canPaste = false;
    DataObject const * dataObj = nullptr;
    try
    {
        dataObj = QApplication::clipboard()->mimeData();
    }
    catch (...)
    {
        //harden against ExternalException
        return false;
    }
    if ( dataObj != nullptr )
    {
        canPaste = GetClipboardProcessor().CheckDataFormats(dataObj);
    }

    return canPaste;
}

/// <summary>
/// This method pastes data from an DataObject object
/// </summary>
void InkCanvas::PasteFromDataObject(DataObject const * dataObj, QPointF const & point)
{
    // Reset the current selection
    ClearSelection(false);

    // Assume that there is nothing to be selected.
    QSharedPointer<StrokeCollection> newStrokes(new StrokeCollection());
    QList<UIElement*> newElements;

    // Paste the data from the data object.
    if ( !GetClipboardProcessor().PasteData(dataObj, newStrokes, newElements) )
    {
        // Paste was failed.
        return;
    }
    else if ( newStrokes->size() == 0 && newElements.size() == 0 )
    {
        // Nothing has been received from the clipboard.
        return;
    }

    // We add elements here. Then we have to wait for the layout update.
    QList<UIElement*> children = Children();
    for ( UIElement* element : newElements )
    {
        children.append(element);
    }

    if ( newStrokes != nullptr )
    {
        Strokes()->Add(newStrokes);
    }

    FinallyHelper final([this, &point](){
        QRectF bounds = GetSelectionBounds( );
        GetInkCanvasSelection().CommitChanges(bounds.translated(-bounds.left() + point.x(), -bounds.top() + point.y()), false);

        if (EditingMode() != InkCanvasEditingMode::Select)
        {
            // Clear the selection without the event if the editing mode is not Select.
            ClearSelection(false);
        }
    });
    //try
    {
        // We should fire SelectionChanged event if the current editing mode is Select.
        CoreChangeSelection(newStrokes, newElements, EditingMode() == InkCanvasEditingMode::Select);
    }
    //finally
    //{
        // Now move the selection to the desired location.
    //    QRectF bounds = GetSelectionBounds( );
    //    GetInkCanvasSelection().CommitChanges(Rect.Offset(bounds, -bounds.Left + point.X, -bounds.Top + point.Y), false);

    //    if (EditingMode != InkCanvasEditingMode::Select)
    //    {
            // Clear the selection without the event if the editing mode is not Select.
    //        ClearSelection(false);
    //    }
    //}
}

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
InkCanvasClipboardDataFormats InkCanvas::CopyToDataObject()
{
    DataObject* dataObj;
    //(new UIPermission(UIPermissionClipboard.AllClipboard)).Assert();//BlessedAssert
    //try
    {
        dataObj = new DataObject();
    }
    //finally
    //{
    //    UIPermission.RevertAssert();
    //}
    InkCanvasClipboardDataFormats copiedDataFormats = InkCanvasClipboardDataFormat::None;

    // Try to copy the data from the InkCanvas to the clipboard.
    copiedDataFormats = GetClipboardProcessor().CopySelectedData(dataObj);

    if ( copiedDataFormats != InkCanvasClipboardDataFormats() )
    {
        //PermissionSet ps = new PermissionSet(PermissionState.None);
        //ps.AddPermission(new SecurityPermission(SecurityPermissionFlag.SerializationFormatter));
        //ps.AddPermission(new UIPermission(UIPermissionClipboard.AllClipboard));
        //ps.Assert(); // BlessedAssert
        //try
        {
            // Put our data object into the clipboard.
            QApplication::clipboard()->setMimeData(dataObj);
        }
        //finally
        //{
        //    SecurityPermission.RevertAssert();
        //}
    }

    return copiedDataFormats;
}


/// <summary>
/// Return the inner Canvas.
/// </summary>
InkCanvasInnerCanvas& InkCanvas::InnerCanvas()
{
        // We have to create our visual at this point.
    if (_innerCanvas == nullptr)
    {
        // Create our InnerCanvas to change the logical parent of Canvas' children.
        _innerCanvas = new InkCanvasInnerCanvas(*this);

        // Bind the inner Canvas' Background to InkCanvas' Background
        //Binding background = new Binding();
        //background.Path = new PropertyPath(InkCanvas.BackgroundProperty);
        //background.Mode = BindingMode.OneWay;
        //background.Source = this;
        //_innerCanvas.SetBinding(Panel.BackgroundProperty, background);
    }

    return *_innerCanvas;
}

/// <summary>
/// Internal access to the current selection
/// </summary>
InkCanvasSelection& InkCanvas::GetInkCanvasSelection()
{
    if ( _selection == nullptr )
    {
        _selection = new InkCanvasSelection(*this);
    }

    return *_selection;
}


/// <summary>
/// Internal helper called by the LassoSelectionBehavior
/// </summary>
void InkCanvas::BeginDynamicSelection(Visual* visual)
{
    GetEditingCoordinator().DebugCheckActiveBehavior(GetEditingCoordinator().GetLassoSelectionBehavior());

    _dynamicallySelectedStrokes.reset(new StrokeCollection());

    GetInkPresenter().AttachVisuals(visual, QSharedPointer<DrawingAttributes>(new DrawingAttributes()));
}

/// <summary>
/// Internal helper called by LassoSelectionBehavior to update the display
/// of dynamically added strokes
/// </summary>
void InkCanvas::UpdateDynamicSelection(   QSharedPointer<StrokeCollection> strokesToDynamicallySelect,
                                        QSharedPointer<StrokeCollection> strokesToDynamicallyUnselect)
{
    GetEditingCoordinator().DebugCheckActiveBehavior(GetEditingCoordinator().GetLassoSelectionBehavior());

    //
    // update our internal stroke collections used by dynamic selection
    //
    if (strokesToDynamicallySelect != nullptr)
    {
        for (QSharedPointer<Stroke> s : *strokesToDynamicallySelect)
        {
            _dynamicallySelectedStrokes->AddItem(s);
            s->SetIsSelected(true);
        }
    }

    if (strokesToDynamicallyUnselect != nullptr)
    {
        for (QSharedPointer<Stroke> s : *strokesToDynamicallyUnselect)
        {
            //System.Diagnostics.Debug::Assert(_dynamicallySelectedStrokes.Contains(s));
            _dynamicallySelectedStrokes->RemoveItem(s);
            s->SetIsSelected(false);
        }
    }
}

/// <summary>
/// Internal helper used by LassoSelectionBehavior
/// </summary>
QSharedPointer<StrokeCollection> InkCanvas::EndDynamicSelection(Visual* visual)
{
    GetEditingCoordinator().DebugCheckActiveBehavior(GetEditingCoordinator().GetLassoSelectionBehavior());

    GetInkPresenter().DetachVisuals(visual);

    QSharedPointer<StrokeCollection> selectedStrokes = _dynamicallySelectedStrokes;
    _dynamicallySelectedStrokes = nullptr;

    return selectedStrokes;
}

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
bool InkCanvas::ClearSelectionRaiseSelectionChanging()
{
    if ( !GetInkCanvasSelection().HasSelection() )
    {
        return true;
    }

    //
    // attempt to clear selection
    //
    ChangeInkCanvasSelection(QSharedPointer<StrokeCollection>(new StrokeCollection()), QList<UIElement*>());

    return !GetInkCanvasSelection().HasSelection();
}

/// <summary>
/// ClearSelection
///     Called by:
///         PasteFromDataObject
///         EditingCoordinator.UpdateEditingState
/// </summary>
void InkCanvas::ClearSelection(bool raiseSelectionChangedEvent)
{
    if ( GetInkCanvasSelection().HasSelection() )
    {
        // Reset the current selection
        CoreChangeSelection(QSharedPointer<StrokeCollection>(new StrokeCollection()), QList<UIElement*>(), raiseSelectionChangedEvent);
    }
}


/// <summary>
/// Helper that creates selection for an InkCanvas.  Used by the SelectedStrokes and
/// SelectedElements properties
/// </summary>
void InkCanvas::ChangeInkCanvasSelection(QSharedPointer<StrokeCollection> strokes, QList<UIElement*> elements)
{
    //validate in debug only for this internal static
    Debug::Assert(strokes != nullptr,
                "Invalid arguments in ChangeInkCanvasSelection");

    bool strokesAreDifferent;
    bool elementsAreDifferent;
    GetInkCanvasSelection().SelectionIsDifferentThanCurrent(strokes, strokesAreDifferent, elements, elementsAreDifferent);
    if ( strokesAreDifferent || elementsAreDifferent )
    {
        InkCanvasSelectionChangingEventArgs args(strokes, elements);

        QSharedPointer<StrokeCollection> validStrokes = strokes;
        QList<UIElement*> validElements = elements;

        RaiseSelectionChanging(args);

        //now that the event has been raised and all of the delegates
        //have had their way with it, process the result
        if ( !args.Cancel() )
        {

            //
            // rock and roll, time to validate our arguments
            // note: these event args are visible outside the apis,
            // so we need to validate them again
            //

            // PERF-2006/05/02-WAYNEZEN,
            // Check our internal flag. If the SelectedStrokes has been changed, we shouldn't do any extra work here.
            if ( args.StrokesChanged() )
            {
                validStrokes = ValidateSelectedStrokes(args.GetSelectedStrokes());
                int countOldSelectedStrokes = strokes->size();
                for ( int i = 0; i < countOldSelectedStrokes; i++ )
                {
                    // PERF-2006/05/02-WAYNEZEN,
                    // We only have to reset IsSelected for the elements no longer exists in the new collection.
                    if ( !validStrokes->contains((*strokes)[i]) )
                    {
                        // NTRAID#WINDOWS-1045099-2006/05/02-waynezen,
                        // Make sure we reset the IsSelected property which could have been
                        // set to true in the dynamic selection.
                        (*strokes)[i]->SetIsSelected(false);
                    }
                }
            }


            // PERF-2006/05/02-WAYNEZEN,
            // Check our internal flag. If the SelectedElements has been changed, we shouldn't do any extra work here.
            if ( args.ElementsChanged() )
            {
                validElements = ValidateSelectedElements(args.GetSelectedElements());
            }

            CoreChangeSelection(validStrokes, validElements, true);
        }
        else
        {
            QSharedPointer<StrokeCollection> currentSelectedStrokes = GetInkCanvasSelection().SelectedStrokes();
            int countOldSelectedStrokes = strokes->size();
            for ( int i = 0; i < countOldSelectedStrokes; i++ )
            {
                // Make sure we reset the IsSelected property which could have been
                // set to true in the dynamic selection but not being selected previously.
                if ( !currentSelectedStrokes->contains((*strokes)[i]) )
                {
                    (*strokes)[i]->SetIsSelected(false);
                }
            }
        }
    }
}


/// <summary>
/// Helper method used by ChangeInkCanvasSelection and directly by ClearSelectionWithoutSelectionChanging
/// </summary>
/// <param name="validStrokes">validStrokes</param>
/// <param name="validElements">validElements</param>
/// <param name="raiseSelectionChanged">raiseSelectionChanged</param>
void InkCanvas::CoreChangeSelection(QSharedPointer<StrokeCollection> validStrokes, QList<UIElement*> validElements, bool raiseSelectionChanged)
{
    GetInkCanvasSelection().Select(validStrokes, validElements, raiseSelectionChanged);
}

#if DEBUG_LASSO_FEEDBACK
ContainerVisual RendererRootContainer()
{
    return _inkContainerVisual;
}
#endif


/// <summary>
/// Private helper method used to retrieve a StrokeCollection which does AND operation on two input collections.
/// </summary>
/// <param name="subset">The possible subset</param>
/// <param name="superset">The container set</param>
/// <returns>True if subset is a subset of superset, false otherwise</returns>
QSharedPointer<StrokeCollection> InkCanvas::GetValidStrokes(QSharedPointer<StrokeCollection> subset, QSharedPointer<StrokeCollection> superset)
{
    QSharedPointer<StrokeCollection> validStrokes(new StrokeCollection());

    int subsetCount = subset->size();

    // special case an empty subset as a guaranteed subset
    if ( subsetCount == 0 )
    {
        return validStrokes;
    }

    for ( int i = 0; i < subsetCount; i++ )
    {
        QSharedPointer<Stroke> stroke = (*subset)[i];
        if ( superset->contains(stroke) )
        {
            validStrokes->AddItem(stroke);
        }
    }

    return validStrokes;
}

/// <summary>
/// Register the commanding handlers for the clipboard operations
/// </summary>
/// <SecurityNote>
///     Critical: Elevates to associate a protected command (paste) with keyboard
///     TreatAsSafe: We don't take user input here. Shift+Insert is the correct key binding,
///                  and therefore is expected by the user.
/// </SecurityNote>
//[SecurityCritical, SecurityTreatAsSafe]

void InkCanvas::_RegisterClipboardHandlers()
{
    /*
    Type ownerType = typeof(InkCanvas);

    CommandHelpers.RegisterCommandHandler(ownerType, ApplicationCommands::Cut,
        new ExecutedRoutedEventHandler(_OnCommandExecuted), new CanExecuteRoutedEventHandler(_OnQueryCommandEnabled),
        SRID.KeyShiftDelete, SRID.KeyShiftDeleteDisplayString);
    CommandHelpers.RegisterCommandHandler(ownerType, ApplicationCommands::Copy,
        new ExecutedRoutedEventHandler(_OnCommandExecuted), new CanExecuteRoutedEventHandler(_OnQueryCommandEnabled),
        SRID.KeyCtrlInsert, SRID.KeyCtrlInsertDisplayString);

    // Use temp variables to reduce code under elevation
    ExecutedRoutedEventHandler pasteExecuteEventHandler = new ExecutedRoutedEventHandler(_OnCommandExecuted);
    CanExecuteRoutedEventHandler pasteQueryEnabledEventHandler = new CanExecuteRoutedEventHandler(_OnQueryCommandEnabled);
    InputGesture pasteInputGesture = KeyGesture.CreateFromResourceStrings(SR.Get(SRID.KeyShiftInsert), SR.Get(SRID.KeyShiftInsertDisplayString));

    new UIPermission(UIPermissionClipboard.AllClipboard).Assert(); // BlessedAssert:
    try
    {
        CommandHelpers.RegisterCommandHandler(ownerType, ApplicationCommands::Paste,
            pasteExecuteEventHandler, pasteQueryEnabledEventHandler, pasteInputGesture);
    }
    finally
    {
        CodeAccessPermission.RevertAssert();
    }
    */
    QWidget* widget = scene()->views().first();
    if (!widget->property("inkcanvasshortcut").isValid()) {
        QKeySequence::StandardKey keys[] = {
            QKeySequence::Copy,
            QKeySequence::Cut,
            QKeySequence::Paste,
            QKeySequence::SelectAll,
            QKeySequence::Delete,
            QKeySequence::Cancel
        };
        for (QObject * c : widget->children()) {
            QShortcut * shortcut = qobject_cast<QShortcut *>(c);
            if (!shortcut) continue;
            for (QKeySequence::StandardKey & k : keys) {
                if (k != QKeySequence::UnknownKey && shortcut->key().matches(k))
                    k = QKeySequence::UnknownKey;
            }
        }
        for (QKeySequence::StandardKey k : keys) {
            if (k != QKeySequence::UnknownKey)
                new QShortcut(k, widget);
        }
        widget->setProperty("inkcanvasshortcut", true);
    }
    for (QObject * c : widget->children()) {
        QShortcut * shortcut = qobject_cast<QShortcut *>(c);
        if (shortcut)
            QObject::connect(shortcut, &QShortcut::activated, this, &InkCanvas::_OnCommandExecuted);
    }
}

/// <summary>
/// Private helper used to ensure that any stroke collection
/// passed to the InkCanvas is valid.  Throws exceptions if the argument is invalid
/// </summary>
QSharedPointer<StrokeCollection> InkCanvas::ValidateSelectedStrokes(QSharedPointer<StrokeCollection> strokes)
{
    //
    //  null is a valid input
    //
    if (strokes == nullptr)
    {
        return QSharedPointer<StrokeCollection>(new StrokeCollection());
    }
    else
    {
        return GetValidStrokes(strokes, Strokes());
    }
}

/// <summary>
/// Private helper used to ensure that a UIElement* argument passed in
/// is valid.
/// </summary>
QList<UIElement*> InkCanvas::ValidateSelectedElements(QList<UIElement*> selectedElements)
{
    //if (selectedElements == nullptr)
    //{
    //    return new UIElement*[]{};
    //}

    QList<UIElement*> elements;
    for (UIElement* element : selectedElements)
    {
        // NTRAID:WINDOWSOS#1621480-2006/04/26-WAYNEZEN,
        // Don't add the duplicated element.
        if ( !elements.contains(element) )
        {
            //
            // check the common case first, e
            //
            if ( InkCanvasIsAncestorOf(element) )
            {
                elements.append(element);
            }
        }
    }

    return elements;
}

/// <summary>
/// Helper method used by DesignActivation to see if an element
/// has this InkCanvas as an Ancestor
/// </summary>
bool InkCanvas::InkCanvasIsAncestorOf(UIElement* element)
{
    if (this != element && IsAncestorOf(element))
    {
        return true;
    }
    return false;
}

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
void InkCanvas::DefaultDrawingAttributes_Changed(PropertyDataChangedEventArgs& args)
{
    // note that sender should be the same as _defaultDrawingAttributes
    // If a developer writes code to change the DefaultDrawingAttributes inside of the event
    //      handler before the InkCanvas receives the notification (multi-cast delegate scenario) -
    //      The DefaultDrawingAttributes should still be updated, and in that case we would
    //      update the RTI DAC twice. Typically, however, this will just refresh the
    //      attributes in the RTI thread.
    //System.Diagnostics.Debug::Assert(object.ReferenceEquals(sender, DefaultDrawingAttributes));

    InvalidateSubProperty(DefaultDrawingAttributesProperty);

    // Be sure to update the RealTimeInking PlugIn with the drawing attribute changes.
    UpdateDynamicRenderer();

    // Invalidate the inking cursor
    _editingCoordinator->InvalidateBehaviorCursor(_editingCoordinator->GetInkCollectionBehavior());
}

/// <summary>
/// Helper method used to set up the GetDynamicRenderer()->
/// </summary>
void InkCanvas::UpdateDynamicRenderer()
{
    UpdateDynamicRenderer(DefaultDrawingAttributes());
}
/// <summary>
/// Helper method used to set up the GetDynamicRenderer()->
/// </summary>
void InkCanvas::UpdateDynamicRenderer(QSharedPointer<DrawingAttributes> newDrawingAttributes)
{
    ApplyTemplate();

    if (GetDynamicRenderer() != nullptr)
    {
        GetDynamicRenderer()->SetDrawingAttributes(newDrawingAttributes);

        if (!GetInkPresenter().AttachedVisualIsPositionedCorrectly(GetDynamicRenderer()->RootVisual(), newDrawingAttributes))
        {
            if (GetInkPresenter().ContainsAttachedVisual(GetDynamicRenderer()->RootVisual()))
            {
                GetInkPresenter().DetachVisuals(GetDynamicRenderer()->RootVisual());
            }

            // Only hook up if we are enabled.  As we change editing modes this routine will be called
            // to clean up things.
            if (GetDynamicRenderer()->Enabled() && GetDynamicRenderer()->RootVisual() != nullptr)
            {
                GetInkPresenter().AttachVisuals(GetDynamicRenderer()->RootVisual(), newDrawingAttributes);
            }
        }
    }
}

bool InkCanvas::EnsureActiveEditingMode(InkCanvasEditingMode newEditingMode)
{
    bool ret = true;

    if ( ActiveEditingMode() != newEditingMode )
    {
        if ( GetEditingCoordinator().IsStylusInverted() )
        {
            SetEditingModeInverted(newEditingMode);
        }
        else
        {
            SetEditingMode(newEditingMode);
        }

        // Verify whether user has cancelled the change in EditingModeChanging event.
        ret = ( ActiveEditingMode() == newEditingMode );
    }

    return ret;
}

// The ClipboardProcessor instance which deals with the operations relevant to the clipboard.
ClipboardProcessor& InkCanvas::GetClipboardProcessor()
{
    if ( _clipboardProcessor == nullptr )
    {
        _clipboardProcessor = new ClipboardProcessor(*this);
    }

    return *_clipboardProcessor;
}

//lazy instance the gesture recognizer
GestureRecognizer& InkCanvas::GetGestureRecognizer()
{
    if (_gestureRecognizer == nullptr)
    {
#if WIN32
        _gestureRecognizer = new GestureRecognizer();
#endif
    }
    return *_gestureRecognizer;
}

/// <summary>
/// Delete the current selection
/// </summary>
void InkCanvas::DeleteCurrentSelection(bool removeSelectedStrokes, bool removeSelectedElements)
{
    //Debug::Assert(removeSelectedStrokes || removeSelectedElements, "At least either Strokes or Elements should be removed!");

    // Now delete the current selection.
    QSharedPointer<StrokeCollection> strokes = GetSelectedStrokes();
    QList<UIElement*> elements = GetSelectedElements();

    // Clear the selection first.
    CoreChangeSelection(
        removeSelectedStrokes ? QSharedPointer<StrokeCollection>(new StrokeCollection()) : strokes,
        removeSelectedElements ? QList<UIElement*>() : elements,
        true);

    // Remove the ink.
    if ( removeSelectedStrokes && strokes != nullptr && strokes->size() != 0 )
    {
        Strokes()->Remove(strokes);
    }

    // Remove the elements.
    if ( removeSelectedElements )
    {
        QList<UIElement*> children = Children();
        for ( UIElement* element : elements )
        {
            children.removeOne(element);
        }
    }
}

/// <summary>
/// A class handler of the Commands
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>

void InkCanvas::_OnCommandExecuted()
{
    /*
    ICommand command = args.Command;
    InkCanvas& inkCanvas = static_cast<InkCanvas&>(sender);

    Debug::Assert(inkCanvas != nullptr);

    if ( inkCanvas.IsEnabled() && !inkCanvas.GetEditingCoordinator().UserIsEditing() )
    {
        if ( command == ApplicationCommands::Delete )
        {
            inkCanvas.DeleteCurrentSelection(true, true);
        }
        else if ( command == ApplicationCommands::Cut )
        {
            inkCanvas.CutSelection();
        }
        else if ( command == ApplicationCommands::Copy )
        {
            inkCanvas.CopySelection();
        }
        else if ( command == ApplicationCommands::SelectAll )
        {
            if ( inkCanvas.ActiveEditingMode() == InkCanvasEditingMode::Select )
            {
                QList<UIElement*> uiElementCollection = inkCanvas.Children();
                inkCanvas.Select(inkCanvas.Strokes(), uiElementCollection);
            }
        }
        else if ( command == ApplicationCommands::Paste )
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
        else if ( command == InkCanvas::DeselectCommand )
        {
            inkCanvas.ClearSelectionRaiseSelectionChanging();
        }
    }
    */
    if (!scene()/* || scene()->mouseGrabberItem() != this*/)
        return;
    QShortcut* shortcut = qobject_cast<QShortcut*>(sender());
    if ( IsEnabled() && !GetEditingCoordinator().UserIsEditing() ) {
        if (shortcut->key().matches(QKeySequence::Delete)) {
            DeleteCurrentSelection(true, true);
        } else if (shortcut->key().matches(QKeySequence::Cut)) {
            CutSelection();
        } else if (shortcut->key().matches(QKeySequence::Copy)) {
            CopySelection();
        } else if (shortcut->key().matches(QKeySequence::SelectAll)) {
            if ( ActiveEditingMode() == InkCanvasEditingMode::Select )
            {
                QList<UIElement*> uiElementCollection;// = Children();
                Select(Strokes(), uiElementCollection);
            }
        } else if (shortcut->key().matches(QKeySequence::Paste)) {
            try {
                Paste();
            } catch (...) {
            }
        } else if (shortcut->key().matches(QKeySequence::Cancel)) {
            ClearSelectionRaiseSelectionChanging();
        }
    }
}

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
void InkCanvas::_OnQueryCommandEnabled(CanExecuteRoutedEventArgs& args)
{
    RoutedCommand command = (RoutedCommand)(args.Command);
    InkCanvas inkCanvas = sender as InkCanvas;

    Debug::Assert(inkCanvas != nullptr);

    if ( inkCanvas.IsEnabled
        // NTRAID-WINDOWSOS#1578484-2006/04/14-WAYNEZEN,
        // If user is editing, we should disable all commands.
        && !inkCanvas.GetEditingCoordinator().UserIsEditing )
    {
        if ( command == ApplicationCommands::Delete
            || command == ApplicationCommands::Cut
            || command == ApplicationCommands::Copy
            || command == InkCanvas.DeselectCommand )
        {
            args.CanExecute = inkCanvas.GetInkCanvasSelection().HasSelection;
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
            args.CanExecute = ( inkCanvas.ActiveEditingMode() == InkCanvasEditingMode::Select
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

InkCanvasClipboardDataFormats InkCanvas::PrivateCopySelection()
{
    InkCanvasClipboardDataFormats copiedDataFormats = InkCanvasClipboardDataFormat::None;

    // Don't even bother if we don't have a selection or UserIsEditing has been set.
    if ( GetInkCanvasSelection().HasSelection() && !_editingCoordinator->UserIsEditing())
    {
        copiedDataFormats = CopyToDataObject();
    }

    return copiedDataFormats;
}


/// <summary>
/// _OnDeviceDown
/// </summary>
/// <typeparam name="TEventArgs"></typeparam>
/// <param name="sender"></param>
/// <param name="e"></param>
void InkCanvas::_OnDeviceDown(EventArgs& e)
{
    GetEditingCoordinator().OnInkCanvasDeviceDown(static_cast<InputEventArgs&>(e));
}

/// <summary>
/// _OnDeviceUp
/// </summary>
/// <typeparam name="TEventArgs"></typeparam>
/// <param name="sender"></param>
/// <param name="e"></param>
void InkCanvas::_OnDeviceUp(EventArgs& e)
{
    GetEditingCoordinator().OnInkCanvasDeviceUp(static_cast<InputEventArgs&>(e));
}

/// <summary>
/// _OnQueryCursor
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void InkCanvas::_OnQueryCursor(QueryCursorEventArgs& e)
{
    if ( UseCustomCursor() )
    {
        // If UseCustomCursor is set, we bail out. Let the base class (FrameworkElement) to do the rest.
        return;
    }

    // We should behave like our base - honor ForceCursor property.
    if ( !e.Handled() || ForceCursor() )
    {
        QCursor cursor = GetEditingCoordinator().GetActiveBehaviorCursor();

        // If cursor is null, we don't handle the event and leave it as whatever the default is.
        //if ( cursor != QCursor() )
        {
            e.SetCursor(cursor);
            e.SetHandled(true);
        }
    }
}

/// <summary>
/// Update the current cursor if mouse is over InkCanvas. Called by
///     EditingCoordinator.InvalidateBehaviorCursor
///     EditingCoordinator.UpdateEditingState
///     InkCanvas.set_UseCustomCursor
/// </summary>
void InkCanvas::UpdateCursor()
{
    //if ( IsMouseOver() )
    {
        Mouse::UpdateCursor(this);
    }
}


//------------------------------------------------------
//
//  Private Classes
//
//------------------------------------------------------



/// <summary>
/// A helper class for RTI high contrast support
/// </summary>
class InkCanvas::RTIHighContrastCallback
{
    //------------------------------------------------------
    //
    //  Cnostructors
    //
    //------------------------------------------------------

public:
    RTIHighContrastCallback(InkCanvas& inkCanvas)
        : _thisInkCanvas(inkCanvas)
    {
        //_thisInkCanvas = inkCanvas;
    }

    //RTIHighContrastCallback() { }


    //------------------------------------------------------
    //
    //  Internal Methods
    //
    //------------------------------------------------------


    /// <summary>
    /// TurnHighContrastOn
    /// </summary>
    /// <param name="highContrastColor"></param>
    void TurnHighContrastOn(QColor highContrastColor)
    {
        // The static strokes already have been taken care of by GetInkPresenter().
        // We only update the RTI renderer here.
        QSharedPointer<DrawingAttributes> highContrastDa = _thisInkCanvas.DefaultDrawingAttributes()->Clone();
        highContrastDa->SetColor(highContrastColor);
        _thisInkCanvas.UpdateDynamicRenderer(highContrastDa);
    }

    /// <summary>
    /// TurnHighContrastOff
    /// </summary>
    void TurnHighContrastOff()
    {
        // The static strokes already have been taken care of by GetInkPresenter().
        // We only update the RTI renderer here.
        _thisInkCanvas.UpdateDynamicRenderer(_thisInkCanvas.DefaultDrawingAttributes());
    }


    //------------------------------------------------------
    //
    //  Internal Properties
    //
    //------------------------------------------------------


    /// <summary>
    /// Returns the dispatcher if the object is associated to a UIContext.
    /// </summary>
    virtual Dispatcher* GetDispatcher()
    {
        return _thisInkCanvas.GetDispatcher();
    }


    //------------------------------------------------------
    //
    //  Private Fields
    //
    //------------------------------------------------------


private:
    InkCanvas& _thisInkCanvas;

};

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
        if ( activeMode != InkCanvasEditingMode::None )
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

