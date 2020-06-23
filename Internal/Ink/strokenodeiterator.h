#ifndef STROKENODEITERATOR_H
#define STROKENODEITERATOR_H

#include "Windows/Ink/stroke.h"
#include "Internal/Ink/strokenodeoperations.h"

// namespace MS.Internal.Ink
INKCANVAS_BEGIN_NAMESPACE

class StrokeNodeOperations;
class StrokeNode;

/// <summary>
/// This class serves as a unified tool for enumerating through stroke nodes
/// for all kinds of rendering and/or hit-testing that uses stroke contours.
/// It provides static API for static (atomic) rendering, and it needs to be
/// instantiated for dynamic (incremental) rendering. It generates stroke nodes
/// from Stroke objects with or w/o overriden drawing attributes, as well as from
/// a arrays of points (for a given StylusShape), and from raw stylus packets.
/// In either case, the output collection of nodes is represented by a disposable
/// iterator (i.e. good for a single enumeration only).
/// </summary>
class StrokeNodeIterator
{
public:
    /// <summary>
    /// Helper wrapper
    /// </summary>
    static StrokeNodeIterator GetIterator(Stroke & stroke, DrawingAttributes& drawingAttributes);

    /// <summary>
    /// Creates a default enumerator for a given stroke
    /// If using the strokes drawing attributes, pass stroke.DrawingAttributes for the second
    /// argument.  If using an overridden DA, use that instance.
    /// </summary>
    static StrokeNodeIterator GetIterator(QSharedPointer<StylusPointCollection> stylusPoints, DrawingAttributes& drawingAttributes);


    /// <summary>
    /// GetNormalizedPressureFactor
    /// </summary>
    static float GetNormalizedPressureFactor(float stylusPointPressureFactor)
    {
        //
        // create a compatible pressure value that maps 0-1 to 0.25 - 1.75
        //
        return (1.5f * stylusPointPressureFactor) + 0.25f;
    }

    /// <summary>
    /// Constructor for an incremental node enumerator that builds nodes
    /// from array(s) of points and a given stylus shape.
    /// </summary>
    /// <param name="nodeShape">a shape that defines the stroke contour</param>
    StrokeNodeIterator(StylusShape & nodeShape);

    /// <summary>
    /// Constructor for an incremental node enumerator that builds nodes
    /// from StylusPointCollections
    /// called by the IncrementalRenderer
    /// </summary>
    /// <param name="drawingAttributes">drawing attributes</param>
    StrokeNodeIterator(DrawingAttributes& drawingAttributes);

    /// <summary>
    /// Private ctor
    /// </summary>
    /// <param name="stylusPoints"></param>
    /// <param name="operations"></param>
    /// <param name="usePressure"></param>
    StrokeNodeIterator(QSharedPointer<StylusPointCollection> stylusPoints,
                                std::unique_ptr<StrokeNodeOperations>&& operations,
                                bool usePressure);

    /// <summary>
    /// Generates (enumerates) StrokeNode objects for a stroke increment
    /// represented by an StylusPointCollection.  Called from IncrementalRenderer
    /// </summary>
    /// <param name="stylusPoints">StylusPointCollection</param>
    /// <returns>yields StrokeNode objects one by one</returns>
    StrokeNodeIterator GetIteratorForNextSegment(QSharedPointer<StylusPointCollection> stylusPoints);

    /// <summary>
    /// Generates (enumerates) StrokeNode objects for a stroke increment
    /// represented by an array of points. This method is supposed to be used only
    /// on objects created via the c-tor with a StylusShape parameter.
    /// </summary>
    /// <param name="points">an array of points representing a stroke increment</param>
    /// <returns>yields StrokeNode objects one by one</returns>
    StrokeNodeIterator GetIteratorForNextSegment(QVector<QPointF> const & points);

    /// <summary>
    /// The count of strokenodes that can be iterated across
    /// </summary>
    int Count() const
    {
        if (_stylusPoints == nullptr)
        {
            return 0;
        }
        return _stylusPoints->size();
    }

    /// <summary>
    /// Gets a StrokeNode at the specified index
    /// </summary>
    /// <param name="index"></param>
    /// <returns></returns>
    StrokeNode operator[](int index) const;

    /// <summary>
    /// Gets a StrokeNode at the specified index that connects to a stroke at the previousIndex
    /// previousIndex can be -1 to signify it should be empty (first strokeNode)
    /// </summary>
    /// <returns></returns>
    StrokeNode GetNode(int index, int previousIndex) const;

    friend bool operator!=(StrokeNodeIterator const & l, nullptr_t)
    {
        return l._stylusPoints != nullptr;
    }

private:
    QSharedPointer<StylusPointCollection>  _stylusPoints;
    std::unique_ptr<StrokeNodeOperations>   _operations;
    bool                    _usePressure;
};

INKCANVAS_END_NAMESPACE

#endif // STROKENODEITERATOR_H
