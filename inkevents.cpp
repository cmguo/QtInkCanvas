#include "inkevents.h"

#include "stroke.h"
#include "strokecollection.h"
#include "inkcanvas.h"

/// <summary>
/// [TBS]
/// </summary>
InkCanvasStrokeCollectedEventArgs::InkCanvasStrokeCollectedEventArgs(QSharedPointer<Stroke> stroke)
    : RoutedEventArgs(InkCanvas::StrokeCollectedEvent)
{
    if (stroke == nullptr)
    {
        throw std::exception("stroke");
    }
    _stroke = stroke;
}




/// <summary>
/// InkCanvasStrokesReplacedEventArgs
/// </summary>
InkCanvasStrokesReplacedEventArgs::InkCanvasStrokesReplacedEventArgs(QSharedPointer<StrokeCollection> newStrokes, QSharedPointer<StrokeCollection> previousStrokes)
{
    if (newStrokes == nullptr)
    {
        throw std::exception("newStrokes");
    }
    if (previousStrokes == nullptr)
    {
        throw std::exception("previousStrokes");
    }
    _newStrokes = newStrokes;
    _previousStrokes = previousStrokes;
}


/// <summary>
/// Constructor
/// </summary>
InkCanvasSelectionChangingEventArgs::InkCanvasSelectionChangingEventArgs(QSharedPointer<StrokeCollection> selectedStrokes, QList<UIElement*> selectedElements)
{
    if (selectedStrokes == nullptr)
    {
        throw std::exception("selectedStrokes");
    }
    //if (selectedElements == nullptr)
    //{
    //    throw std::exception("selectedElements");
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
void InkCanvasSelectionChangingEventArgs::SetSelectedElements(QList<UIElement*> selectedElements)
{
    //if ( selectedElements == nullptr )
    //{
    //    throw std::exception("selectedElements");
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
void InkCanvasSelectionChangingEventArgs::SetSelectedStrokes(QSharedPointer<StrokeCollection> selectedStrokes)
{
    if ( selectedStrokes == nullptr )
    {
        throw std::exception("selectedStrokes");
    }

    _strokes = selectedStrokes;
    _strokesChanged = true;
}

/// <summary>
/// Get the selected strokes
/// </summary>
/// <returns>The selected strokes</returns>
QSharedPointer<StrokeCollection> InkCanvasSelectionChangingEventArgs::GetSelectedStrokes()
{
    //
    // make a copy of out collection.
    //
    QSharedPointer<StrokeCollection> sc(new StrokeCollection());
    sc->Add(_strokes);
    return sc;
}


/// <summary>
/// Constructor
/// </summary>
 InkCanvasSelectionEditingEventArgs::InkCanvasSelectionEditingEventArgs(QRectF const & oldRectangle, QRectF const & newRectangle)
{
    _oldRectangle = oldRectangle;
    _newRectangle = newRectangle;
}


/// <summary>
/// Constructor
/// </summary>
InkCanvasStrokeErasingEventArgs::InkCanvasStrokeErasingEventArgs(QSharedPointer<Stroke> stroke)
{
    if (stroke == nullptr)
    {
        throw std::exception("stroke");
    }
    _stroke = stroke;
}



/// <summary>
/// TBD
/// </summary>
/// <param name="strokes">strokes</param>
/// <param name="gestureRecognitionResults">gestureRecognitionResults</param>
InkCanvasGestureEventArgs::InkCanvasGestureEventArgs(QSharedPointer<StrokeCollection> strokes, QList<GestureRecognitionResult> gestureRecognitionResults)
    : RoutedEventArgs(InkCanvas::GestureEvent)
{
    if (strokes == nullptr)
    {
        throw std::exception("strokes");
    }
    if (strokes->size() < 1)
    {
        throw std::exception("strokes");
    }
    //if (gestureRecognitionResults == nullptr)
    //{
    //    throw std::exception("strokes");
    //}
    //QList<GestureRecognitionResult> results =
    //    new List<GestureRecognitionResult>(gestureRecognitionResults);
    if (gestureRecognitionResults.size() == 0)
    {
        throw std::exception("gestureRecognitionResults");
    }
    _strokes = strokes;
    _gestureRecognitionResults = gestureRecognitionResults;
}


