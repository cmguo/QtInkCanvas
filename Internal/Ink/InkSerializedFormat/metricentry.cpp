#include "Internal/Ink/InkSerializedFormat/metricentry.h"
#include "Internal/Ink/InkSerializedFormat/serializationhelper.h"
#include "Windows/Input/styluspointpropertyinfodefaults.h"
#include "Internal/doubleutil.h"

#include <QBuffer>
#include <QDataStream>

INKCANVAS_BEGIN_NAMESPACE

StylusPointPropertyInfo& MetricEntry::DefaultXMetric = MetricEntry_Optional[0].PropertyMetrics;

StylusPointPropertyInfo& MetricEntry::DefaultYMetric = MetricEntry_Optional[1].PropertyMetrics;

/// <summary>
/// List of MetricEntry that may or may not appear in the serialized form depending on their default Metrics.
/// </summary>
MetricEntryList MetricEntry::MetricEntry_Optional[] = {
    {KnownIdCache::KnownGuidBaseIndex + KnownIdCache::OriginalISFIdIndex::X,                     StylusPointPropertyInfoDefaults::X},
    {KnownIdCache::KnownGuidBaseIndex + KnownIdCache::OriginalISFIdIndex::Y,                     StylusPointPropertyInfoDefaults::Y},
    {KnownIdCache::KnownGuidBaseIndex + KnownIdCache::OriginalISFIdIndex::Z,                     StylusPointPropertyInfoDefaults::Z},
    {KnownIdCache::KnownGuidBaseIndex + KnownIdCache::OriginalISFIdIndex::NormalPressure,        StylusPointPropertyInfoDefaults::NormalPressure},
    {KnownIdCache::KnownGuidBaseIndex + KnownIdCache::OriginalISFIdIndex::TangentPressure,       StylusPointPropertyInfoDefaults::TangentPressure},
    {KnownIdCache::KnownGuidBaseIndex + KnownIdCache::OriginalISFIdIndex::ButtonPressure,        StylusPointPropertyInfoDefaults::ButtonPressure},
    {KnownIdCache::KnownGuidBaseIndex + KnownIdCache::OriginalISFIdIndex::XTiltOrientation,      StylusPointPropertyInfoDefaults::XTiltOrientation},
    {KnownIdCache::KnownGuidBaseIndex + KnownIdCache::OriginalISFIdIndex::YTiltOrientation,      StylusPointPropertyInfoDefaults::YTiltOrientation},
    {KnownIdCache::KnownGuidBaseIndex + KnownIdCache::OriginalISFIdIndex::AzimuthOrientation,    StylusPointPropertyInfoDefaults::AzimuthOrientation},
    {KnownIdCache::KnownGuidBaseIndex + KnownIdCache::OriginalISFIdIndex::AltitudeOrientation,   StylusPointPropertyInfoDefaults::AltitudeOrientation},
    {KnownIdCache::KnownGuidBaseIndex + KnownIdCache::OriginalISFIdIndex::TwistOrientation,      StylusPointPropertyInfoDefaults::TwistOrientation}
};
int MetricEntry::MetricEntry_OptionalLength = sizeof(MetricEntry_Optional) / sizeof(MetricEntry_Optional[0]);

/// <summary>
/// List of MetricEntry whose Metric Information must appear in the serialized form and always written as they do not have proper default
/// </summary>
KnownTagCache::KnownTagIndex MetricEntry::MetricEntry_Must[] = {
    (KnownTagCache::KnownTagIndex)((uint)KnownIdCache::KnownGuidBaseIndex + KnownIdCache::OriginalISFIdIndex::PitchRotation),
    (KnownTagCache::KnownTagIndex)((uint)KnownIdCache::KnownGuidBaseIndex + KnownIdCache::OriginalISFIdIndex::RollRotation),
    (KnownTagCache::KnownTagIndex)((uint)KnownIdCache::KnownGuidBaseIndex + KnownIdCache::OriginalISFIdIndex::YawRotation),
};
int MetricEntry::MetricEntry_MustLength = sizeof(MetricEntry_Must) / sizeof(MetricEntry_Must[0]);

