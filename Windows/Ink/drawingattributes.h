#ifndef DRAWINGATTRIBUTES_H
#define DRAWINGATTRIBUTES_H

#include "knownids.h"
#include "stylustip.h"
#include "drawingflags.h"
#include "Internal/Ink/InkSerializedFormat/drawingattributeserializer.h"

#include <QMatrix>
#include <QMap>
#include <QUuid>
#include <QColor>
#include <QVariant>

class StylusShape;
class PropertyChangedEventArgs;
class PropertyDataChangedEventArgs;

// namespace System.Windows.Ink

class DrawingAttributes : public QObject
{
    Q_OBJECT
public:
    /// <summary>
    /// Creates a DrawingAttributes with default values
    /// </summary>
    DrawingAttributes();

    /// <summary>
    /// only constructor that initializes a DA with an EPC
    /// </summary>
    /// <param name="extendedProperties"></param>
    DrawingAttributes(QMap<QUuid, QVariant> const & extendedProperties);

    virtual ~DrawingAttributes();

private:
    /// <summary>
    /// Common constructor call, also called by Clone
    /// </summary>
    void Initialize();

public:
    /// <summary>
    /// The color of the Stroke
    /// </summary>
    QColor Color() const;

    void SetColor(QColor value);

    /// <summary>
    /// The StylusTip used to draw the stroke
    /// </summary>
    StylusTip GetStylusTip() const;

    void SetStylusTip(StylusTip value);

    /// <summary>
    /// The StylusTip used to draw the stroke
    /// </summary>
    QMatrix StylusTipTransform() const;

    void SetStylusTipTransform(QMatrix value);

    /// <summary>
    /// The height of the StylusTip
    /// </summary>
    double Height() const;
    void SetHeight(double value);

    /// <summary>
    /// The width of the StylusTip
    /// </summary>
    double Width() const;
    void SetWidth(double value);

    /// <summary>
    /// When true, ink will be rendered as a series of curves instead of as
    /// lines between Stylus sample points. This is useful for smoothing the ink, especially
    /// when the person writing the ink has jerky or shaky writing.
    /// This value is TRUE by default in the Avalon implementation
    /// </summary>
    bool FitToCurve() const;
    void SetFitToCurve(bool value);

    /// <summary>
    /// When true, ink will be rendered with any available pressure information
    /// taken into account
    /// </summary>
    bool IgnorePressure() const;
    void SetIgnorePressure(bool value);

    /// <summary>
    /// Determines if the stroke should be treated as a highlighter
    /// </summary>
    bool IsHighlighter() const;
    void SetIsHighlighter(bool value);

    /// <summary>
    /// Allows addition of objects to the EPC
    /// </summary>
    /// <param name="propertyDataId"></param>
    /// <param name="propertyData"></param>
    void AddPropertyData(QUuid const & propertyDataId, QVariant propertyData);

    /// <summary>
    /// Allows removal of objects from the EPC
    /// </summary>
    /// <param name="propertyDataId"></param>
    void RemovePropertyData(QUuid const & propertyDataId);

    /// <summary>
    /// Allows retrieval of objects from the EPC
    /// </summary>
    /// <param name="propertyDataId"></param>
    QVariant GetPropertyData(QUuid const & propertyDataId);

    /// <summary>
    /// Allows retrieval of a Array of QUuids that are contained in the EPC
    /// </summary>
    QVector<QUuid> GetPropertyDataIds();

    /// <summary>
    /// Allows check of containment of objects to the EPC
    /// </summary>
    /// <param name="propertyDataId"></param>
    bool ContainsPropertyData(QUuid const & propertyDataId);

    /// <summary>
    /// ExtendedProperties
    /// </summary>
    QMap<QUuid, QVariant> ExtendedProperties();


    /// <summary>
    /// Returns a copy of the EPC
    /// </summary>
    QMap<QUuid, QVariant> CopyPropertyData();

    /// <summary>
    /// StylusShape
    /// </summary>
    StylusShape * GetStylusShape();

    /// <summary>
    /// Sets the Fitting error for this drawing attributes
    /// </summary>
    int FittingError();
    void SetFittingError(int value);

    /// <summary>
    /// Sets the Fitting error for this drawing attributes
    /// </summary>
    DrawingFlags GetDrawingFlags() const;
    void SetDrawingFlags(DrawingFlags value);


    /// <summary>
    /// we need to preserve this for round tripping
    /// </summary>
    /// <value></value>
    uint RasterOperation()
    {
        return _v1RasterOperation;
    }
    void SetRasterOperation(uint value);

    /// <summary>
    /// When we load ISF from V1 if width is set and height is not
    /// and PenTip is Circle, we need to set height to the same as width
    /// or else we'll render different as an Ellipse.  We use this flag to
    /// preserve state for round tripping.
    /// </summary>
    bool HeightChangedForCompatabity()
    {
        return _heightChangedForCompatabity;
    }
    void SetHeightChangedForCompatabity(bool value);


    //------------------------------------------------------
    //
    //  INotifyPropertyChanged Interface
    //
    //------------------------------------------------------

signals:
    void PropertyChanged(PropertyChangedEventArgs & e);

public:
    /// <summary>Overload of the Equals method which determines if two DrawingAttributes
    /// objects contain the same drawing attributes</summary>
    bool Equals(DrawingAttributes const & that) const;

    /// <summary>Overload of the equality operator which determines
    /// if two DrawingAttributes are equal</summary>
    friend bool operator ==(DrawingAttributes const & first, DrawingAttributes const & second)
    {
        // finally use the full `blown value-style comparison against the collection contents
        return first.Equals(second);
    }

