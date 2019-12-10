#include "Windows/Ink/drawingattributes.h"
#include "Windows/Ink/stylusshape.h"
#include "Windows/Ink/knownids.h"
#include "Windows/Ink/events.h"
#include "Windows/Ink/extendedpropertycollection.h"
#include "Internal/doubleutil.h"
#include "Internal/debug.h"

/// <summary>
/// Creates a DrawingAttributes with default values
/// </summary>
DrawingAttributes::DrawingAttributes()
{
    _extendedProperties = new ExtendedPropertyCollection;
    Initialize();
}

/// <summary>
/// only constructor that initializes a DA with an EPC
/// </summary>
/// <param name="extendedProperties"></param>
DrawingAttributes::DrawingAttributes(ExtendedPropertyCollection* extendedProperties)
{
    Debug::Assert(extendedProperties != nullptr);
    _extendedProperties = extendedProperties;

    Initialize();
}

DrawingAttributes::~DrawingAttributes()
{
}

/// <summary>
/// Common constructor call, also called by Clone
/// </summary>
void DrawingAttributes::Initialize()
{
    Debug::Assert(_extendedProperties != nullptr);
    //_extendedProperties->Changed +=
    //    new ExtendedPropertiesChangedEventHandler(this.ExtendedPropertiesChanged_EventForwarder);
}


/// <summary>
/// The color of the Stroke
/// </summary>
QColor DrawingAttributes::Color() const
{
    //prevent boxing / unboxing if possible
    if (!_extendedProperties->Contains(KnownIds::Color))
    {
        Debug::Assert(QColor(Qt::black) == GetDefaultDrawingAttributeValue(KnownIds::Color).value<QColor>());
        return QColor(Qt::black);
    }
    return GetExtendedPropertyBackedProperty(KnownIds::Color).value<QColor>();
}

void DrawingAttributes::SetColor(QColor value)
{
    //no need to raise change events, they will bubble up from the EPC
    //underneath us
    // Validation of value is done in EPC
    SetExtendedPropertyBackedProperty(KnownIds::Color, value);
}

/// <summary>
/// The StylusTip used to draw the stroke
/// </summary>
StylusTip DrawingAttributes::GetStylusTip() const
{
    //prevent boxing / unboxing if possible
    if (!_extendedProperties->Contains(KnownIds::StylusTip))
    {
        Debug::Assert(StylusTip::Ellipse == GetDefaultDrawingAttributeValue(KnownIds::StylusTip).value<StylusTip>());
        return StylusTip::Ellipse;
    }
    else
    {
        //if we ever add to StylusTip enumeration, we need to just return GetExtendedPropertyBackedProperty
        Debug::Assert(StylusTip::Rectangle == GetExtendedPropertyBackedProperty(KnownIds::StylusTip).value<StylusTip>());
        return StylusTip::Rectangle;
    }
}

void DrawingAttributes::SetStylusTip(StylusTip value)
{
    //no need to raise change events, they will bubble up from the EPC
    //underneath us
    // Validation of value is done in EPC
    SetExtendedPropertyBackedProperty(KnownIds::StylusTip, QVariant::fromValue(value));
}

/// <summary>
/// The StylusTip used to draw the stroke
/// </summary>
QMatrix DrawingAttributes::StylusTipTransform() const
{
    //prevent boxing / unboxing if possible
    if (!_extendedProperties->Contains(KnownIds::StylusTipTransform))
    {
        Debug::Assert(QMatrix() == GetDefaultDrawingAttributeValue(KnownIds::StylusTipTransform).value<QMatrix>());
        return QMatrix();
    }
    return GetExtendedPropertyBackedProperty(KnownIds::StylusTipTransform).value<QMatrix>();
}

void DrawingAttributes::SetStylusTipTransform(QMatrix const & value)
{
    QMatrix const & m = value;
    if (m.dx() != 0 || m.dy() != 0)
    {
        throw std::exception("value");
    }
    //no need to raise change events, they will bubble up from the EPC
    //underneath us
    // Validation of value is done in EPC
    SetExtendedPropertyBackedProperty(KnownIds::StylusTipTransform, value);
}