/// <summary>
/// List of MetricEntry whose Metric information will never appear in the Serialized format and always ignored
/// </summary>
KnownTagCache::KnownTagIndex MetricEntry::MetricEntry_Never[] = {
    (KnownTagCache::KnownTagIndex)((uint)KnownIdCache::KnownGuidBaseIndex + KnownIdCache::OriginalISFIdIndex::PacketStatus),
    (KnownTagCache::KnownTagIndex)((uint)KnownIdCache::KnownGuidBaseIndex + KnownIdCache::OriginalISFIdIndex::TimerTick),
    (KnownTagCache::KnownTagIndex)((uint)KnownIdCache::KnownGuidBaseIndex + KnownIdCache::OriginalISFIdIndex::SerialNumber),
};
int MetricEntry::MetricEntry_NeverLength = sizeof(MetricEntry_Never) / sizeof(MetricEntry_Never[0]);

StylusPointPropertyInfo& MetricEntry::DefaultPropertyMetrics = StylusPointPropertyInfoDefaults::DefaultValue;

MetricEntry::MetricEntry()
{

}

void MetricEntry::SetData(QVector<quint8> data)
{
    if( data.size() > MAX_METRIC_DATA_BUFF )
        _size = (uint)MAX_METRIC_DATA_BUFF;
    else
        _size = (uint)data.size();
    for( int i = 0; i < (int)_size; i++ )
        _data[i] = data[i];
}

/// <summary>
/// Compares a metricEntry object with this one and returns true or false
/// </summary>
/// <param name="metricEntry"></param>
/// <returns></returns>
bool MetricEntry::Compare(MetricEntry const & metricEntry)
{
    if( Tag() != metricEntry.Tag() )
        return false;
    if( Size() != metricEntry.Size() )
        return false;
    for( int i = 0; i < Size(); i++ )
    {
        if( Data()[i] != metricEntry.Data()[i] )
            return false;
    }
    return true;
}

/// <summary>
/// Adds an entry in the list
/// </summary>
/// <param name="next"></param>
void MetricEntry::Add(MetricEntry* next)
{
    if( nullptr == _next )
    {
        _next = next;
        return;
    }
    MetricEntry* prev = _next;
    while( nullptr != prev->_next )
    {
        prev = prev->_next;
    }
    prev->_next = next;
}

/// <summary>
/// Initializes a MetricEntry based on StylusPointPropertyInfo and default StylusPointPropertyInfo for the property
/// </summary>
/// <param name="originalInfo"></param>
/// <param name="defaultInfo"></param>
/// <returns></returns>
void MetricEntry::Initialize(StylusPointPropertyInfo originalInfo, StylusPointPropertyInfo defaultInfo)
{
    _size = 0;
    //using (MemoryStream strm = new MemoryStream(_data))
    QByteArray data((char *)_data, sizeof (_data));
    QBuffer strm(&data);
    {
        if (!DoubleUtil::AreClose(originalInfo.Resolution(), defaultInfo.Resolution()))
        {
            // First min value
            _size += SerializationHelper::SignEncode(strm, originalInfo.Minimum());
            // Max value
            _size += SerializationHelper::SignEncode(strm, originalInfo.Maximum());
            // Units
            _size += SerializationHelper::Encode(strm, (uint)originalInfo.Unit());
            // resolution
            //using (BinaryWriter bw = new BinaryWriter(strm))
            QDataStream bw(&strm);
            bw.setVersion(QDataStream::Qt_4_0);
            {
                bw << (originalInfo.Resolution());
                _size += 4; // sizeof(float)
            }
        }
        else if (originalInfo.Unit() != defaultInfo.Unit())
        {
            // First min value
            _size += SerializationHelper::SignEncode(strm, originalInfo.Minimum());
            // Max value
            _size += SerializationHelper::SignEncode(strm, originalInfo.Maximum());
            // Units
            _size += SerializationHelper::Encode(strm, (uint)originalInfo.Unit());
        }
        else if (originalInfo.Maximum() != defaultInfo.Maximum())
        {
            // First min value
            _size += SerializationHelper::SignEncode(strm, originalInfo.Minimum());
            // Max value
            _size += SerializationHelper::SignEncode(strm, originalInfo.Maximum());
        }
        else if (originalInfo.Minimum() != defaultInfo.Minimum())
        {
            _size += SerializationHelper::SignEncode(strm, originalInfo.Minimum());
        }
    }
}

