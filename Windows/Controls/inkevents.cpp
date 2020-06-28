#include "Windows/Controls/inkevents.h"
#include "Windows/Ink/stroke.h"
#include "Windows/Ink/strokecollection.h"
#include "Windows/Controls/inkcanvas.h"

INKCANVAS_BEGIN_NAMESPACE

/// <summary>
/// [TBS]
/// </summary>
InkCanvasStrokeCollectedEventArgs::InkCanvasStrokeCollectedEventArgs(SharedPointer<Stroke> stroke)
    : RoutedEventArgs(InkCanvas::StrokeCollectedEvent)
{
    if (stroke == nullptr)
    {
        throw std::runtime_error("stroke");
    }
    _stroke = stroke;
}




/// <summary>
/// InkCanvasStrokesReplacedEventArgs
/// </summary>
InkCanvasStrokesReplacedEventArgs::InkCanvasStrokesReplacedEventArgs(SharedPointer<StrokeCollection> newStrokes, SharedPointer<StrokeCollection> previousStrokes)
{
    if (newStrokes == nullptr)
    {
        throw std::runtime_error("newStrokes");
    }
    if (previousStrokes == nullptr)
    {
        throw std::runtime_error("previousStrokes");
    }
    _newStrokes = newStrokes;
    _previousStrokes = previousStrokes;
}


/// <summary>
/// Constructor
/// </summary>
InkCanvasSelectionChangingEventArgs::InkCanvasSelectionChangingEventArgs(SharedPointer<StrokeCollection> selectedStrokes, List<UIElement*> selectedElements)
{
    if (selectedStrokes == nullptr)
    {
        throw std::runtime_error("selectedStrokes");
    }
    //if (selectedElements == nullptr)
    //{
    //    throw std::runtime_error("selectedElements");
    //}
    _strokes = selectedStrokes;
    //List<UIElement*> elements =
    //    new List<UIElement*>(selectedElements);
    _elements = selectedElements;

    _strokesChanged = false;
    _elementsChanged = false;
}



/// <summary>
/// Set the selected elements
/// </summary>
/// <param name="selectedElements">The new selected elements</param>
void InkCanvasSelectionChangingEventArgs::SetSelectedElements(List<UIElement*> selectedElements)
{
    //if ( selectedElements == nullptr )
    //{
    //    throw std::runtime_error("selectedElements");
    //}

    //List<UIElement*> elements =
    //    new List<UIElement*>(selectedElements);
    _elements = selectedElements;
    _elementsChanged = true;
}



/// <summary>
/// Set the selected strokes
/// </summary>
/// <param name="selectedStrokes">The new selected strokes</param>
void InkCanvasSelectionChangingEventArgs::SetSelectedStrokes(SharedPointer<StrokeCollection> selectedStrokes)
{
    if ( selectedStrokes == nullptr )
    {
        throw std::runtime_error("selectedStrokes");
    }

    _strokes = selectedStrokes;
    _strokesChanged = true;
}

/// <summary>
/// Get the selected strokes
/// </summary>
/// <returns>The selected strokes</returns>
SharedPointer<StrokeCollection> InkCanvasSelectionChangingEventArgs::GetSelectedStrokes()
{
    //
    // make a copy of out collection.
    //
    SharedPointer<StrokeCollection> sc(new StrokeCollection());
    sc->Add(_strokes);
    return sc;
}


/// <summary>
/// Constructor
/// </summary>
InkCanvasSelectionEditingEventArgs::InkCanvasSelectionEditingEventArgs(Rect const & oldRectangle, Rect const & newRectangle)
{
    _oldRectangle = oldRectangle;
    _newRectangle = newRectangle;
}


/// <summary>
/// Constructor
/// </summary>
InkCanvasStrokeErasingEventArgs::InkCanvasStrokeErasingEventArgs(SharedPointer<Stroke> stroke)
{
    if (stroke == nullptr)
    {
        throw std::runtime_error("stroke");
    }
    _stroke = stroke;
}



/// <summary>
/// TBD
/// </summary>
/// <param name="strokes">strokes</param>
/// <param name="gestureRecognitionResults">gestureRecognitionResults</param>
InkCanvasGestureEventArgs::InkCanvasGestureEventArgs(SharedPointer<StrokeCollection> strokes, List<GestureRecognitionResult> gestureRecognitionResults)
    : RoutedEventArgs(InkCanvas::GestureEvent)
{
    if (strokes == nullptr)
    {
        throw std::runtime_error("strokes");
    }
    if (strokes->Count() < 1)
    {
        throw std::runtime_error("strokes");
    }
    //if (gestureRecognitionResults == nullptr)
    //{
    //    throw std::runtime_error("strokes");
    //}
    //List<GestureRecognitionResult> results =
    //    new List<GestureRecognitionResult>(gestureRecognitionResults);
    if (gestureRecognitionResults.Count() == 0)
    {
        throw std::runtime_error("gestureRecognitionResults");
    }
    _strokes = strokes;
    _gestureRecognitionResults = gestureRecognitionResults;
}

INKCANVAS_END_NAMESPACE
