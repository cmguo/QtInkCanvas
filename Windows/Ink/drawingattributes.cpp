#include "Windows/Ink/drawingattributes.h"
#include "Windows/Ink/stylusshape.h"
#include "Windows/Ink/knownids.h"
#include "Windows/Ink/events.h"
#include "Windows/Ink/extendedpropertycollection.h"
#include "Internal/doubleutil.h"
#include "Internal/debug.h"

INKCANVAS_BEGIN_NAMESPACE

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
    delete _extendedProperties;
}

/// <summary>
/// Common constructor call, also called by Clone
/// </summary>
void DrawingAttributes::Initialize()
{
    Debug::Assert(_extendedProperties != nullptr);
#ifdef INKCANVAS_QT
    //_extendedProperties->Changed +=
    //    new ExtendedPropertiesChangedEventHandler(this.ExtendedPropertiesChanged_EventForwarder);
    QObject::connect(_extendedProperties, &ExtendedPropertyCollection::Changed,
                     this, &DrawingAttributes::ExtendedPropertiesChanged_EventForwarder);
#endif
}

#ifdef INKCANVAS_QT

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

#endif

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
    SetExtendedPropertyBackedProperty(KnownIds::StylusTip, Variant::fromValue(value));
}

/// <summary>
/// The StylusTip used to draw the stroke
/// </summary>
Matrix DrawingAttributes::StylusTipTransform() const
{
    //prevent boxing / unboxing if possible
    if (!_extendedProperties->Contains(KnownIds::StylusTipTransform))
    {
        Debug::Assert(Matrix::Identity() == GetDefaultDrawingAttributeValue(KnownIds::StylusTipTransform).value<Matrix>());
        return Matrix();
    }
    return GetExtendedPropertyBackedProperty(KnownIds::StylusTipTransform).value<Matrix>();
}

void DrawingAttributes::SetStylusTipTransform(Matrix const & value)
{
    Matrix const & m = value;
    if (m.OffsetX() != 0 || m.OffsetY() != 0)
    {
        throw std::runtime_error("value");
    }
    //no need to raise change events, they will bubble up from the EPC
    //underneath us
    // Validation of value is done in EPC
    SetExtendedPropertyBackedProperty(KnownIds::StylusTipTransform, value);
    drawingShape_.reset();
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
    return GetExtendedPropertyBackedProperty(KnownIds::StylusHeight).value<double>();
}

void DrawingAttributes::SetHeight(double value)
{
    if (Double::IsNaN(value) || value < MinHeight || value > MaxHeight)
    {
        throw std::runtime_error("Height");
    }
    //no need to raise change events, they will bubble up from the EPC
    //underneath us
    SetExtendedPropertyBackedProperty(KnownIds::StylusHeight, value);
    drawingShape_.reset();
}

/// <summary>
/// The width of the StylusTip
/// </summary>
double DrawingAttributes::Width() const
{
    //prevent boxing / unboxing if possible
    if (!_extendedProperties->Contains(KnownIds::StylusWidth))
    {
        Debug::Assert(DefaultWidth == GetDefaultDrawingAttributeValue(KnownIds::StylusWidth).value<double>());
        return DefaultWidth;
    }
    return GetExtendedPropertyBackedProperty(KnownIds::StylusWidth).value<double>();
}
void DrawingAttributes::SetWidth(double value)
{
    if (Double::IsNaN(value) || value < MinWidth || value > MaxWidth)
    {
        throw std::runtime_error("Width");
    }
    //no need to raise change events, they will bubble up from the EPC
    //underneath us
    SetExtendedPropertyBackedProperty(KnownIds::StylusWidth, value);
    drawingShape_.reset();
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
        flags = flags | DrawingFlag::FitToCurve;
    }
    else
    {
        //turn off the bit
        flags = flags & ~DrawingFlag::FitToCurve;
    }
    SetExtendedPropertyBackedProperty(KnownIds::DrawingFlags, Variant::fromValue(flags));
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
        flags = flags | DrawingFlag::IgnorePressure;
    }
    else
    {
        //turn off the bit
        flags = flags & ~DrawingFlag::IgnorePressure;
    }
    SetExtendedPropertyBackedProperty(KnownIds::DrawingFlags, Variant::fromValue(flags));
}