/// <summary>
/// The height of the StylusTip
/// </summary>
double DrawingAttributes::Height() const
{
    //prevent boxing / unboxing if possible
    if (!_extendedProperties->Contains(KnownIds::StylusHeight))
    {
        Debug::Assert(DefaultHeight == GetDefaultDrawingAttributeValue(KnownIds::StylusHeight));
        return DefaultHeight;
    }
    return GetExtendedPropertyBackedProperty(KnownIds::StylusHeight).toDouble();
}

void DrawingAttributes::SetHeight(double value)
{
    if (qIsNaN(value) || value < MinHeight || value > MaxHeight)
    {
        throw std::exception("Height");
    }
    //no need to raise change events, they will bubble up from the EPC
    //underneath us
    SetExtendedPropertyBackedProperty(KnownIds::StylusHeight, value);
}

/// <summary>
/// The width of the StylusTip
/// </summary>
double DrawingAttributes::Width() const
{
    //prevent boxing / unboxing if possible
    if (!_extendedProperties->Contains(KnownIds::StylusWidth))
    {
        Debug::Assert(DefaultWidth == GetDefaultDrawingAttributeValue(KnownIds::StylusWidth).toDouble());
        return DefaultWidth;
    }
    return GetExtendedPropertyBackedProperty(KnownIds::StylusWidth).toDouble();
}
void DrawingAttributes::SetWidth(double value)
{
    if (qIsNaN(value) || value < MinWidth || value > MaxWidth)
    {
        throw std::exception("Width");
    }
    //no need to raise change events, they will bubble up from the EPC
    //underneath us
    SetExtendedPropertyBackedProperty(KnownIds::StylusWidth, value);
}

/// <summary>
/// When true, ink will be rendered as a series of curves instead of as
/// lines between Stylus sample points. This is useful for smoothing the ink, especially
/// when the person writing the ink has jerky or shaky writing.
/// This value is TRUE by default in the Avalon implementation
/// </summary>
bool DrawingAttributes::FitToCurve() const
{
    return (0 != (GetDrawingFlags() & DrawingFlag::FitToCurve));
}
void DrawingAttributes::SetFitToCurve(bool value)
{
    //no need to raise change events, they will bubble up from the EPC
    //underneath us
    DrawingFlags flags = GetDrawingFlags();
    if (value)
    {
        //turn on the bit
        flags |= DrawingFlag::FitToCurve;
    }
    else
    {
        //turn off the bit
        flags.setFlag(DrawingFlag::FitToCurve, false);
    }
    SetExtendedPropertyBackedProperty(KnownIds::DrawingFlags, QVariant::fromValue(flags));
}

/// <summary>
/// When true, ink will be rendered with any available pressure information
/// taken into account
/// </summary>
bool DrawingAttributes::IgnorePressure() const
{
    return (0 != (GetDrawingFlags() & DrawingFlag::IgnorePressure));
}
void DrawingAttributes::SetIgnorePressure(bool value)
{
    //no need to raise change events, they will bubble up from the EPC
    //underneath us
    DrawingFlags flags = GetDrawingFlags();
    if (value)
    {
        //turn on the bit
        flags |= DrawingFlag::IgnorePressure;
    }
    else
    {
        //turn off the bit
        flags.setFlag(DrawingFlag::IgnorePressure, false);
    }
    SetExtendedPropertyBackedProperty(KnownIds::DrawingFlags, QVariant::fromValue(flags));
}

/// <summary>
/// Determines if the stroke should be treated as a highlighter
/// </summary>
bool DrawingAttributes::IsHighlighter() const
{
    //prevent boxing / unboxing if possible
    if (!_extendedProperties->Contains(KnownIds::IsHighlighter))
    {
        Debug::Assert(false == GetDefaultDrawingAttributeValue(KnownIds::IsHighlighter).toBool());
        return false;
    }
    else
    {
        Debug::Assert(true == GetExtendedPropertyBackedProperty(KnownIds::IsHighlighter).toBool());
        return true;
    }
}
void DrawingAttributes::SetIsHighlighter(bool value)
{
    //no need to raise change events, they will bubble up from the EPC
    //underneath us
    SetExtendedPropertyBackedProperty(KnownIds::IsHighlighter, value);

    //
    // set RasterOp for V1 interop
    //
    if (value)
    {
        _v1RasterOperation = DrawingAttributeSerializer::RasterOperationMaskPen;
    }
    else
    {
        _v1RasterOperation = DrawingAttributeSerializer::RasterOperationDefaultV1;
    }
}

