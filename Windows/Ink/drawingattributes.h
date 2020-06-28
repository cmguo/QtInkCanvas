#ifndef DRAWINGATTRIBUTES_H
#define DRAWINGATTRIBUTES_H

#include "InkCanvas_global.h"

#include "Windows/Ink/stylustip.h"
#include "Windows/Ink/drawingflags.h"
#include "Windows/Media/matrix.h"
#include "sharedptr.h"
#include "guid.h"
#include "variant.h"

#ifndef INKCANVAS_CORE
#include "Internal/Ink/InkSerializedFormat/drawingattributeserializer.h"
#endif

#ifdef INKCANVAS_QT
#include <QColor>
#endif

INKCANVAS_BEGIN_NAMESPACE

class StylusShape;
class PropertyChangedEventArgs;
class PropertyDataChangedEventArgs;
class ExtendedPropertiesChangedEventArgs;
class ExtendedPropertyCollection;

// namespace System.Windows.Ink

#ifdef INKCANVAS_QT
class INKCANVAS_EXPORT DrawingAttributes : public QObject
{
    Q_OBJECT
#else
class INKCANVAS_EXPORT DrawingAttributes
{
#endif
public:
    /// <summary>
    /// Creates a DrawingAttributes with default values
    /// </summary>
    DrawingAttributes();

    /// <summary>
    /// only constructor that initializes a DA with an EPC
    /// </summary>
    /// <param name="extendedProperties"></param>
    DrawingAttributes(ExtendedPropertyCollection* extendedProperties);

#ifdef INKCANVAS_QT
    virtual ~DrawingAttributes() override;
#else
    virtual ~DrawingAttributes();
#endif

private:
    /// <summary>
    /// Common constructor call, also called by Clone
    /// </summary>
    void Initialize();

public:

#ifndef INKCANVAS_CORE

    /// <summary>
    /// The color of the Stroke
    /// </summary>
    QColor Color() const;

    void SetColor(QColor value);

#endif

    /// <summary>
    /// The StylusTip used to draw the stroke
    /// </summary>
    StylusTip GetStylusTip() const;

    void SetStylusTip(StylusTip value);

    /// <summary>
    /// The StylusTip used to draw the stroke
    /// </summary>
    Matrix StylusTipTransform() const;

    void SetStylusTipTransform(Matrix const & value);

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
    void AddPropertyData(Guid const & propertyDataId, Variant const & propertyData);

    /// <summary>
    /// Allows removal of objects from the EPC
    /// </summary>
    /// <param name="propertyDataId"></param>
    void RemovePropertyData(Guid const & propertyDataId);

    /// <summary>
    /// Allows retrieval of objects from the EPC
    /// </summary>
    /// <param name="propertyDataId"></param>
    Variant GetPropertyData(Guid const & propertyDataId);

    /// <summary>
    /// Allows retrieval of a Array of Guids that are contained in the EPC
    /// </summary>
    Array<Guid> GetPropertyDataIds();

    /// <summary>
    /// Allows check of containment of objects to the EPC
    /// </summary>
    /// <param name="propertyDataId"></param>
    bool ContainsPropertyData(Guid const & propertyDataId);

    /// <summary>
    /// ExtendedProperties
    /// </summary>
    ExtendedPropertyCollection& ExtendedProperties();


    /// <summary>
    /// Returns a copy of the EPC
    /// </summary>
    ExtendedPropertyCollection* CopyPropertyData();

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

#ifndef INKCANVAS_CORE

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

#endif


    //------------------------------------------------------
    //
    //  INotifyPropertyChanged Interface
    //
    //------------------------------------------------------

#ifdef INKCANVAS_QT

signals:
    void PropertyChanged(PropertyChangedEventArgs & e);

#endif

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
    virtual SharedPointer<DrawingAttributes> Clone();

    /// <summary>
    /// Simple helper method used to determine if a Guid
    /// from an ExtendedProperty is used as the backing store
    /// of a DrawingAttribute
    /// </summary>
    /// <param name="id"></param>
    /// <returns></returns>
    static Variant GetDefaultDrawingAttributeValue(Guid const & id);

