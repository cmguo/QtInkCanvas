#ifndef INKCANVASSTROKESREPLACEDEVENTARGS_H
#define INKCANVASSTROKESREPLACEDEVENTARGS_H

#include "Windows/routedeventargs.h"
#include "Windows/Ink/events.h"
#include "Windows/Ink/gesturerecognitionresult.h"

#include <QSharedPointer>
#include <QRectF>
#include <QPointF>

INKCANVAS_BEGIN_NAMESPACE

class Stroke;
class StrokeCollection;
class UIElement;

// namespace System.Windows.Controls

/// <summary>
///    InkCanvasStrokeCollectedEventArgs
/// </summary>
class InkCanvasStrokeCollectedEventArgs : public RoutedEventArgs
{
public:
    /// <summary>
    /// [TBS]
    /// </summary>
    InkCanvasStrokeCollectedEventArgs(QSharedPointer<Stroke> stroke);

    /// <summary>
    /// [TBS]
    /// </summary>
    QSharedPointer<Stroke> GetStroke()
    {
        return _stroke;
    }

    /// <summary>
    ///     The mechanism used to call the type-specific handler on the
    ///     target.
    /// </summary>
    /// <param name="genericHandler">
    ///     The generic handler to call in a type-specific way.
    /// </param>
    /// <param name="genericTarget">
    ///     The target to call the handler on.
    /// </param>
    //virtual void InvokeEventHandler(Delegate genericHandler, QObject* genericTarget);

private:
    QSharedPointer<Stroke> _stroke;
};

/// <summary>
/// The delegate to use for the StrokesChanged event
/// </summary>
//delegate void InkCanvasStrokesReplacedEventHandler(QObject* sender, InkCanvasStrokesReplacedEventArgs e);

/// <summary>
///    InkCanvasStrokesChangedEventArgs
/// </summary>
class InkCanvasStrokesReplacedEventArgs : public EventArgs
{
public:
    /// <summary>
    /// InkCanvasStrokesReplacedEventArgs
    /// </summary>
    InkCanvasStrokesReplacedEventArgs(QSharedPointer<StrokeCollection> newStrokes, QSharedPointer<StrokeCollection> previousStrokes);

    /// <summary>
    /// [TBS]
    /// </summary>
    QSharedPointer<StrokeCollection> NewStrokes()
    {
        return _newStrokes;
    }

    /// <summary>
    /// [TBS]
    /// </summary>
    QSharedPointer<StrokeCollection> PreviousStrokes()
    {
        return _previousStrokes;
    }

private:
    QSharedPointer<StrokeCollection> _newStrokes;
    QSharedPointer<StrokeCollection> _previousStrokes;

};

/// <summary>
///     The delegate to use for the SelectionChanging event
///
///     This event is only thrown when you change the selection programmatically (through our APIs, not the SelectionService's
///     or through our lasso selection behavior
/// </summary>
//delegate void InkCanvasSelectionChangingEventHandler(QObject* sender, InkCanvasSelectionChangingEventArgs e);

/// <summary>
/// Event arguments sent when the SelectionChanging event is raised.
/// </summary>
class InkCanvasSelectionChangingEventArgs : public CancelEventArgs
{
private:
    QSharedPointer<StrokeCollection>        _strokes;
    QList<UIElement*>         _elements;
    bool                    _strokesChanged;
    bool                    _elementsChanged;

public:
    /// <summary>
    /// Constructor
    /// </summary>
    InkCanvasSelectionChangingEventArgs(QSharedPointer<StrokeCollection> selectedStrokes, QList<UIElement*> selectedElements);

    /// <summary>
    /// An flag which indicates the Strokes has changed.
    /// </summary>
    bool StrokesChanged()
    {
        return _strokesChanged;
    }

    /// <summary>
    /// An flag which indicates the Elements has changed.
    /// </summary>
    bool ElementsChanged()
    {
        return _elementsChanged;
    }

    /// <summary>
    /// Set the selected elements
    /// </summary>
    /// <param name="selectedElements">The new selected elements</param>
    void SetSelectedElements(QList<UIElement*> selectedElements);

    /// <summary>
    /// Get the selected elements
    /// </summary>
    /// <returns>The selected elements</returns>
   QList<UIElement*> GetSelectedElements()
    {
        return _elements;
    }