/// <summary>
/// Determines if the stroke should be treated as a highlighter
/// </summary>
bool DrawingAttributes::IsHighlighter() const
{
    //prevent boxing / unboxing if possible
    if (!_extendedProperties->Contains(KnownIds::IsHighlighter))
    {
        Debug::Assert(false == GetDefaultDrawingAttributeValue(KnownIds::IsHighlighter).value<bool>());
        return false;
    }
    else
    {
        Debug::Assert(true == GetExtendedPropertyBackedProperty(KnownIds::IsHighlighter).value<bool>());
        return true;
    }
}
void DrawingAttributes::SetIsHighlighter(bool value)
{
    //no need to raise change events, they will bubble up from the EPC
    //underneath us
    SetExtendedPropertyBackedProperty(KnownIds::IsHighlighter, value);

#ifndef INKCANVAS_CORE
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
#endif
}

/// <summary>
/// Allows addition of objects to the EPC
/// </summary>
/// <param name="propertyDataId"></param>
/// <param name="propertyData"></param>
void DrawingAttributes::AddPropertyData(Guid const & propertyDataId, Variant const & propertyData)
{
    ValidateStylusTipTransform(propertyDataId, propertyData);
    SetExtendedPropertyBackedProperty(propertyDataId, propertyData);
}

/// <summary>
/// Allows removal of objects from the EPC
/// </summary>
/// <param name="propertyDataId"></param>
void DrawingAttributes::RemovePropertyData(Guid const & propertyDataId)
{
    _extendedProperties->Remove(propertyDataId);
}

/// <summary>
/// Allows retrieval of objects from the EPC
/// </summary>
/// <param name="propertyDataId"></param>
Variant DrawingAttributes::GetPropertyData(Guid const & propertyDataId)
{
    return GetExtendedPropertyBackedProperty(propertyDataId);
}

/// <summary>
/// Allows retrieval of a Array of Guids that are contained in the EPC
/// </summary>
Array<Guid> DrawingAttributes::GetPropertyDataIds()
{
    return _extendedProperties->GetGuidArray();
}

/// <summary>
/// Allows check of containment of objects to the EPC
/// </summary>
/// <param name="propertyDataId"></param>
bool DrawingAttributes::ContainsPropertyData(Guid const & propertyDataId)
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
    if (drawingShape_)
        return drawingShape_.get();
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
    drawingShape_.reset(s);
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
        return (*_extendedProperties)[KnownIds::CurveFittingError].value<int>();
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
    SetExtendedPropertyBackedProperty(KnownIds::DrawingFlags, Variant::fromValue(value));
}


/// <summary>
/// we need to preserve this for round tripping
/// </summary>
/// <value></value>

#ifndef INKCANVAS_CORE

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

#endif


//------------------------------------------------------
//
//  INotifyPropertyChanged Interface
//
//------------------------------------------------------


/// <summary>Overload of the Equals method which determines if two DrawingAttributes
/// objects contain the same drawing attributes</summary>
bool DrawingAttributes::Equals(DrawingAttributes const & that) const
{
    return (*_extendedProperties == *that._extendedProperties);
}


/// <summary>
/// Copies the DrawingAttributes
/// </summary>
/// <returns>Deep copy of the DrawingAttributes</returns>
/// <remarks></remarks>
SharedPointer<DrawingAttributes> DrawingAttributes::Clone()
{
    //
    // use MemberwiseClone, which will instance the most derived type
    // We use this instead of Activator.CreateInstance because it does not
    // require ReflectionPermission.  One thing to note, all references
    // are shared, including event delegates, so we need to set those to null
    //
    SharedPointer<DrawingAttributes> clone(new DrawingAttributes(_extendedProperties->Clone()));

    clone->Initialize();

    //don't need to clone these, it is a value type
    //and is copied by MemberwiseClone
    //_v1RasterOperation
    //_heightChangedForCompatabity
    return clone;
}