/// <summary>
/// Allows addition of objects to the EPC
/// </summary>
/// <param name="propertyDataId"></param>
/// <param name="propertyData"></param>
void DrawingAttributes::AddPropertyData(QUuid const & propertyDataId, QVariant propertyData)
{
    ValidateStylusTipTransform(propertyDataId, propertyData);
    SetExtendedPropertyBackedProperty(propertyDataId, propertyData);
}

/// <summary>
/// Allows removal of objects from the EPC
/// </summary>
/// <param name="propertyDataId"></param>
void DrawingAttributes::RemovePropertyData(QUuid const & propertyDataId)
{
    _extendedProperties->Remove(propertyDataId);
}

/// <summary>
/// Allows retrieval of objects from the EPC
/// </summary>
/// <param name="propertyDataId"></param>
QVariant DrawingAttributes::GetPropertyData(QUuid const & propertyDataId)
{
    return GetExtendedPropertyBackedProperty(propertyDataId);
}

/// <summary>
/// Allows retrieval of a Array of QUuids that are contained in the EPC
/// </summary>
QVector<QUuid> DrawingAttributes::GetPropertyDataIds()
{
    return _extendedProperties->GetGuidArray();
}

/// <summary>
/// Allows check of containment of objects to the EPC
/// </summary>
/// <param name="propertyDataId"></param>
bool DrawingAttributes::ContainsPropertyData(QUuid const & propertyDataId)
{
    return _extendedProperties->Contains(propertyDataId);
}

/// <summary>
/// ExtendedProperties
/// </summary>
ExtendedPropertyCollection& DrawingAttributes::ExtendedProperties()
{
    return *_extendedProperties;
}


/// <summary>
/// Returns a copy of the EPC
/// </summary>
ExtendedPropertyCollection* DrawingAttributes::CopyPropertyData()
{
    return _extendedProperties;
}

/// <summary>
/// StylusShape
/// </summary>
StylusShape * DrawingAttributes::GetStylusShape()
{
    StylusShape * s;
    if (GetStylusTip() == StylusTip::Rectangle)
    {
        s =  new RectangleStylusShape(Width(), Height());
    }
    else
    {
        s = new EllipseStylusShape(Width(), Height());
    }

    s->SetTransform(StylusTipTransform());
    return s;
}

/// <summary>
/// Sets the Fitting error for this drawing attributes
/// </summary>
int DrawingAttributes::FittingError()
{
    if (!_extendedProperties->Contains(KnownIds::CurveFittingError))
    {
        return 0;
    }
    else
    {
        return (*_extendedProperties)[KnownIds::CurveFittingError].toInt();
    }
}
void DrawingAttributes::SetFittingError(int value)
{
    _extendedProperties->Set(KnownIds::CurveFittingError, value);
}

/// <summary>
/// Sets the Fitting error for this drawing attributes
/// </summary>
DrawingFlags DrawingAttributes::GetDrawingFlags() const
{
    return GetExtendedPropertyBackedProperty(KnownIds::DrawingFlags).value<DrawingFlags>();
}
void DrawingAttributes::SetDrawingFlags(DrawingFlags value)
{
    //no need to raise change events, they will bubble up from the EPC
    //underneath us
    SetExtendedPropertyBackedProperty(KnownIds::DrawingFlags, QVariant::fromValue(value));
}


/// <summary>
/// we need to preserve this for round tripping
/// </summary>
/// <value></value>

void DrawingAttributes::SetRasterOperation(uint value)
{
    _v1RasterOperation = value;
}

/// <summary>
/// When we load ISF from V1 if width is set and height is not
/// and PenTip is Circle, we need to set height to the same as width
/// or else we'll render different as an Ellipse.  We use this flag to
/// preserve state for round tripping.
/// </summary>

void DrawingAttributes::SetHeightChangedForCompatabity(bool value)
{
    _heightChangedForCompatabity = value;
}


//------------------------------------------------------
//
//  INotifyPropertyChanged Interface
//
//------------------------------------------------------


/// <summary>Overload of the Equals method which determines if two DrawingAttributes
/// objects contain the same drawing attributes</summary>
bool DrawingAttributes::Equals(DrawingAttributes const & that) const
{
    return (_extendedProperties == that._extendedProperties);
}