    /// <summary>
    /// Set the selected strokes
    /// </summary>
    /// <param name="selectedStrokes">The new selected strokes</param>
    void SetSelectedStrokes(QSharedPointer<StrokeCollection> selectedStrokes);

    /// <summary>
    /// Get the selected strokes
    /// </summary>
    /// <returns>The selected strokes</returns>
    QSharedPointer<StrokeCollection> GetSelectedStrokes();
};


/// <summary>
///     The delegate to use for the SelectionMoving, SelectionResizing events
/// </summary>
//delegate void  InkCanvasSelectionEditingEventHandler(QObject* sender,  InkCanvasSelectionEditingEventArgs e);

/// <summary>
/// Event arguments sent when the SelectionChanging event is raised.
/// </summary>
class  InkCanvasSelectionEditingEventArgs : public CancelEventArgs
{
    QRectF _oldRectangle;
    QRectF _newRectangle;

public:
    /// <summary>
    /// Constructor
    /// </summary>
     InkCanvasSelectionEditingEventArgs(QRectF const & oldRectangle, QRectF const & newRectangle);

    /// <summary>
    /// Read access to the OldRectangle, from before the edit.
    /// </summary>
    QRectF& OldRectangle()
    {
        return _oldRectangle;
    }

    /// <summary>
    /// Read access to the NewRectangle, resulting from this edit.
    /// </summary>
    QRectF& NewRectangle()
    {
        return _newRectangle;
    }
    void SetNewRectangle(QRectF& value)
    {
        _newRectangle = value;
    }
};

/// <summary>
///     The delegate to use for the InkErasing event
/// </summary>
//delegate void InkCanvasStrokeErasingEventHandler(QObject* sender, InkCanvasStrokeErasingEventArgs e);

/// <summary>
/// Event arguments sent when the SelectionChanging event is raised.
/// </summary>
class InkCanvasStrokeErasingEventArgs : public CancelEventArgs
{
    QSharedPointer<Stroke> _stroke;

public:
    /// <summary>
    /// Constructor
    /// </summary>
    InkCanvasStrokeErasingEventArgs(QSharedPointer<Stroke> stroke);

    /// <summary>
    /// Read access to the stroke about to be deleted
    /// </summary>
    QSharedPointer<Stroke> GetStroke()
    {
        return _stroke;
    }
};

/// <summary>
/// TBD
/// </summary>
/// <param name="sender">sender</param>
/// <param name="e">e</param>
//delegate void InkCanvasGestureEventHandler(QObject* sender, InkCanvasGestureEventArgs e);

/// <summary>
/// ApplicationGestureEventArgs
/// </summary>

class InkCanvasGestureEventArgs : public RoutedEventArgs
{
    QSharedPointer<StrokeCollection> _strokes;
    QList<GestureRecognitionResult> _gestureRecognitionResults;
    bool                _cancel;

public:
    /// <summary>
    /// TBD
    /// </summary>
    /// <param name="strokes">strokes</param>
    /// <param name="gestureRecognitionResults">gestureRecognitionResults</param>
    InkCanvasGestureEventArgs(QSharedPointer<StrokeCollection> strokes, QList<GestureRecognitionResult> gestureRecognitionResults);

    /// <summary>
    /// TBD
    /// </summary>
    QSharedPointer<StrokeCollection> Strokes()
    {
        return _strokes;
    }

    /// <summary>
    /// TBD
    /// </summary>
    /// <returns></returns>
    QList<GestureRecognitionResult> GetGestureRecognitionResults()
    {
        return _gestureRecognitionResults;
    }

    /// <summary>
    /// Indicates whether the Gesture event needs to be cancelled.
    /// </summary>
    bool Cancel()
    {
         return _cancel;
    }
    void SetCancel(bool value)
    {
         _cancel = value;
    }


    /// <summary>
    ///     The mechanism used to call the type-specific handler on the
    ///     target.
    /// </summary>
    /// <param name="genericHandler">
    ///     The generic handler to call in a type-specific way.
    /// </param>
    /// <param name="genericTarget">
    ///     The target to call the handler on.
    /// </param>
    //virtual void InvokeEventHandler(Delegate genericHandler, QObject* genericTarget);


};

INKCANVAS_END_NAMESPACE

#endif // INKCANVASSTROKESREPLACEDEVENTARGS_H