/// <summary>
/// Simple helper method used to determine if a Guid
/// from an ExtendedProperty is used as the backing store
/// of a DrawingAttribute
/// </summary>
/// <param name="id"></param>
/// <returns></returns>
Variant DrawingAttributes::GetDefaultDrawingAttributeValue(Guid const & id)
{
#ifdef INKCANVAS_QT
    if (KnownIds::Color == id)
    {
        return QColor(Qt::black);
    }
#endif
    if (KnownIds::StylusWidth == id)
    {
        return DefaultWidth;
    }
    if (KnownIds::StylusTip == id)
    {
        return Variant::fromValue(StylusTip::Ellipse);
    }
    if (KnownIds::DrawingFlags == id)
    {
        //note that in this implementation, FitToCurve is false by default
        return Variant::fromValue(DrawingFlags(DrawingFlag::AntiAliased));
    }
    if (KnownIds::StylusHeight == id)
    {
        return DefaultHeight;
    }
    if (KnownIds::StylusTipTransform == id)
    {
        return Matrix();
    }
    if (KnownIds::IsHighlighter == id)
    {
        return false;
    }
    // this is a valid case
    // as this helper method is used not only to
    // get the default value, but also to see if
    // the Guid is a drawing attribute value
    return Variant();
}

void DrawingAttributes::ValidateStylusTipTransform(Guid const & propertyDataId, Variant propertyData)
{
    // NTRAID#WINOS-1136720-2005/06/22-XIAOTU:
    // Calling AddPropertyData(KnownIds::StylusTipTransform, "d") does not throw an ArgumentException.
    //  ExtendedPropertySerializer.Validate take a string as a valid type since StylusTipTransform
    //  gets serialized as a String, but at runtime is a Matrix
    if (propertyData == nullptr)
    {
        throw std::runtime_error("propertyData");
    }
    else if (propertyDataId == KnownIds::StylusTipTransform)
    {
#ifdef INKCANVAS_QT
        // StylusTipTransform gets serialized as a String, but at runtime is a Matrix
        if (propertyData.type() == Variant::Type::String)
        {
            throw std::runtime_error("propertyData");
        }
#endif
    }
}

/// <summary>
/// Simple helper method used to determine if a Guid
/// needs to be removed from the ExtendedPropertyCollection in ISF
/// before serializing
/// </summary>
/// <param name="id"></param>
/// <returns></returns>
bool DrawingAttributes::RemoveIdFromExtendedProperties(Guid const & id)
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
/// Returns true if the Guid passed in has impact on geometry of the stroke
/// </summary>
bool DrawingAttributes::IsGeometricalDaGuid(Guid const & Guid)
{
    // Assert it is a DA Guid
    Debug::Assert(Variant() != GetDefaultDrawingAttributeValue(Guid));

    if (Guid == KnownIds::StylusHeight || Guid == KnownIds::StylusWidth ||
        Guid == KnownIds::StylusTipTransform || Guid == KnownIds::StylusTip ||
        Guid == KnownIds::DrawingFlags)
    {
        return true;
    }

    return false;
}