    static void ValidateStylusTipTransform(Guid const & propertyDataId, Variant propertyData);

    /// <summary>
    /// Simple helper method used to determine if a Guid
    /// needs to be removed from the ExtendedPropertyCollection in ISF
    /// before serializing
    /// </summary>
    /// <param name="id"></param>
    /// <returns></returns>
    static bool RemoveIdFromExtendedProperties(Guid const & id);

    /// <summary>
    /// Returns true if two DrawingAttributes lead to the same PathGeometry.
    /// </summary>
    static bool GeometricallyEqual(DrawingAttributes const & left, DrawingAttributes const & right);

    /// <summary>
    /// Returns true if the Guid passed in has impact on geometry of the stroke
    /// </summary>
    static bool IsGeometricalDaGuid(Guid const & Guid);

    /// <summary>
    /// Whenever the base class fires the generic ExtendedPropertiesChanged
    /// event, we need to fire the DrawingAttributesChanged event also.
    /// </summary>
    /// <param name="sender">Should be 'this' object</param>
    /// <param name="args">The custom attributes that changed</param>
    void ExtendedPropertiesChanged_EventForwarder(ExtendedPropertiesChangedEventArgs& args);

#ifdef INKCANVAS_QT

signals:
    /// <summary>
    /// Event fired whenever a DrawingAttribute is modified
    /// </summary>
    void AttributeChanged(PropertyDataChangedEventArgs& e);

    void _propertyChanged(PropertyChangedEventArgs& e);

#endif

protected:
    /// <summary>
    /// Method called when a change occurs to any DrawingAttribute
    /// </summary>
    /// <param name="e">The change information for the DrawingAttribute that was modified</param>
    virtual void OnAttributeChanged(PropertyDataChangedEventArgs &e);

#ifdef INKCANVAS_QT

signals:
     /// <summary>
    /// Event fired whenever a DrawingAttribute is modified
    /// </summary>
    void PropertyDataChanged(PropertyDataChangedEventArgs& e);

#endif

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

#ifdef INKCANVAS_QT
        //if (this.PropertyDataChanged != null)
        //{
            emit PropertyDataChanged(e);
        //}
#endif
    }


    virtual void OnPropertyChanged(PropertyChangedEventArgs& e)
    {
#ifdef INKCANVAS_QT
        //if ( _propertyChanged != null )
        {
            emit  _propertyChanged(e);
        }
#endif
    }

    /// <summary>
    /// All DrawingAttributes are backed by an ExtendedProperty
    /// this is a simple helper to set a property
    /// </summary>
    /// <param name="id">id</param>
    /// <param name="value">value</param>
    void SetExtendedPropertyBackedProperty(Guid const & id, Variant const & value);

    /// <summary>
    /// All DrawingAttributes are backed by an ExtendedProperty
    /// this is a simple helper to set a property
    /// </summary>
    /// <param name="id">id</param>
    /// <returns></returns>
    Variant GetExtendedPropertyBackedProperty(Guid const & id) const;

    /// <summary>
    /// A help method which fires INotifyPropertyChanged.PropertyChanged event
    /// </summary>
    /// <param name="e"></param>
    void PrivateNotifyPropertyChanged(PropertyDataChangedEventArgs &e);

    void OnPropertyChanged(char const * propertyName);

private:
    ExtendedPropertyCollection* _extendedProperties;
#ifndef INKCANVAS_CORE
    uint _v1RasterOperation = DrawingAttributeSerializer::RasterOperationDefaultV1;
    bool _heightChangedForCompatabity = false;
#endif
    std::unique_ptr<StylusShape> drawingShape_;

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

INKCANVAS_END_NAMESPACE

#ifndef INKCANVAS_CORE

#include "Windows/dependencyproperty.h"

INKCANVAS_BEGIN_NAMESPACE

class DrawingAttributesDefaultValueFactory : public DefaultValueFactory
{
private:
    virtual Variant DefaultValue() override;
};

INKCANVAS_END_NAMESPACE

#endif

#endif // DRAWINGATTRIBUTES_H