/// <summary>
/// Copies the DrawingAttributes
/// </summary>
/// <returns>Deep copy of the DrawingAttributes</returns>
/// <remarks></remarks>
QSharedPointer<DrawingAttributes> DrawingAttributes::Clone()
{
    //
    // use MemberwiseClone, which will instance the most derived type
    // We use this instead of Activator.CreateInstance because it does not
    // require ReflectionPermission.  One thing to note, all references
    // are shared, including event delegates, so we need to set those to null
    //
    QSharedPointer<DrawingAttributes> clone(new DrawingAttributes(_extendedProperties->Clone()));

    clone->Initialize();

    //don't need to clone these, it is a value type
    //and is copied by MemberwiseClone
    //_v1RasterOperation
    //_heightChangedForCompatabity
    return clone;
}


/// <summary>
/// Simple helper method used to determine if a QUuid
/// from an ExtendedProperty is used as the backing store
/// of a DrawingAttribute
/// </summary>
/// <param name="id"></param>
/// <returns></returns>
QVariant DrawingAttributes::GetDefaultDrawingAttributeValue(QUuid const & id)
{
    if (KnownIds::Color == id)
    {
        return QColor(Qt::black);
    }
    if (KnownIds::StylusWidth == id)
    {
        return DefaultWidth;
    }
    if (KnownIds::StylusTip == id)
    {
        return QVariant::fromValue(StylusTip::Ellipse);
    }
    if (KnownIds::DrawingFlags == id)
    {
        //note that in this implementation, FitToCurve is false by default
        return QVariant::fromValue(DrawingFlags({DrawingFlag::AntiAliased}));
    }
    if (KnownIds::StylusHeight == id)
    {
        return DefaultHeight;
    }
    if (KnownIds::StylusTipTransform == id)
    {
        return QMatrix();
    }
    if (KnownIds::IsHighlighter == id)
    {
        return false;
    }
    // this is a valid case
    // as this helper method is used not only to
    // get the default value, but also to see if
    // the QUuid is a drawing attribute value
    return QVariant();
}

void DrawingAttributes::ValidateStylusTipTransform(QUuid const & propertyDataId, QVariant propertyData)
{
    // NTRAID#WINOS-1136720-2005/06/22-XIAOTU:
    // Calling AddPropertyData(KnownIds::StylusTipTransform, "d") does not throw an ArgumentException.
    //  ExtendedPropertySerializer.Validate take a string as a valid type since StylusTipTransform
    //  gets serialized as a String, but at runtime is a Matrix
    if (propertyData.isNull())
    {
        throw std::exception("propertyData");
    }
    else if (propertyDataId == KnownIds::StylusTipTransform)
    {
        // StylusTipTransform gets serialized as a String, but at runtime is a Matrix
        if (propertyData.type() == QVariant::Type::String)
        {
            throw std::exception("propertyData");
        }
    }
}

/// <summary>
/// Simple helper method used to determine if a QUuid
/// needs to be removed from the ExtendedPropertyCollection in ISF
/// before serializing
/// </summary>
/// <param name="id"></param>
/// <returns></returns>
bool DrawingAttributes::RemoveIdFromExtendedProperties(QUuid const & id)
{
    if (KnownIds::Color == id ||
        KnownIds::Transparency == id ||
        KnownIds::StylusWidth == id ||
        KnownIds::DrawingFlags == id ||
        KnownIds::StylusHeight == id ||
        KnownIds::CurveFittingError == id )
    {
        return true;
    }
    return false;
}

/// <summary>
/// Returns true if two DrawingAttributes lead to the same PathGeometry.
/// </summary>
bool DrawingAttributes::GeometricallyEqual(DrawingAttributes const & left, DrawingAttributes const & right)
{

    // Optimization case:
    // must correspond to the same path geometry if they refer to the same instance.
    if (&left == &right)
    {
        return true;
    }

    if (left.GetStylusTip() == right.GetStylusTip() &&
        left.StylusTipTransform() == right.StylusTipTransform() &&
        DoubleUtil::AreClose(left.Width(), right.Width()) &&
        DoubleUtil::AreClose(left.Height(), right.Height()) &&
        left.GetDrawingFlags() == right.GetDrawingFlags() /*contains IgnorePressure / FitToCurve*/)
    {
        return true;
    }
    return false;
}