    /// <summary>Overload of the not equals operator to determine if two
    /// DrawingAttributes are different</summary>
    friend bool operator !=(DrawingAttributes const & first, DrawingAttributes const & second)
    {
        return !(first == second);
    }

    /// <summary>
    /// Copies the DrawingAttributes
    /// </summary>
    /// <returns>Deep copy of the DrawingAttributes</returns>
    /// <remarks></remarks>
    virtual QSharedPointer<DrawingAttributes> Clone();


    /// <summary>
    /// Simple helper method used to determine if a QUuid
    /// from an ExtendedProperty is used as the backing store
    /// of a DrawingAttribute
    /// </summary>
    /// <param name="id"></param>
    /// <returns></returns>
    static QVariant GetDefaultDrawingAttributeValue(QUuid const & id);

    static void ValidateStylusTipTransform(QUuid const & propertyDataId, QVariant propertyData);

    /// <summary>
    /// Simple helper method used to determine if a QUuid
    /// needs to be removed from the ExtendedPropertyCollection in ISF
    /// before serializing
    /// </summary>
    /// <param name="id"></param>
    /// <returns></returns>
    static bool RemoveIdFromExtendedProperties(QUuid const & id);

    /// <summary>
    /// Returns true if two DrawingAttributes lead to the same PathGeometry.
    /// </summary>
    static bool GeometricallyEqual(DrawingAttributes const & left, DrawingAttributes const & right);

    /// <summary>
    /// Returns true if the QUuid passed in has impact on geometry of the stroke
    /// </summary>
    static bool IsGeometricalDaGuid(QUuid const & QUuid);

signals:
    /// <summary>
    /// Event fired whenever a DrawingAttribute is modified
    /// </summary>
    void AttributeChanged(PropertyDataChangedEventArgs& e);

    void _propertyChanged(PropertyChangedEventArgs& e);

protected:
    /// <summary>
    /// Method called when a change occurs to any DrawingAttribute
    /// </summary>
    /// <param name="e">The change information for the DrawingAttribute that was modified</param>
    virtual void OnAttributeChanged(PropertyDataChangedEventArgs &e)
    {
        //if (null == e)
        //{
        //    throw new ArgumentNullException("e", SR.Get(SRID.EventArgIsNull));
        //}

        //try
        //{
            PrivateNotifyPropertyChanged(e);
        //}
        //finally
        //{
            //if ( this.AttributeChanged != null )
            //{
                emit AttributeChanged(e);
            //}
        //}
    }

signals:
     /// <summary>
    /// Event fired whenever a DrawingAttribute is modified
    /// </summary>
    void PropertyDataChanged(PropertyDataChangedEventArgs& e);

protected:
    /// <summary>
    /// Method called when a change occurs to any PropertyData
    /// </summary>
    /// <param name="e">The change information for the PropertyData that was modified</param>
    virtual void OnPropertyDataChanged(PropertyDataChangedEventArgs& e)
    {
        //if (null == e)
        //{
        //    throw new ArgumentNullException("e", SR.Get(SRID.EventArgIsNull));
        //}

        //if (this.PropertyDataChanged != null)
        //{
            emit PropertyDataChanged(e);
        //}
    }


    virtual void OnPropertyChanged(PropertyChangedEventArgs& e)
    {
        //if ( _propertyChanged != null )
        {
            emit  _propertyChanged(e);
        }
    }

    /// <summary>
    /// All DrawingAttributes are backed by an ExtendedProperty
    /// this is a simple helper to set a property
    /// </summary>
    /// <param name="id">id</param>
    /// <param name="value">value</param>
    void SetExtendedPropertyBackedProperty(QUuid const & id, QVariant value);

    /// <summary>
    /// All DrawingAttributes are backed by an ExtendedProperty
    /// this is a simple helper to set a property
    /// </summary>
    /// <param name="id">id</param>
    /// <returns></returns>
    QVariant GetExtendedPropertyBackedProperty(QUuid const & id) const;

    /// <summary>
    /// A help method which fires INotifyPropertyChanged.PropertyChanged event
    /// </summary>
    /// <param name="e"></param>
    void PrivateNotifyPropertyChanged(PropertyDataChangedEventArgs &e);

    void OnPropertyChanged(QString propertyName);

private:
    QMap<QUuid, QVariant> _extendedProperties;
    uint _v1RasterOperation = DrawingAttributeSerializer::RasterOperationDefaultV1;
    bool _heightChangedForCompatabity = false;

public:
    /// <summary>
    /// Statics
    /// </summary>
    static constexpr float StylusPrecision = 1000.0f;
    static constexpr double DefaultWidth = 2.0031496062992127;
    static constexpr double DefaultHeight = 2.0031496062992127;

    /// <summary>
    /// Mininum acceptable stylus tip height, corresponds to 0.001 in V1
    /// </summary>
    /// <remarks>corresponds to 0.001 in V1  (0.001 / (2540/96))</remarks>
    static constexpr double MinHeight = 0.00003779527559055120;

    /// <summary>
    /// Minimum acceptable stylus tip width
    /// </summary>
    /// <remarks>corresponds to 0.001 in V1  (0.001 / (2540/96))</remarks>
    static constexpr double MinWidth =  0.00003779527559055120;

    /// <summary>
    /// Maximum acceptable stylus tip height.
    /// </summary>
    /// <remarks>corresponds to 4294967 in V1 (4294967 / (2540/96))</remarks>
    static constexpr double MaxHeight = 162329.4614173230;


    /// <summary>
    /// Maximum acceptable stylus tip width.
    /// </summary>
    /// <remarks>corresponds to 4294967 in V1 (4294967 / (2540/96))</remarks>
    static constexpr double MaxWidth = 162329.4614173230;
};

#endif // DRAWINGATTRIBUTES_H
