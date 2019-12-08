#ifndef METRICENTRY_H
#define METRICENTRY_H

#include "Internal/Ink/InkSerializedFormat/isftagandguidcache.h"
#include "Windows/Input/styluspointpropertyinfo.h"

#include <QVector>

/// <summary>
/// Summary description for MetricEnty.
/// </summary>
enum class MetricEntryType
{
    Optional = 0,
    Must,
    Never,
    Custom,
};

// This is used while comparing two Metric Collections. This defines the relationship between
// two collections when one is superset of the other and can replace the other in the global
// list of collections
enum class SetType
{
    SubSet = 0,
    SuperSet,
};

struct MetricEntryList
{
    KnownTagCache::KnownTagIndex Tag;
    StylusPointPropertyInfo PropertyMetrics;

    MetricEntryList (KnownTagCache::KnownTagIndex tag, StylusPointPropertyInfo prop)
    {
        Tag = tag;
        PropertyMetrics = prop;
    }
};


class StylusPointPropertyInfo;

/// <summary>
/// This class holds the MetricEntries corresponding to a PacketProperty in the form of a link list
/// </summary>
class MetricEntry
{
    //Maximum buffer size required to store the largest MetricEntry
private:
    static int const MAX_METRIC_DATA_BUFF = 24;

private:
    KnownTagCache::KnownTagIndex _tag = KnownTagCache::KnownTagIndex(0);
    uint _size = 0;
    MetricEntry* _next;
    quint8 _data[MAX_METRIC_DATA_BUFF]; // We always allocate the max buffer needed to store the largest possible Metric Information blob
    static MetricEntryList _metricEntryOptional[];

    // helpers for Ink-local property metrics for X/Y coordiantes
public:
    static StylusPointPropertyInfo& DefaultXMetric;

    static StylusPointPropertyInfo& DefaultYMetric;


    /// <summary>
    /// List of MetricEntry that may or may not appear in the serialized form depending on their default Metrics.
    /// </summary>
    static MetricEntryList MetricEntry_Optional[];
    static int MetricEntry_OptionalLength;

    /// <summary>
    /// List of MetricEntry whose Metric Information must appear in the serialized form and always written as they do not have proper default
    /// </summary>
    static KnownTagCache::KnownTagIndex MetricEntry_Must[];
    static int MetricEntry_MustLength;

    /// <summary>
    /// List of MetricEntry whose Metric information will never appear in the Serialized format and always ignored
    /// </summary>
    static KnownTagCache::KnownTagIndex MetricEntry_Never[];
    static int MetricEntry_NeverLength;

    /// <summary>
    /// Default StylusPointPropertyInfo for any property
    /// </summary>
    static StylusPointPropertyInfo& DefaultPropertyMetrics;

    /// <summary>
    /// Gets or sets the Tag associated with this entry
    /// </summary>
    KnownTagCache::KnownTagIndex Tag() const
    {
         return _tag;
    }
    void SetTag(KnownTagCache::KnownTagIndex value)
    {
         _tag = value;
    }

    /// <summary>
    /// Gets the size associated with this entry
    /// </summary>
    uint Size() const
    {
        return _size;
    }

    /// <summary>
    /// Gets or Sets the data associated with this metric entry
    /// </summary>
    quint8 const * Data() const
    {
        return _data;
    }
    void SetData(QVector<quint8> data);

    /// <summary>
    /// Compares a metricEntry object with this one and returns true or false
    /// </summary>
    /// <param name="metricEntry"></param>
    /// <returns></returns>
    bool Compare(MetricEntry const & metricEntry);

    /// <summary>
    /// Gets/Sets the next entry in the list
    /// </summary>
    MetricEntry* Next()
    {
         return _next;
    }
    void SetNext(MetricEntry* value)
    {
         _next = value;
    }

    /// <summary>
    /// Constructro
    /// </summary>
    MetricEntry();

    /// <summary>
    /// Adds an entry in the list
    /// </summary>
    /// <param name="next"></param>
    void Add(MetricEntry* next);

    /// <summary>
    /// Initializes a MetricEntry based on StylusPointPropertyInfo and default StylusPointPropertyInfo for the property
    /// </summary>
    /// <param name="originalInfo"></param>
    /// <param name="defaultInfo"></param>
    /// <returns></returns>
    void Initialize(StylusPointPropertyInfo originalInfo, StylusPointPropertyInfo defaultInfo);

    /// <summary>
    /// Creates a metric entry based on a PropertyInfo and Tag and returns the Metric Entry Type created
    /// </summary>
    /// <param name="propertyInfo"></param>
    /// <param name="tag"></param>
    /// <returns></returns>
    MetricEntryType CreateMetricEntry(StylusPointPropertyInfo propertyInfo, KnownTagCache::KnownTagIndex tag);

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

    static bool IsValidMetricEntry(StylusPointPropertyInfo propertyInfo, KnownTagCache::KnownTagIndex tag, MetricEntryType& metricEntryType, uint& index);
};


#endif // METRICENTRY_H