/// <summary>
/// Returns true if the QUuid passed in has impact on geometry of the stroke
/// </summary>
bool DrawingAttributes::IsGeometricalDaGuid(QUuid const & QUuid)
{
    // Assert it is a DA QUuid
    Debug::Assert(QVariant() != GetDefaultDrawingAttributeValue(QUuid));

    if (QUuid == KnownIds::StylusHeight || QUuid == KnownIds::StylusWidth ||
        QUuid == KnownIds::StylusTipTransform || QUuid == KnownIds::StylusTip ||
        QUuid == KnownIds::DrawingFlags)
    {
        return true;
    }

    return false;
}


/// <summary>
/// All DrawingAttributes are backed by an ExtendedProperty
/// this is a simple helper to set a property
/// </summary>
/// <param name="id">id</param>
/// <param name="value">value</param>
void DrawingAttributes::SetExtendedPropertyBackedProperty(QUuid const & id, QVariant value)
{
    if (_extendedProperties->Contains(id))
    {
        //
        // check to see if we're setting the property back
        // to a default value.  If we are we should remove it from
        // the EPC
        //
        QVariant defaultValue = GetDefaultDrawingAttributeValue(id);
        if (defaultValue.isValid())
        {
            if (defaultValue == value)
            {
                _extendedProperties->Remove(id);
                return;
            }
        }
        //
        // we're setting a non-default value on a EP that
        // already exists, check for equality before we do
        // so we don't raise unnecessary EPC changed events
        //
        QVariant o = GetExtendedPropertyBackedProperty(id);
        if (o != value)
        {
            _extendedProperties->Set(id, value);
        }
    }
    else
    {
        //
        // make sure we're not setting a default value of the QUuid
        // there is no need to do this
        //
        QVariant defaultValue = GetDefaultDrawingAttributeValue(id);
        if (defaultValue.isNull() || defaultValue != value)
        {
            _extendedProperties->Set(id, value);
        }
    }
}

/// <summary>
/// All DrawingAttributes are backed by an ExtendedProperty
/// this is a simple helper to set a property
/// </summary>
/// <param name="id">id</param>
/// <returns></returns>
QVariant DrawingAttributes::GetExtendedPropertyBackedProperty(QUuid const & id) const
{
    if (!_extendedProperties->Contains(id))
    {
        if (GetDefaultDrawingAttributeValue(id).isValid())
        {
            return GetDefaultDrawingAttributeValue(id);
        }
        throw std::exception("id");
    }
    else
    {
        return (*_extendedProperties)[id];
    }
}

/// <summary>
/// A help method which fires INotifyPropertyChanged.PropertyChanged event
/// </summary>
/// <param name="e"></param>
void DrawingAttributes::PrivateNotifyPropertyChanged(PropertyDataChangedEventArgs & e)
{
    if ( e.PropertyGuid() == KnownIds::Color)
    {
        OnPropertyChanged("Color");
    }
    else if ( e.PropertyGuid() == KnownIds::StylusTip)
    {
        OnPropertyChanged("StylusTip");
    }
    else if ( e.PropertyGuid() == KnownIds::StylusTipTransform)
    {
        OnPropertyChanged("StylusTipTransform");
    }
    else if ( e.PropertyGuid() == KnownIds::StylusHeight)
    {
        OnPropertyChanged("Height");
    }
    else if ( e.PropertyGuid() == KnownIds::StylusWidth)
    {
        OnPropertyChanged("Width");
    }
    else if ( e.PropertyGuid() == KnownIds::IsHighlighter)
    {
        OnPropertyChanged("IsHighlighter");
    }
    else if ( e.PropertyGuid() == KnownIds::DrawingFlags )
    {
        DrawingFlags changedBits;// = ( ( (DrawingFlags)e.PreviousValue ) ^ ( (DrawingFlags)e.NewValue ) );

        // NOTICE-2006/01/20-WAYNEZEN,
        // If someone changes FitToCurve and IgnorePressure simultaneously via AddPropertyData/RemovePropertyData,
        // we will fire both OnPropertyChangeds in advance the order of the values.
        if ( (changedBits & DrawingFlag::FitToCurve) != 0 )
        {
            OnPropertyChanged("FitToCurve");
        }

        if ( (changedBits & DrawingFlag::IgnorePressure) != 0 )
        {
            OnPropertyChanged("IgnorePressure");
        }
    }
}

void DrawingAttributes::OnPropertyChanged(QString propertyName)
{
    PropertyChangedEventArgs args(propertyName);
    OnPropertyChanged(args);
}