/// <summary>
/// Creates a metric entry based on a PropertyInfo and Tag and returns the Metric Entry Type created
/// </summary>
/// <param name="propertyInfo"></param>
/// <param name="tag"></param>
/// <returns></returns>
MetricEntryType MetricEntry::CreateMetricEntry(StylusPointPropertyInfo propertyInfo, KnownTagCache::KnownTagIndex tag)
{
    // First create the default Metric entry based on the property and type of metric entry and then use that to initialize the
    // metric entry data.
    uint index = 0;
    SetTag(tag);

    MetricEntryType type;
    if( IsValidMetricEntry(propertyInfo, Tag(), type, index) )
    {
        switch(type)
        {
            case MetricEntryType::Optional:
            {
                Initialize(propertyInfo, MetricEntry_Optional[index].PropertyMetrics);
                break;
            }
            case MetricEntryType::Must :
            case MetricEntryType::Custom:
                Initialize(propertyInfo, DefaultPropertyMetrics);
                break;
            default:
                throw std::runtime_error(("MetricEntryType was persisted with Never flag which should never happen"));
        }
    }
    return type;
}
/// <summary>
/// This function checks if this packet property results in a valid metric entry. This will be a valid entry if
/// 1. it is a custom property, 2. Does not belong to the global list of gaMetricEntry_Never, 3. Belongs to the
/// global list of gaMetricEntry_Must and 4. Belongs to global list of gaMetricEntry_Optional and at least one of
/// its metric values is different from the corresponding default.
/// </summary>
/// <param name="propertyInfo"></param>
/// <param name="tag"></param>
/// <param name="metricEntryType"></param>
/// <param name="index"></param>
/// <returns></returns>

bool MetricEntry::IsValidMetricEntry(StylusPointPropertyInfo propertyInfo, KnownTagCache::KnownTagIndex tag, MetricEntryType& metricEntryType, uint& index)
{
    index = 0;
    // If this is a custom property, check if all the Metric values are null or not. If they are then this is not a
    // valid metric entry
    if (tag >= (KnownTagCache::KnownTagIndex)KnownIdCache::CustomGuidBaseIndex)
    {
        metricEntryType = MetricEntryType::Custom;
        if( INT_MIN == propertyInfo.Minimum() &&
            INT_MAX == propertyInfo.Maximum() &&
            StylusPointPropertyUnit::None == propertyInfo.Unit() &&
            DoubleUtil::AreClose(1.0, propertyInfo.Resolution()) )
            return false;
        else
            return true;
    }
    else
    {
        int ul;
        // First find the property in the gaMetricEntry_Never. If it belongs to this list,
        // we will never write the metric table for this prop. So return FALSE;
        for( ul = 0; ul < MetricEntry_NeverLength ; ul++ )
        {
            if( MetricEntry_Never[ul] == tag )
            {
                metricEntryType = MetricEntryType::Never;
                return false;
            }
        }

        // Then search the property in the gaMetricEntry_Must list. If it belongs to this list,
        // we must always write the metric table for this prop. So return TRUE;
        for( ul = 0; ul<MetricEntry_MustLength; ul++ )
        {
            if( MetricEntry_Must[ul] == tag )
            {
                metricEntryType = MetricEntryType::Must;
                if( propertyInfo.Minimum() == DefaultPropertyMetrics.Minimum() &&
                    propertyInfo.Maximum() == DefaultPropertyMetrics.Maximum() &&
                    propertyInfo.Unit() == DefaultPropertyMetrics.Unit() &&
                    DoubleUtil::AreClose(propertyInfo.Resolution(), DefaultPropertyMetrics.Resolution() ))
                    return false;
                else
                    return true;
            }
        }

        // Now seach it in the gaMetricEntry_Optional list. If it is there, check the metric values
        // agianst the default values and if there is any non default value, return TRUE;
        for( ul = 0; ul<MetricEntry_OptionalLength; ul++ )
        {
            if( ((MetricEntryList)MetricEntry_Optional[ul]).Tag == tag )
            {
                metricEntryType = MetricEntryType::Optional;
                if( propertyInfo.Minimum() == MetricEntry_Optional[ul].PropertyMetrics.Minimum() &&
                    propertyInfo.Maximum() == MetricEntry_Optional[ul].PropertyMetrics.Maximum() &&
                    propertyInfo.Unit() == MetricEntry_Optional[ul].PropertyMetrics.Unit() &&
                    DoubleUtil::AreClose(propertyInfo.Resolution(), MetricEntry_Optional[ul].PropertyMetrics.Resolution()) )
                    return false;
                else
                {
                    index = (uint)ul;
                    return true;
                }
            }
        }
        // it is not found in any of the list. Force to write all metric entries for the property.
        metricEntryType = MetricEntryType::Must;
        return true;
    }
}

INKCANVAS_END_NAMESPACE