/// <summary>
/// Whenever the base class fires the generic ExtendedPropertiesChanged
/// event, we need to fire the DrawingAttributesChanged event also.
/// </summary>
/// <param name="sender">Should be 'this' object</param>
/// <param name="args">The custom attributes that changed</param>
void DrawingAttributes::ExtendedPropertiesChanged_EventForwarder(ExtendedPropertiesChangedEventArgs& args)
{
    //Debug::Assert(sender != null);
    //Debug::Assert(args != null);

    //see if the EP that changed is a drawingattribute
    if (args.NewProperty().Value() == nullptr)
    {
        //a property was removed, see if it is a drawing attribute property
        Variant defaultValueIfDrawingAttribute
            = DrawingAttributes::GetDefaultDrawingAttributeValue(args.OldProperty().Id());
        if (defaultValueIfDrawingAttribute != nullptr)
        {
            ExtendedProperty newProperty(   args.OldProperty().Id(),
                                        defaultValueIfDrawingAttribute);
            //this is a da guid
            PropertyDataChangedEventArgs dargs(  args.OldProperty().Id(),
                                                        newProperty.Value(),      //the property
                                                        args.OldProperty().Value());//previous value

            OnAttributeChanged(dargs);
        }
        else
        {
            PropertyDataChangedEventArgs dargs(  args.OldProperty().Id(),
                                                        Variant(),      //the property
                                                        args.OldProperty().Value());//previous value

            OnPropertyDataChanged(dargs);

        }
    }
    else if (args.OldProperty().Value() == nullptr)
    {
        //a property was added, see if it is a drawing attribute property
        Variant defaultValueIfDrawingAttribute
            = DrawingAttributes::GetDefaultDrawingAttributeValue(args.NewProperty().Id());
        if (defaultValueIfDrawingAttribute != nullptr)
        {
            if (defaultValueIfDrawingAttribute != args.NewProperty().Value())
            {
                //this is a da guid
                PropertyDataChangedEventArgs dargs (  args.NewProperty().Id(),
                                                            args.NewProperty().Value(),   //the property
                                                            defaultValueIfDrawingAttribute);     //previous value

                OnAttributeChanged(dargs);
            }
        }
        else
        {
            PropertyDataChangedEventArgs dargs(args.NewProperty().Id(),
                                                 args.NewProperty().Value(),   //the property
                                                 Variant());     //previous value
            OnPropertyDataChanged(dargs);

        }
    }
    else
    {
        //something was modified, see if it is a drawing attribute property
        Variant defaultValueIfDrawingAttribute
            = DrawingAttributes::GetDefaultDrawingAttributeValue(args.NewProperty().Id());
        if (defaultValueIfDrawingAttribute != nullptr)
        {
            //
            // we only raise DA changed when the value actually changes
            //
            if (args.NewProperty().Value() != (args.OldProperty().Value()))
            {
                //this is a da guid
                PropertyDataChangedEventArgs dargs(  args.NewProperty().Id(),
                                                            args.NewProperty().Value(),       //the da
                                                            args.OldProperty().Value());//old value

                OnAttributeChanged(dargs);
            }
        }
        else
        {
            if (args.NewProperty().Value() != (args.OldProperty().Value()))
            {
                PropertyDataChangedEventArgs dargs(  args.NewProperty().Id(),
                                                            args.NewProperty().Value(),
                                                            args.OldProperty().Value());//old value

                OnPropertyDataChanged(dargs);
            }
        }
    }
}

/// <summary>
/// Method called when a change occurs to any DrawingAttribute
/// </summary>
/// <param name="e">The change information for the DrawingAttribute that was modified</param>
void DrawingAttributes::OnAttributeChanged(PropertyDataChangedEventArgs &e)
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
#ifdef INKCANVAS_QT
        //if ( this.AttributeChanged != null )
        //{
            emit AttributeChanged(e);
        //}
#endif
    //}
}

/// <summary>
/// All DrawingAttributes are backed by an ExtendedProperty
/// this is a simple helper to set a property
/// </summary>
/// <param name="id">id</param>
/// <param name="value">value</param>
void DrawingAttributes::SetExtendedPropertyBackedProperty(Guid const & id, Variant const & value)
{
    if (_extendedProperties->Contains(id))
    {
        //
        // check to see if we're setting the property back
        // to a default value.  If we are we should remove it from
        // the EPC
        //
        Variant defaultValue = GetDefaultDrawingAttributeValue(id);
        if (defaultValue != nullptr)
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
        Variant o = GetExtendedPropertyBackedProperty(id);
        if (o != value)
        {
            _extendedProperties->Set(id, value);
        }
    }
    else
    {
        //
        // make sure we're not setting a default value of the Guid
        // there is no need to do this
        //
        Variant defaultValue = GetDefaultDrawingAttributeValue(id);
        if (defaultValue == nullptr || defaultValue != value)
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
Variant DrawingAttributes::GetExtendedPropertyBackedProperty(Guid const & id) const
{
    if (!_extendedProperties->Contains(id))
    {
        if (GetDefaultDrawingAttributeValue(id) != nullptr)
        {
            return GetDefaultDrawingAttributeValue(id);
        }
        throw std::runtime_error("id");
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
        DrawingFlags changedBits = ( ( e.PreviousValue().value<DrawingFlags>() ) ^ ( e.NewValue().value<DrawingFlags>() ) );

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

void DrawingAttributes::OnPropertyChanged(char const * propertyName)
{
    PropertyChangedEventArgs args(propertyName);
    OnPropertyChanged(args);
}

#ifdef INKCANVAS_QT
Variant DrawingAttributesDefaultValueFactory::DefaultValue()
{
    return Variant::fromValue(SharedPointer<DrawingAttributes>(new DrawingAttributes));
}
#endif

INKCANVAS_END_NAMESPACE
