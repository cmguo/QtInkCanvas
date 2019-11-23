#include "inkcanvas.h"
#include "styluspointdescription.h"
#include "drawingattributes.h"
#include "dynamicrenderer.h"
#include "editingcoordinator.h"
#include "stylusshape.h"

/// <summary>
/// Public constructor.
/// </summary>
InkCanvas::InkCanvas(QWidget * parent)
    : QWidget(parent)
{
    Initialize();
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
    _dynamicRenderer->Enabled = false;
    //StylusPlugIns().Add(_dynamicRenderer);

    //
    // create and initialize an editing coordinator
    //
    _editingCoordinator = new EditingCoordinator(this);
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



/// <summary>
/// HitTestCore implements precise hit testing against render contents
/// </summary>
HitTestResult InkCanvas::HitTestCore(PointHitTestParameters hitTestParams)
{
    VerifyAccess();

    QRectF r = new Rect(new Point(), RenderSize);
    if (r.Contains(hitTestParams.HitPoint))
    {
        return new PointHitTestResult(this, hitTestParams.HitPoint);
    }

    return nullptr;
}


/// <summary>
///     The DependencyProperty for the Strokes property.
/// </summary>
//public static readonly DependencyProperty StrokesProperty =
//        InkPresenter().StrokesProperty.AddOwner(
//                typeof(InkCanvas),
//                new FrameworkPropertyMetadata(
//                        new StrokeCollectionDefaultValueFactory(),
//                        new PropertyChangedCallback(OnStrokesChanged)));


void InkCanvas::OnStrokesChanged(DependencyObject& d, DependencyPropertyChangedEventArgs& e)
{
    InkCanvas& inkCanvas = static_cast<InkCanvas&>(d);
    QSharedPointer<StrokeCollection> oldValue = e.OldValue;
    QSharedPointer<StrokeCollection> newValue = e.NewValue;

    //
    // only change the prop if it's a different object.  We don't
    // want to be doing this for no reason
    //
    if ( oldValue != newValue )
    {
        // Clear the selected strokes without raising event.
        inkCanvas.CoreChangeSelection(new StrokeCollection(), inkCanvas.InkCanvasSelection.SelectedElements, false);

        inkCanvas.InitializeInkObject();

        InkCanvasStrokesReplacedEventArgs& args =
            new InkCanvasStrokesReplacedEventArgs(newValue, oldValue); //new, previous

        //raise the StrokesChanged event through our protected virtual
        inkCanvas.OnStrokesReplaced(args);
    }

}

/// <summary>
/// Returns the SelectionAdorner
/// </summary>
InkCanvasSelectionAdorner InkCanvas::SelectionAdorner()
{
    // We have to create our visual at this point.
    if ( _selectionAdorner == nullptr )
    {
        // Create the selection Adorner.
        _selectionAdorner = new InkCanvasSelectionAdorner(InnerCanvas);

        // Bind the InkCanvas.ActiveEditingModeProperty
        // to SelectionAdorner.VisibilityProperty.
        Binding activeEditingModeBinding = new Binding();
        activeEditingModeBinding.Path = new PropertyPath(InkCanvas.ActiveEditingModeProperty);
        activeEditingModeBinding.Mode = BindingMode.OneWay;
        activeEditingModeBinding.Source = this;
        activeEditingModeBinding.Converter = new ActiveEditingMode2VisibilityConverter();
        _selectionAdorner.SetBinding(QWidget*.VisibilityProperty, activeEditingModeBinding);
    }

    return _selectionAdorner;
}

/// <summary>
/// Returns the FeedbackAdorner
/// </summary>
InkCanvasFeedbackAdorner* InkCanvas::FeedbackAdorner()
{
    VerifyAccess();

    if ( _feedbackAdorner == nullptr )
    {
        _feedbackAdorner = new InkCanvasFeedbackAdorner(this);
    }

    return _feedbackAdorner;
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
//                    { return value != nullptr; });


void InkCanvas::OnDefaultDrawingAttributesChanged(DependencyObject& d, DependencyPropertyChangedEventArgs& e)
{
    InkCanvas& inkCanvas = static_cast<InkCanvas&>(d);

    QSharedPointer<DrawingAttributes> oldValue = e.OldValue;
    QSharedPointer<DrawingAttributes> newValue = e.NewValue;

    // This can throw, so call it first
    inkCanvas.UpdateDynamicRenderer(newValue);

    // We only fire Changed event when there is an instance change.
    if ( oldValue != newValue )
    {
        //we didn't throw, change our backing value
        //oldValue.AttributeChanged -= new PropertyDataChangedEventHandler(inkCanvas.DefaultDrawingAttributes_Changed);
        QObject::disconnect(oldValue.get(), &DrawingAttributes::AttributeChanged, &inkCanvas, &InkCanvas::DefaultDrawingAttributes_Changed);
        DrawingAttributesReplacedEventArgs& args =
            new DrawingAttributesReplacedEventArgs(newValue, oldValue);

        newValue.AttributeChanged += new PropertyDataChangedEventHandler(inkCanvas.DefaultDrawingAttributes_Changed);
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
        _eraserShape = new RectangleStylusShape(8, 8);
    }
    return _eraserShape;

}
void InkCanvas::SetEraserShape(StylusShape * value)
{
    VerifyAccess();
    if (value == nullptr)
    {
        throw new std::exception("value");
    }
    else
    {
        // Invoke getter since this property is lazily created.
        StylusShape* oldShape = EraserShape();

        _eraserShape = value;


        if ( oldShape->Width() != _eraserShape->Width() || oldShape->Height() != _eraserShape->Height()
            || oldShape->Rotation() != _eraserShape->Rotation() || oldShape->GetType() != _eraserShape->GetType())
        {
            GetEditingCoordinator()->UpdatePointEraserCursor();
        }
    }
}


void InkCanvas::OnEditingModeChanged(DependencyObject& d, DependencyPropertyChangedEventArgs& e)
{
    ( (InkCanvas&)d ).RaiseEditingModeChanged(
                        new RoutedEventArgs(InkCanvas.EditingModeChangedEvent, d));
}

void InkCanvas::OnEditingModeInvertedChanged(DependencyObject& d, DependencyPropertyChangedEventArgs& e)
{
    ( (InkCanvas)d ).RaiseEditingModeInvertedChanged(
        new RoutedEventArgs(InkCanvas.EditingModeInvertedChangedEvent, d));
}

bool InkCanvas::ValidateEditingMode(QVariant value)
{
    return EditingModeHelper.IsDefined((InkCanvasEditingMode)value);
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
/// Read/Write the enabled ClipboardFormats
/// </summary>
//[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
QList<InkCanvasClipboardFormat> InkCanvas::PreferredPasteFormats()
{
    VerifyAccess();

    return ClipboardProcessor()->PreferredFormats;
}
void InkCanvas::SetPreferredPasteFormats(QList<InkCanvasClipboardFormat> value)
{
    VerifyAccess();

    // Cannot be null
    //if ( value == nullptr )
    //{
        // Null is not allowed as the argument value
    //    throw new std::exception("value");
    //}

    ClipboardProcessor()->PreferredFormats = value;
}


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
void InkCanvas::OnStrokeCollected(InkCanvasStrokeCollectedEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
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
void RaiseGestureOrStrokeCollected(InkCanvasStrokeCollectedEventArgs& e, bool userInitiated)
{
    //Debug.Assert(e != null, "EventArg can not be null");
    bool addStrokeToInkCanvas = true; // Initialize our flag.

    // The follow code raises Gesture event
    // The out-side code could throw exception in the their handlers. We use try/finally block to protect our status.
    try
    {
        //
        // perform gesture reco before raising this event
        // if we're in the right mode
        //
        //IMPORTANT: only call gesture recognition if userInitiated.  See SecurityNote.
        if (userInitiated)
        {
            if ((ActiveEditingMode == InkCanvasEditingMode::InkAndGesture ||
                  ActiveEditingMode == InkCanvasEditingMode::GestureOnly) &&
                  GestureRecognizer.IsRecognizerAvailable)
            {
                QSharedPointer<StrokeCollection> strokes = new StrokeCollection();
                strokes.Add(e.Stroke);

                //
                // GestureRecognizer.Recognize demands unmanaged code, we assert it here
                // as this codepath is only called in response to user input
                //
                ReadOnlyCollection<GestureRecognitionResult> results =
                    GestureRecognizer.CriticalRecognize(strokes);

                if (results.Count > 0)
                {
                    InkCanvasGestureEventArgs& args =
                        new InkCanvasGestureEventArgs(strokes, results);

                    if (results[0].ApplicationGesture == ApplicationGesture.NoGesture)
                    {
                        //
                        // we set Cancel=true if we didn't detect a gesture
                        //
                        args.Cancel = true;
                    }
                    else
                    {
                        args.Cancel = false;
                    }

                    OnGesture(args);

                    //
                    // now that we've raised the Gesture event and the developer
                    // has had a chance to change args.Cancel, see what their intent is.
                    //
                    if (args.Cancel == false)
                    {
                        //bail out and don't add
                        //the stroke to InkCanvas.Strokes
                        addStrokeToInkCanvas = false; // Reset the flag.
                        return;
                    }
                }
            }
        }

        // Reset the flag.
        addStrokeToInkCanvas = false;

        //
        // only raise StrokeCollected if we're in InkCanvasEditingMode::Ink or InkCanvasEditingMode::InkAndGesture
        //
        if ( ActiveEditingMode == InkCanvasEditingMode::Ink ||
            ActiveEditingMode == InkCanvasEditingMode::InkAndGesture )
        {
            //add the stroke to the StrokeCollection and raise this event
            Strokes.Add(e.Stroke);
            OnStrokeCollected(e);
        }
    }
    finally
    {
        // If the gesture events are failed, we should still add Stroke to the InkCanvas so that the data won't be lost.
        if ( addStrokeToInkCanvas )
        {
            Strokes.Add(e.Stroke);
        }
    }
}

/// <summary>
///     The Gesture Routed Event
/// </summary>
//public static readonly RoutedEvent GestureEvent =
 //   EventManager.RegisterRoutedEvent("Gesture", RoutingStrategy.Bubble, typeof(InkCanvasGestureEventHandler), typeof(InkCanvas));

/// <summary>
///     Add / Remove Gesture handler
/// </summary>
[Category("Behavior")]
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
void InkCanvas::OnGesture(InkCanvasGestureEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
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
void InkCanvas::OnStrokesReplaced(InkCanvasStrokesReplacedEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
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
void DrawingAttributesReplacedEventHandler(DrawingAttributesReplacedEventArgs& e);

protected:
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
void RaiseDefaultDrawingAttributeReplaced(DrawingAttributesReplacedEventArgs& e)
{
    OnDefaultDrawingAttributesReplaced(e);

    // Invalidate the inking cursor
    _editingCoordinator->InvalidateBehaviorCursor(_editingCoordinator->InkCollectionBehavior);
}

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
void InkCanvas::OnActiveEditingModeChanged(RoutedEventArgs& e)
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
void RaiseActiveEditingModeChanged(RoutedEventArgs& e)
{
    //Debug.Assert(e != null, "EventArg can not be null");

    InkCanvasEditingMode mode = ActiveEditingMode;
    if (mode != _editingCoordinator->ActiveEditingMode)
    {
        //change our DP, then raise the event via our protected override
        SetValue(ActiveEditingModePropertyKey, _editingCoordinator->ActiveEditingMode);

        OnActiveEditingModeChanged(e);
    }
}



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
void InkCanvas::OnEditingModeChanged(RoutedEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
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
void RaiseEditingModeChanged(RoutedEventArgs& e)
{
    //Debug.Assert(e != null, "EventArg can not be null");

    _editingCoordinator->UpdateEditingState(false /* EditingMode */);

    OnEditingModeChanged(e);
}

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
void InkCanvas::OnEditingModeInvertedChanged(RoutedEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
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
void RaiseEditingModeInvertedChanged(RoutedEventArgs& e)
{
    //Debug.Assert(e != null, "EventArg can not be null");

    _editingCoordinator->UpdateEditingState(true /* EditingModeInverted */);

    OnEditingModeInvertedChanged(e);
}

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
void InkCanvas::OnSelectionMoving( InkCanvasSelectionEditingEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
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
void InkCanvas::OnSelectionMoved(EventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
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
void RaiseSelectionMoved(EventArgs& e)
{
    //Debug.Assert(e != null, "EventArg can not be null");

    OnSelectionMoved(e);
    // Update the cursor of SelectionEditor behavior.
    GetEditingCoordinator()->SelectionEditor.OnInkCanvasSelectionChanged();
}

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
void InkCanvas::OnStrokeErasing(InkCanvasStrokeErasingEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
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
void InkCanvas::OnStrokeErased(RoutedEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    if ( e == nullptr )
    {
        throw new std::exception("e");
    }
    RaiseEvent(e);
}

/// <summary>
/// Allows the EditingBehaviors to raise the InkErasing event via the protected virtual
/// </summary>
void RaiseInkErased()
{
    OnStrokeErased(
        new RoutedEventArgs(InkCanvas.StrokeErasedEvent, this));
}

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
void InkCanvas::OnSelectionResizing( InkCanvasSelectionEditingEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
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
void InkCanvas::OnSelectionResized(EventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
    //{
    //    throw new std::exception("e");
    //}
    if (nullptr != SelectionResized)
    {
        SelectionResized(e);
    }
}

/// <summary>
/// Allows the EditingBehaviors to raise the SelectionResized event via the protected virtual
/// </summary>
/// <param name="e">EventArgs& to raise the event with</param>
void RaiseSelectionResized(EventArgs& e)
{
    //Debug.Assert(e != null, "EventArg can not be null");

    OnSelectionResized(e);
    // Update the cursor of SelectionEditor behavior.
    GetEditingCoordinator()->SelectionEditor.OnInkCanvasSelectionChanged();
}

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
void InkCanvas::OnSelectionChanging(InkCanvasSelectionChangingEventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
    //{
    //    throw new std::exception("e");
    //}
    if (nullptr != SelectionChanging)
    {
        SelectionChanging(e);
    }
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
void InkCanvas::OnSelectionChanged(EventArgs& e)
{
    // No need to invoke VerifyAccess since this method is thread free.

    //if ( e == nullptr )
    //{
    //    throw new std::exception("e");
    //}
    if (nullptr != SelectionChanged)
    {
        SelectionChanged(e);
    }
}

/// <summary>
/// Allows the EditingBehaviors to raise the SelectionChanged event via the protected virtual
/// </summary>
/// <param name="e">EventArgs& to raise the event with</param>
void RaiseSelectionChanged(EventArgs& e)
{
    //Debug.Assert(e != null, "EventArg can not be null");

    OnSelectionChanged(e);
    // Update the cursor of SelectionEditor behavior.
    GetEditingCoordinator()->SelectionEditor.OnInkCanvasSelectionChanged();
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


public:
/// <summary>
/// Returns the enabled gestures.  This method throws an exception if GestureRecognizerAvailable
/// is false
/// </summary>
/// <returns></returns>
ReadOnlyCollection<ApplicationGesture> GetEnabledGestures()
{
    // No need to invoke VerifyAccess since it's checked in GestureRecognizer.GetEnabledGestures.

    //gestureRecognizer throws appropriately if there is no gesture recognizer available
    return new ReadOnlyCollection<ApplicationGesture>(GestureRecognizer.GetEnabledGestures());
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
    GestureRecognizer.SetEnabledGestures(applicationGestures);
}

/// <summary>
/// Get the selection bounds.
/// </summary>
/// <returns></returns>
QRectF GetSelectionBounds()
{
    VerifyAccess();

    return InkCanvasSelection.SelectionBounds;
}

/// <summary>
/// provides access to the currently selected elements which are children of this InkCanvas
/// </summary>
//ReadOnlyCollection<QWidget*> GetSelectedElements()
//{
//    VerifyAccess();
//    return InkCanvasSelection.SelectedElements;
//}

/// <summary>
/// provides read access to the currently selected strokes
/// </summary>
QSharedPointer<StrokeCollection> GetSelectedStrokes()
{
    VerifyAccess();

    QSharedPointer<StrokeCollection> sc(new StrokeCollection());
    sc->Add(InkCanvasSelection.SelectedStrokes);
    return sc;
}

/// <summary>
/// Overload which calls the more complex version, passing null for selectedElements
/// </summary>
/// <param name="selectedStrokes">The strokes to select</param>
void Select(QSharedPointer<StrokeCollection> selectedStrokes)
{
    // No need to invoke VerifyAccess since this call is forwarded.
    Select(selectedStrokes, nullptr);
}

/// <summary>
/// Overload which calls the more complex version, passing null for selectedStrokes
/// </summary>
/// <param name="selectedElements">The elements to select</param>
void Select(QList<QWidget*> selectedElements)
{
    // No need to invoke VerifyAccess since this call is forwarded.
    Select(nullptr, selectedElements);
}

/// <summary>
/// Overload which calls the more complex version, passing null for selectedStrokes
/// </summary>
/// <param name="selectedStrokes">The strokes to select</param>
/// <param name="selectedElements">The elements to select</param>
void Select(QSharedPointer<StrokeCollection> selectedStrokes, QList<QWidget*> selectedElements)
{
    VerifyAccess();

    // NTRAID-WINDOWS#1134932-2005/12/01-WAYNEZEN
    // Try to switch to Select mode first. If we fail to change the mode, then just simply no-op.
    if ( EnsureActiveEditingMode(InkCanvasEditingMode::Select) )
    {
        //
        // validate
        //
        QWidget*[] validElements = ValidateSelectedElements(selectedElements);
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
InkCanvasSelectionHitResult HitTestSelection(QPointF const &point)
{
    VerifyAccess();

    // Ensure the visual tree.
    if ( _localAdornerDecorator == nullptr )
    {
        ApplyTemplate();
    }

    return InkCanvasSelection.HitTestSelection(point);
}

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
void CutSelection()
{
    VerifyAccess();

    // Copy first
    InkCanvasClipboardDataFormats copiedDataFormats = PrivateCopySelection();

    // Don't even bother if we don't have a selection.
    if ( copiedDataFormats != InkCanvasClipboardDataFormats.None )
    {
        // Then delete the current selection. Note the XAML format won't be avaliable under Partial
        // Trust. So, the selected element shouldn't be copied or removed.
        DeleteCurrentSelection(
            /* We want to delete the selected Strokes if there is ISF and/or XAML data being copied */
            (copiedDataFormats &
                (InkCanvasClipboardDataFormats.ISF | InkCanvasClipboardDataFormats.XAML)) != 0,
            /* We only want to delete the selected elements if there is XAML data being copied */
            (copiedDataFormats & InkCanvasClipboardDataFormats.XAML) != 0);
    }
}

/// <summary>
/// Paste the contents of the clipboard into the InkCanvas
/// </summary>
void Paste()
{
    // No need to call VerifyAccess since this call is forwarded.

    // We always paste the data to the default location which is (0,0).
    Paste(new Point(c_pasteDefaultLocation, c_pasteDefaultLocation));
}

/// <summary>
/// Paste the contents of the clipboard to the specified location in the InkCanvas
/// </summary>
void Paste(QPointF const &point)
{
    VerifyAccess();

    if (DoubleUtil.IsNaN(point.X) ||
        DoubleUtil.IsNaN(point.Y) ||
        Double.IsInfinity(point.X)||
        Double.IsInfinity(point.Y) )
    {
            throw new ArgumentException(SR.Get(SRID.InvalidPoint), "point");
    }


    //
    // only do this if the user is not editing (input active)
    // or we will violate a dispatcher lock
    //
    if (!_editingCoordinator->UserIsEditing)
    {
        IDataObject dataObj = nullptr;
        try
        {
            dataObj = Clipboard.GetDataObject();
        }
        catch (ExternalException)
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
bool CanPaste()
{
    VerifyAccess();

    bool ret = false;
    //
    // can't paste if the user is editing (input active)
    // or we will violate a dispatcher lock
    //
    if (_editingCoordinator->UserIsEditing)
    {
        return false;
    }

    // Check whether the caller has the clipboard permission.
    if ( !SecurityHelper.CallerHasAllClipboardPermission() )
    {
        return false;
    }

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
DynamicRenderer* InkCanvas::GetDynamicRenderer()
{
    VerifyAccess();
    return InternalDynamicRenderer();
}
void InkCanvas::SetDynamicRenderer(DynamicRenderer* value)
{
    VerifyAccess();
    if (!object.ReferenceEquals(value, _dynamicRenderer))
    {
        int previousIndex = -1;
        //remove the existing plugin
        if (_dynamicRenderer* != nullptr)
        {
            //remove the plugin from the collection
            previousIndex = StylusPlugIns().IndexOf(_dynamicRenderer);
            if (-1 != previousIndex)
            {
                StylusPlugIns().RemoveAt(previousIndex);
            }

            //remove the plugin's visual from the InkPresenter
            if (InkPresenter().ContainsAttachedVisual(_dynamicRenderer->RootVisual))
            {
                InkPresenter().DetachVisuals(_dynamicRenderer->RootVisual);
            }
        }

        _dynamicRenderer* = value;

        if (_dynamicRenderer* != nullptr) //null is acceptable
        {
            //remove the plugin from the collection
            if (!StylusPlugIns().Contains(_dynamicRenderer))
            {
                if (-1 != previousIndex)
                {
                    //insert the new DR in the same location as the old one
                    StylusPlugIns().Insert(previousIndex, _dynamicRenderer);
                }
                else
                {
                    StylusPlugIns().Add(_dynamicRenderer);
                }
            }

            //refer to the same DrawingAttributes as the InkCanvas
            _dynamicRenderer->SetDrawingAttributes(DefaultDrawingAttributes);

            //attach the DynamicRenderer* if it is not already
            if (!(InkPresenter().ContainsAttachedVisual(_dynamicRenderer->RootVisual)) &&
                _dynamicRenderer->Enabled &&
                _dynamicRenderer->RootVisual != nullptr)
            {
                InkPresenter().AttachVisuals(_dynamicRenderer->RootVisual, DefaultDrawingAttributes);
            }
        }
    }
}

/// <summary>
/// Protected read only access to the InkPresenter this InkCanvas uses
/// </summary>
InkPresenter* InkCanvas::GetInkPresenter()
{
    VerifyAccess();
    if ( _inkPresenter == nullptr )
    {
        _inkPresenter = new InkPresenter();

        // Bind the InkPresenter().Strokes to InkCanvas.Strokes
        Binding strokes = new Binding();
        strokes.Path = new PropertyPath(InkCanvas.StrokesProperty);
        strokes.Mode = BindingMode.OneWay;
        strokes.Source = this;
        _InkPresenter().SetBinding(InkPresenter().StrokesProperty, strokes);

    }
    return _inkPresenter;
}


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
    ( new UIPermission(UIPermissionClipboard.AllClipboard) ).Assert();//BlessedAssert
    try
    {
        return PrivateCanPaste();
    }
    finally
    {
        UIPermission.RevertAssert();
    }

}

/// <summary>
/// PrivateCanPaste
/// </summary>
/// <returns></returns>
bool InkCanvas::PrivateCanPaste()
{
    bool canPaste = false;
    IDataObject dataObj = nullptr;
    try
    {
        dataObj = Clipboard.GetDataObject();
    }
    catch (ExternalException)
    {
        //harden against ExternalException
        return false;
    }
    if ( dataObj != nullptr )
    {
        canPaste = ClipboardProcessor()->CheckDataFormats(dataObj);
    }

    return canPaste;
}

/// <summary>
/// This method pastes data from an IDataObject object
/// </summary>
void InkCanvas::PasteFromDataObject(IDataObject dataObj, Point point)
{
    // Reset the current selection
    ClearSelection(false);

    // Assume that there is nothing to be selected.
    QSharedPointer<StrokeCollection> newStrokes = new StrokeCollection();
    List<QWidget*> newElements = new List<QWidget*>();

    // Paste the data from the data object.
    if ( !ClipboardProcessor()->PasteData(dataObj, ref newStrokes, ref newElements) )
    {
        // Paste was failed.
        return;
    }
    else if ( newStrokes.Count == 0 && newElements.Count == 0 )
    {
        // Nothing has been received from the clipboard.
        return;
    }

    // We add elements here. Then we have to wait for the layout update.
    QWidget*Collection children = Children;
    foreach ( QWidget* element in newElements )
    {
        children.Add(element);
    }

    if ( newStrokes != nullptr )
    {
        Strokes.Add(newStrokes);
    }

    try
    {
        // We should fire SelectionChanged event if the current editing mode is Select.
        CoreChangeSelection(newStrokes, newElements.ToArray(), EditingMode == InkCanvasEditingMode::Select);
    }
    finally
    {
        // Now move the selection to the desired location.
        QRectF bounds = GetSelectionBounds( );
        InkCanvasSelection.CommitChanges(Rect.Offset(bounds, -bounds.Left + point.X, -bounds.Top + point.Y), false);

        if (EditingMode != InkCanvasEditingMode::Select)
        {
            // Clear the selection without the event if the editing mode is not Select.
            ClearSelection(false);
        }
    }
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
     DataObject dataObj;
    (new UIPermission(UIPermissionClipboard.AllClipboard)).Assert();//BlessedAssert
    try
    {
        dataObj = new DataObject();
    }
    finally
    {
        UIPermission.RevertAssert();
    }
    InkCanvasClipboardDataFormats copiedDataFormats = InkCanvasClipboardDataFormats.None;

    // Try to copy the data from the InkCanvas to the clipboard.
    copiedDataFormats = ClipboardProcessor()->CopySelectedData(dataObj);

    if ( copiedDataFormats != InkCanvasClipboardDataFormats.None )
    {
        PermissionSet ps = new PermissionSet(PermissionState.None);
        ps.AddPermission(new SecurityPermission(SecurityPermissionFlag.SerializationFormatter));
        ps.AddPermission(new UIPermission(UIPermissionClipboard.AllClipboard));
        ps.Assert(); // BlessedAssert
        try
        {
            // Put our data object into the clipboard.
            Clipboard.SetDataObject(dataObj, true);
        }
        finally
        {
            SecurityPermission.RevertAssert();
        }
    }

    return copiedDataFormats;
}


/// <summary>
/// Return the inner Canvas.
/// </summary>
InkCanvasInnerCanvas* InkCanvas::InnerCanvas()
{
        // We have to create our visual at this point.
    if (_innerCanvas == nullptr)
    {
        // Create our InnerCanvas to change the logical parent of Canvas' children.
        _innerCanvas = new InkCanvasInnerCanvas(this);

        // Bind the inner Canvas' Background to InkCanvas' Background
        Binding background = new Binding();
        background.Path = new PropertyPath(InkCanvas.BackgroundProperty);
        background.Mode = BindingMode.OneWay;
        background.Source = this;
        _innerCanvas.SetBinding(Panel.BackgroundProperty, background);
    }

    return _innerCanvas;
}

/// <summary>
/// Internal access to the current selection
/// </summary>
InkCanvasSelection InkCanvas::GetInkCanvasSelection()
{
    if ( _selection == nullptr )
    {
        _selection = new InkCanvasSelection(this);
    }

    return _selection;
}


/// <summary>
/// Internal helper called by the LassoSelectionBehavior
/// </summary>
void InkCanvas::BeginDynamicSelection(Visual visual)
{
    GetEditingCoordinator()->DebugCheckActiveBehavior(GetEditingCoordinator()->LassoSelectionBehavior);

    _dynamicallySelectedStrokes = new StrokeCollection();

    InkPresenter().AttachVisuals(visual, new DrawingAttributes());
}

/// <summary>
/// Internal helper called by LassoSelectionBehavior to update the display
/// of dynamically added strokes
/// </summary>
void InkCanvas::UpdateDynamicSelection(   QSharedPointer<StrokeCollection> strokesToDynamicallySelect,
                                        QSharedPointer<StrokeCollection> strokesToDynamicallyUnselect)
{
    GetEditingCoordinator()->DebugCheckActiveBehavior(GetEditingCoordinator()->LassoSelectionBehavior);

    //
    // update our internal stroke collections used by dynamic selection
    //
    if (strokesToDynamicallySelect != nullptr)
    {
        foreach (Stroke s in strokesToDynamicallySelect)
        {
            _dynamicallySelectedStrokes.Add(s);
            s.IsSelected = true;
        }
    }

    if (strokesToDynamicallyUnselect != nullptr)
    {
        foreach (Stroke s in strokesToDynamicallyUnselect)
        {
            System.Diagnostics.//Debug.Assert(_dynamicallySelectedStrokes.Contains(s));
            _dynamicallySelectedStrokes.Remove(s);
            s.IsSelected = false;
        }
    }
}

/// <summary>
/// Internal helper used by LassoSelectionBehavior
/// </summary>
QSharedPointer<StrokeCollection> InkCanvas::EndDynamicSelection(Visual visual)
{
    GetEditingCoordinator()->DebugCheckActiveBehavior(GetEditingCoordinator()->LassoSelectionBehavior);

    InkPresenter().DetachVisuals(visual);

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
    if ( !InkCanvasSelection.HasSelection )
    {
        return true;
    }

    //
    // attempt to clear selection
    //
    ChangeInkCanvasSelection(new StrokeCollection(), new QWidget*[]{});

    return !InkCanvasSelection.HasSelection;
}

/// <summary>
/// ClearSelection
///     Called by:
///         PasteFromDataObject
///         EditingCoordinator.UpdateEditingState
/// </summary>
void InkCanvas::ClearSelection(bool raiseSelectionChangedEvent)
{
    if ( InkCanvasSelection.HasSelection )
    {
        // Reset the current selection
        CoreChangeSelection(new StrokeCollection(), new QWidget*[] { }, raiseSelectionChangedEvent);
    }
}


/// <summary>
/// Helper that creates selection for an InkCanvas.  Used by the SelectedStrokes and
/// SelectedElements properties
/// </summary>
void InkCanvas::ChangeInkCanvasSelection(QSharedPointer<StrokeCollection> strokes, QWidget*[] elements)
{
    //validate in debug only for this internal static
    //Debug.Assert(strokes != null
                && elements != null,
                "Invalid arguments in ChangeInkCanvasSelection");

    bool strokesAreDifferent;
    bool elementsAreDifferent;
    InkCanvasSelection.SelectionIsDifferentThanCurrent(strokes, out strokesAreDifferent, elements, out elementsAreDifferent);
    if ( strokesAreDifferent || elementsAreDifferent )
    {
        InkCanvasSelectionChangingEventArgs& args = new InkCanvasSelectionChangingEventArgs(strokes, elements);

        QSharedPointer<StrokeCollection> validStrokes = strokes;
        QWidget*[] validElements = elements;

        RaiseSelectionChanging(args);

        //now that the event has been raised and all of the delegates
        //have had their way with it, process the result
        if ( !args.Cancel )
        {

            //
            // rock and roll, time to validate our arguments
            // note: these event args are visible outside the apis,
            // so we need to validate them again
            //

            // PERF-2006/05/02-WAYNEZEN,
            // Check our internal flag. If the SelectedStrokes has been changed, we shouldn't do any extra work here.
            if ( args.StrokesChanged )
            {
                validStrokes = ValidateSelectedStrokes(args.GetSelectedStrokes());
                int countOldSelectedStrokes = strokes.Count;
                for ( int i = 0; i < countOldSelectedStrokes; i++ )
                {
                    // PERF-2006/05/02-WAYNEZEN,
                    // We only have to reset IsSelected for the elements no longer exists in the new collection.
                    if ( !validStrokes.Contains(strokes[i]) )
                    {
                        // NTRAID#WINDOWS-1045099-2006/05/02-waynezen,
                        // Make sure we reset the IsSelected property which could have been
                        // set to true in the dynamic selection.
                        strokes[i].IsSelected = false;
                    }
                }
            }


            // PERF-2006/05/02-WAYNEZEN,
            // Check our internal flag. If the SelectedElements has been changed, we shouldn't do any extra work here.
            if ( args.ElementsChanged )
            {
                validElements = ValidateSelectedElements(args.GetSelectedElements());
            }

            CoreChangeSelection(validStrokes, validElements, true);
        }
        else
        {
            QSharedPointer<StrokeCollection> currentSelectedStrokes = InkCanvasSelection.SelectedStrokes;
            int countOldSelectedStrokes = strokes.Count;
            for ( int i = 0; i < countOldSelectedStrokes; i++ )
            {
                // Make sure we reset the IsSelected property which could have been
                // set to true in the dynamic selection but not being selected previously.
                if ( !currentSelectedStrokes.Contains(strokes[i]) )
                {
                    strokes[i].IsSelected = false;
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
void InkCanvas::CoreChangeSelection(QSharedPointer<StrokeCollection> validStrokes, IList<QWidget*> validElements, bool raiseSelectionChanged)
{
    InkCanvasSelection.Select(validStrokes, validElements, raiseSelectionChanged);
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
static QSharedPointer<StrokeCollection> InkCanvas::GetValidStrokes(QSharedPointer<StrokeCollection> subset, QSharedPointer<StrokeCollection> superset)
{
    QSharedPointer<StrokeCollection> validStrokes = new StrokeCollection();

    int subsetCount = subset.Count;

    // special case an empty subset as a guaranteed subset
    if ( subsetCount == 0 )
    {
        return validStrokes;
    }

    for ( int i = 0; i < subsetCount; i++ )
    {
        Stroke stroke = subset[i];
        if ( superset.Contains(stroke) )
        {
            validStrokes.Add(stroke);
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
    Type ownerType = typeof(InkCanvas);

    CommandHelpers.RegisterCommandHandler(ownerType, ApplicationCommands.Cut,
        new ExecutedRoutedEventHandler(_OnCommandExecuted), new CanExecuteRoutedEventHandler(_OnQueryCommandEnabled),
        SRID.KeyShiftDelete, SRID.KeyShiftDeleteDisplayString);
    CommandHelpers.RegisterCommandHandler(ownerType, ApplicationCommands.Copy,
        new ExecutedRoutedEventHandler(_OnCommandExecuted), new CanExecuteRoutedEventHandler(_OnQueryCommandEnabled),
        SRID.KeyCtrlInsert, SRID.KeyCtrlInsertDisplayString);

    // Use temp variables to reduce code under elevation
    ExecutedRoutedEventHandler pasteExecuteEventHandler = new ExecutedRoutedEventHandler(_OnCommandExecuted);
    CanExecuteRoutedEventHandler pasteQueryEnabledEventHandler = new CanExecuteRoutedEventHandler(_OnQueryCommandEnabled);
    InputGesture pasteInputGesture = KeyGesture.CreateFromResourceStrings(SR.Get(SRID.KeyShiftInsert), SR.Get(SRID.KeyShiftInsertDisplayString));

    new UIPermission(UIPermissionClipboard.AllClipboard).Assert(); // BlessedAssert:
    try
    {
        CommandHelpers.RegisterCommandHandler(ownerType, ApplicationCommands.Paste,
            pasteExecuteEventHandler, pasteQueryEnabledEventHandler, pasteInputGesture);
    }
    finally
    {
        CodeAccessPermission.RevertAssert();
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
        return new StrokeCollection();
    }
    else
    {
        return GetValidStrokes(strokes, Strokes);
    }
}

/// <summary>
/// Private helper used to ensure that a QWidget* argument passed in
/// is valid.
/// </summary>
QWidget*[] InkCanvas::ValidateSelectedElements(QList<QWidget*> selectedElements)
{
    if (selectedElements == nullptr)
    {
        return new QWidget*[]{};
    }

    List<QWidget*> elements = new List<QWidget*>();
    foreach (QWidget* element in selectedElements)
    {
        // NTRAID:WINDOWSOS#1621480-2006/04/26-WAYNEZEN,
        // Don't add the duplicated element.
        if ( !elements.Contains(element) )
        {
            //
            // check the common case first, e
            //
            if ( InkCanvasIsAncestorOf(element) )
            {
                elements.Add(element);
            }
        }
    }

    return elements.ToArray();
}

/// <summary>
/// Helper method used by DesignActivation to see if an element
/// has this InkCanvas as an Ancestor
/// </summary>
bool InkCanvas::InkCanvasIsAncestorOf(QWidget* element)
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
void InkCanvas::DefaultDrawingAttributes_Changed(object sender, PropertyDataChangedEventArgs& args)
{
    // note that sender should be the same as _defaultDrawingAttributes
    // If a developer writes code to change the DefaultDrawingAttributes inside of the event
    //      handler before the InkCanvas receives the notification (multi-cast delegate scenario) -
    //      The DefaultDrawingAttributes should still be updated, and in that case we would
    //      update the RTI DAC twice. Typically, however, this will just refresh the
    //      attributes in the RTI thread.
    System.Diagnostics.//Debug.Assert(object.ReferenceEquals(sender, DefaultDrawingAttributes));

    InvalidateSubProperty(DefaultDrawingAttributesProperty);

    // Be sure to update the RealTimeInking PlugIn with the drawing attribute changes.
    UpdateDynamicRenderer();

    // Invalidate the inking cursor
    _editingCoordinator->InvalidateBehaviorCursor(_editingCoordinator->InkCollectionBehavior);
}

/// <summary>
/// Helper method used to set up the DynamicRenderer.
/// </summary>
void InkCanvas::UpdateDynamicRenderer()
{
    UpdateDynamicRenderer(DefaultDrawingAttributes);
}
/// <summary>
/// Helper method used to set up the DynamicRenderer.
/// </summary>
void InkCanvas::UpdateDynamicRenderer(QSharedPointer<DrawingAttributes> newDrawingAttributes)
{
    ApplyTemplate();

    if (DynamicRenderer* != nullptr)
    {
        DynamicRenderer.DrawingAttributes = newDrawingAttributes;

        if (!InkPresenter().AttachedVisualIsPositionedCorrectly(DynamicRenderer.RootVisual, newDrawingAttributes))
        {
            if (InkPresenter().ContainsAttachedVisual(DynamicRenderer.RootVisual))
            {
                InkPresenter().DetachVisuals(DynamicRenderer.RootVisual);
            }

            // Only hook up if we are enabled.  As we change editing modes this routine will be called
            // to clean up things.
            if (DynamicRenderer.Enabled && DynamicRenderer.RootVisual != nullptr)
            {
                InkPresenter().AttachVisuals(DynamicRenderer.RootVisual, newDrawingAttributes);
            }
        }
    }
}

bool InkCanvas::EnsureActiveEditingMode(InkCanvasEditingMode newEditingMode)
{
    bool ret = true;

    if ( ActiveEditingMode != newEditingMode )
    {
        if ( GetEditingCoordinator()->IsStylusInverted )
        {
            EditingModeInverted = newEditingMode;
        }
        else
        {
            EditingMode = newEditingMode;
        }

        // Verify whether user has cancelled the change in EditingModeChanging event.
        ret = ( ActiveEditingMode == newEditingMode );
    }

    return ret;
}

// The ClipboardProcessor instance which deals with the operations relevant to the clipboard.
ClipboardProcessor* InkCanvas::GetClipboardProcessor()
{
    if ( _clipboardProcessor == nullptr )
    {
        _clipboardProcessor = new ClipboardProcessor(this);
    }

    return _clipboardProcessor;
}

//lazy instance the gesture recognizer
GestureRecognizer* InkCanvas::GetGestureRecognizer()
{
    if (_gestureRecognizer == nullptr)
    {
        _gestureRecognizer = new GestureRecognizer();
    }
    return _gestureRecognizer;
}

/// <summary>
/// Delete the current selection
/// </summary>
void InkCanvas::DeleteCurrentSelection(bool removeSelectedStrokes, bool removeSelectedElements)
{
    ////Debug.Assert(removeSelectedStrokes || removeSelectedElements, "At least either Strokes or Elements should be removed!");

    // Now delete the current selection.
    QSharedPointer<StrokeCollection> strokes = GetSelectedStrokes();
    IList<QWidget*> elements = GetSelectedElements();

    // Clear the selection first.
    CoreChangeSelection(
        removeSelectedStrokes ? new StrokeCollection() : strokes,
        removeSelectedElements ? new List<QWidget*>() : elements,
        true);

    // Remove the ink.
    if ( removeSelectedStrokes && strokes != null && strokes.Count != 0 )
    {
        Strokes.Remove(strokes);
    }

    // Remove the elements.
    if ( removeSelectedElements )
    {
        QWidget*Collection children = Children;
        foreach ( QWidget* element in elements )
        {
            children.Remove(element);
        }
    }
}

/// <summary>
/// A class handler of the Commands
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
/*
void InkCanvas::_OnCommandExecuted(object sender, ExecutedRoutedEventArgs& args)
{
    ICommand command = args.Command;
    InkCanvas inkCanvas = sender as InkCanvas;

    //Debug.Assert(inkCanvas != nullptr);

    if ( inkCanvas.IsEnabled && !inkCanvas.GetEditingCoordinator()->UserIsEditing )
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
            if ( inkCanvas.ActiveEditingMode == InkCanvasEditingMode::Select )
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
void InkCanvas::_OnQueryCommandEnabled(object sender, CanExecuteRoutedEventArgs& args)
{
    RoutedCommand command = (RoutedCommand)(args.Command);
    InkCanvas inkCanvas = sender as InkCanvas;

    //Debug.Assert(inkCanvas != nullptr);

    if ( inkCanvas.IsEnabled
        // NTRAID-WINDOWSOS#1578484-2006/04/14-WAYNEZEN,
        // If user is editing, we should disable all commands.
        && !inkCanvas.GetEditingCoordinator()->UserIsEditing )
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
            args.CanExecute = ( inkCanvas.ActiveEditingMode == InkCanvasEditingMode::Select
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

InkCanvasClipboardDataFormats InkCanvas::PrivateCopySelection()
{
    InkCanvasClipboardDataFormats copiedDataFormats = InkCanvasClipboardDataFormats.None;

    // Don't even bother if we don't have a selection or UserIsEditing has been set.
    if ( InkCanvasSelection.HasSelection && !_editingCoordinator->UserIsEditing)
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
void InkCanvas::_OnDeviceDown(object sender, EventArgs& e)
{
    ( (InkCanvas)sender ).GetEditingCoordinator()->OnInkCanvasDeviceDown(sender, e);
}

/// <summary>
/// _OnDeviceUp
/// </summary>
/// <typeparam name="TEventArgs"></typeparam>
/// <param name="sender"></param>
/// <param name="e"></param>
void InkCanvas::_OnDeviceUp(object sender, EventArgs& e)
{
    ((InkCanvas)sender).GetEditingCoordinator()->OnInkCanvasDeviceUp(sender, e);
}

/// <summary>
/// _OnQueryCursor
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void InkCanvas::_OnQueryCursor(object sender, QueryCursorEventArgs& e)
{
    InkCanvas inkCanvas = (InkCanvas)sender;

    if ( inkCanvas.UseCustomCursor )
    {
        // If UseCustomCursor is set, we bail out. Let the base class (FrameworkElement) to do the rest.
        return;
    }

    // We should behave like our base - honor ForceCursor property.
    if ( !e.Handled || inkCanvas.ForceCursor )
    {
        Cursor cursor = inkCanvas.GetEditingCoordinator()->GetActiveBehaviorCursor();

        // If cursor is null, we don't handle the event and leave it as whatever the default is.
        if ( cursor != nullptr )
        {
            e.Cursor = cursor;
            e.Handled = true;
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
    if ( IsMouseOver() )
    {
        Mouse.UpdateCursor();
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
class InkCanvas::RTIHighContrastCallback : HighContrastCallback
{
    //------------------------------------------------------
    //
    //  Cnostructors
    //
    //------------------------------------------------------

public:
    RTIHighContrastCallback(InkCanvas* inkCanvas)
    {
        _thisInkCanvas = inkCanvas;
    }

    RTIHighContrastCallback() { }


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
        // The static strokes already have been taken care of by InkPresenter().
        // We only update the RTI renderer here.
        QSharedPointer<DrawingAttributes> highContrastDa = _thisInkCanvas.DefaultDrawingAttributes.Clone();
        highContrastDa.Color = highContrastColor;
        _thisInkCanvas.UpdateDynamicRenderer(highContrastDa);
    }

    /// <summary>
    /// TurnHighContrastOff
    /// </summary>
    void TurnHighContrastOff()
    {
        // The static strokes already have been taken care of by InkPresenter().
        // We only update the RTI renderer here.
        _thisInkCanvas.UpdateDynamicRenderer(_thisInkCanvas.DefaultDrawingAttributes);
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
        return _thisInkCanvas.Dispatcher;
    }


    //------------------------------------------------------
    //
    //  Private Fields
    //
    //------------------------------------------------------


private:
    InkCanvas* _thisInkCanvas;

};

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

